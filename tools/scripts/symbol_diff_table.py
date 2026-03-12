#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any


IGNORED_DIRS = {
    ".git", ".venv", "venv", "__pycache__", "node_modules",
    ".mypy_cache", ".pytest_cache", ".idea", ".vscode",
}


def eprint(*args: object) -> None:
    print(*args, file=sys.stderr)


def find_repo_root(start: Path) -> Path:
    cur = start.resolve()
    for p in [cur, *cur.parents]:
        if (p / ".git").exists():
            return p
    return cur


def iter_json_files(root: Path):
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in IGNORED_DIRS]
        for filename in filenames:
            if filename.endswith(".json"):
                yield Path(dirpath) / filename


def looks_like_unit_dict(obj: Any) -> bool:
    return (
        isinstance(obj, dict)
        and isinstance(obj.get("name"), str)
        and isinstance(obj.get("base_path"), str)
        and isinstance(obj.get("target_path"), str)
    )


def extract_unit_dicts(obj: Any):
    found = []
    if looks_like_unit_dict(obj):
        found.append(obj)

    if isinstance(obj, dict):
        for v in obj.values():
            found.extend(extract_unit_dicts(v))
    elif isinstance(obj, list):
        for v in obj:
            found.extend(extract_unit_dicts(v))
    return found


def load_units_from_json(path: Path):
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except Exception:
        return []

    units = extract_unit_dicts(data)
    for u in units:
        u["_source_json"] = str(path)
    return units


def get_defined_text_symbols(obj_path: Path) -> set[str]:
    result = subprocess.run(
        ["nm", str(obj_path)],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        text=True,
        check=False,
    )

    out = set()
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[-2] in {"T", "t"}:
            out.add(parts[-1])
    return out


def find_symbol_unit(repo_root: Path, symbol: str):
    matches = []

    for json_file in iter_json_files(repo_root):
        for unit in load_units_from_json(json_file):
            base_path = (repo_root / unit["base_path"]).resolve()
            target_path = (repo_root / unit["target_path"]).resolve()

            if not base_path.is_file() or not target_path.is_file():
                continue

            base_defs = get_defined_text_symbols(base_path)
            target_defs = get_defined_text_symbols(target_path)

            if symbol in base_defs and symbol in target_defs:
                matches.append((unit, base_path, target_path))

    if not matches:
        raise RuntimeError(f"Could not find symbol '{symbol}' in both base and target objects.")
    if len(matches) > 1:
        names = ", ".join(m[0]["name"] for m in matches)
        raise RuntimeError(f"Symbol '{symbol}' appears in multiple units: {names}")

    return matches[0]


def run_objdiff_json(objdiff_path: Path, target_path: Path, base_path: Path, symbol: str) -> dict:
    cmd = [
        str(objdiff_path),
        "diff",
        "-1", str(target_path),
        "-2", str(base_path),
        "--format", "json",
        "-o", "-",
        symbol,
    ]

    result = subprocess.run(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )

    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "objdiff failed")

    return json.loads(result.stdout)


def find_symbol_pair(data: dict, symbol: str):
    left_symbols = data["left"]["symbols"]
    right_symbols = data["right"]["symbols"]

    left_matches = [sym for sym in left_symbols if sym.get("name") == symbol]
    right_matches = [sym for sym in right_symbols if sym.get("name") == symbol]

    if not left_matches:
        raise RuntimeError(f"Symbol '{symbol}' not found in left.symbols")
    if not right_matches:
        raise RuntimeError(f"Symbol '{symbol}' not found in right.symbols")

    if len(left_matches) > 1:
        raise RuntimeError(f"Symbol '{symbol}' appears multiple times in left.symbols")
    if len(right_matches) > 1:
        raise RuntimeError(f"Symbol '{symbol}' appears multiple times in right.symbols")

    return left_matches[0], right_matches[0]


def insn_text(entry: dict | None) -> str:
    if not entry:
        return ""
    insn = entry.get("instruction")
    if not isinstance(insn, dict):
        return ""
    # addr = insn.get("address")
    formatted = insn.get("formatted", "")
    # if addr is None:
    #     return str(formatted).strip()
    # return f"{addr}:    {formatted}".rstrip()
    return f"{formatted}".rstrip()


def classify_note(left_entry: dict | None, right_entry: dict | None) -> str:
    if left_entry is None:
        return "insert"
    if right_entry is None:
        return "delete"

    diff_kind = left_entry.get("diff_kind", "")
    if not diff_kind:
        return ""

    if diff_kind == "DIFF_INSERT":
        return "insert"
    if diff_kind == "DIFF_DELETE":
        return "delete"
    if diff_kind == "DIFF_ARG_MISMATCH":
        return "operand difference"
    if diff_kind == "DIFF_OPCODE_MISMATCH":
        return "instruction difference"
    return diff_kind.lower().replace("diff_", "").replace("_", " ")


def align_rows(left_instructions: list[dict], right_instructions: list[dict]):
    rows = []
    i = 0
    j = 0

    while i < len(left_instructions) or j < len(right_instructions):
        left_entry = left_instructions[i] if i < len(left_instructions) else None

        if left_entry is None:
            rows.append(("", insn_text(right_instructions[j]), "insert"))
            j += 1
            continue

        diff_kind = left_entry.get("diff_kind", "")

        if diff_kind == "DIFF_INSERT":
            right_entry = right_instructions[j] if j < len(right_instructions) else None
            rows.append(("", insn_text(right_entry), "insert"))
            j += 1
            i += 1
            continue

        if diff_kind == "DIFF_DELETE":
            rows.append((insn_text(left_entry), "", "delete"))
            i += 1
            continue

        right_entry = right_instructions[j] if j < len(right_instructions) else None
        note = classify_note(left_entry, right_entry)
        rows.append((insn_text(left_entry), insn_text(right_entry), note))
        i += 1
        j += 1

    return rows


def emit_markdown(rows, only_differences: bool, stream):
    print("| TARGET | BASE   | Note |", file=stream)
    print("|---|---|---|", file=stream)

    for left_text, right_text, note in rows:
        if only_differences and not note:
            continue

        left_cell = f"`{left_text}`" if left_text else "—"
        right_cell = f"`{right_text}`" if right_text else "—"

        print(f"| {left_cell} | {right_cell} | {note} |", file=stream)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("symbol")
    parser.add_argument("--repo-root", default=".")
    parser.add_argument("--objdiff", default="./tools/objdiff/objdiff")
    parser.add_argument("--only-differences", action="store_true")
    parser.add_argument("--show-match-info", action="store_true")
    parser.add_argument( "-o", "--output", help="Write markdown table to file instead of stdout")
    args = parser.parse_args()

    repo_root = find_repo_root(Path(args.repo_root))
    objdiff_path = (repo_root / args.objdiff).resolve()

    unit, base_path, target_path = find_symbol_unit(repo_root, args.symbol)

    if args.show_match_info:
        eprint(f"Matched symbol: {args.symbol}")
        eprint(f"Unit:          {unit['name']}")
        eprint(f"Base object:   {base_path}")
        eprint(f"Target object: {target_path}")

    data = run_objdiff_json(objdiff_path, target_path, base_path, args.symbol)
    left_sym, right_sym = find_symbol_pair(data, args.symbol)

    rows = align_rows(
        left_sym.get("instructions", []),
        right_sym.get("instructions", []),
    )

    out_stream = sys.stdout
    
    if args.output:
        out_stream = open(args.output, "w", encoding="utf-8")
    
    emit_markdown(rows, args.only_differences, out_stream)
    
    if args.output:
        out_stream.close()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

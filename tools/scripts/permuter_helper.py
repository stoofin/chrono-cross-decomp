#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import re
import stat
import subprocess
import sys
from pathlib import Path
from typing import Any


IGNORED_DIRS = {
    ".git",
    ".venv",
    "venv",
    "__pycache__",
    "node_modules",
    ".mypy_cache",
    ".pytest_cache",
    ".idea",
    ".vscode",
}


COMPILE_SH_TEMPLATE = """#!/usr/bin/env bash
set -euo pipefail

INPUT="$(realpath "$1")"
OUTPUT="$(realpath "$3")"

cd {repo_root}

TMPDIR="$(mktemp -d)"
trap 'rm -rf "$TMPDIR"' EXIT

I_FILE="$TMPDIR/cand.i"
S_FILE="$TMPDIR/cand.s"

# preprocess -> .i (text)
mips-linux-gnu-cpp \\
  -Iinclude -I build \\
  -D_LANGUAGE_C -DUSE_INCLUDE_ASM -DNON_MATCHING -DSKIP_ASM \\
  -undef -Wall -lang-c -nostdinc \\
  "$INPUT" -o "$I_FILE"

# cc1 -> .s (PSX gcc 2.8.1 frontend)
tools/gcc-2.8.1-psx/cc1 \\
  -O2 -G0 -mips1 -mcpu=3000 -g2 -w -funsigned-char -fpeephole \\
  -ffunction-cse -fpcc-struct-return -fcommon -fverbose-asm \\
  -msoft-float -mgas -fgnu-linker -quiet \\
  "$I_FILE" -o "$S_FILE"

# maspsx -> .o (real object)
python3 tools/maspsx/maspsx.py \\
  --aspsx-version=2.79 --expand-div --use-comm-section --run-assembler \\
  -EL -Iinclude -I build \\
  -O2 -G0 -march=r3000 -mtune=r3000 -no-pad-sections \\
  -o "$OUTPUT" "$S_FILE"
"""


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


def base_path_to_source_path(repo_root: Path, base_path: Path) -> Path:
    rel = base_path.relative_to(repo_root).as_posix()
    prefix = "build/src/"
    suffix = ".c.o"

    if not rel.startswith(prefix) or not rel.endswith(suffix):
        raise RuntimeError(f"Unexpected base_path shape: {rel}")

    src_rel = "src/" + rel[len(prefix):-len(".o")]
    return repo_root / src_rel


def unit_name_to_nonmatching_dir(repo_root: Path, unit_name: str) -> Path:
    parts = unit_name.split("/")
    if len(parts) < 2:
        raise RuntimeError(f"Unexpected unit name: {unit_name}")

    version = parts[0]
    rest = parts[1:]
    return repo_root / "asm" / version / "nonmatchings" / Path(*rest)


def find_asm_file(repo_root: Path, unit_name: str, symbol: str) -> Path:
    preferred_dir = unit_name_to_nonmatching_dir(repo_root, unit_name)
    preferred = preferred_dir / f"{symbol}.s"
    if preferred.is_file():
        return preferred

    candidates = sorted(repo_root.glob(f"asm/**/nonmatchings/**/{symbol}.s"))
    if not candidates:
        raise RuntimeError(f"Could not find asm file for symbol '{symbol}'")

    # Prefer one whose path contains the unit tail
    unit_tail = "/".join(unit_name.split("/")[1:])
    for c in candidates:
        if unit_tail in c.as_posix():
            return c

    if len(candidates) == 1:
        return candidates[0]

    raise RuntimeError(
        f"Multiple asm candidates for '{symbol}':\n" + "\n".join(str(c) for c in candidates[:20])
    )


def list_permuter_dirs(repo_root: Path, symbol: str) -> set[Path]:
    nm_root = repo_root / "nonmatchings"
    if not nm_root.is_dir():
        return set()

    out = set()
    pattern = re.compile(rf"^{re.escape(symbol)}(?:-\d+)?$")
    for p in nm_root.iterdir():
        if p.is_dir() and pattern.fullmatch(p.name):
            out.add(p.resolve())
    return out


def choose_created_dir(before: set[Path], after: set[Path], symbol: str) -> Path:
    new_dirs = sorted(after - before, key=lambda p: p.name)
    if len(new_dirs) == 1:
        return new_dirs[0]
    if len(new_dirs) > 1:
        return new_dirs[-1]

    # fallback: choose highest suffix
    candidates = sorted(after, key=permuter_dir_sort_key)
    if not candidates:
        raise RuntimeError(f"No nonmatchings directory found for '{symbol}' after import")
    return candidates[-1]


def permuter_dir_sort_key(path: Path):
    name = path.name
    m = re.match(r"^(.*?)(?:-(\d+))?$", name)
    if not m:
        return (name, -1)
    stem = m.group(1)
    num = int(m.group(2)) if m.group(2) else 0
    return (stem, num)


def write_compile_sh(repo_root: Path, dest_dir: Path) -> Path:
    path = dest_dir / "compile.sh"
    text = COMPILE_SH_TEMPLATE.format(repo_root=repo_root.resolve())
    path.write_text(text, encoding="utf-8")

    mode = path.stat().st_mode
    path.chmod(mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    return path


def run(cmd: list[str], cwd: Path) -> None:
    eprint("Running:")
    eprint("  " + " ".join(cmd))
    result = subprocess.run(cmd, cwd=str(cwd))
    if result.returncode != 0:
        raise RuntimeError(f"Command failed with exit code {result.returncode}")


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("symbol", help="Function symbol to import and permute")
    parser.add_argument("-j", default="8", help="Jobs to pass to permuter, e.g. -j8 or 8")
    parser.add_argument("--repo-root", default=".")
    parser.add_argument("--import-only", action="store_true", help="Stop after import + compile.sh generation")
    parser.add_argument("--show-match-info", action="store_true")
    return parser.parse_args()


def normalize_jobs_arg(j: str) -> str:
    return j if str(j).startswith("-j") else f"-j{j}"


def main() -> int:
    args = parse_args()
    repo_root = find_repo_root(Path(args.repo_root))

    try:
        unit, base_path, target_path = find_symbol_unit(repo_root, args.symbol)
        src_path = base_path_to_source_path(repo_root, base_path)
        asm_path = find_asm_file(repo_root, unit["name"], args.symbol)
    except RuntimeError as exc:
        eprint(f"Error: {exc}")
        return 1

    if args.show_match_info:
        eprint(f"Matched symbol: {args.symbol}")
        eprint(f"Unit:          {unit['name']}")
        eprint(f"Source:        {src_path}")
        eprint(f"ASM:           {asm_path}")

    before = list_permuter_dirs(repo_root, args.symbol)

    try:
        run(
            [
                sys.executable,
                "tools/permuter/import.py",
                str(src_path.relative_to(repo_root)),
                str(asm_path.relative_to(repo_root)),
            ],
            cwd=repo_root,
        )
    except RuntimeError as exc:
        eprint(f"Error: {exc}")
        return 1

    after = list_permuter_dirs(repo_root, args.symbol)

    try:
        perm_dir = choose_created_dir(before, after, args.symbol)
        compile_sh = write_compile_sh(repo_root, perm_dir)
    except RuntimeError as exc:
        eprint(f"Error: {exc}")
        return 1

    print(f"Permuter dir:  {perm_dir}")
    print(f"compile.sh:    {compile_sh}")

    if args.import_only:
        return 0

    jobs_arg = normalize_jobs_arg(args.j)

    try:
        run(
            [
                sys.executable,
                "tools/permuter/permuter.py",
                str(perm_dir.relative_to(repo_root)) + "/",
                jobs_arg,
            ],
            cwd=repo_root,
        )
    except RuntimeError as exc:
        eprint(f"Error: {exc}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

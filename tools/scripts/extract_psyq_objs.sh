#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<EOF
Usage: $(basename "$0") [--psyq-root DIR] [--psxlib PATH] [--parser PATH]

Defaults:
    psyq-root = assets/psyq
    psxlib    = tools/psxlib-objdumper/psxlib.py
    parser    = tools/pcsx-redux-tools/psyq-obj-parser

Behavior:
    - For each assets/psyq/<version>/lib/ directory:
        - Copies *.OBJ -> assets/psyq/<version>/obj/
        - Extracts *.LIB via psxlib.py -> assets/psyq/<version>/obj/<LIBNAME>/
EOF
}

PSYQ_ROOT="assets/psyq"
PSXLIB="tools/psxlib-objdumper/psxlib.py"
PARSER="tools/pcsx-redux-tools/psyq-obj-parser"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --psyq-root) PSYQ_ROOT="$2"; shift 2 ;;
        --psxlib)    PSXLIB="$2"; shift 2 ;;
        --parser)    PARSER="$2"; shift 2 ;;
        -h|--help)   usage; exit 0 ;;
        *) echo "ERROR: Unknown arg: $1" >&2; usage >&2; exit 2 ;;
    esac
done

# Repo root = two levels above tools/scripts/
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

psyq_root="$PSYQ_ROOT"
[[ "$psyq_root" = /* ]] || psyq_root="$ROOT/$psyq_root"

psxlib_path="$PSXLIB"
parser_path="$PARSER"
[[ "$psxlib_path" = /* ]] || psxlib_path="$ROOT/$psxlib_path"
[[ "$parser_path" = /* ]] || parser_path="$ROOT/$parser_path"

if [[ ! -d "$psyq_root" ]]; then
    echo "ERROR: psyq root not found: $psyq_root" >&2
    exit 1
fi
if [[ ! -f "$psxlib_path" ]]; then
    echo "ERROR: psxlib.py not found: $psxlib_path" >&2
    exit 1
fi
if [[ ! -x "$parser_path" ]]; then
    echo "ERROR: psyq-obj-parser not found or not executable: $parser_path" >&2
    echo "Hint: chmod +x \"$parser_path\"" >&2
    exit 1
fi

echo "[psyq] root  : $psyq_root"
echo "[psyq] psxlib: $psxlib_path"
echo "[psyq] parser: $parser_path"

total_versions=0
total_copied=0
total_extracted=0
total_skipped=0

# Find all lib dirs: assets/psyq/<version>/lib
while IFS= read -r -d '' lib_dir; do
    version_dir="$(cd "$lib_dir/.." && pwd)"       # .../<version>
    version_name="$(basename "$version_dir")"
    obj_dir="$version_dir/obj"

    echo
    echo "[psyq] === version: $version_name ==="
    echo "[psyq] lib dir: $lib_dir"
    echo "[psyq] obj dir: $obj_dir"

    mkdir -p "$obj_dir"

    copied=0
    extracted=0
    skipped=0

    # Process files in this lib dir (non-recursive)
    while IFS= read -r -d '' f; do
        base="$(basename "$f")"
        ext="${base##*.}"
        ext_lower="$(printf '%s' "$ext" | tr '[:upper:]' '[:lower:]')"

        case "$ext_lower" in
            obj)
                echo "[psyq][$version_name] copy OBJ: $base"
                cp -a "$f" "$obj_dir/$base"
                copied=$((copied+1))
                ;;
            lib)
                echo "[psyq][$version_name] extract LIB: $base"
                python3 "$psxlib_path" -o "$obj_dir" --parser "$parser_path" "$f"
                extracted=$((extracted+1))
                ;;
            *)
                echo "[psyq][$version_name] skip: $base"
                skipped=$((skipped+1))
                ;;
        esac
    done < <(find "$lib_dir" -maxdepth 1 -type f -print0)

    echo "[psyq][$version_name] done. copied OBJ=$copied extracted LIB=$extracted skipped=$skipped"

    total_versions=$((total_versions+1))
    total_copied=$((total_copied+copied))
    total_extracted=$((total_extracted+extracted))
    total_skipped=$((total_skipped+skipped))

done < <(find "$psyq_root" -mindepth 2 -maxdepth 2 -type d -name lib -print0)

echo
echo "[psyq] ALL DONE. versions=$total_versions copied OBJ=$total_copied extracted LIB=$total_extracted skipped=$total_skipped"

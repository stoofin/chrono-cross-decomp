#!/bin/bash

# Inspired by https://github.com/Vatuu/silent-hill-decomp/tree/master

# prebuild.sh [target_name]

# Splat `lib` segments only support loading .o from inside .a file
# Loading .a has issues with dependency checks though
# For now, rewrite the ld linker scripts from splat to load loose .o instead
#
set -euo pipefail

ldfile="$1"
tmp="${ldfile}.temp"

# Convert:
#   lib/libspu.a:s_i.o(.text)
# into:
#   lib/libspu/s_i.o(.text)
#
# (Same spirit as SH : ".a:" -> "/")
sed -E 's/\.a:([[:alnum:]_./-]+\.o)/\/\1/g' "$ldfile" > "$tmp"
mv "$tmp" "$ldfile"

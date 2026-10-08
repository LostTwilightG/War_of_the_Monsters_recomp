#!/bin/sh
# Compile one C/C++ file the way the retail game was built, producing an object for GNU ld.
#   tools/cc.sh <in.c|in.cpp> <out.o> [extra cflags...]
# ee-gcc 2.95.2 (SN ProDG v2.73a) -> .s -> tools/snfix.py (SN macro expansions) -> mips-linux-gnu-as
set -e
in=$1; out=$2; shift 2
root=$(cd "$(dirname "$0")/.." && pwd)
cc=${WOTM_CC:-$root/tools/cc/ee-gcc2.95.2-SN-v2.73a/cc}
py=${PYTHON:-python3}
tmp=$out.gcc.s
WINEDEBUG=-all wine "$cc/bin/ee-gcc.exe" -B"$cc/lib/gcc-lib/ee/2.95.2/" -S -O2 -G8 \
    -I"$root/include" $WOTM_EXTRA_CFLAGS "$@" "$in" -o "$tmp"
$py "$root/tools/snfix.py" "$tmp" "$out.s"
mips-linux-gnu-as -EL -march=r5900 -mabi=o64 -G8 -no-pad-sections -I"$root/include" -o "$out" "$out.s"

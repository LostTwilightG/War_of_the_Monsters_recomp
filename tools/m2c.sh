#!/bin/sh
# Draft C for one INCLUDE_ASM'd function with m2c: tools/m2c.sh <tu> <function> [m2c args...]
# e.g. tools/m2c.sh common/zip zipFlush__FUl
root=$(cd "$(dirname "$0")/.." && pwd)
tu=$1; fn=$2; shift 2
f="$root/asm/nonmatchings/$tu/$fn.s"
[ -f "$f" ] || { echo "no such file: $f" >&2; exit 1; }
"${PYTHON:-$HOME/.venvs/wotm/bin/python}" "${M2C:-$HOME/tools/m2c/m2c.py}" -t mips-gcc-c "$@" "$f"

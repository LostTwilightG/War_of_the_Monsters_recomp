#!/bin/sh
# Usage: flagsweep.sh <src> <compiler...>   (tries a set of common flag combos)
src=$1; shift
for fl in "-O2 -G0" "-O1 -G0" "-O3 -G0" "-O2 -G0 -fno-schedule-insns" "-O2 -G0 -fno-schedule-insns2" "-O2 -G0 -fno-strength-reduce" "-O2 -G0 -funroll-loops"; do
    echo "## $fl"
    ~/.venvs/wotm/bin/python tools/ccmatch.py "$src" "$fl" "$@"
done

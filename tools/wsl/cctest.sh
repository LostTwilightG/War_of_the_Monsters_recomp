#!/bin/sh
# Usage: cctest.sh <file.cpp> "<flags>" [compiler...]  -> compiles with each ee-gcc and prints .s
src=$1; flags=$2; shift 2
for c in ${@:-$(ls ~/compilers)}; do
    d=~/compilers/$c
    if [ -x "$d/bin/ee-gcc" ]; then cc="$d/bin/ee-gcc"; else cc="wine $d/bin/ee-gcc.exe"; fi
    out=/tmp/cctest_$c.s
    echo "=== $c"
    (cd "$(dirname "$src")" && COMPILER_PATH=$d/bin $cc -S $flags "$(basename "$src")" -o "$out") 2>&1 | head -5
done

#!/bin/sh
# Usage: variants.sh <dir> "<flags>" <compiler>  -> scores every *.cpp/*.c in dir
dir=$1; fl=$2; cc=$3
for f in "$dir"/*.c*; do printf '%-14s ' "$(basename "$f")"; ~/.venvs/wotm/bin/python tools/ccmatch.py "$f" "$fl" "$cc" | sed 's/^[^ ]* *//'; done

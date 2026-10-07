#!/bin/sh
# Score every function of the given game TUs against retail with NON_MATCHING enabled (so equivalent code is compared too).
# Usage: sh tools/wsl/scoreall.sh StickShaker HeliVehicle ...   -> one "TU: func: result" line per function, sorted.
cd "$(dirname "$0")/../.."
py=${PYTHON:-$HOME/.venvs/wotm/bin/python}
for t in "$@"; do
    $py tools/ccmatch.py "src/game/$t.cpp" '-DNON_MATCHING' project 2>&1 | tail -1 | sed 's/^project *//' | tr ';' '\n' | sed "s/^ *//; s/^/$t: /"
done | sort

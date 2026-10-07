#!/bin/sh
# Convert game TUs to cpp stubs one at a time; keep each only if the ROM still matches. No git here (commit from Windows).
# Usage: sh tools/wsl/convert_tus.sh TuA TuB ...
cd "$(dirname "$0")/../.."
py=${PYTHON:-$HOME/.venvs/wotm/bin/python}
for t in "$@"; do
    cp SCUS_971.97.yaml "$HOME/.conv_yaml.bak"
    cp config/decomp_tus.txt "$HOME/.conv_tus.bak"
    $py tools/new_tu.py "game/$t" > /dev/null 2>&1
    ok=0
    if sh tools/wsl/gate.sh > /dev/null 2>&1; then
        ok=1
    else
        # data-only stubs (vtables) must sit at their retail rodata position
        for sym in $(grep -o '_vt\$[A-Za-z0-9_]*' "src/game/$t.cpp" | sort -u); do
            $py tools/place_data.py "game/$t" "$sym" > /dev/null 2>&1
        done
        if sh tools/wsl/gate.sh > /dev/null 2>&1; then ok=1; fi
    fi
    if [ $ok = 1 ]; then
        echo "ok $t"
    else
        echo "FAIL $t"
        rm -f "src/game/$t.cpp"
        cp "$HOME/.conv_yaml.bak" SCUS_971.97.yaml
        cp "$HOME/.conv_tus.bak" config/decomp_tus.txt
        $py configure.py --split > /dev/null 2>&1
    fi
done

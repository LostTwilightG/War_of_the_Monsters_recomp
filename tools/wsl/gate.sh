#!/bin/sh
# Build + sha1 check; exits non-zero unless the ROM matches. Use as: sh tools/wsl/gate.sh && git commit ...
log=${TMPDIR:-$HOME}/.wotm_gate.log
sh "$(dirname "$0")/check.sh" > "$log" 2>&1
if tail -1 "$log" | grep -q "ROM OK"; then
    echo "ROM OK"
else
    grep -n 'rror\|undefined\|FAILED' "$log" | head -10
    exit 1
fi

~/.venvs/wotm/bin/python -m splat split SCUS_971.97.yaml >/tmp/splat.log 2>&1
for f in asm/nonmatchings/game/CrushLevel/*.s; do n=$(basename "$f" .s); echo "// ===== $n"; sh tools/m2c.sh game/CrushLevel "$n" 2>&1; done

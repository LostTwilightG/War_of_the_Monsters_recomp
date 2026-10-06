for f in asm/nonmatchings/common/zip/*.s; do n=$(basename "$f" .s); echo "// ===== $n"; sh tools/m2c.sh common/zip "$n" 2>&1; done

for f in tools/cctest/sdata/*.cpp; do echo "== $f"; sh tools/cc.sh $f /tmp/sd.o 2>&1 | head -3; mips-linux-gnu-nm -n /tmp/sd.o | grep -iE " [dsgb] " ; done

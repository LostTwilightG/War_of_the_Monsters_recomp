for f in tools/cctest/gp/*.cpp; do
  sh tools/cc.sh $f /tmp/gp.o >/dev/null 2>&1 || { echo "$f: compile error"; continue; }
  printf '%-40s ' $f; mips-linux-gnu-objdump -d -r /tmp/gp.o | grep -oE 'R_MIPS_(GPREL16|HI16)' | head -1
  mips-linux-gnu-objdump -h /tmp/gp.o | grep -E 'sdata|sbss|\.bss|\.data' | awk '{printf "   %s %s\n",$2,$3}' | grep -v ' 00000000'
done

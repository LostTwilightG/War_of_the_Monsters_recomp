D=~/compilers/ee-gcc2.95.2-SN-v2.73a/cc
cd ~/.cache/ccmatch/src
export WINEDEBUG=-all
wine $D/bin/ee-gcc.exe -B$D/lib/gcc-lib/ee/2.95.2/ -S -O2 -G0 localptr2.cpp -o lp2.s
for abi in eabi o64; do
mips-linux-gnu-as -EL -march=r5900 -mabi=$abi -G0 -o modern_$abi.o lp2.s 2>&1 | head -3
echo "== $abi"; mips-linux-gnu-objdump -d -M no-aliases modern_$abi.o | sed -n 12,17p
done

D=~/compilers/ee-gcc2.95.2-SN-v2.73a/cc
cd ~/.cache/ccmatch/src
export WINEDEBUG=-all
wine $D/bin/ee-gcc.exe -B$D/lib/gcc-lib/ee/2.95.2/ -S -O2 -G0 crc32.cpp -o crc32.s
wine $D/ee/bin/Ps2EeAs.exe 2>&1 | head -15
echo ---
wine $D/ee/bin/Ps2EeAs.exe -o crc32_sn.o crc32.s 2>&1 | head; ls -la crc32_sn.o 2>&1
mips-linux-gnu-objdump -d -M no-aliases crc32_sn.o | sed -n 1,30p

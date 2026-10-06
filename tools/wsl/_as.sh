D=~/compilers/ee-gcc2.95.2-SN-v2.73a/cc
cd ~/.cache/ccmatch/src && cp /mnt/c/Users/TwistZero/WotM/tools/cctest/crc32_variants/localptr2.cpp . 
export WINEDEBUG=-all
wine $D/ee/bin/as.exe --version 2>&1 | head -2
wine $D/bin/ee-gcc.exe -B$D/lib/gcc-lib/ee/2.95.2/ -B$D/ee/bin/ -c -O2 -G0 localptr2.cpp -o gnuas.o -v 2>&1 | grep -iE " as|as.exe" | head -3
mips-linux-gnu-objdump -d -M no-aliases gnuas.o | sed -n 12,16p

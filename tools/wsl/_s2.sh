D=tools/cc/ee-gcc2.95.2-SN-v2.73a/cc
WINEDEBUG=-all wine $D/bin/ee-gcc.exe -B$D/lib/gcc-lib/ee/2.95.2/ -S -O2 -G8 tools/cctest/gp/h_extern_mix.cpp -o - | grep -vE "^\s*\.(set|frame|mask|fmask)"

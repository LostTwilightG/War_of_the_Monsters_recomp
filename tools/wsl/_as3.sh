D=~/compilers/ee-gcc2.95.2-SN-v2.73a/cc
W=~/.cache/ccmatch/astest; mkdir -p $W; cd $W
cp /mnt/c/Users/TwistZero/WotM/asm/common/zip.s /mnt/c/Users/TwistZero/WotM/include/macro.inc /mnt/c/Users/TwistZero/WotM/include/labels.inc .
export WINEDEBUG=-all
wine $D/ee/bin/Ps2EeAs.exe -o zip.o zip.s 2>&1 | head -15
ls -la zip.o 2>&1

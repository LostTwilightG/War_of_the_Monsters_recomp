~/.venvs/wotm/bin/python tools/ccmatch.py tools/cctest/crc32_variants/localptr.cpp "-O2 -G0" ee-gcc2.95.2-SN-v2.73a >/dev/null
sh tools/wsl/objdiff.sh zipCrc32__FUlPCUcl ~/.cache/ccmatch/ee-gcc2.95.2-SN-v2.73a.o | grep -nE '[|<>]'

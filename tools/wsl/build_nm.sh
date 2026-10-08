#!/bin/sh
# Build an ELF with every NON_MATCHING (equivalent) function compiled in, in a scratch copy (~/wotm_nm) so the real build/ stays
# byte-identical to retail. Output: build/pcsx2_test/SCUS_971.97_halfcpp.elf (+ the matching build as a control), program headers
# patched so p_paddr == p_vaddr (PCSX2 loads by p_paddr).
#   run in WSL:  sh tools/wsl/build_nm.sh
#   PCSX2:       pcsx2-qt.exe -elf <that elf> -- "<iso>"
set -e
src=$(cd "$(dirname "$0")/../.." && pwd)
dst=$HOME/wotm_nm
mkdir -p "$dst"
(cd "$src" && tar cf - --exclude=./ISO --exclude=./.git --exclude='./disc/[0-9A-Z]*' --exclude=./build .) | (cd "$dst" && tar xf -)
cd "$dst"
# NM code needs its own .sdata for CrushLevel (its data comes from the splat asm object); keep it
sed -i 's|^\(\s*\)build/asm/data/game/CrushLevel.sdata.o(.sdata\*);|&\n\1build/src/game/CrushLevel.o(.sdata*);|' SCUS_971.97.ld
export WOTM_EXTRA_CFLAGS=-DNON_MATCHING
export PATH=$HOME/.venvs/wotm/bin:$PATH
ninja build/SCUS_971.97.elf -j8 2>&1 | grep -v '^\[' | grep -v warning | head -30
mkdir -p "$src/build/pcsx2_test"
python3 - "$dst/build/SCUS_971.97.elf" "$src/build/SCUS_971.97.elf" "$src/build/pcsx2_test" <<'PYEOF'
import struct, sys
def patch(srcf, dstf):
    d = bytearray(open(srcf, 'rb').read())
    phoff, = struct.unpack_from('<I', d, 0x1C)
    phentsize, phnum = struct.unpack_from('<HH', d, 0x2A)
    for i in range(phnum):
        o = phoff + i * phentsize
        t, off, va, pa, fs, ms, fl, al = struct.unpack_from('<8I', d, o)
        if t == 1:
            struct.pack_into('<I', d, o + 12, va)
    open(dstf, 'wb').write(d)
patch(sys.argv[1], sys.argv[3] + '/SCUS_971.97_halfcpp.elf')
patch(sys.argv[2], sys.argv[3] + '/SCUS_971.97_control_matching.elf')
print('written to', sys.argv[3])
PYEOF

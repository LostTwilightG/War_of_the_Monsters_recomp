#!/bin/sh
# Build an ELF with every NON_MATCHING (equivalent) function compiled in, in a scratch copy (~/wotm_nm) so the real build/ stays
# byte-identical to retail. Everything that did not change stays at its retail address (tools/gen_nm_ld.py); pieces that grew are
# moved behind the image. Output (program headers patched so p_paddr == p_vaddr, PCSX2 loads by p_paddr):
#   build/pcsx2_test/SCUS_971.97_halfcpp.elf           equivalents compiled in
#   build/pcsx2_test/SCUS_971.97_control_matching.elf  the normal (retail-identical) build, as a control
# Run in WSL:  sh tools/wsl/build_nm.sh      Run in PCSX2:  pcsx2-qt.exe -elf <elf> -- "<iso>"
set -e
src=$(cd "$(dirname "$0")/../.." && pwd)
dst=$HOME/wotm_nm
mkdir -p "$dst"
(cd "$src" && tar cf - --exclude=./ISO --exclude=./.git --exclude='./disc/[0-9A-Z]*' --exclude=./build .) | (cd "$dst" && tar xf -)
cd "$dst"
export PATH=$HOME/.venvs/wotm/bin:$PATH
# CrushLevel's NM code brings its own .sdata strings; -G0 keeps them out of the gp-relative area (which has no free room)
echo 'game/CrushLevel -G0' >> config/tu_flags.txt
python configure.py > /dev/null
WOTM_EXTRA_CFLAGS=-DNON_MATCHING ninja build/SCUS_971.97.elf -j8 2>&1 | grep -v '^\[' | grep -v warning | head -30 || true
python3 tools/gen_nm_ld.py "$src" "$dst"
mips-linux-gnu-ld -EL --no-check-sections -T SCUS_971.97.nm.ld -T undefined_syms_auto.txt -T undefined_funcs_auto.txt \
    -T linker_script_extra.ld -Map nm.map -o build/SCUS_971.97.nm.elf 2>&1 | grep -v RWX || true
mkdir -p "$src/build/pcsx2_test"
python3 - "$dst/build/SCUS_971.97.nm.elf" "$src/build/SCUS_971.97.elf" "$src/build/pcsx2_test" <<'PYEOF'
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

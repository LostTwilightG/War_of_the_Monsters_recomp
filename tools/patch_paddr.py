#!/usr/bin/env python3
"""Copy an ELF with every PT_LOAD's p_paddr set to p_vaddr (PCSX2 loads segments by p_paddr; the splat linker script leaves it 0).

    patch_paddr.py in.elf out.elf [--heap 0xADDR]

--heap moves the start of the game heap past the NM build's extra code. The extra pieces (.nm_extra, pieces that grew) sit behind the bss,
where the retail heap starts, and the whole 32 MB gets used once a stage is loaded, so there is nowhere else to put them. Two places hold the
retail end of bss (0x880F10) and are rewritten: the SetupHeap(...) call in crt0 (lui/addiu pair at 0x10006C/0x100074) and the word in
libkernl's glue data (0x6E5D2C, the `_end` the SDK's malloc/sbrk start from). The bss-clearing loop keeps the old end (it must not wipe the extra code).
"""
import struct
import sys

d = bytearray(open(sys.argv[1], 'rb').read())
phoff, = struct.unpack_from('<I', d, 0x1C)
phentsize, phnum = struct.unpack_from('<HH', d, 0x2A)
for i in range(phnum):
    o = phoff + i * phentsize
    t, off, va, pa, fs, ms, fl, al = struct.unpack_from('<8I', d, o)
    if t == 1:
        struct.pack_into('<I', d, o + 12, va)


def file_off(va):
    for i in range(phnum):
        o = phoff + i * phentsize
        t, off, v, pa, fs, ms, fl, al = struct.unpack_from('<8I', d, o)
        if t == 1 and v <= va < v + fs:
            return off + va - v
    raise SystemExit('address %#x not in a loaded segment' % va)


if '--heap' in sys.argv:
    heap = (int(sys.argv[sys.argv.index('--heap') + 1], 0) + 0x3F) & ~0x3F
    lui, addiu, glue = file_off(0x10006C), file_off(0x100074), file_off(0x6E5D2C)
    assert struct.unpack_from('<I', d, lui)[0] == 0x3C040088 and struct.unpack_from('<I', d, addiu)[0] == 0x24840F10, 'crt0 SetupHeap args not as expected'
    assert struct.unpack_from('<I', d, glue)[0] == 0x880F10, 'libkernl _end word not as expected'
    struct.pack_into('<I', d, lui, 0x3C040000 | (((heap + 0x8000) >> 16) & 0xFFFF))
    struct.pack_into('<I', d, addiu, 0x24840000 | (heap & 0xFFFF))
    struct.pack_into('<I', d, glue, heap)
    print('heap starts at %#x' % heap)
open(sys.argv[2], 'wb').write(d)
print('written', sys.argv[2])

#!/usr/bin/env python3
"""Copy an ELF with every PT_LOAD's p_paddr set to p_vaddr (PCSX2 loads segments by p_paddr; the splat linker script leaves it 0).

    patch_paddr.py in.elf out.elf
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
open(sys.argv[2], 'wb').write(d)
print('written', sys.argv[2])

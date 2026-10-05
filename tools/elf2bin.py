"""Minimal stand-in for `objcopy -O binary --gap-fill=0x00 <elf> <out>` (PT_LOAD file contents)."""
import sys
from elftools.elf.elffile import ELFFile

args = [a for a in sys.argv[1:] if not a.startswith('-') and a != 'binary']
if '--version' in sys.argv:
    print('elf2bin (objcopy stand-in)'); sys.exit(0)
src, dst = args
with open(src, 'rb') as f:
    elf = ELFFile(f)
    loads = [p for p in elf.iter_segments() if p['p_type'] == 'PT_LOAD' and p['p_filesz']]
    base = min(p['p_paddr'] for p in loads)
    end = max(p['p_paddr'] + p['p_filesz'] for p in loads)
    out = bytearray(end - base)
    for p in loads:
        out[p['p_paddr'] - base:p['p_paddr'] - base + p['p_filesz']] = p.data()
open(dst, 'wb').write(out)

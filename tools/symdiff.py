"""List symbols whose address differs between the retail ELF and the rebuilt ELF (first N, by address)."""
import sys
from elftools.elf.elffile import ELFFile

def syms(path):
    with open(path, 'rb') as f:
        return {s.name: s['st_value'] for s in ELFFile(f).get_section_by_name('.symtab').iter_symbols()
                if s['st_info']['type'] in ('STT_FUNC', 'STT_OBJECT') and s.name}

old, new = syms('disc/SCUS_971.97'), syms(sys.argv[1] if len(sys.argv) > 1 else 'build/SCUS_971.97.elf')
bad = sorted((old[n], n, new[n]) for n in old.keys() & new.keys() if old[n] != new[n])
print(f'{len(bad)} symbols moved')
for a, n, b in bad[:int(sys.argv[2]) if len(sys.argv) > 2 else 10]:
    print(f'  {n}: {a:08X} -> {b:08X} ({b - a:+#x})')

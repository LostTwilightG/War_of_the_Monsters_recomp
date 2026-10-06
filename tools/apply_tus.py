"""Rewrite the .text subsegments in the splat yaml from config/tus.csv (one asm file per TU)."""
import csv, re

YAML = 'SCUS_971.97.yaml'
TEXT_VRAM = 0x100000
BEGIN, END = '      # BEGIN text TUs (tools/apply_tus.py)', '      # END text TUs'

rows = list(csv.DictReader(open('config/tus.csv')))
# TUs being decompiled: config/decomp_tus.txt lists one TU name per line (splat type c or cpp by language)
# A trailing "+data" means every data section of the TU also comes from the C file.
decomp, decomp_data = set(), set()
try:
    for l in open('config/decomp_tus.txt'):
        parts = l.split('#')[0].split()
        if parts:
            decomp.add(parts[0])
            if '+data' in parts[1:]:
                decomp_data.add(parts[0])
except FileNotFoundError:
    pass
lines = [BEGIN]
for r in rows:
    start = int(r['start'], 16) - TEXT_VRAM
    seg = ('cpp' if r['lang'] == 'c++' else 'c') if r['name'] in decomp else 'asm'
    lines.append(f"      - [0x{start:06X}, {seg}, {r['name']}]  # {r['lang']}, {r['nfuncs']} funcs, name: {r['source']}")
lines.append(END)
block = '\n'.join(lines)

text = open(YAML).read()
if BEGIN in text:
    text = re.sub(re.escape(BEGIN) + r'.*?' + re.escape(END), lambda m: block, text, flags=re.S)
else:
    text = re.sub(r'^      - \[0x000000, asm, cod/000000\] # \.text$', lambda m: block, text, flags=re.M)
open(YAML, 'w', newline='\n').write(text)
print(f'{len(rows)} text subsegments written to {YAML}')

# ---- data sections: per-TU subsegments from config/data_tus.csv (tools/find_data_tus.py) ----
DBEGIN, DEND = '      # BEGIN data TUs (tools/apply_tus.py)', '      # END data TUs'
SPLAT_TYPE = {'.data': 'data', '.rodata': 'rodata', '.gcc_except_table': 'gcc_except_table',
              '.sdata': 'sdata', '.sbss': 'sbss', '.bss': 'bss'}
drows = list(csv.DictReader(open('config/data_tus.csv')))
dlines = [DBEGIN]
for r in drows:
    vram, typ = int(r['start'], 16), SPLAT_TYPE[r['section']]
    if (typ == 'rodata' and r['tu'] in decomp) or r['tu'] in decomp_data:
        typ = '.' + typ  # comes from the C object (rodata used by one INCLUDE_ASM'd function migrates into it)
    if typ in ('sbss', 'bss'):
        dlines.append(f"      - {{ type: {typ}, vram: 0x{vram:08X}, name: {r['tu']} }}")
    else:
        dlines.append(f"      - [0x{vram - TEXT_VRAM:06X}, {typ}, {r['tu']}]")
dlines.append(DEND)
dblock = '\n'.join(dlines)
text = open(YAML).read()
if DBEGIN in text:
    text = re.sub(re.escape(DBEGIN) + r'.*?' + re.escape(DEND), lambda m: dblock, text, flags=re.S)
else:
    text = re.sub(r'^      - \[0x15C480, data, cod/15C480\].*?\n(?=  - \[0x5F8B44\])',
                  lambda m: dblock + '\n', text, flags=re.M | re.S)
open(YAML, 'w', newline='\n').write(text)
print(f'{len(drows)} data subsegments written to {YAML}')

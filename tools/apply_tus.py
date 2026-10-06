"""Rewrite the .text subsegments in the splat yaml from config/tus.csv (one asm file per TU)."""
import csv, re

YAML = 'SCUS_971.97.yaml'
TEXT_VRAM = 0x100000
BEGIN, END = '      # BEGIN text TUs (tools/apply_tus.py)', '      # END text TUs'

rows = list(csv.DictReader(open('config/tus.csv')))
lines = [BEGIN]
for r in rows:
    start = int(r['start'], 16) - TEXT_VRAM
    lines.append(f"      - [0x{start:06X}, asm, {r['name']}]  # {r['lang']}, {r['nfuncs']} funcs, name: {r['source']}")
lines.append(END)
block = '\n'.join(lines)

text = open(YAML).read()
if BEGIN in text:
    text = re.sub(re.escape(BEGIN) + r'.*?' + re.escape(END), lambda m: block, text, flags=re.S)
else:
    text = re.sub(r'^      - \[0x000000, asm, cod/000000\] # \.text$', lambda m: block, text, flags=re.M)
open(YAML, 'w', newline='\n').write(text)
print(f'{len(rows)} text subsegments written to {YAML}')

#!/usr/bin/env python3
"""Find where a data-only INCLUDE_ASM (e.g. a vtable) belongs inside a TU stub by trying every position until the ROM
matches. Run inside WSL.   python3 tools/place_data.py common/PointToolKit '_vt$12PointToolKit'
The order of INCLUDE_ASMs sets the order of that TU's .rodata, which is why the position matters.
"""
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
tu, sym = sys.argv[1:3]
path = ROOT / f'src/{tu}.cpp'
orig = path.read_text()
line = f'INCLUDE_ASM("asm/nonmatchings/{tu}", {sym});\n'
base = orig.replace(line, '')
asm_lines = [m for m in re.finditer(r'INCLUDE_ASM\([^\n]*\n', base)]
spots = [0] + [m.end() for m in asm_lines]
spots = sorted({0} | {m.end() for m in asm_lines})
for pos in spots:
    # insert after the include line / at the file start, only at statement boundaries
    text = base[:pos] + line + base[pos:] if pos else base.replace('\n\n', '\n\n' + line, 1)
    path.write_text(text)
    r = subprocess.run(['sh', 'tools/wsl/check.sh'], cwd=ROOT, capture_output=True, text=True)
    print(pos, r.stdout.strip().splitlines()[-1] if r.stdout.strip() else r.stderr[-200:], flush=True)
    if 'ROM OK' in r.stdout:
        print('placed')
        sys.exit(0)
path.write_text(orig)
sys.exit('no position matched')

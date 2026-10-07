#!/usr/bin/env python3
"""Find where a data-only INCLUDE_ASM (e.g. a vtable) belongs inside a TU stub by trying every position until the symbol's
address equals the retail one (so other TUs with the same problem do not get in the way). Run inside WSL.   python3 tools/place_data.py common/PointToolKit '_vt$12PointToolKit'
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


def addr(elf):
    out = subprocess.run(['mips-linux-gnu-nm', str(ROOT / elf)], capture_output=True, text=True).stdout
    for ln in out.splitlines():
        p = ln.split()
        if len(p) == 3 and p[2] == sym:
            return p[0]
    return None


want = addr('disc/SCUS_971.97')
line = f'INCLUDE_ASM("asm/nonmatchings/{tu}", {sym});\n'
base = orig.replace(line, '')
asm_lines = [m for m in re.finditer(r'INCLUDE_ASM\([^\n]*\n', base)]
spots = [0] + [m.end() for m in asm_lines]
spots = sorted({0} | {m.end() for m in asm_lines})

def build_with(pos):
    text = base[:pos] + line + base[pos:] if pos else base.replace('\n\n', '\n\n' + line, 1)
    path.write_text(text)
    subprocess.run(['ninja', 'build/SCUS_971.97.elf'], cwd=ROOT, capture_output=True, text=True)
    return addr('build/SCUS_971.97.elf')

# The symbol address grows with the number of rodata-bearing stubs placed before it, so binary-search the first
# position whose address reaches the retail one (a linear scan needs one build per position).
lo, hi = 0, len(spots) - 1
found = None
while lo <= hi:
    mid = (lo + hi) // 2
    mine = build_with(spots[mid])
    print(spots[mid], mine, 'retail', want, flush=True)
    if mine is None:
        break
    if mine == want:
        found = mid
        hi = mid - 1
    elif mine < want:
        lo = mid + 1
    else:
        hi = mid - 1
if found is not None:
    build_with(spots[found])
    print('placed')
    sys.exit(0)
path.write_text(orig)
sys.exit('no position matched')

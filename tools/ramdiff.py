#!/usr/bin/env python3
"""Compare two EE RAM dumps (32 MB, e.g. from the retail-identical build and from the half-C++ build at the same point of a stage)
and report which global symbols differ. Differences due to timing (frame counters, positions) are expected; a data structure that
is built at load time (pickup lists, collision grid, path nets) and differs points at the function that builds it.

    python3 tools/ramdiff.py a.bin b.bin [retail.elf] [--from 0x24F000] [--to 0x880F10] [--heap]
"""
import bisect
import struct
import subprocess
import sys
from pathlib import Path

root = Path(__file__).resolve().parent.parent
args = [a for a in sys.argv[1:] if not a.startswith('--')]
opt = {a: sys.argv[sys.argv.index(a) + 1] for a in sys.argv if a in ('--from', '--to')}
a = Path(args[0]).read_bytes()
b = Path(args[1]).read_bytes()
elf = Path(args[2]) if len(args) > 2 else root / 'build/SCUS_971.97.elf'
lo = int(opt.get('--from', '0x24F000'), 0)
hi = int(opt.get('--to', '0x880F10'), 0)

out = subprocess.run(['mips-linux-gnu-nm', '-n', '-S', str(elf)], capture_output=True, text=True).stdout
syms = []
for line in out.splitlines():
    p = line.split()
    if len(p) == 4 and not p[3].endswith('.NON_MATCHING'):
        syms.append((int(p[0], 16), int(p[1], 16), p[3]))
    elif len(p) == 3 and not p[2].endswith('.NON_MATCHING'):
        syms.append((int(p[0], 16), 0, p[2]))
syms.sort()
keys = [s[0] for s in syms]


def sym_of(addr):
    i = bisect.bisect_right(keys, addr) - 1
    return (syms[i][2], addr - syms[i][0]) if i >= 0 else ('?', addr)


diff = {}
for off in range(lo, hi, 4):
    if a[off:off + 4] != b[off:off + 4]:
        name, rel = sym_of(off)
        d = diff.setdefault(name, [0, off, struct.unpack_from('<I', a, off)[0], struct.unpack_from('<I', b, off)[0], rel])
        d[0] += 1
rows = sorted(diff.items(), key=lambda kv: -kv[1][0])
print(f'{len(rows)} symbols differ in {lo:#x}..{hi:#x}')
for name, (n, off, va, vb, rel) in rows[:int(opt.get('--top', 80))]:
    print(f'{n:6d} words  {name:48s} first at {off:#x} (+{rel:#x}): {va:#010x} vs {vb:#010x}')

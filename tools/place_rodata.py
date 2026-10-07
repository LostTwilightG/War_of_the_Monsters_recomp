#!/usr/bin/env python3
"""Reorder the INCLUDE_ASM stubs of a freshly converted TU so every data-only stub (strings, tables, vtables) sits where the
retail .rodata puts it, without one build per symbol (place_data.py needs several).

    python3 tools/place_rodata.py game/Monster

Each stub's .s file lists its rodata labels with their retail addresses in the name (dlabel D_006EBF30 / jtbl_006EBF70).
Functions keep their order (that is the text order); data-only stubs are slotted before the first function whose rodata
starts above them. Run `sh tools/wsl/gate.sh` afterwards; place_data.py can still fix leftovers one by one.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
tu = sys.argv[1]
cpp = ROOT / f'src/{tu}.cpp'
text = cpp.read_text()
lines = text.splitlines(keepends=True)
INC = re.compile(r'INCLUDE_ASM\("asm/nonmatchings/([^"]+)", (\S+)\);')

head = []          # lines before the first stub (includes, pragmas, ...)
stubs = []         # (line, symbol, is_func, rodata_addrs)
other = []         # non-stub lines after the first stub are kept in place relative to functions (not expected)
for ln in lines:
    m = INC.search(ln)
    if not m:
        if stubs:
            other.append(ln)
        else:
            head.append(ln)
        continue
    s = ROOT / 'asm/nonmatchings' / m.group(1) / f'{m.group(2)}.s'
    body = s.read_text(errors='replace')
    is_func = bool(re.search(r'^glabel ', body, re.M))
    addrs = []
    in_ro = False
    for b in body.splitlines():
        if b.startswith('.section'):
            in_ro = '.rodata' in b
        if in_ro:
            d = re.match(r'dlabel \S*?_([0-9A-Fa-f]{8})$', b)
            if d:
                addrs.append(int(d.group(1), 16))
            c = re.match(r'\s*/\* [0-9A-Fa-f]+ ([0-9A-Fa-f]{8}) ', b)  # per-line "file offset, vram" comments
            if c:
                addrs.append(int(c.group(1), 16))
    stubs.append((ln, m.group(2), is_func, addrs))
if other:
    sys.exit('non-stub lines between stubs; refusing to reorder')

funcs = [s for s in stubs if s[2]]
data = sorted((s for s in stubs if not s[2]), key=lambda s: min(s[3]) if s[3] else 1 << 60)
out = []
di = 0
for f in funcs:
    lo = min(f[3]) if f[3] else None
    if lo is not None:
        while di < len(data) and data[di][3] and min(data[di][3]) < lo:
            out.append(data[di][0])
            di += 1
    out.append(f[0])
out.extend(d[0] for d in data[di:])
cpp.write_text(''.join(head) + ''.join(out))
print(f'{len(funcs)} functions, {len(data)} data-only stubs reordered')

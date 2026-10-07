#!/usr/bin/env python3
"""Make the return types of Monster:: definitions in src/game/Monster.cpp follow the declarations in include/game/game.h."""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
h = (ROOT / 'include/game/game.h').read_text()
a = h.index('class Monster {')
b = h.index('typedef char _size_Monster', a)
decl = {}
for ln in h[a:b].splitlines():
    m = re.match(r'\s+([\w ]+?\s*\**)\s*(\w+)\(([^)]*)\)( const)?;', ln)
    if m and m.group(1).strip() not in ('void', 'enum', 'class'):
        decl[m.group(2)] = m.group(1).strip() if not m.group(1).strip().endswith('*') else m.group(1).strip()
p = ROOT / 'src/game/Monster.cpp'
s = p.read_text()
n = 0
def fix(m):
    global n
    name = m.group(2)
    want = decl.get(name)
    if want and want != m.group(1).strip():
        n += 1
        return f'{want} Monster::{name}('
    return m.group(0)
s = re.sub(r'^([\w ]+?\s*\**)\s*Monster::(\w+)\(', lambda m: fix(m) if m.group(1).strip() not in ('void',) else m.group(0), s, flags=re.M)
s = s.replace('* Monster::', ' *Monster::') if False else s
p.write_text(s)
print('fixed', n)

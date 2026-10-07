#!/usr/bin/env python3
"""Static call graph from the generated asm (jal edges only; jalr/virtual calls are not followed).

    python3 tools/callgraph.py main            # functions reachable from main, grouped by TU, with sizes
    python3 tools/callgraph.py main --depth 3  # only the first 3 call levels
Writes config/callgraph.csv (function,tu,size,min_depth) for the reachable set.
"""
import re
import sys
from collections import deque
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GLABEL = re.compile(r'^glabel (\S+)')
NONMATCH = re.compile(r'^nonmatching (\S+), (0x[0-9A-Fa-f]+)')
JAL = re.compile(r'\bjal\s+(\S+)')


def load():
    funcs = {}   # name -> dict(tu, size, calls)
    for path in sorted((ROOT / 'asm').rglob('*.s')):
        rel = path.relative_to(ROOT / 'asm').parts
        if rel[0] in ('data',):
            continue
        tu = '/'.join(rel[1:-1]) if rel[0] == 'nonmatchings' else '/'.join(rel[:-1] + (path.stem,))
        if rel[0] == 'nonmatchings':
            tu = '/'.join(rel[1:3]) if len(rel) > 3 else rel[1]
        cur = None
        size = {}
        for line in path.read_text(errors='replace').splitlines():
            m = NONMATCH.match(line)
            if m:
                size[m.group(1)] = int(m.group(2), 16)
                continue
            m = GLABEL.match(line)
            if m:
                cur = m.group(1)
                f = funcs.setdefault(cur, {'tu': tu, 'size': size.get(cur, 0), 'calls': set()})
                if not f['size']:
                    f['size'] = size.get(cur, 0)
                continue
            m = JAL.search(line)
            if m and cur:
                funcs[cur]['calls'].add(m.group(1))
    return funcs


def main():
    args=[a for a in sys.argv[1:] if not a.startswith('--') and not a.isdigit()]
    roots = args[0].split(',')
    nolibs = '--no-libs' in sys.argv
    depth_limit = int(sys.argv[sys.argv.index('--depth') + 1]) if '--depth' in sys.argv else 99
    funcs = load()
    for r in roots:
        if r not in funcs:
            sys.exit(f'{r}: not found')
    seen = {r: 0 for r in roots}
    q = deque(roots)
    while q:
        f = q.popleft()
        if seen[f] >= depth_limit:
            continue
        for c in funcs[f]['calls']:
            if c in funcs and c not in seen and not (nolibs and funcs[c]['tu'].split('/')[0] in ('sce', 'lib989snd', 'newlib', 'gcc', 'crt0')):
                seen[c] = seen[f] + 1
                q.append(c)
    out = ROOT / 'config/callgraph.csv'
    with open(out, 'w') as fh:
        fh.write('function,tu,size,depth\n')
        for f, d in sorted(seen.items(), key=lambda x: (x[1], x[0])):
            fh.write(f"{f},{funcs[f]['tu']},{funcs[f]['size']},{d}\n")
    tot = sum(funcs[f]['size'] for f in seen)
    allsz = sum(v['size'] for v in funcs.values())
    print(f'{len(seen)} functions / {tot} bytes reachable from {','.join(roots)} (of {len(funcs)} / {allsz})')
    bytu = {}
    for f in seen:
        t = funcs[f]['tu']
        bytu.setdefault(t, [0, 0])
        bytu[t][0] += 1
        bytu[t][1] += funcs[f]['size']
    for t, (n, b) in sorted(bytu.items(), key=lambda x: -x[1][1])[:25]:
        print(f'  {t:40s} {n:4d} fns {b:7d} bytes')


main()

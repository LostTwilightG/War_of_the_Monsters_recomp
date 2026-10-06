#!/usr/bin/env python3
"""Try statement-order permutations of a function body in parallel and report scores.

    python3 tools/wsl/permute.py FILE.cpp FUNC_MANGLED START_MARK END_MARK [N] [JOBS]

The lines between the line containing START_MARK and the line containing END_MARK (exclusive) are shuffled (N random
orders, plus the original); each variant is compiled with tools/ccmatch.py in its own work dir. Run inside WSL.
"""
import os, random, re, subprocess, sys, tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
src, func, a, b = sys.argv[1:5]
n = int(sys.argv[5]) if len(sys.argv) > 5 else 24
jobs = int(sys.argv[6]) if len(sys.argv) > 6 else 6
lines = Path(src).read_text().split('\n')
i0 = next(i for i, l in enumerate(lines) if a in l) + 1
i1 = next(i for i, l in enumerate(lines) if i > i0 and b in l)
body = lines[i0:i1]
random.seed(1)
orders = [list(range(len(body)))]
while len(orders) < n + 1:
    o = list(range(len(body)))
    random.shuffle(o)
    if o not in orders:
        orders.append(o)

def run(k):
    o = orders[k]
    d = Path(tempfile.mkdtemp(prefix='perm', dir=Path.home() / '.cache'))
    f = d / 'v.cpp'
    f.write_text('\n'.join(lines[:i0] + [body[j] for j in o] + lines[i1:]))
    env = dict(os.environ, CCMATCH_WORK=str(d / 'w'))
    r = subprocess.run([sys.executable, str(ROOT / 'tools/ccmatch.py'), str(f), '', 'project'], capture_output=True, text=True, env=env, cwd=ROOT)
    m = re.search(re.escape(func) + r': (MATCH|(\d+)/(\d+) words)', r.stdout)
    score = 10**6 if m and m.group(1) == 'MATCH' else (int(m.group(2)) if m else -1)
    return score, o

with ThreadPoolExecutor(jobs) as ex:
    res = sorted(ex.map(run, range(len(orders))), reverse=True)
for s, o in res[:5]:
    print(s, o)
best = res[0][1]
print('\n'.join(body[j] for j in best))

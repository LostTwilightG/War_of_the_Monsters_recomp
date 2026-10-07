#!/usr/bin/env python3
"""Score many variants of one source file in parallel (each in its own ccmatch work dir). Run inside WSL.

    python3 tools/wsl/variants.py FUNC_MANGLED VARIANT.cpp [VARIANT.cpp ...]

Prints `score  file` sorted best first; score is words matched (MATCH = 1000000).
The variants must be self-contained (they are compiled from wherever they live; include paths come from tools/cc.sh).
"""
import os
import re
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
func = sys.argv[1]
files = sys.argv[2:]


def run(f):
    d = Path(tempfile.mkdtemp(prefix='var', dir=Path.home() / '.cache'))
    env = dict(os.environ, CCMATCH_WORK=str(d / 'w'))
    r = subprocess.run([sys.executable, str(ROOT / 'tools/ccmatch.py'), f, '', 'project'], capture_output=True, text=True, env=env, cwd=ROOT)
    m = re.search(re.escape(func) + r': (MATCH|(\d+)/(\d+) words)', r.stdout)
    sc = 10**6 if m and m.group(1) == 'MATCH' else (int(m.group(2)) if m else -1)
    subprocess.run(['rm', '-rf', str(d)])
    return sc, f


with ThreadPoolExecutor(int(os.environ.get('JOBS', '8'))) as ex:
    for sc, f in sorted(ex.map(run, files), reverse=True):
        print(sc, f)

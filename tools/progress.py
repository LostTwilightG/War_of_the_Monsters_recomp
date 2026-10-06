#!/usr/bin/env python3
"""Report decompilation progress: functions/bytes written in C/C++ vs still in assembly.

A function counts as decompiled when its TU is built from src/ and it is not INCLUDE_ASM'd.
Library code (gcc, newlib, sce, lib989snd, crt0) is reported separately since it doesn't need decompiling.
"""
import collections
import csv
import re
import sys
from pathlib import Path

from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parent.parent
INCLUDE_ASM = re.compile(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*([^)\s]+)\s*\)')


def main():
    tus = list(csv.DictReader(open(ROOT / 'config/tus.csv')))
    starts = [int(r['start'], 16) for r in tus]
    ends = [int(r['end'], 16) for r in tus]
    with open(ROOT / 'disc/SCUS_971.97', 'rb') as f:
        elf = ELFFile(f)
        funcs = sorted({(s['st_value'], s['st_size'], s.name) for s in elf.get_section_by_name('.symtab').iter_symbols()
                        if s['st_info']['type'] == 'STT_FUNC' and s['st_shndx'] == 1 and s['st_size']})

    stats = collections.defaultdict(lambda: [0, 0, 0, 0])  # area -> [done funcs, total funcs, done bytes, total bytes]
    per_tu = {}
    i = 0
    for t, r in enumerate(tus):
        name = r['name']
        area = name.split('/')[0] if '/' in name else name
        area = area if area in ('game', 'common') else 'libs'
        src = next((ROOT / f'src/{name}{e}' for e in ('.cpp', '.c') if (ROOT / f'src/{name}{e}').exists()), None)
        asm_names = set(INCLUDE_ASM.findall(src.read_text())) if src else None
        done_f = done_b = tot_f = tot_b = 0
        for addr, size, fname in funcs:
            if not (starts[t] <= addr < ends[t]):
                continue
            tot_f += 1
            tot_b += size
            if asm_names is not None and not any(fname == n or n.startswith(fname + '_') for n in asm_names):
                done_f += 1
                done_b += size
        for k, v in zip(range(4), (done_f, tot_f, done_b, tot_b)):
            stats[area][k] += v
        if src:
            per_tu[name] = (done_f, tot_f, done_b, tot_b)

    print(f"{'area':8} {'functions':>16} {'bytes':>24}")
    for area in ('game', 'common', 'libs'):
        df, tf, db, tb = stats[area]
        print(f'{area:8} {df:6}/{tf:<6} {100 * df / max(tf, 1):5.1f}%  {db:9}/{tb:<9} {100 * db / max(tb, 1):5.2f}%')
    if per_tu and '-v' in sys.argv:
        print('\nTUs in progress:')
        for name, (df, tf, db, tb) in sorted(per_tu.items()):
            print(f'  {name:32} {df}/{tf} funcs, {db}/{tb} bytes')


if __name__ == '__main__':
    main()

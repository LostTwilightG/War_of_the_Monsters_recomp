#!/usr/bin/env python3
"""Split the data sections (.data/.rodata/.gcc_except_table/.sdata/.sbss/.bss) into per-TU ranges.

Evidence, per data address:
  * local (static) symbols: the retail .symtab groups locals per object, delimited by `gcc2_compiled.`;
    each group maps to a TU by its .text address. These are exact.
  * references from code: %hi/%lo/%gp_rel operands in asm/<tu>.s (and asm/nonmatchings/<tu>/).
    Data referenced by a single TU most likely belongs to it.
Every section is linked in the same object order as .text, so the TU ranges must be monotonic.
A DP assigns each data item to a TU (non-decreasing in address), maximizing the evidence weight.

Writes config/data_tus.csv: section,start,end,tu,anchors,refs
"""
import bisect
import collections
import csv
import re
from pathlib import Path

from elftools.elf.elffile import ELFFile

ELF = 'disc/SCUS_971.97'
SECTIONS = ['.data', '.rodata', '.gcc_except_table', '.sdata', '.sbss', '.bss']
ANCHOR_WEIGHT = 1000.0
REF = re.compile(r'%(?:hi|lo|gp_rel)\(([A-Za-z_$.][\w$.]*)\s*(?:\+\s*(0x[0-9A-Fa-f]+|\d+))?\)')
HEXNAME = re.compile(r'^(?:D|jtbl|B|R)_([0-9A-F]{8})$')


def load_tus():
    rows = list(csv.DictReader(open('config/tus.csv')))
    return [r['name'] for r in rows], [int(r['start'], 16) for r in rows]


def tu_of_text(addr, starts):
    return bisect.bisect_right(starts, addr) - 1


def main():
    names, starts = load_tus()
    with open(ELF, 'rb') as f:
        elf = ELFFile(f)
        shdrs = list(elf.iter_sections())
        secname = {i: s.name for i, s in enumerate(shdrs)}
        bounds = {s.name: (s['sh_addr'], s['sh_addr'] + s['sh_size']) for s in shdrs if s.name in SECTIONS}
        syms = list(elf.get_section_by_name('.symtab').iter_symbols())

    def section_of(addr):
        for n, (a, b) in bounds.items():
            if a <= addr < b:
                return n
        return None

    # symbol name -> address (globals + unique locals) for resolving asm references
    addr_of = {}
    for s in syms:
        if s.name and isinstance(s['st_shndx'], int) and secname.get(s['st_shndx']) in SECTIONS:
            addr_of.setdefault(s.name, s['st_value'])
    for line in open('symbol_addrs.txt'):
        m = re.match(r'(\S+) = 0x([0-9A-F]+);', line)
        if m:
            addr_of.setdefault(m.group(1), int(m.group(2), 16))

    # evidence[section][addr][tu] = weight ; items = every symbol start + every referenced address
    evidence = {s: collections.defaultdict(lambda: collections.defaultdict(float)) for s in SECTIONS}
    items = {s: set() for s in SECTIONS}
    for s in syms:
        if isinstance(s['st_shndx'], int) and secname.get(s['st_shndx']) in SECTIONS and s['st_value']:
            items[secname[s['st_shndx']]].add(s['st_value'])

    # 1) local symbol groups
    group_tu, nanchors = None, collections.Counter()
    for s in syms:
        if s['st_info']['bind'] != 'STB_LOCAL':
            continue
        if s['st_info']['type'] == 'STT_FILE':
            group_tu = None
            continue
        sec = secname.get(s['st_shndx']) if isinstance(s['st_shndx'], int) else None
        if s.name == 'gcc2_compiled.' or (group_tu is None and sec == '.text'):
            group_tu = tu_of_text(s['st_value'], starts)
            continue
        if group_tu is not None and sec in SECTIONS:
            evidence[sec][s['st_value']][group_tu] += ANCHOR_WEIGHT
            nanchors[group_tu] += 1

    # 2) code references
    tu_index = {n: i for i, n in enumerate(names)}
    refs = collections.defaultdict(set)  # addr -> {tu}
    for path in Path('asm').rglob('*.s'):
        rel = path.relative_to('asm').as_posix()
        if rel.startswith('data/'):
            continue
        tu = rel[len('nonmatchings/'):].rsplit('/', 1)[0] if rel.startswith('nonmatchings/') else rel[:-2]
        if tu not in tu_index:
            continue
        for m in REF.finditer(path.read_text(errors='replace')):
            name = m.group(1)
            hm = HEXNAME.match(name)
            addr = int(hm.group(1), 16) if hm else addr_of.get(name)
            if addr is not None and section_of(addr):
                refs[addr].add(tu_index[tu])
    for addr, tus in refs.items():
        sec = section_of(addr)
        items[sec].add(addr)
        for t in tus:
            evidence[sec][addr][t] += 1.0 / len(tus)

    # 3) monotonic DP per section
    out = []
    for sec in SECTIONS:
        lo, hi = bounds[sec]
        if hi <= lo:
            continue
        addrs = sorted(a for a in items[sec] | {lo} if lo <= a < hi)
        T = len(names)
        best = [0.0] * T  # best score with current item assigned to tu t (prefix-max applied)
        back = []
        for a in addrs:
            ev = evidence[sec].get(a, {})
            pm, arg, choice = float('-inf'), 0, [0] * T
            new = [0.0] * T
            for t in range(T):
                if best[t] > pm:
                    pm, arg = best[t], t
                new[t] = pm + ev.get(t, 0.0)
                choice[t] = arg
            back.append(choice)
            best = new
        t = max(range(T), key=lambda i: best[i])
        assign = []
        for choice in reversed(back):
            assign.append(t)
            t = choice[t]
        assign.reverse()
        # collapse runs into ranges, start of a run = first item address of that TU
        runs = []
        for a, t in zip(addrs, assign):
            if not runs or runs[-1][1] != t:
                runs.append([a, t])
        runs[0][0] = lo
        for i, (a, t) in enumerate(runs):
            end = runs[i + 1][0] if i + 1 < len(runs) else hi
            n_anchor = sum(1 for x in addrs if a <= x < end and evidence[sec].get(x, {}).get(t, 0) >= ANCHOR_WEIGHT)
            n_ref = sum(1 for x in addrs if a <= x < end and t in refs.get(x, ()))
            out.append((sec, f'0x{a:08X}', f'0x{end:08X}', names[t], n_anchor, n_ref))

    with open('config/data_tus.csv', 'w', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['section', 'start', 'end', 'tu', 'anchors', 'refs'])
        w.writerows(out)
    per = collections.Counter(r[0] for r in out)
    print(f'{len(out)} data ranges written to config/data_tus.csv', dict(per))


if __name__ == '__main__':
    main()

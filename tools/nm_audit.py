#!/usr/bin/env python3
"""Audit every NON_MATCHING (equivalent) function for layout mistakes by comparing the LINKED code of a NM build against the retail ELF:
the multiset of memory-access offsets (sp excluded) and of integer immediates, per function. A struct whose C++ size differs from retail
(stride 0x40 instead of 0x30) shows up as a different immediate; a field at the wrong offset as a different memory offset.
Instruction selection differences (lb/lbu, andi vs ori, branch layout) are noise: read the report, biggest differences first.

    python3 tools/nm_audit.py [nm.elf [retail.elf]]     (run in WSL; defaults: ~/wotm_nm_halfcpp/build/SCUS_971.97.nm.elf, build/SCUS_971.97.elf)
"""
import collections
import re
import subprocess
import sys
from pathlib import Path

root = Path(__file__).resolve().parent.parent
nm_elf = Path(sys.argv[1]).expanduser() if len(sys.argv) > 1 else Path.home() / 'wotm_nm_halfcpp/build/SCUS_971.97.nm.elf'
ret_elf = Path(sys.argv[2]).expanduser() if len(sys.argv) > 2 else root / 'build/SCUS_971.97.elf'
MEM = re.compile(r'\t(?:l[bhwdq]u?|lwc1|lwl|lwr|s[bhwdq]|swc1|swl|swr|lqc2|sqc2|ldl|ldr|sdl|sdr)\s+\S+?,(-?\d+)\((\w+)\)')
IMM = re.compile(r'\t(addiu|daddiu|ori|andi|xori|slti|sltiu|lui|li)\s+(\w+),(?:(\w+),)?(-?(?:0x)?[0-9a-f]+)\s*$')


def funcs_of(elf, wanted):
    out = subprocess.run(['mips-linux-gnu-objdump', '-d', '-z', '--no-show-raw-insn', str(elf)], capture_output=True, text=True).stdout
    res, cur = {}, None
    for l in out.split('\n'):
        m = re.match(r'^[0-9a-f]+ <(.+)>:$', l)
        if m:
            cur = m.group(1) if m.group(1) in wanted else None
            if cur and cur not in res:
                res[cur] = []
        elif cur is not None:
            res[cur].append(l)
    return res


def feats(lines):
    mem, imm = collections.Counter(), collections.Counter()
    for l in lines:
        l = l.split('#')[0].rstrip()
        m = MEM.search(l)
        if m and m.group(2) != 'sp':
            mem[int(m.group(1))] += 1
        m = IMM.search(l)
        if m:
            op, rt, rs, v = m.groups()
            if 'sp' in (rt, rs):
                continue
            v = int(v, 0) if v.lstrip('-').startswith('0x') else int(v)
            if op in ('addiu', 'daddiu') and v in (1, -1, 0):
                continue
            imm[v & 0xFFFFFFFF] += 1
    return mem, imm


wanted = {}
for src in sorted(root.glob('src/**/*.cpp')):
    text = src.read_text(errors='ignore')
    for m in re.finditer(r'#ifdef NON_MATCHING\n(?:/\*.*?\*/\n)?.*?#else\nINCLUDE_ASM\("[^"]+", (\S+)\);\n#endif', text, re.S):
        wanted[m.group(1)] = str(src.relative_to(root / 'src').with_suffix(''))
ours, retail = funcs_of(nm_elf, set(wanted)), funcs_of(ret_elf, set(wanted))
report = []
for name, tu in wanted.items():
    if name not in ours or name not in retail:
        continue
    om, oi = feats(ours[name])
    rm, ri = feats(retail[name])
    dm, di = (om - rm) + (rm - om), (oi - ri) + (ri - oi)
    if dm or di:
        report.append((sum(dm.values()) + sum(di.values()), tu, name, {
            'mem only ours': sorted((om - rm).items()), 'mem only retail': sorted((rm - om).items()),
            'imm only ours': sorted((hex(k), v) for k, v in (oi - ri).items()), 'imm only retail': sorted((hex(k), v) for k, v in (ri - oi).items())}))
report.sort(reverse=True)
for n, tu, name, d in report:
    print(f'{tu}: {name}  (diff {n})')
    for k, v in d.items():
        if v:
            print(f'    {k}: {v}')
print(f'{len(report)} of {len([n for n in wanted if n in ours and n in retail])} functions differ')

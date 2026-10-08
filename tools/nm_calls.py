#!/usr/bin/env python3
"""For every NON_MATCHING (equivalent) function, compare the ordered list of call targets (jal/jalr) in the NM build with the retail ELF.
A call that is missing, extra or in another order is a behaviour difference (or a harmless inlining/ordering choice: read it).

    python3 tools/nm_calls.py [nm.elf [retail.elf]]     (WSL; defaults like nm_audit.py)
"""
import re
import subprocess
import sys
from pathlib import Path

root = Path(__file__).resolve().parent.parent
nm_elf = Path(sys.argv[1]).expanduser() if len(sys.argv) > 1 else Path.home() / 'wotm_nm_halfcpp/build/SCUS_971.97.nm.elf'
ret_elf = Path(sys.argv[2]).expanduser() if len(sys.argv) > 2 else root / 'build/SCUS_971.97.elf'
wanted = {}
for src in sorted(root.glob('src/**/*.cpp')):
    text = src.read_text(errors='ignore')
    for m in re.finditer(r'#ifdef NON_MATCHING\n(?:/\*.*?\*/\n)?.*?#else\nINCLUDE_ASM\("[^"]+", (\S+)\);\n#endif', text, re.S):
        wanted[m.group(1)] = str(src.relative_to(root / 'src').with_suffix(''))


def calls(elf):
    out = subprocess.run(['mips-linux-gnu-objdump', '-d', '-z', '--no-show-raw-insn', str(elf)], capture_output=True, text=True).stdout
    res, cur = {}, None
    for l in out.split('\n'):
        m = re.match(r'^[0-9a-f]+ <(.+)>:$', l)
        if m:
            cur = m.group(1) if m.group(1) in wanted else None
            if cur and cur not in res:
                res[cur] = []
        elif cur is not None:
            m = re.search(r'\t(?:jal|j)\s+[0-9a-f]+ <([^>+]+)(?:\+0x[0-9a-f]+)?>', l)
            if m and m.group(1) != cur:
                res[cur].append(m.group(1).replace('.NON_MATCHING', ''))
            elif re.search(r'\tjalr\s', l):
                res[cur].append('<jalr>')
    return res


o, r = calls(nm_elf), calls(ret_elf)
bad = 0
for name, tu in wanted.items():
    if name in o and name in r and o[name] != r[name]:
        bad += 1
        print(f'{tu}: {name}')
        print(f'    ours  : {o[name]}')
        print(f'    retail: {r[name]}')
print(f'{bad} of {len([n for n in wanted if n in o and n in r])} functions have different call sequences')

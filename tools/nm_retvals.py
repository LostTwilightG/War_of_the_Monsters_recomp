#!/usr/bin/env python3
"""Find NON_MATCHING (equivalent) functions declared void (or int) whose retail callers consume a return value ($v0 / $f0 read right after the
jal before being overwritten). A `void` stand-in leaves garbage in $v0 (this is how addInteractive broke every hat id).

    python3 tools/nm_retvals.py
"""
import re
import glob
from pathlib import Path

root = Path(__file__).resolve().parent.parent
funcs = {}
for src in sorted(glob.glob(str(root / 'src/**/*.cpp'), recursive=True)):
    text = Path(src).read_text(errors='ignore')
    for m in re.finditer(r'#ifdef NON_MATCHING\n(?:/\*.*?\*/\n)?(.*?)#else\nINCLUDE_ASM\("[^"]+", (\S+)\);\n#endif', text, re.S):
        body, mangled = m.group(1), m.group(2)
        sig = re.search(r'^([^\n(#/]*?)[\w:~]+\(', body, re.M)
        ret = sig.group(1).replace('static ', '').replace('const', '').strip() if sig else ''
        funcs[mangled] = ret or 'void'
instr = re.compile(r'^\s*(?:/\*.*?\*/)?\s*([a-z][\w.]*)\s*(.*)$')
callers = {k: [] for k in funcs}
files = [f for f in glob.glob(str(root / 'asm/**/*.s'), recursive=True) if 'nonmatchings' not in f.replace(chr(92), '/')]
for f in files:
    lines = [re.sub(r'/\*.*?\*/', '', l).split('#')[0].rstrip() for l in Path(f).read_text(errors='ignore').split('\n')]
    cur = ''
    for i, l in enumerate(lines):
        if l.strip().startswith('glabel '):
            cur = l.split()[1]
        m = re.match(r'\s*jal\s+(\S+)', l)
        if m and m.group(1) in funcs:
            name = m.group(1)
            used = None
            for j in range(i + 2, min(i + 9, len(lines))):      # skip the delay slot
                t = lines[j].strip()
                if not t or t.startswith('.') or t.endswith(':') or t.startswith(('glabel', 'endlabel', 'nonmatching')):
                    continue
                if re.match(r'(jal|j|jr|jalr|b|beq|bne|bnez|beqz|bgez|bltz|bgtz|blez)\b', t.split()[0]):
                    # a branch/call reads its operands: stop after checking the operands themselves
                    if re.search(r'\$v0|\$v1', t) and t.split()[0] not in ('jal', 'j', 'jalr') and '$ra' not in t:
                        used = ('v0', t)
                    break
                ops = t.split(None, 1)
                opn = ops[0]
                rest = ops[1] if len(ops) > 1 else ''
                dst, _, srcs = rest.partition(',')
                if opn in ('mtc1', 'dmtc1', 'ctc1'):          # these write their SECOND operand
                    dst, srcs = srcs.strip(), dst
                if '$v0' in srcs and opn not in ('sw', 'sb', 'sh', 'sd', 'sq', 'swc1'):
                    used = ('v0', t)
                    break
                if opn in ('sw', 'sb', 'sh', 'sd', 'sq') and dst.strip() == '$v0':
                    used = ('v0', t)
                    break
                if dst.strip() == '$v0' and '$v0' not in srcs:
                    break
                if '$f0' in srcs or (opn.startswith(('swc1', 'mov.s', 'mfc1')) and '$f0' in rest):
                    used = ('f0', t)
                    break
                if dst.strip() == '$f0':
                    break
            if used:
                callers[name].append((cur, used))
for name, c in callers.items():
    if not c:
        continue
    ret = funcs[name]
    kind = {x[1][0] for x in c}
    if ('v0' in kind and ret == 'void') or ('f0' in kind and ret != 'float'):
        print(f'{name}: declared "{ret}" but consumed as {sorted(kind)} by {len(c)} caller(s), e.g. {c[0][0]}: {c[0][1][1].strip()}')

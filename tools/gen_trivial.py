#!/usr/bin/env python3
"""Replace INCLUDE_ASM lines of 8-byte accessor functions (getter/setter/address-of/empty) in src/game/*.cpp with C++ definitions.

The mangled symbol is bound with an asm label on a free function: `this` arrives in $a0 exactly like the first argument, so
    int f(void *self) __asm__("getFocus__14MilitaryPickup");
compiles to the same code as the member. Patterns recognised in the delay slot of `jr $ra`:
    lw/lwc1/lbu/lb/lhu/lh  -> getter       sw/swc1/sb/sh -> setter (value in $a1 / $f12)
    addiu v0,a0,N          -> address of member at +N      nop -> empty        daddu v0,zero,zero / addiu v0,zero,N -> constant

    python3 tools/gen_trivial.py [--apply]      (without --apply only prints what it would do)
"""
import glob
import os
import re
import sys

apply = '--apply' in sys.argv
GET = {'lw': 'int', 'lwc1': 'float', 'lbu': 'unsigned char', 'lb': 'signed char', 'lhu': 'unsigned short', 'lh': 'short'}
SET = {'sw': ('int', 'int'), 'swc1': ('float', 'float'), 'sb': ('unsigned char', 'char'), 'sh': ('short', 'short')}
total = 0
for src in sorted(glob.glob('src/game/*.cpp') + glob.glob('src/common/*.cpp')):
    text = open(src, newline='').read()
    nl = '\r\n' if '\r\n' in text else '\n'
    lines = text.replace('\r\n', '\n').split('\n')
    out = []
    changed = 0
    for i, l in enumerate(lines):
        m = re.match(r'INCLUDE_ASM\("asm/nonmatchings/([^"]+)", (\S+)\);$', l)
        if not m or (i > 0 and lines[i - 1].strip() == '#else'):
            out.append(l)
            continue
        tu, fn = m.groups()
        a = 'asm/nonmatchings/%s/%s.s' % (tu, fn)
        if not os.path.exists(a) or fn.startswith(('func_', 'D_', 'jtbl', '_vt', '__tf', '_$', '__')):
            out.append(l)
            continue
        t = open(a).read()
        mm = re.search(r'^nonmatching %s, (0x[0-9A-Fa-f]+)' % re.escape(fn), t, re.M)
        if not mm or int(mm.group(1), 16) != 8:
            out.append(l)
            continue
        ins = re.findall(r'/\* [0-9A-F]+ [0-9A-F]+ [0-9A-F]+ \*/\s+(\S+)\s+([^\n]*)', t)
        if len(ins) != 2 or ins[0][0] != 'jr':
            out.append(l)
            continue
        op, args = ins[1][0], re.sub(r'\s*#.*', '', ins[1][1]).strip()
        ident = re.sub(r'[^A-Za-z0-9]', '_', fn)
        decl = None
        mo = re.match(r'\$v0, (-?0x[0-9A-Fa-f]+|-?\d+)\(\$a0\)$', args)
        if op in GET and mo:
            off = int(mo.group(1), 0)
            ty = GET[op]
            body = '    return *(%s *)((char *)self + 0x%X);' % (ty, off)
            decl = (ty, 'void *self', body)
        elif op in SET:
            ms = re.match(r'\$(a1|f12), (-?0x[0-9A-Fa-f]+|-?\d+)\(\$a0\)$', args)
            if ms:
                off = int(ms.group(2), 0)
                ty, pty = SET[op]
                body = '    *(%s *)((char *)self + 0x%X) = v;' % (pty, off)
                decl = ('void', 'void *self, %s v' % ty, body)
        elif op == 'addiu':
            ma = re.match(r'\$v0, \$a0, (-?0x[0-9A-Fa-f]+|-?\d+)$', args)
            if ma:
                off = int(ma.group(1), 0)
                decl = ('void *', 'void *self', '    return (char *)self + 0x%X;' % off)
        elif op == 'nop':
            decl = ('void', 'void *self', '')
        if not decl:
            out.append(l)
            continue
        ret, params, body = decl
        out.append('%s %s(%s) __asm__("%s");' % (ret, ident, params, fn))
        out.append('%s %s(%s)' % (ret, ident, params))
        out.append('{')
        if body:
            out.append(body)
        out.append('}')
        changed += 1
    if changed:
        total += changed
        print('%s: %d' % (src, changed))
        if apply:
            open(src, 'w', newline='').write(nl.join(out))
print('total', total)

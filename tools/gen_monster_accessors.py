#!/usr/bin/env python3
"""Turn every one-instruction Monster getter/setter (jr $ra + one load/store in the delay slot) into C++ and name the field.

    python3 tools/gen_monster_accessors.py            # dry run: prints the field table and the generated functions
    python3 tools/gen_monster_accessors.py --apply    # rewrites class Monster in include/game/game.h and src/game/Monster.cpp

The field names come from the accessor names (getFallTime -> m_fallTime). Types come from the access width and the mangled
parameter letters; they are first guesses and get refined as larger functions use the fields.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASM = ROOT / 'asm/nonmatchings/game/Monster'
CPP = ROOT / 'src/game/Monster.cpp'
HDR = ROOT / 'include/game/game.h'
SIZE = 0x11190

# Fields already declared by hand in game.h: offset -> (ctype, name, size). Kept as they are.
EXISTING = {
    0xC: ('_cs *', 'm_cs', 4), 0x14: ('int', 'm_typeBits', 4), 0x18: ('int', 'm_playerNum', 4), 0x20: ('int', 'm_id', 4),
    0x3C: ('int', 'm_unk3C', 4), 0x49: ('unsigned char', 'm_unk49', 1), 0xE8: ('signed char', 'm_dead', 1),
    0xEC: ('unsigned char', 'm_unkEC', 1), 0xF6: ('unsigned char', 'm_unkF6', 1), 0xF7: ('unsigned char', 'm_unkF7', 1),
    0xF9: ('unsigned char', 'm_unkF9', 1), 0x44C: ('float', 'm_health', 4), 0x460: ('StaminaMeter', 'm_stamina', 0x2C),
    0x5040: ('PadFlags', 'm_padFlags', 0x1814), 0x68B4: ('void *', 'm_target', 4), 0x7970: ('int', 'm_camUnify', 4),
    0x10E70: ('char', 'm_victoryState[1]', 1),
}

LOADS = {'lw': ('int', 4), 'lwc1': ('float', 4), 'lbu': ('unsigned char', 1), 'lb': ('signed char', 1),
         'lhu': ('unsigned short', 2), 'lh': ('short', 2)}
STORES = {'sw': ('int', 4), 'swc1': ('float', 4), 'sb': ('unsigned char', 1), 'sh': ('short', 2)}
PARAM = {'b': 'bool', 'i': 'int', 'f': 'float', 'c': 'char', 'Uc': 'unsigned char', 's': 'short', 'Us': 'unsigned short',
         'Ui': 'unsigned', 'Ul': 'unsigned long', 'l': 'long'}


def field_name(func):
    base = func
    for pre in ('get', 'set', 'enable', 'is'):
        if base.startswith(pre) and len(base) > len(pre) and base[len(pre)].isupper():
            base = base[len(pre):]
            if pre == 'is':
                base = 'is' + base
            break
    return 'm_' + base[0].lower() + base[1:]


def parse():
    out = []   # (func, mangled, const, kind, ctype, size, off, paramtype)
    for s in sorted(ASM.glob('*.s')):
        body = s.read_text(errors='replace')
        if 'glabel' not in body:
            continue
        ins = [m.groups() for m in re.finditer(r'^\s+/\*[^*]*\*/\s+(\S+)\s+(.*)$', body, re.M)]
        ins = [(o, a) for o, a in ins if o != 'nop']
        if len(ins) != 2 or ins[0][0] != 'jr':
            continue
        op, args = ins[1]
        mangled = s.stem
        m = re.match(r'(\w+?)__(C?)7Monster(.*)$', mangled)
        if not m:
            continue
        func, const, rest = m.groups()
        a = re.match(r'\$(\w+), (0x[0-9A-Fa-f]+|\d+)\(\$a0\)$', args.strip())
        if op in LOADS and a and a.group(1) in ('v0', 'f0') and not rest:
            ct, sz = LOADS[op]
            out.append((func, mangled, bool(const), 'get', ct, sz, int(a.group(2), 0), None))
        elif op in STORES and a and a.group(1) in ('a1', 'f12') and rest in PARAM:
            ct, sz = STORES[op]
            out.append((func, mangled, bool(const), 'set', ct, sz, int(a.group(2), 0), PARAM[rest]))
    return out


def main():
    acc = parse()
    fields = dict(EXISTING)
    names = {v[1]: k for k, v in EXISTING.items()}
    skipped = []
    for func, mangled, const, kind, ct, sz, off, pt in acc:
        if off in fields:
            continue
        if any(o < off + sz and off < o + s2 for o, (_, _, s2) in fields.items()):
            skipped.append((func, hex(off), 'overlaps'))
            continue
        nm = field_name(func)
        if nm in names:
            nm = f'{nm}_{off:X}'
        fields[off] = (ct, nm, sz)
        names[nm] = off
    code = []
    for func, mangled, const, kind, ct, sz, off, pt in acc:
        if off not in fields:
            continue
        fct, fnm, fsz = fields[off]
        if kind == 'get':
            code.append((mangled, f'{fct if fct != "char" else "char"} Monster::{func}(void){" const" if const else ""}\n{{\n    return {fnm};\n}}\n'))
        else:
            code.append((mangled, f'void Monster::{func}({pt} v)\n{{\n    {fnm} = v;\n}}\n'))
    decl = []
    for func, mangled, const, kind, ct, sz, off, pt in acc:
        if off not in fields:
            continue
        fct = fields[off][0]
        decl.append(f'    {fct} {func}(void){" const" if const else ""};' if kind == 'get' else f'    void {func}({pt} v);')
    print(f'{len(acc)} accessors, {len(fields) - len(EXISTING)} new fields, skipped: {skipped}')
    if '--apply' not in sys.argv:
        for off in sorted(fields):
            print(f'  0x{off:X} {fields[off][0]} {fields[off][1]}')
        return
    # class body
    lines = []
    cur = 0
    for off in sorted(fields):
        ct, nm, sz = fields[off]
        if off > cur:
            lines.append(f'    char pad{cur:X}[0x{off:X} - 0x{cur:X}];')
        lines.append(f'    {ct} {nm};   /* 0x{off:X} */')
        cur = off + sz
    if cur < SIZE:
        lines.append(f'    char pad{cur:X}[0x{SIZE:X} - 0x{cur:X}];')
    h = HDR.read_text()
    a = h.index('class Monster {')
    b = h.index('typedef char _size_Monster', a)
    cls = ('class Monster {\npublic:\n'
           '    void drainSpecial();\n    void enterNewState(MonsterState *state);\n    void takeDamage(float dmg, bool b, Monster *src);\n'
           '    void initAfterDbLoad(void);\n    void update(void);\n    void updateCinema(void);\n    void updatePosition(void);\n'
           + '\n'.join(sorted(set(decl))) + '\n\n' + '\n'.join(lines) + '\n};\n')
    HDR.write_text(h[:a] + cls + h[b:])
    cpp = CPP.read_text()
    for mangled, fn in code:
        line = f'INCLUDE_ASM("asm/nonmatchings/game/Monster", {mangled});\n'
        if line in cpp:
            cpp = cpp.replace(line, fn)
    cpp = cpp.replace('#include "common.h"\n', '#include "common.h"\n#include "game/game.h"\n', 1) if '#include "game/game.h"' not in cpp else cpp
    CPP.write_text(cpp)
    print('applied')


main()

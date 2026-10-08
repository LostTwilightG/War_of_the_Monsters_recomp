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
    0x3C: ('int', 'm_winsThisGame', 4), 0x49: ('signed char', 'm_unk49', 1), 0xE8: ('signed char', 'm_dead', 1),
    0xEC: ('unsigned char', 'm_unkEC', 1), 0xF6: ('signed char', 'm_unkF6', 1), 0xF7: ('signed char', 'm_unkF7', 1),
    0xF9: ('unsigned char', 'm_unkF9', 1), 0x44C: ('float', 'm_health', 4), 0x460: ('StaminaMeter', 'm_stamina', 0x2C),
    0x5040: ('PadFlags', 'm_padFlags', 0x1814), 0x68B4: ('void *', 'm_target', 4), 0x7970: ('int', 'm_camUnify', 4),
    0x10E70: ('char', 'm_victoryState[1]', 1),
    0x4A: ('signed char', 'm_attacksEnabled', 1), 0xF1: ('signed char', 'm_turning', 1), 0x280: ('signed char', 'm_freeFalling', 1),
    0x6CB8: ('float', 'm_onFireCount', 4), 0x6CCC: ('int', 'm_cloakTime', 4), 0x7980: ('int *', 'm_specialState', 4),
    0x4: ('unsigned short', 'm_flags', 2), 0x7DD0: ('int', 'm_blockFlag1B', 4), 0x7E94: ('int', 'm_blockFlag1C', 4),
    0x298: ('float', 'm_climbSpeedBase', 4), 0x2A0: ('float', 'm_climbStrafeBase', 4), 0xFD70: ('float', 'm_fd70', 4), 0xFD74: ('float', 'm_fd74', 4),
    0x6C34: ('int', 'm_pinToggle', 4), 0x6C38: ('int', 'm_pinMode', 4), 0x1A70: ('int', 'm_shadowOff', 4), 0x1A74: ('char *', 'm_shadowCs', 4), 0x1A78: ('int', 'm_shadowSaved', 4),
    0x44: ('int', 'm_frameTime', 4), 0xEA: ('signed char', 'm_unkEA', 1), 0xEB: ('signed char', 'm_unkEB', 1), 0xF5: ('signed char', 'm_unkF5', 1),
    0x7978: ('int *', 'm_stateRef', 4), 0x6874: ('char *', 'm_x6874', 4), 0x69A0: ('float', 'm_padScale', 4),
    0x6CB4: ('int', 'm_fireFx', 4), 0x6CBC: ('float', 'm_onFireDamage', 4), 0x6CC0: ('Monster *', 'm_fireSource', 4),
    0x440: ('float', 'm_collisionBase', 4), 0x444: ('float', 'm_collisionScale', 4), 0x4C8: ('int', 'm_specialGlow[3]', 12),
    0x34: ('int *', 'm_state', 4), 0x38: ('int *', 'm_prevState', 4), 0xB4: ('float', 'm_bodyHeight', 4),
    0xEF: ('signed char', 'm_cloaked', 1), 0x1A3C: ('_cs *', 'm_shadow', 4), 0x846C: ('Monster *', 'm_killer', 4),
    0x4B0: ('int', 'm_healthGlow[3]', 12), 0x4BC: ('int', 'm_staminaGlow[3]', 12),
    0x6CC4: ('float', 'm_beingShockedCount', 4), 0x6CC8: ('float', 'm_beingShockedDamage', 4), 0x6CD4: ('int', 'm_cameraFollows', 4),
    0x6CD8: ('int', 'm_cameraView', 4),
    0x69C8: ('float', 'm_puPunchDamageMod[28]', 0x70), 0x6A38: ('float', 'm_puLaunchDamageMod[28]', 0x70),
    0x6AA8: ('float', 'm_puStaminaGainMod[28]', 0x70), 0x6B18: ('float', 'm_puDurationMod[28]', 0x70), 0x6B88: ('float', 'm_puSpeedMod[28]', 0x70),
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
        ad = re.match(r'\$v0, \$a0, (0x[0-9A-Fa-f]+|\d+)$', args.strip()) if op == 'addiu' else None
        if ad and not rest:
            out.append((func, mangled, bool(const), 'addr', 'void *', 4, int(ad.group(1), 0), None))
        elif op in LOADS and a and a.group(1) in ('v0', 'f0') and not rest:
            ct, sz = LOADS[op]
            out.append((func, mangled, bool(const), 'get', ct, sz, int(a.group(2), 0), None))
        elif op in STORES and a and a.group(1) in ('a1', 'f12') and (rest in PARAM or re.match(r'^P\d+\w+$', rest)):
            ct, sz = STORES[op]
            pm = re.match(r'^P(\d+)(\w+)$', rest)
            pt = (pm.group(2)[:int(pm.group(1))] + ' *') if pm else PARAM[rest]
            out.append((func, mangled, bool(const), 'set', ct, sz, int(a.group(2), 0), pt))
    return out


def main():
    acc = parse()
    fields = dict(EXISTING)
    names = {v[1]: k for k, v in EXISTING.items()}
    skipped = []
    for func, mangled, const, kind, ct, sz, off, pt in acc:
        if kind == 'addr' or off in fields:
            continue
        if any(o < off + sz and off < o + s2 for o, (_, _, s2) in fields.items()):
            skipped.append((func, hex(off), 'overlaps'))
            continue
        nm = field_name(func)
        if nm in names:
            nm = f'{nm}_{off:X}'
        fields[off] = (ct, nm, sz)
        names[nm] = off
    # a setter taking a pointer retypes an int field to that pointer type
    fwd = set()
    for func, mangled, const, kind, ct, sz, off, pt in acc:
        if kind == 'set' and pt and pt.endswith('*') and off in fields and fields[off][0] == 'int':
            fields[off] = (pt.strip(), fields[off][1], 4)
            fwd.add(pt.strip()[:-1].strip())
    code, decl = [], []
    for func, mangled, const, kind, ct, sz, off, pt in acc:
        cq = ' const' if const else ''
        if kind == 'addr':
            code.append((mangled, f'void *Monster::{func}(void){cq}\n{{\n    return (char *)this + 0x{off:X};\n}}\n'))
            decl.append(f'    void *{func}(void){cq};')
            continue
        if off not in fields:
            continue
        fct, fnm, fsz = fields[off]
        if kind == 'get':
            code.append((mangled, f'{fct} Monster::{func}(void){cq}\n{{\n    return {fnm};\n}}\n'))
            decl.append(f'    {fct} {func}(void){cq};')
        else:
            code.append((mangled, f'void Monster::{func}({pt} v)\n{{\n    {fnm} = v;\n}}\n'))
            decl.append(f'    void {func}({pt} v);')
    print(f'{len(acc)} accessors, {len(fields) - len(EXISTING)} new fields, skipped: {skipped}')
    if '--apply' not in sys.argv:
        for off in sorted(fields):
            print(f'  0x{off:X} {fields[off][0]} {fields[off][1]}')
        return
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
    old = h[a:b]
    # keep the hand-written method declarations (everything before the first "/* 0x" field line)
    keep = []
    for ln in old.splitlines()[2:]:
        if '/* 0x' in ln or ln.strip().startswith('char pad'):
            break
        if ln.strip().endswith(';') and '(' in ln:
            keep.append(ln.rstrip())
    allm = {}
    for ln in keep + decl:
        m = re.search(r'(\w+)\(([^)]*)\)', ln)
        allm[(m.group(1), m.group(2))] = ln          # newest wins per (name, parameters)
    fwdtxt = ''.join(f'class {t};\n' for t in sorted(fwd) if t not in ('Monster', 'void', '_cs', 'char'))
    cls = ('class Monster {\npublic:\n' + '\n'.join(allm.values()) + '\n\n' + '\n'.join(lines) + '\n};\n')
    pre = h[:a]
    if fwdtxt and fwdtxt not in pre:
        pre = pre.replace('enum ePickupType', fwdtxt + 'enum ePickupType', 1)
    HDR.write_text(pre + cls + h[b:])
    cpp = CPP.read_text()
    for mangled, fn in code:
        line = f'INCLUDE_ASM("asm/nonmatchings/game/Monster", {mangled});\n'
        if line in cpp:
            cpp = cpp.replace(line, fn)
    CPP.write_text(cpp)
    print('applied')


main()

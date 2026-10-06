#!/usr/bin/env python3
"""Rewrite ee-gcc 2.95.2 assembly output so modern GNU as produces the same bytes as SN's Ps2EeAs.

The retail game was assembled with SN Systems' ps2eeas, which expands some assembler macros
differently from GNU as. We expand those macros ourselves before handing the file to
mips-linux-gnu-as (which we need for INCLUDE_ASM'd splat output). Unknown expansions raise,
so a new case shows up as a build error instead of a silent mismatch.

    python3 tools/snfix.py in.s out.s
"""
import re
import sys

REG = r'\$(?:\d+|[a-z][a-z0-9]*)'
INSN = re.compile(r'^(\s*)([a-z][a-z0-9.]*)\s+(.*?)\s*(#.*)?$')


def parse_imm(text):
    text = text.strip()
    neg = text.startswith('-')
    v = int(text.lstrip('-'), 0)
    return -v if neg else v


def expand_dli(rd, value):
    value &= 0xFFFFFFFFFFFFFFFF
    signed = value - (1 << 64) if value >> 63 else value
    if -0x8000 <= signed < 0x8000:
        return [f'addiu {rd},$0,{signed}']
    if 0 <= signed < 0x10000:
        return [f'ori {rd},$0,{signed:#x}']
    if -0x80000000 <= signed < 0x80000000:
        return [f'li {rd},{signed:#x}']  # 32-bit li: same expansion in both assemblers
    if value == 0xFFFFFFFF:
        return [f'addiu {rd},$0,-1', f'dsrl32 {rd},{rd},0']
    raise ValueError(f'snfix: no known SN expansion for dli {rd},{value:#x}')


def fix_line(line):
    m = INSN.match(line)
    if not m:
        return [line]
    indent, op, args, comment = m.groups()
    ops = [a.strip() for a in args.split(',')] if args else []
    if op == 'move' and len(ops) == 2:
        return [f'{indent}daddu\t{ops[0]},{ops[1]},$0']
    if op == 'dli' and len(ops) == 2:
        return [f'{indent}{x}' for x in expand_dli(ops[0], parse_imm(ops[1]))]
    return [line]


def main():
    src, dst = sys.argv[1], sys.argv[2]
    out = []
    for n, line in enumerate(open(src, encoding='latin1').read().splitlines(), 1):
        try:
            out.extend(fix_line(line))
        except ValueError as e:
            sys.exit(f'{src}:{n}: {e}')
    open(dst, 'w', encoding='latin1', newline='\n').write('\n'.join(out) + '\n')


if __name__ == '__main__':
    main()

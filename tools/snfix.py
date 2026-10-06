#!/usr/bin/env python3
"""Rewrite ee-gcc 2.95.2 assembly output so modern GNU as produces the same bytes as SN's ps2eeas.

The retail game was assembled with SN Systems' ps2eeas, which expands some assembler macros
differently from GNU as. We expand those macros ourselves before handing the file to
mips-linux-gnu-as (which we need for INCLUDE_ASM'd splat output). Unknown expansions raise,
so a new case shows up as a build error instead of a silent mismatch.

Rules reproduced:
  * `move` -> `daddu rd,rs,$0`
  * `dli r,0xffffffff` -> `addiu r,$0,-1; dsrl32 r,r,0`
  * symbolic loads/stores/`la` (gcc emits e.g. `lw $3,sym`): ps2eeas decides in one pass. A symbol it
    has already seen defined in .sdata/.sbss is accessed off $gp; any other symbol gets
    `lui`/`%lo` (loads into a GPR use the destination as the temporary, everything else uses $at).
    Inside `.set nomacro` (gcc's filled delay slots) the access must be one instruction, so $gp is used.
    GNU as instead decides at the end of the file, which would make these differ.
  * R5900 short-loop errata: a backward conditional branch closing a loop of fewer than 6 instructions
    (label through branch) gets nops inserted before the branch until the loop is 6 long.

    python3 tools/snfix.py in.s out.s
"""
import re
import sys

INSN = re.compile(r'^(\s*)([a-z][a-z0-9.]*)\s+(.*?)\s*(#.*)?$')
LABEL = re.compile(r'^([A-Za-z_$.][\w$.]*):')
MEM_OPS = {
    # op: (is_load, dest_is_gpr)
    'lb': (1, 1), 'lbu': (1, 1), 'lh': (1, 1), 'lhu': (1, 1), 'lw': (1, 1), 'lwu': (1, 1), 'ld': (1, 1), 'lq': (1, 1),
    'sb': (0, 0), 'sh': (0, 0), 'sw': (0, 0), 'sd': (0, 0), 'sq': (0, 0),
    'lwc1': (1, 0), 'swc1': (0, 0), 'l.s': (1, 0), 's.s': (0, 0),
}
CANON = {'l.s': 'lwc1', 's.s': 'swc1'}
# a bare symbol expression: sym, sym+off, sym-off (no base register)
SYMEXPR = re.compile(r'^([A-Za-z_$.][\w$.]*)\s*([+-]\s*(?:0x[0-9A-Fa-f]+|\d+))?$')


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


class Fixer:
    def __init__(self):
        self.section = '.text'
        self.nomacro = False
        self.small = set()  # symbols seen defined in .sdata/.sbss so far
        self.reorder = True
        self.after_asm = False  # previous line was #NO_APP

    def directive(self, line):
        s = line.strip()
        parts = s.split(None, 1)
        d = parts[0] if parts else ''
        if d in ('.text', '.data', '.sdata', '.sbss', '.bss', '.rdata', '.rodata'):
            self.section = d
        elif d == '.section' and len(parts) > 1:
            self.section = parts[1].split(',')[0].strip().strip('"')
        elif d == '.set' and len(parts) > 1:
            if parts[1].strip() == 'nomacro':
                self.nomacro = True
            elif parts[1].strip() == 'macro':
                self.nomacro = False
            elif parts[1].strip() in ('reorder', 'noreorder'):
                self.reorder = parts[1].strip() == 'reorder'

    def sym_access(self, indent, op, reg, expr):
        m = SYMEXPR.match(expr)
        if not m:
            return None
        sym = m.group(1)
        off = (m.group(2) or '').replace(' ', '')
        target = f'{sym}{off}'
        op = CANON.get(op, op)
        if sym in self.small or self.nomacro:
            return [f'{indent}{op}\t{reg},%gp_rel({target})($28)']
        if op == 'la':
            return [f'{indent}lui\t{reg},%hi({target})', f'{indent}addiu\t{reg},{reg},%lo({target})']
        is_load, gpr_dest = MEM_OPS[op]
        tmp = reg if (is_load and gpr_dest) else '$1'
        out = [f'{indent}lui\t{tmp},%hi({target})', f'{indent}{op}\t{reg},%lo({target})({tmp})']
        if tmp == '$1':
            out = [f'{indent}.set\tnoat'] + out + [f'{indent}.set\tat']
        return out

    def fix_line(self, line):
        after_asm = self.after_asm
        if line.strip():
            self.after_asm = line.strip() == '#NO_APP'
        lm = LABEL.match(line)
        if lm and self.section in ('.sdata', '.sbss'):
            self.small.add(lm.group(1))
        if line.lstrip().startswith('.'):
            self.directive(line)
            return [line]
        m = INSN.match(line)
        if not m:
            return [line]
        indent, op, args, comment = m.groups()
        if after_asm and self.reorder and op in JUMPS and op != 'jal':
            # gcc leaves the delay slot to the assembler after an asm block; the SN assembler puts a nop there,
            # GNU as would pull the last instruction of the asm into it
            return [f'{indent}.set	noreorder', line, f'{indent}nop', f'{indent}.set	reorder']
        ops = [a.strip() for a in args.split(',')] if args else []
        if op == 'move' and len(ops) == 2:
            return [f'{indent}daddu\t{ops[0]},{ops[1]},$0']
        if op == 'dli' and len(ops) == 2:
            return [f'{indent}{x}' for x in expand_dli(ops[0], parse_imm(ops[1]))]
        if (op in MEM_OPS or op == 'la') and len(ops) == 2:
            if op == 'la' and self.small.isdisjoint({ops[1]}) and not SYMEXPR.match(ops[1]):
                return [line]
            out = self.sym_access(indent, op, ops[0], ops[1])
            if out is not None:
                if op == 'la' and (ops[1] in self.small or self.nomacro):
                    return [f'{indent}addiu\t{ops[0]},$28,%gp_rel({ops[1]})']
                return out
        return [line]


BRANCHES = {'beq', 'bne', 'beqz', 'bnez', 'blez', 'bgtz', 'bltz', 'bgez', 'beql', 'bnel', 'beqzl', 'bnezl',
            'blezl', 'bgtzl', 'bltzl', 'bgezl', 'bc1t', 'bc1f', 'bc1tl', 'bc1fl', 'bc0t', 'bc0f'}
SHORT_LOOP = 6


def insn_count(op, args):
    if op == 'li':
        try:
            v = parse_imm(args.split(',')[1])
        except (ValueError, IndexError):
            return 2
        return 1 if -0x8000 <= v < 0x10000 else 2
    return 1


JUMPS = BRANCHES | {'b', 'j', 'jal', 'jalr', 'jr', 'bal'}


def pad_short_loops(lines):
    """Insert nops before backward conditional branches that close loops shorter than SHORT_LOOP.

    In `.set reorder` mode the assembler adds the delay-slot nop of a jump/branch itself, so those count 2.
    """
    labels = {}   # label -> instruction index
    count = 0     # instructions emitted so far
    reorder = True
    out = []
    for line in lines:
        stripped = line.strip()
        if stripped.startswith('.set'):
            arg = stripped.split()[-1]
            if arg in ('reorder', 'noreorder'):
                reorder = arg == 'reorder'
        lm = LABEL.match(line)
        if lm:
            labels[lm.group(1)] = count
        m = INSN.match(line) if not stripped.startswith('.') and not lm else None
        if m:
            op, args = m.group(2), m.group(3)
            target = args.split(',')[-1].strip() if args else ''
            if op in BRANCHES and target in labels:
                length = count - labels[target] + 1
                if length < SHORT_LOOP:
                    for _ in range(SHORT_LOOP - length):
                        out.append('	nop')
                        count += 1
            count += insn_count(op, args or '') + (1 if reorder and op in JUMPS else 0)
        out.append(line)
    return out


def main():
    src, dst = sys.argv[1], sys.argv[2]
    fixer = Fixer()
    out = []
    for n, line in enumerate(open(src, encoding='latin1').read().splitlines(), 1):
        try:
            out.extend(fixer.fix_line(line))
        except ValueError as e:
            sys.exit(f'{src}:{n}: {e}')
    out = pad_short_loops(out)
    open(dst, 'w', encoding='latin1', newline='\n').write('\n'.join(out) + '\n')


if __name__ == '__main__':
    main()

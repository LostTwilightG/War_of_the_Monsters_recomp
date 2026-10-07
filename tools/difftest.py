#!/usr/bin/env python3
"""Differential test of a decompiled function against the retail one, both run in a MIPS emulator (unicorn).

    python3 tools/difftest.py game/Monster updateOnFire__7Monster --args this --ret void --runs 200

For each seed the same pseudo-random machine state (a junk arena that `this`/pointer arguments point into, the `game`
object and so on) is given to the retail function and to the one compiled from src/ with -DNON_MATCHING. Calls to any other
function are intercepted (not executed) and logged. The two runs must agree on: the return value, the calls made (callee
name and arguments, in order) and the final contents of every non-stack byte that was written.

It is evidence, not proof: it only exercises the paths the random state reaches. Functions that use VU0/COP2 or MMI
instructions other than the few patched below cannot be run.
Run inside WSL (needs unicorn and pyelftools in ~/.venvs/wotm, plus mips-linux-gnu binutils).
"""
import argparse
import numpy as np
import random
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

from elftools.elf.elffile import ELFFile
from unicorn import (UC_ARCH_MIPS, UC_HOOK_CODE, UC_HOOK_MEM_READ_UNMAPPED, UC_HOOK_MEM_WRITE, UC_HOOK_MEM_WRITE_UNMAPPED,
                     UC_MODE_LITTLE_ENDIAN, UC_MODE_MIPS64, Uc, UcError)
from unicorn.mips_const import (UC_MIPS_REG_0, UC_MIPS_REG_4, UC_MIPS_REG_5, UC_MIPS_REG_6, UC_MIPS_REG_7, UC_MIPS_REG_28,
                                UC_MIPS_REG_29, UC_MIPS_REG_31, UC_MIPS_REG_2, UC_MIPS_REG_PC, UC_MIPS_REG_F0, UC_MIPS_REG_F12,
                                UC_MIPS_REG_F13)

ROOT = Path(__file__).resolve().parent.parent
RETAIL = ROOT / 'disc/SCUS_971.97'
GP = 0x6FF8F0
ARENA = 0x06000000
ARENA_SIZE = 0x200000
STACK_TOP = 0x07000000
STACK_SIZE = 0x40000
ALT_TEXT = 0x08000000
ALT_DATA = 0x08400000
RET_ADDR = 0x00001000


def retail_symbols():
    with open(RETAIL, 'rb') as f:
        elf = ELFFile(f)
        sym = {s.name: s['st_value'] for s in elf.get_section_by_name('.symtab').iter_symbols() if s.name}
        segs = []
        for ph in elf.iter_segments():
            if ph['p_type'] == 'PT_LOAD':
                f.seek(ph['p_offset'])
                segs.append((ph['p_vaddr'], f.read(ph['p_filesz']), ph['p_memsz']))
    return sym, segs


def build_alt(tu, workdir):
    """Compile the TU with NON_MATCHING and link it at ALT_TEXT against the retail symbol addresses."""
    obj = workdir / 'alt.o'
    subprocess.run(['sh', str(ROOT / 'tools/cc.sh'), str(ROOT / f'src/{tu}.cpp'), str(obj), '-DNON_MATCHING'],
                   check=True, capture_output=True, cwd=ROOT)
    und = subprocess.run(['mips-linux-gnu-nm', '-u', str(obj)], capture_output=True, text=True, check=True).stdout.split()
    und = [u for u in und if u not in ('U',)]
    sym, _ = retail_symbols()
    script = workdir / 'alt.ld'
    nl = chr(10)
    script.write_text(nl.join([
        'SECTIONS {',
        f'  . = {ALT_TEXT:#x};',
        '  .text : { *(.text*) }',
        f'  . = {ALT_DATA:#x};',
        '  .rodata : { *(.rodata*) }',
        '  .data : { *(.data*) *(.sdata*) }',
        '  .bss : { *(.sbss*) *(.bss*) *(COMMON) }',
        '}',
        f'_gp = {GP:#x};',
        '']))
    cmd = ['mips-linux-gnu-ld', '-EL', '-T', str(script), '--unresolved-symbols=ignore-all', '--no-check-sections', '--noinhibit-exec',
           '-o', str(workdir / 'alt.elf'), str(obj)]
    missing = []
    for u in und:
        if u in sym:
            cmd.append(f'--defsym={u}={sym[u]:#x}')
        else:
            missing.append(u)
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit('link failed:\n' + r.stderr[-1500:])
    with open(workdir / 'alt.elf', 'rb') as f:
        elf = ELFFile(f)
        secs = []
        for s in elf.iter_sections():
            if s['sh_flags'] & 2 and s['sh_type'] == 'SHT_PROGBITS':
                secs.append((s['sh_addr'], s.data()))
        asym = {s.name: (s['st_value'], s['st_size']) for s in elf.get_section_by_name('.symtab').iter_symbols()
                if s.name and s['st_info']['type'] == 'STT_FUNC'}
    return secs, asym, missing


def patch_code(buf):
    """R5900-only encodings -> plain MIPS64 equivalents (see module docstring)."""
    out = bytearray(buf)
    for i in range(0, len(out) - 3, 4):
        w = struct.unpack_from('<I', out, i)[0]
        op = w >> 26
        if op == 0x1F:      # sq -> sd
            w = (0x3F << 26) | (w & 0x03FFFFFF)
        elif op == 0x1E:    # lq -> ld
            w = (0x37 << 26) | (w & 0x03FFFFFF)
        elif op == 0 and (w & 0x3F) in (0x18, 0x19) and ((w >> 11) & 31) != 0:   # mult/multu rd,rs,rt -> mul
            w = (0x1C << 26) | (w & 0x03FFFFC0) | 0x02
        elif op == 0x1C and (w & 0x7FF) == ((0x12 << 6) | 0x29):                   # por -> or
            w = (w & 0x03FFF800) | 0x25
        else:
            continue
        struct.pack_into('<I', out, i, w)
    return bytes(out)


class Run:
    def __init__(self, name, retail_segs, alt_secs, entry, fn_range, entries, seed, spec):
        self.name = name
        self.entries = entries                # addr -> callee name (every known function start)
        self.entry = entry
        self.fn_range = fn_range
        self.calls = []
        self.writes = {}
        self.fault = None
        uc = Uc(UC_ARCH_MIPS, UC_MODE_MIPS64 | UC_MODE_LITTLE_ENDIAN)
        self.uc = uc
        for vaddr, data, memsz in retail_segs:
            lo = vaddr & ~0xFFF
            hi = (vaddr + memsz + 0xFFF) & ~0xFFF
            try:
                uc.mem_map(lo, hi - lo)
            except UcError:
                pass
            uc.mem_write(vaddr, patch_code(data) if vaddr < 0x400000 else data)
        for addr, data in alt_secs:
            lo = addr & ~0xFFF
            hi = (addr + len(data) + 0xFFF) & ~0xFFF
            try:
                uc.mem_map(lo, hi - lo)
            except UcError:
                pass
            uc.mem_write(addr, patch_code(data))
        uc.mem_map(0, 0x10000)
        uc.mem_map(ARENA, ARENA_SIZE)
        uc.mem_map(STACK_TOP - STACK_SIZE, STACK_SIZE)
        rnd = random.Random(seed)
        rs = np.random.RandomState(seed)
        n = ARENA_SIZE // 4
        kind = rs.random_sample(n)
        words = np.empty(n, dtype=np.uint32)
        small = rs.randint(0, 9, n).astype(np.uint32)
        flt = rs.uniform(-100, 100, n).astype(np.float32).view(np.uint32)
        special = np.array([0xFFFFFFFF, 0x7FFF, 0x80000000], dtype=np.uint32)[rs.randint(0, 3, n)]
        ptr = (ARENA + (rs.randint(0x1000, ARENA_SIZE - 0x1000, n) & ~3)).astype(np.uint32)
        words = np.where(kind < 0.40, small, np.where(kind < 0.55, flt, np.where(kind < 0.60, special, ptr))).astype(np.uint32)
        uc.mem_write(ARENA, words.tobytes())
        self.rnd = rnd
        for r in range(32):
            uc.reg_write(UC_MIPS_REG_0 + r, 0)
        uc.reg_write(UC_MIPS_REG_29, STACK_TOP - 0x100)
        uc.reg_write(UC_MIPS_REG_28, GP)
        uc.reg_write(UC_MIPS_REG_31, RET_ADDR)
        uc.mem_write(RET_ADDR, struct.pack('<I', 0) * 4)
        self.hooked = False
        # arguments
        self.setup_args(spec, rnd)

    def setup_args(self, spec, rnd):
        uc = self.uc
        ireg = [UC_MIPS_REG_4, UC_MIPS_REG_5, UC_MIPS_REG_6, UC_MIPS_REG_7]
        freg = [UC_MIPS_REG_F12, UC_MIPS_REG_F13]
        ni = nf = 0
        for t in spec:
            if t == 'this':
                val = ARENA + 0x20000
            elif t == 'p':
                val = ARENA + (rnd.randrange(0x1000, ARENA_SIZE - 0x1000) & ~15)
            elif t == 'i':
                val = rnd.randrange(0, 9)
            elif t == 'b':
                val = rnd.randrange(0, 2)
            elif t == 'f':
                fv = rnd.uniform(-10, 10)
                uc.reg_write(freg[nf], struct.unpack('<I', struct.pack('<f', fv))[0])
                nf += 1
                continue
            else:
                raise SystemExit(f'bad arg spec {t}')
            uc.reg_write(ireg[ni], val)
            ni += 1

    def hook_code(self, uc, addr, size, ud):
        if addr == self.entry and not self.started:
            self.started = True
            return
        if addr in self.entries and not (self.fn_range[0] <= addr < self.fn_range[1]):
            a = [uc.reg_read(UC_MIPS_REG_4 + i) & 0xFFFFFFFF for i in range(4)]
            f12 = uc.reg_read(UC_MIPS_REG_F12) & 0xFFFFFFFF
            f13 = uc.reg_read(UC_MIPS_REG_F13) & 0xFFFFFFFF
            self.calls.append((self.entries[addr], tuple(a), f12, f13))
            n = len(self.calls)
            uc.reg_write(UC_MIPS_REG_2, ARENA + 0x100000 + 0x40 * (n % 64))
            uc.reg_write(UC_MIPS_REG_F0, 0)
            uc.reg_write(UC_MIPS_REG_PC, uc.reg_read(UC_MIPS_REG_31))
            self.redirected = True
            uc.emu_stop()

    def hook_write(self, uc, access, addr, size, value, ud):
        if STACK_TOP - STACK_SIZE <= addr < STACK_TOP:
            return
        for k in range(size):
            self.writes[addr + k] = (value >> (8 * k)) & 0xFF

    def execute(self, max_steps=200000):
        uc = self.uc
        self.started = False
        uc.hook_add(UC_HOOK_CODE, self.hook_code)
        uc.hook_add(UC_HOOK_MEM_WRITE, self.hook_write)
        pc = self.entry
        steps = 0
        while True:
            self.redirected = False
            try:
                uc.emu_start(pc, RET_ADDR, count=max_steps)
            except UcError as e:
                pcx = uc.reg_read(UC_MIPS_REG_PC)
                try:
                    w = struct.unpack('<I', bytes(uc.mem_read(pcx, 4)))[0]
                except UcError:
                    w = 0
                self.fault = f'{e} at pc={pcx:#x} insn={w:#010x} calls={len(self.calls)}'
                return False
            pc = uc.reg_read(UC_MIPS_REG_PC)
            if pc == RET_ADDR:
                return True
            steps += 1
            if not self.redirected or steps > 400:
                self.fault = 'did not finish (loop or stop)'
                return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('tu')
    ap.add_argument('func')
    ap.add_argument('--args', default='this')
    ap.add_argument('--ret', default='void', choices=['void', 'int', 'float'])
    ap.add_argument('--runs', type=int, default=100)
    ap.add_argument('--verbose', action='store_true')
    ap.add_argument('--alt', default=None, help='negative control: run this function from src/ instead (should DIFFER)')
    o = ap.parse_args()
    spec = [t for t in o.args.split(',') if t]
    sym, segs = retail_symbols()
    if o.func not in sym:
        sys.exit(f'{o.func} not in the retail symbol table')
    with tempfile.TemporaryDirectory() as td:
        alt_secs, asym, missing = build_alt(o.tu, Path(td))
    altname = o.alt or o.func
    if altname not in asym:
        sys.exit(f'{altname} not compiled from src/ (still INCLUDE_ASM?)')
    r_entry = sym[o.func]
    a_entry, a_size = asym[altname]
    r_size = None
    with open(RETAIL, 'rb') as f:
        for s in ELFFile(f).get_section_by_name('.symtab').iter_symbols():
            if s.name == o.func:
                r_size = s['st_size']
    r_entries = {v: k for k, v in sym.items() if not k.startswith('.') and v < 0x400000}
    a_entries = {v: k for k, (v, _) in asym.items()}
    ok = bad = skipped = 0
    for seed in range(o.runs):
        ra = Run('retail', segs, [], r_entry, (r_entry, r_entry + r_size), r_entries, seed, spec)
        rb = Run('alt', segs, alt_secs, a_entry, (a_entry, a_entry + a_size), {**a_entries}, seed, spec)
        # both runs patch the retail code identically; the alt run must also intercept calls into retail code
        rb.entries = {**r_entries, **a_entries}
        fa = ra.execute()
        fb = rb.execute()
        if not (fa and fb):
            skipped += 1
            if o.verbose:
                print(f'seed {seed}: skipped (retail: {ra.fault}; alt: {rb.fault})')
            continue
        diffs = []
        if o.ret == 'int' and (ra.uc.reg_read(UC_MIPS_REG_2) & 0xFFFFFFFF) != (rb.uc.reg_read(UC_MIPS_REG_2) & 0xFFFFFFFF):
            diffs.append(('v0', hex(ra.uc.reg_read(UC_MIPS_REG_2) & 0xFFFFFFFF), hex(rb.uc.reg_read(UC_MIPS_REG_2) & 0xFFFFFFFF)))
        if o.ret == 'float' and (ra.uc.reg_read(UC_MIPS_REG_F0) & 0xFFFFFFFF) != (rb.uc.reg_read(UC_MIPS_REG_F0) & 0xFFFFFFFF):
            diffs.append(('f0', hex(ra.uc.reg_read(UC_MIPS_REG_F0) & 0xFFFFFFFF), hex(rb.uc.reg_read(UC_MIPS_REG_F0) & 0xFFFFFFFF)))
        if ra.calls != rb.calls:
            diffs.append(('calls', ra.calls[:4], rb.calls[:4]))
        if ra.writes != rb.writes:
            keys = sorted(set(ra.writes) | set(rb.writes))
            d = [(hex(k), ra.writes.get(k), rb.writes.get(k)) for k in keys if ra.writes.get(k) != rb.writes.get(k)]
            diffs.append(('writes', d[:6]))
        if diffs:
            bad += 1
            if bad <= 3:
                print(f'seed {seed}: MISMATCH {diffs}')
        else:
            ok += 1
    print(f'{o.func}: {ok} agree, {bad} differ, {skipped} skipped of {o.runs} (unresolved symbols: {len(missing)})')
    sys.exit(1 if bad else 0)


main()

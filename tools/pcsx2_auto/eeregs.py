"""eeregs.py <elf>: find PCSX2's EE register file (cpuRegs) in the emulator process and print pc/ra/sp with symbols.
GPR[0]==0, sp/ra/pc plausible; cpuRegs = 32 GPR quads, HI, LO, sa, IsDelaySlot, pc (offset 0x228)."""
import ctypes, ctypes.wintypes as wt, subprocess, sys, struct, bisect
from elftools.elf.elffile import ELFFile

elf = ELFFile(open(sys.argv[1], 'rb'))
syms = sorted((s['st_value'], s['st_size'], s.name) for s in elf.get_section_by_name('.symtab').iter_symbols() if s['st_info']['type'] == 'STT_FUNC')
addrs = [a for a, _, _ in syms]
allsyms = {x.name: x['st_value'] for x in elf.get_section_by_name('.symtab').iter_symbols()}
GP = allsyms.get('_gp') or allsyms.get('__gp')
print('gp', hex(GP) if GP else None)


def sym(a):
    i = bisect.bisect_right(addrs, a) - 1
    if i < 0:
        return '?'
    s = syms[i]
    return '%s+%#x' % (s[2], a - s[0])


k = ctypes.WinDLL('kernel32', use_last_error=True)
pid = int(subprocess.check_output(['powershell', '-NoProfile', '-Command', '(Get-Process pcsx2-qt).Id']).split()[0])
h = k.OpenProcess(0x0410, False, pid)


class MBI(ctypes.Structure):
    _fields_ = [('Base', ctypes.c_void_p), ('AllocBase', ctypes.c_void_p), ('AllocProt', wt.DWORD), ('Pad', wt.DWORD),
                ('Size', ctypes.c_size_t), ('State', wt.DWORD), ('Prot', wt.DWORD), ('Type', wt.DWORD)]


def regions():
    addr = 0
    mbi = MBI()
    while k.VirtualQueryEx(h, ctypes.c_void_p(addr), ctypes.byref(mbi), ctypes.sizeof(mbi)):
        if mbi.State == 0x1000 and mbi.Prot in (0x02, 0x04, 0x08, 0x20, 0x40, 0x80) and 0x1000 <= mbi.Size <= 0x80000000:
            yield mbi.Base or 0, mbi.Size
        addr = (mbi.Base or 0) + mbi.Size
        if addr >= 1 << 47:
            break


def read(a, n):
    buf = ctypes.create_string_buffer(n)
    got = ctypes.c_size_t()
    if not k.ReadProcessMemory(h, ctypes.c_void_p(a), buf, n, ctypes.byref(got)):
        return None
    return buf.raw[:got.value]


found = 0
for base, size in regions():
    for off in range(0, size, 0x400000):
        n = min(0x400000 + 0x240, size - off)
        d = read(base + off, n)
        if not d or len(d) < 0x240:
            continue
        v = memoryview(d[:len(d) // 4 * 4]).cast('I')
        for i in range(0, len(v) - 0x240 // 4, 4):
            if v[i] or v[i + 1] or v[i + 2] or v[i + 3]:
                continue
            sp = v[i + 116]
            pc = v[i + 138]
            ra = v[i + 124]
            okpc = (0x80000000 <= pc < 0x80100000) or (0xBFC00000 <= pc < 0xBFD00000) or (0x1FC00000 <= pc < 0x1FD00000)
            if not (okpc and pc % 4 == 0 and v[i + 137] in (0, 1)):
                continue
            if v[i+112] != GP:
                continue
            found += 1
            gpr = lambda r: v[i + r * 4]
            print('cpuRegs @ %#x: pc %#x (%s) delay %d | ra %#x (%s) | sp %#x | a0 %#x a1 %#x v0 %#x' % (
                base + off + i * 4, pc, sym(pc), v[i + 137], ra, sym(ra), sp, gpr(4), gpr(5), gpr(2)))
print('candidates', found)

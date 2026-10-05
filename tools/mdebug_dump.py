"""Dump ECOFF .mdebug info (file descriptors, procedures, local symbols) from a PS2 ELF."""
import struct, sys
from elftools.elf.elffile import ELFFile

HDR_FIELDS = ("magic vstamp ilineMax cbLine cbLineOffset idnMax cbDnOffset ipdMax cbPdOffset "
              "isymMax cbSymOffset ioptMax cbOptOffset iauxMax cbAuxOffset issMax cbSsOffset "
              "issExtMax cbSsExtOffset ifdMax cbFdOffset crfd cbRfdOffset iextMax cbExtOffset").split()

def load(path):
    data = open(path, 'rb').read()
    elf = ELFFile(open(path, 'rb'))
    off = elf.get_section_by_name('.mdebug')['sh_offset']
    vals = struct.unpack_from('<hh' + 'i' * 23, data, off)
    return data, dict(zip(HDR_FIELDS, vals))

def cstr(data, off):
    return data[off:data.index(b'\0', off)].decode('latin1')

def fdrs(data, h):
    # FDR: adr, rss, issBase, cbSs, isymBase, csym, ilineBase, cline, ioptBase, copt,
    #      ipdFirst(h), cpd(h), iauxBase, caux, rfdBase, crfd, bits, cbLineOffset, cbLine
    fmt = '<IiiiiiiiiihhiiiiIii'
    sz = struct.calcsize(fmt)
    for i in range(h['ifdMax']):
        f = struct.unpack_from(fmt, data, h['cbFdOffset'] + i * sz)
        name = cstr(data, h['cbSsOffset'] + f[2] + f[1]) if f[1] >= 0 else '?'
        yield dict(idx=i, adr=f[0], name=name, issBase=f[2], isymBase=f[4], csym=f[5],
                   ipdFirst=f[10], cpd=f[11], cline=f[7])

def syms(data, h, fd):
    # SYMR: iss, value, bits(st:6 sc:5 reserved:1 index:20)
    for j in range(fd['csym']):
        iss, val, bits = struct.unpack_from('<iiI', data, h['cbSymOffset'] + (fd['isymBase'] + j) * 12)
        name = cstr(data, h['cbSsOffset'] + fd['issBase'] + iss) if iss >= 0 else ''
        yield name, val, bits & 0x3f, (bits >> 6) & 0x1f, bits >> 12

if __name__ == '__main__':
    data, h = load(sys.argv[1])
    print({k: v for k, v in h.items()})
    for fd in fdrs(data, h):
        print(f"{fd['idx']:4} {fd['adr']:08x} syms={fd['csym']:5} procs={fd['cpd']:4} lines={fd['cline']:6} {fd['name']}")

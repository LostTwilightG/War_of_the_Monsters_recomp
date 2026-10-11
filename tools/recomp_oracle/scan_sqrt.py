"""Varre o .text de um ELF por sqrt.s/rsqrt.s e classifica a codificacao.
R5900: SQRT.S fd, ft  -> fonte em ft, fs=0   (forma retail)
MIPS32: sqrt.s fd, fs -> fonte em fs, ft=0   (forma errada no EE)
Uso: python scan_sqrt.py elf [tabela de funcoes: config/status.csv]"""
import struct, sys, csv, bisect, os

def load(elf):
    d = open(elf, "rb").read()
    shoff, shentsize, shnum, shstrndx = struct.unpack_from("<I", d, 0x20)[0], *struct.unpack_from("<HHH", d, 0x2E)
    secs = []
    for i in range(shnum):
        n, t, f, addr, off, size = struct.unpack_from("<IIIIII", d, shoff + i * shentsize)
        secs.append((n, t, f, addr, off, size))
    stroff = secs[shstrndx][4]
    out = []
    for n, t, f, addr, off, size in secs:
        name = d[stroff + n: d.index(b"\0", stroff + n)].decode()
        if name.startswith(".text") or name == ".nm_extra":
            out.append((name, addr, d[off:off + size]))
        elif name == ".cod":  # build NM: codigo + dados; so ate o fim do .text retail
            out.append((name, addr, d[off:off + min(size, 0x251020 - addr)]))
    return out

def scan(elf):
    hits = {"retail(ft)": [], "errado(fs)": []}
    for name, addr, blob in load(elf):
        for i in range(0, len(blob) - 3, 4):
            w = struct.unpack_from("<I", blob, i)[0]
            if (w >> 21) != 0x230:      # COP1, fmt=S
                continue
            funct = w & 0x3F
            if funct not in (4, 0x16):  # sqrt.s, rsqrt.s
                continue
            ft, fs = (w >> 16) & 31, (w >> 11) & 31
            if funct == 4:
                kind = "retail(ft)" if (fs == 0 and ft != 0) else ("errado(fs)" if (ft == 0 and fs != 0) else None)
            else:
                kind = None
            if kind:
                hits[kind].append(addr + i)
    return hits

if __name__ == "__main__":
    elf = sys.argv[1]
    rows = []
    for r in csv.DictReader(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "config", "status.csv"))):
        rows.append((int(r["address"], 16), int(r["size"]), r["tu"], r["function"]))
    rows.sort(); starts = [r[0] for r in rows]
    h = scan(elf)
    print(elf)
    for k, v in h.items():
        print(f"  sqrt.s {k}: {len(v)}")
    seen = {}
    for a in h["errado(fs)"]:
        j = bisect.bisect_right(starts, a) - 1
        key = (rows[j][2], rows[j][3]) if j >= 0 and a < rows[j][0] + rows[j][1] else ("?", hex(a))
        seen[key] = seen.get(key, 0) + 1
    for (tu, fn), n in sorted(seen.items()):
        print(f"    ERRADO {tu}  {fn}  x{n}")

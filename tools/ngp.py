#!/usr/bin/env python3
"""Leitor de imagens .NGP do WotM e extrator de malhas (formatos em docs/FORMATOS.md).

O .NGP de fase e um zip com o magic "IE\\3\\4" (no lugar de "PK\\3\\4"), um unico arquivo em deflate; o de
monstro vem cru. A imagem e pre-ligada para uma base (0xA00000); ponteiros dentro dela sao enderecos
absolutos da RAM do PS2, entao `Image.u32(addr)` aceita o endereco como o jogo o ve.
Nao grava nada do jogo: le o disco do usuario em tempo de execucao.

Malha (`HierObject::polyPkt`): uma sub-cadeia de DMA (tag QWC/ID=ret, 8 bytes + 8 bytes de padding) seguida de um
fluxo VIF para o microcodigo do VU1. Ver `decode_object` e docs/FORMATOS.md ("Malhas").
"""
import struct
import sys
import zlib

LEVEL_BASE = 0xA00000


def inflate_ngp(path):
    with open(path, "rb") as f:
        raw = f.read()
    if raw[:4] != b"IE\x03\x04":
        return raw  # monstros (.ngp em MON/) vem sem zip (fileReadf)
    (_ver, _flags, method, _time, _date, _crc, csize, usize, nlen, xlen) = struct.unpack_from("<HHHHHIIIHH", raw, 4)
    if method != 8:
        raise ValueError("%s: metodo %d (so deflate)" % (path, method))
    off = 4 + struct.calcsize("<HHHHHIIIHH") + nlen + xlen
    data = zlib.decompressobj(-15).decompress(raw[off:off + csize])
    if len(data) != usize:
        raise ValueError("%s: tamanho %d != %d" % (path, len(data), usize))
    return data


class Image:
    def __init__(self, data, base=LEVEL_BASE):
        self.d = data
        self.base = base

    @classmethod
    def load(cls, path, base=LEVEL_BASE):
        return cls(inflate_ngp(path), base)

    def ok(self, addr, n=4):
        return self.base <= addr and addr - self.base + n <= len(self.d)

    def u8(self, a):
        return self.d[a - self.base]

    def s8(self, a):
        return struct.unpack_from("<b", self.d, a - self.base)[0]

    def u16(self, a):
        return struct.unpack_from("<H", self.d, a - self.base)[0]

    def s16(self, a):
        return struct.unpack_from("<h", self.d, a - self.base)[0]

    def u32(self, a):
        return struct.unpack_from("<I", self.d, a - self.base)[0]

    def f32(self, a):
        return struct.unpack_from("<f", self.d, a - self.base)[0]

    def roots(self):
        n = self.u32(self.base)
        return [self.u32(self.base + 4 + 4 * i) for i in range(n)]


OPNAMES = {0: "OBJECT", 1: "GROUP", 2: "LOD", 3: "TRANSLATE", 4: "ROTATE", 5: "LIGHT", 6: "SWITCH", 7: "ANIM_MGR",
           8: "CONTROL", 9: "NULL", 10: "COLL_VOL", 11: "COLL_TRIG", 12: "CHAR_ANIM", 13: "TERRAIN", 14: "TERRAIN_OBJ",
           15: "COLL_GRID", 16: "PART_MGR", 17: "ANIM_XFORM", 18: "BSP", 19: "PARTICLE", 20: "PART_FIELD",
           21: "PART_VISUAL", 22: "PART_EMITTER", 23: "ACTION_DATA", 24: "CHARACTER", 25: "CHAR_INSTANCE",
           30: "SCALE", 31: "DESTRUCTIBLE", 32: "DESTRUCT_TABLE", 33: "ANIM_CONTROL", 34: "ANIM_BLEND",
           35: "SKEL_BONE", 36: "ANIM_PLAYER", 37: "CAMERA", 38: "ANIM_SCALE", 39: "INTERACTIVE", 40: "ATTACHPT",
           41: "SCRIPT", 42: "POLY_COLL", 43: "ANIM_PROC"}

# ---------------------------------------------------------------- malha (VIF)

# UNPACK: cmd = 0x60 | vn << 2 | vl ; bits por componente
_VN_COMPS = (1, 2, 3, 4)
_VL_BITS = (32, 16, 8, 5)


def _unpack_words(vn, vl, num):
    if vl == 3:
        return (num * 16 + 31) // 32
    return (num * _VL_BITS[vl] * _VN_COMPS[vn] + 31) // 32


# Enderecos da memoria de dados do VU1 usados pelo microcodigo dos objetos (descobertos nos dados)
VU_POS = 0xB5       # V3-32  posicoes (ate 190)
VU_NRM = 0x175      # V3-8   normais (signed, /127)
# secao de strip: tag GIF em H, depois 3 qwords por vertice (ST, RGBAQ, XYZF)
#   H+1+3k  S-8    indice do vertice em VU_POS
#   H+2+3k  V4-8   RGBA (0x80 = 1.0)
#   H+3+3k  V2-16  UV (4096 = 1.0)


class Strip:
    def __init__(self):
        self.verts = []   # (px,py,pz, nx,ny,nz, r,g,b,a, u,v)


class Mesh:
    def __init__(self):
        self.strips = []
        self.prim = None   # campo PRIM da GIF tag (tipo, IIP, TME, FGE, ABE, ...)


def decode_object(img, node):
    """Decodifica o `polyPkt` de um HierObject e devolve `Mesh` (strips de vertices ja resolvidos)."""
    mesh = Mesh()
    pp = img.u32(node + 4)
    if not img.ok(pp, 16):
        return mesh
    qwc = img.u32(pp) & 0xFFFF
    nwords = (qwc + 1) * 4 - 2
    base = pp + 8
    if not img.ok(base, nwords * 4):
        return mesh

    pos, nrm = [], []
    hdr = None            # (addr, nloop, prim)
    idx, rgba, uv, adc = [], [], [], set()
    mask = 0
    i = 0

    def flush():
        nonlocal hdr, idx, rgba, uv, adc
        if hdr is None or not idx:
            hdr, idx, rgba, uv, adc = None, [], [], [], set()
            return
        n = min(hdr[1], len(idx))
        cur = None
        for k in range(n):
            if cur is None or k in adc:
                cur = Strip()
                mesh.strips.append(cur)
            j = idx[k]
            p = pos[j] if j < len(pos) else (0.0, 0.0, 0.0)
            q = nrm[j] if j < len(nrm) else (0.0, 1.0, 0.0)
            c = rgba[k] if k < len(rgba) else (128, 128, 128, 128)
            t = uv[k] if k < len(uv) else (0, 0)
            cur.verts.append((p[0], p[1], p[2], q[0], q[1], q[2], c[0], c[1], c[2], c[3], t[0] / 4096.0, t[1] / 4096.0))
        hdr, idx, rgba, uv, adc = None, [], [], [], set()

    while i < nwords:
        w = img.u32(base + 4 * i)
        cmd = (w >> 24) & 0x7F
        num = (w >> 16) & 0xFF
        imm = w & 0xFFFF
        a = base + 4 * (i + 1)
        if cmd >= 0x60:
            vn, vl = (cmd >> 2) & 3, cmd & 3
            n = num or 256
            addr = imm & 0x3FF
            fmt = (vn, vl)
            if fmt == (2, 0) and addr == VU_POS:                       # V3-32 posicoes
                if hdr is not None:
                    flush()
                pos = [(img.f32(a + 12 * k), img.f32(a + 12 * k + 4), img.f32(a + 12 * k + 8)) for k in range(n)]
            elif fmt == (2, 2) and addr == VU_NRM:                     # V3-8 normais
                # 3 bytes por normal, empacotados em bytes consecutivos (sem preenchimento por elemento)
                nrm = [tuple(img.s8(a + 3 * k + c) / 127.0 for c in range(3)) for k in range(n)]
            elif fmt == (3, 0) and n == 1:                             # V4-32: tag GIF da secao
                g0, g1 = img.u32(a), img.u32(a + 4)
                hdr = (addr, g0 & 0x7FFF, (g1 >> 15) & 0x7FF)
                mesh.prim = hdr[2]
            elif hdr is not None and fmt == (0, 2) and (mask & 0xFF) == 0xBF:   # S-8 sob mascara: marca ADC
                for k in range(n):
                    e = addr - hdr[0] - 3 + 3 * k
                    if e % 3 == 0:
                        adc.add(e // 3)
            elif hdr is not None and fmt == (0, 2) and addr == hdr[0] + 1:      # S-8 indices
                idx = [img.u8(a + k) for k in range(n)]
            elif hdr is not None and fmt == (3, 2) and addr == hdr[0] + 2:      # V4-8 RGBA
                rgba = [(img.u8(a + 4 * k), img.u8(a + 4 * k + 1), img.u8(a + 4 * k + 2), img.u8(a + 4 * k + 3)) for k in range(n)]
            elif hdr is not None and fmt == (1, 1) and addr == hdr[0] + 3:      # V2-16 UV
                uv = [(img.s16(a + 4 * k), img.s16(a + 4 * k + 2)) for k in range(n)]
            i += 1 + _unpack_words(vn, vl, n)
            continue
        extra = 0
        if cmd == 0x20:                       # STMASK
            mask = img.u32(a)
            extra = 1
        elif cmd in (0x30, 0x31):             # STROW / STCOL
            extra = 4
        elif cmd == 0x4A:                     # MPG
            extra = (num or 256) * 2
        elif cmd in (0x50, 0x51):             # DIRECT
            extra = (imm or 65536) * 4
        elif cmd in (0x14, 0x15, 0x17):       # MSCAL/MSCALF/MSCNT: fim de uma secao de strip
            flush()
        i += 1 + extra
    flush()
    return mesh


# ---------------------------------------------------------------- hierarquia

def ident():
    return [[1.0 if r == c else 0.0 for c in range(4)] for r in range(4)]


def mul(a, b):
    """a*b com vetores-linha (p' = p*M): aplica `a` primeiro, depois `b`."""
    return [[sum(a[r][k] * b[k][c] for k in range(4)) for c in range(4)] for r in range(4)]


def translate_m(t):
    m = ident()
    m[3][0], m[3][1], m[3][2] = t
    return m


def scale_m(s):
    m = ident()
    m[0][0], m[1][1], m[2][2] = s
    return m


def xform(m, p):
    return (p[0] * m[0][0] + p[1] * m[1][0] + p[2] * m[2][0] + m[3][0],
            p[0] * m[0][1] + p[1] * m[1][1] + p[2] * m[2][1] + m[3][1],
            p[0] * m[0][2] + p[1] * m[1][2] + p[2] * m[2][2] + m[3][2])


def walk(img, node, visit, m=None, seen=None, depth=0, lod=0):
    """Percorre a hierarquia chamando visit(node, matriz) em cada OBJECT. `lod`: indice do LOD (0 = mais detalhado)."""
    if m is None:
        m = ident()
    if seen is None:
        seen = set()
    if depth > 64 or not img.ok(node, 4):
        return
    key = (node, tuple(map(tuple, m)))
    if key in seen:
        return
    seen.add(key)
    op = img.u32(node) & 63

    def kids(arr, n, mm):
        for k in range(min(n, 4096)):
            walk(img, img.u32(arr + 4 * k), visit, mm, seen, depth + 1, lod)

    if op == 0:
        visit(node, m)
    elif op == 1:
        kids(node + 0x20, img.u16(node + 8), m)
    elif op == 3:
        t = (img.f32(node + 0x10), img.f32(node + 0x14), img.f32(node + 0x18))
        kids(node + 0x1C, img.u32(node + 8), mul(translate_m(t), m))
    elif op == 4:
        rm = [[img.f32(node + 0x10 + 16 * r + 4 * c) for c in range(4)] for r in range(4)]
        kids(node + 0x50, img.u32(node + 8), mul(rm, m))
    elif op == 30:
        t = (img.f32(node + 0x10), img.f32(node + 0x14), img.f32(node + 0x18))
        s = (img.f32(node + 0x20), img.f32(node + 0x24), img.f32(node + 0x28))
        kids(node + 0x2C, img.u32(node + 8), mul(mul(scale_m(s), translate_m(t)), m))
    elif op == 6:
        which, n = img.u8(node + 8), img.u8(node + 0xB)
        if which < n:
            walk(img, img.u32(node + 0xC + 4 * which), visit, m, seen, depth + 1, lod)
    elif op == 2:
        n = img.u32(node + 4)
        if n:
            walk(img, img.u32(node + 0x20 + 16 * min(lod, n - 1) + 8), visit, m, seen, depth + 1, lod)


def main():
    img = Image.load(sys.argv[1])
    print("imagem: %d bytes, %d raizes" % (len(img.d), len(img.roots())))
    for r in img.roots():
        w = img.u32(r)
        print("  raiz %06x op=%s id=%d" % (r, OPNAMES.get(w & 63, w & 63), w >> 18))


if __name__ == "__main__":
    main()

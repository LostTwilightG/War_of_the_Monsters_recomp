import struct, sys
for name in sys.argv[1:]:
    ram = open(name, 'rb').read()
    u = lambda a: struct.unpack_from('<I', ram, a)[0]
    db = 0x7d3620; num = u(0x6f8df8)
    same = off = other = 0; offs = {}
    for i in range(1, num):
        it = u(db + 4 * i)
        if not it or not u(it + 0x14): continue
        cs = u(it + 0xC)
        if not cs or not (0x100000 < cs < 0x2000000): continue
        hh = u(cs)
        if not (0x100000 < hh < 0x2000000): continue
        idx = (u(hh) >> 7) & 0x7FF
        d = idx - i
        offs[d] = offs.get(d, 0) + 1
    print(name.split('/')[-1], 'numInteractives', num, 'hat idx - array index histogram:', dict(sorted(offs.items())))

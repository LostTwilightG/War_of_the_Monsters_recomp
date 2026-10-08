import struct, sys
ram = open(sys.argv[1], 'rb').read()
u = lambda a: struct.unpack_from('<I', ram, a)[0]
s8 = lambda a: struct.unpack_from('<b', ram, a)[0]
i32 = lambda a: struct.unpack_from('<i', ram, a)[0]
f = lambda a: struct.unpack_from('<f', ram, a)[0]
db = 0x7d3620
def show(name, cs):
    print('%-14s cs=%#x epNode=%#x drawMe=%d testColl=%d inFov=%#x  cellCol[%d..%d] cellRow[%d..%d] pos=(%.0f,%.0f,%.0f)' % (
        name, cs, u(cs), s8(cs + 0xC), s8(cs + 0xD), ram[cs + 0xE], i32(cs + 0x98), i32(cs + 0xA0), i32(cs + 0x9C), i32(cs + 0xA4), f(cs + 0x10), f(cs + 0x14), f(cs + 0x18)))
show('player', 0x7fa820)
for i in (93, 169, 64, 66, 76, 95):
    it = u(db + 4 * i); cs = u(it + 0xC)
    show('interactive#%d' % i, cs)
    hh = u(cs); 
    print('      hierhead[0]=%#x  hat-id bits: type=%#x idx=%d  (it+0xD8=%#x, it+0x14=%#x)' % (u(hh), u(hh) & 0xFFFC0000, (u(hh) >> 7) & 0x7FF, u(it + 0xD8), u(it + 0x14)))
# how many pickup cs are inside the grid (min<=max)?
inside = outside = 0
for i in range(1, 183):
    it = u(db + 4 * i)
    if not it or not u(it + 0x14): continue
    cs = u(it + 0xC)
    if not cs: continue
    if i32(cs + 0x98) <= i32(cs + 0xA0): inside += 1
    else: outside += 1
print('pickup cs inside grid:', inside, 'outside grid (never inserted / removed):', outside)

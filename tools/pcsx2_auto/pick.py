import struct, sys, math
ram = open(sys.argv[1], 'rb').read()
u = lambda a: struct.unpack_from('<I', ram, a)[0]
f = lambda a: struct.unpack_from('<f', ram, a)[0]
vec = lambda a: tuple(f(a + 4 * i) for i in range(4))
game = u(0x6f81f8)
n = u(game + 0x1203D4)
print('game %#x numSlots %d' % (game, n))
player = None
for i in range(n):
    s = game + 0xB80 + i * 0x11190
    pn = u(s + 0x18)
    if pn:
        print('slot', i, 'playerNum', pn, 'cs %#x' % u(s + 0xC), 'state %#x' % u(s + 0x34))
    if pn == 1 and player is None:
        player = s
s = player
cs = u(s + 0xC)
trans = vec(cs + 0x10)
rows = [vec(cs + 0x20 + 0x10 * i) for i in range(3)]
sp = s + 0xDC70
print('player slot %#x cs %#x trans %s' % (s, cs, tuple(round(x, 2) for x in trans[:3])))
print('StatePickUp %#x owner(+0xC) %#x  (should be %#x)' % (sp, u(sp + 0xC), s))
Al = vec(sp + 0x40); Bl = vec(sp + 0x50); rad = f(sp + 0x4C)
print('A local', [round(x, 3) for x in Al[:3]], 'radius(+0x4C)', rad, 'B local', [round(x, 3) for x in Bl[:3]])
rot = lambda v: tuple(sum(rows[r][c] * v[r] for r in range(3)) for c in range(3))
A = tuple(a + b for a, b in zip(rot(Al), trans[:3])); B = tuple(a + b for a, b in zip(rot(Bl), trans[:3]))
print('A world', [round(x, 2) for x in A], 'B world', [round(x, 2) for x in B])
print('monster state ptr %#x, StatePickUp is state? %s' % (u(s + 0x34), u(s + 0x34) == sp))
num = u(0x6f8df8)
db = 0x7d3620
print('numInteractives', num)
d2 = lambda p, q: sum((p[i] - q[i]) ** 2 for i in range(3))
rows_out = []
for i in range(1, num):
    it = u(db + 4 * i)
    if not it:
        continue
    pk = u(it + 0x14)
    if not pk:
        continue
    icsp = u(it + 0xC)
    if not icsp:
        continue
    pos = vec(icsp + 0x10)
    r = f(it + 0x9C)
    rows_out.append((d2(pos, A) ** 0.5, i, it, pk, pos, r, u(it + 0xD8), u(icsp + 0xC) & 0xff))
rows_out.sort()
print('pickup interactives (with +0x14 != 0): %d; nearest 12 by distance to A:' % len(rows_out))
for d, i, it, pk, pos, r, flag, drawMe in rows_out[:12]:
    ok = (A_w := rad) + r
    print('  #%d it=%#x dist=%.1f radius=%.2f need dist<=%.2f  D8=%#x drawMe=%d pos=%s' % (i, it, d, r, rad + r, flag, drawMe, [round(x, 1) for x in pos[:3]]))
print('highlight[0..3]:', [hex(u(0x2aec60 + 4 * i)) for i in range(4)])

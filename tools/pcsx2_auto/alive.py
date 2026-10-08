"""alive.py <elf> [seconds]: dump EE RAM twice and report whether g_frame (the rtMain frame counter) advanced, plus the game mode / slot states.
Exit 0 = running, 1 = frozen/unreadable."""
import subprocess, struct, sys, time, os
from elftools.elf.elffile import ELFFile

elf = sys.argv[1]
gap = float(sys.argv[2]) if len(sys.argv) > 2 else 2.0
here = os.path.dirname(os.path.abspath(__file__))
sym = {s.name: s['st_value'] for s in ELFFile(open(elf, 'rb')).get_section_by_name('.symtab').iter_symbols()}
gf = sym['g_frame']


def dump(path):
    return subprocess.run([sys.executable, os.path.join(here, 'ramdump.py'), elf, path], capture_output=True).returncode == 0


a, b = os.path.join(here, 'alive_a.bin'), os.path.join(here, 'alive_b.bin')
if not dump(a):
    print('no emulator RAM'); sys.exit(1)
time.sleep(gap)
if not dump(b):
    print('no emulator RAM'); sys.exit(1)
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
ra, rb = open(a, 'rb').read(), open(b, 'rb').read()
game = u(rb, 0x6f81f8)
print('g_frame', u(ra, gf), '->', u(rb, gf), '| game %#x mode %d level %d slots %d' % (game, u(rb, game + 0x1203C8), u(rb, game + 0x1203D0), u(rb, game + 0x1203D4)))
for i in range(u(rb, game + 0x1203D4)):
    s = game + 0xB80 + i * 0x11190
    if u(rb, s + 0x18):
        print(' slot', i, 'playerNum', u(rb, s + 0x18), 'state', hex(u(rb, s + 0x34)), 'dead', rb[s + 0xE8], 'F6', rb[s + 0xF6])
os.remove(a); os.remove(b)
sys.exit(0 if u(rb, gf) != u(ra, gf) else 1)

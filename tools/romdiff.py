"""Show word-level differences between the retail ROM and the rebuilt one."""
import sys
a = open(sys.argv[1] if len(sys.argv) > 1 else 'SCUS_971.97.rom', 'rb').read()
b = open(sys.argv[2] if len(sys.argv) > 2 else 'build/SCUS_971.97.rom', 'rb').read()
print('sizes', hex(len(a)), hex(len(b)))
diffs = [i for i in range(0, min(len(a), len(b)), 4) if a[i:i + 4] != b[i:i + 4]]
print('differing words:', len(diffs))
for i in diffs[:40]:
    print(f'  {i + 0x100000:08X}: {a[i:i + 4].hex()} -> {b[i:i + 4].hex()}')

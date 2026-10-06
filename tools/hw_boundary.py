#!/usr/bin/env python3
"""Map the PS2 hardware boundary: which game/common TUs touch hardware directly.

Per TU (from the split asm), counts:
  sce     calls into SCE libraries (sce*, libkernl syscalls such as FlushCache/SyncDCache/iSignalSema...)
  io      %hi/lui of hardware register space (0x1000xxxx EE regs/DMA/VIF/GIF/IPU, 0x1200xxxx GS)
  cop2    VU0 macro-mode instructions (v*, qmfc2/qmtc2, lqc2/sqc2, ctc2/cfc2, vcallms)
  spad    scratchpad addresses (0x70000000)
  snd     calls into the 989snd library (snd_*)
Writes config/hw_boundary.csv and prints a summary sorted by total.
"""
import csv
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SYSCALLS = {'FlushCache', 'SyncDCache', 'iSyncDCache', 'InvalidDCache', 'iInvalidDCache', 'SignalSema', 'iSignalSema',
            'WaitSema', 'PollSema', 'CreateSema', 'DeleteSema', 'CreateThread', 'StartThread', 'SleepThread',
            'WakeupThread', 'iWakeupThread', 'AddIntcHandler', 'AddDmacHandler', 'EnableIntc', 'DisableIntc',
            'EnableDmac', 'DisableDmac', 'ExitHandler', 'GsPutIMR', 'SetGsCrt', 'ChangeThreadPriority',
            'GetThreadId', 'RotateThreadReadyQueue', 'SetAlarm', 'ReleaseAlarm', 'DIntr', 'EIntr'}
CALL = re.compile(r'\bjal\s+([A-Za-z_][\w$.]*)')
LUI = re.compile(r'\blui\s+\$\w+,\s*(?:\(0x([0-9A-F]+) >> 16\)|(0x[0-9A-Fa-f]+))')
COP2 = re.compile(r'\b(v(?:add|sub|mul|madd|msub|max|mini|opmula|opmsub|ftoi\d|itof\d|div|sqrt|rsqrt|abs|clip|nop|mr32|mfir|mtir|ilwr|iswr|lqi|sqi|lqd|sqd|waitq|callms|callmsr|move|iadd|isub|iand|ior)\w*|qmfc2|qmtc2|lqc2|sqc2|ctc2|cfc2)\b')


def scan(text):
    c = dict(sce=0, io=0, cop2=0, spad=0, snd=0)
    for name in CALL.findall(text):
        if name.startswith('sce') or name in SYSCALLS:
            c['sce'] += 1
        elif name.startswith('snd_'):
            c['snd'] += 1
    for a, b in LUI.findall(text):
        hi = int(a, 16) >> 16 if a else int(b, 16)
        if hi in (0x1000, 0x1001, 0x1002, 0x1003, 0x1100, 0x1200):
            c['io'] += 1
        elif hi == 0x7000:
            c['spad'] += 1
    c['cop2'] = len(COP2.findall(text))
    return c


def main():
    rows = []
    for r in csv.DictReader(open(ROOT / 'config/tus.csv')):
        name = r['name']
        if not name.startswith(('game/', 'common/')):
            continue
        files = [ROOT / f'asm/{name}.s'] + sorted((ROOT / f'asm/nonmatchings/{name}').glob('*.s'))
        text = '\n'.join(f.read_text(errors='replace') for f in files if f.exists())
        c = scan(text)
        size = int(r['end'], 16) - int(r['start'], 16)
        rows.append((name, size, c['sce'], c['io'], c['cop2'], c['spad'], c['snd']))
    with open(ROOT / 'config/hw_boundary.csv', 'w', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['tu', 'bytes', 'sce_calls', 'hw_regs', 'cop2', 'scratchpad', 'snd_calls'])
        w.writerows(rows)
    touched = [r for r in rows if any(r[2:])]
    print(f'{len(touched)}/{len(rows)} game/common TUs touch hardware/SDK directly; '
          f'{sum(r[1] for r in touched)} of {sum(r[1] for r in rows)} bytes')
    print(f"{'tu':28} {'bytes':>7} {'sce':>4} {'io':>4} {'cop2':>5} {'spad':>4} {'snd':>4}")
    for r in sorted(touched, key=lambda r: -(r[2] + r[3] + r[4] + r[5] + r[6]))[:40]:
        print(f'{r[0]:28} {r[1]:7} {r[2]:4} {r[3]:4} {r[4]:5} {r[5]:4} {r[6]:4}')


if __name__ == '__main__':
    main()

#!/usr/bin/env python3
"""Update the decompilation progress block of README.md and docs/progress.svg from config/status.csv.

Run by the versioned pre-commit hook (.githooks/pre-commit); can also be run by hand:
    python tools/update_readme.py            # works on Windows Python 3 and inside WSL, stdlib only

Definitions follow tools/progress.py (the wotm-hud mod parses that script's output, which is untouched):
done = status `matched` + `equivalent`; areas are `game`, `common` (the engine) and `libs` (everything else:
SDK/gcc/newlib/crt0). `hw` (PS2 hardware code that the port replaces) is counted as "ignored", with libs.

status.csv is written by tools/progress.py (needs elftools and disc/SCUS_971.97, so it normally runs in WSL).
Refreshing it takes ~12 s, so it is opt-in here:  WOTM_REFRESH=1 git commit ...
Never fails the commit: any error prints a warning and exits 0.
"""
import csv
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CSV = ROOT / 'config' / 'status.csv'
README = ROOT / 'README.md'
SVG = ROOT / 'docs' / 'progress.svg'
START, END = '<!-- PROGRESS:START -->', '<!-- PROGRESS:END -->'
DONE = ('matched', 'equivalent')


def warn(msg):
    print(f'update_readme: {msg}', file=sys.stderr)


def refresh_status():
    """Re-run tools/progress.py (via WSL on Windows) so status.csv is current. Best effort."""
    try:
        if os.name == 'nt':
            wsl = shutil.which('wsl')
            if not wsl:
                warn('WOTM_REFRESH set but wsl not found; using committed status.csv')
                return
            r = subprocess.run([wsl, '-d', 'Ubuntu', '--', 'wslpath', '-a', str(ROOT)],
                               capture_output=True, text=True, timeout=30)
            wpath = r.stdout.strip()
            cmd = [wsl, '-d', 'Ubuntu', '--', 'bash', '-c',
                   'cd "$1" && ~/.venvs/wotm/bin/python tools/progress.py >/dev/null', '_', wpath]
        else:
            venv = Path.home() / '.venvs/wotm/bin/python'
            cmd = [str(venv) if venv.exists() else sys.executable, str(ROOT / 'tools/progress.py')]
        r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT, timeout=120)
        if r.returncode:
            warn('progress.py failed, using committed status.csv: ' + (r.stderr.strip().splitlines() or ['?'])[-1])
    except Exception as e:  # noqa: BLE001 - must never fail the commit
        warn(f'could not refresh status.csv ({e}); using committed one')


def collect():
    """-> {area: {'f': {state: n}, 'b': {state: bytes}}}"""
    stats = {a: {'f': {}, 'b': {}} for a in ('game', 'common', 'libs')}
    with open(CSV, newline='', encoding='utf-8') as f:
        rd = csv.DictReader(f)
        if not {'tu', 'size'} <= set(rd.fieldnames or ()):
            raise ValueError('status.csv has an unexpected header')
        for row in rd:
            tu = row['tu']
            area = tu.split('/')[0] if '/' in tu else tu
            area = area if area in ('game', 'common') else 'libs'
            st = row.get('state') or row.get('status')
            s = stats[area]
            s['f'][st] = s['f'].get(st, 0) + 1
            s['b'][st] = s['b'].get(st, 0) + int(row['size'])
    return stats


def pct(a, b):
    return 100.0 * a / b if b else 0.0


def fmt_pct(x):
    return f'{x:.1f}'.replace('.', ',')


def numbers(stats):
    n = {}
    for area, s in stats.items():
        tf, tb = sum(s['f'].values()), sum(s['b'].values())
        n[area] = dict(total=tf, bytes_total=tb,
                       done=sum(s['f'].get(x, 0) for x in DONE), bytes_done=sum(s['b'].get(x, 0) for x in DONE),
                       hw=s['f'].get('hw', 0), asm=s['f'].get('asm', 0))
    return n


def kb(b):
    return f'{b / 1024:.1f}'.replace('.', ',') + ' KB'


def make_svg(n):
    g, c, lb = n['game'], n['common'], n['libs']
    total = g['total'] + c['total'] + lb['total']
    ignored = lb['total'] + g['hw'] + c['hw']
    pending = total - g['done'] - c['done'] - ignored
    segs = [('#2f81f7', g['done']), ('#2da44e', c['done']), ('#c9d1d9', pending), ('#8b949e', ignored)]
    W, X0, BW, BY, BH = 720, 20, 680, 54, 28
    out = []
    x = float(X0)
    for i, (col, v) in enumerate(segs):
        w = BW * v / total if total else 0
        if v and w < 2:
            w = 2
        if i == len(segs) - 1:
            w = X0 + BW - x  # absorb rounding so the bar ends exactly at the right edge
        if w > 0:
            out.append(f'<rect x="{x:.2f}" y="{BY}" width="{w:.2f}" height="{BH}" fill="{col}"/>')
        x += w
    bar = '\n  '.join(out)
    done = g['done'] + c['done']
    rows = [
        ('#2f81f7', f'game: {g["done"]}/{g["total"]} ({fmt_pct(pct(g["done"], g["total"]))}%)'),
        ('#2da44e', f'common / engine: {c["done"]}/{c["total"]} ({fmt_pct(pct(c["done"], c["total"]))}%)'),
        ('#c9d1d9', f'pendente (asm): {pending}'),
        ('#8b949e', f'ignorado: {ignored} (libs {lb["total"]} + hw {g["hw"] + c["hw"]})'),
    ]
    leg = []
    for i, (col, txt) in enumerate(rows):
        lx = 20 + (i % 2) * 350
        ly = 110 + (i // 2) * 24
        leg.append(f'<rect x="{lx}" y="{ly - 11}" width="12" height="12" rx="2" fill="{col}"/>'
                   f'<text x="{lx + 20}" y="{ly}" font-size="13" fill="#24292f">{txt}</text>')
    legend = '\n  '.join(leg)
    title = (f'War of the Monsters: {done} de {total} funcoes decompiladas '
             f'({fmt_pct(pct(done, total))}%), {ignored} ignoradas')
    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="150" viewBox="0 0 {W} 150" role="img" aria-label="{title}">
  <title>{title}</title>
  <rect x="0.5" y="0.5" width="{W - 1}" height="149" rx="8" fill="#f6f8fa" stroke="#d0d7de"/>
  <g font-family="-apple-system, 'Segoe UI', Helvetica, Arial, sans-serif">
  <text x="20" y="30" font-size="16" font-weight="600" fill="#24292f">Progresso da decompilação: {done} de {total} funções ({fmt_pct(pct(done, total))}%)</text>
  <rect x="{X0}" y="{BY}" width="{BW}" height="{BH}" rx="4" fill="#c9d1d9"/>
  <clipPath id="r"><rect x="{X0}" y="{BY}" width="{BW}" height="{BH}" rx="4"/></clipPath>
  <g clip-path="url(#r)">
  {bar}
  </g>
  {legend}
  </g>
</svg>
'''


def make_block(n):
    g, c, lb = n['game'], n['common'], n['libs']
    total = g['total'] + c['total'] + lb['total']
    hw = g['hw'] + c['hw']
    ignored = lb['total'] + hw

    def row(label, a):
        return (f'| {label} | {a["done"]} | {a["total"]} | {fmt_pct(pct(a["done"], a["total"]))}% | '
                f'{kb(a["bytes_done"])} de {kb(a["bytes_total"])} ({fmt_pct(pct(a["bytes_done"], a["bytes_total"]))}%) |')

    return f'''{START}
## Progresso da decompilação

![Progresso da decompilação](docs/progress.svg)

| Área | Feitas | Total | % | Bytes |
|---|---:|---:|---:|---|
{row('`game`', g)}
{row('`common` / engine', c)}

"Feita" = função `matched` (byte a byte igual ao original) ou `equivalent` (C++ equivalente, ainda sem bater).
Ignoradas: **{ignored}** funções, sendo {lb["total"]} de bibliotecas/SDK (`libs`: gcc, newlib, sce, lib989snd, crt0) e {hw} de código de hardware do PS2 (`hw`) que o port substitui.
Gerado por `tools/update_readme.py` a partir de `config/status.csv` (veja `tools/progress.py`); total de {total} funções.
{END}'''


def read_text(p):
    with open(p, newline='', encoding='utf-8') as f:
        return f.read()


def write_text(p, s):
    with open(p, 'w', newline='', encoding='utf-8') as f:
        f.write(s)


def update_readme(block):
    text = read_text(README) if README.exists() else ''
    eol = '\r\n' if text.count('\r\n') > text.count('\n') / 2 and '\r\n' in text else '\n'
    new_block = block.replace('\n', eol)
    a, b = text.find(START), text.find(END)
    if a != -1 and b > a:
        new = text[:a] + new_block + text[b + len(END):]
    else:
        head = text + (eol if text and not text.endswith('\n') else '')
        new = head + (eol if head else '') + new_block + eol
    if new != text:
        write_text(README, new)


def main():
    if os.environ.get('WOTM_REFRESH') == '1':
        refresh_status()
    if not CSV.exists():
        warn('config/status.csv not found; nothing to do')
        return
    n = numbers(collect())
    SVG.parent.mkdir(exist_ok=True)
    svg = make_svg(n)
    if not SVG.exists() or read_text(SVG) != svg:
        write_text(SVG, svg)
    update_readme(make_block(n))


if __name__ == '__main__':
    try:
        main()
    except Exception as e:  # noqa: BLE001 - never block a commit
        warn(f'failed: {e!r}')
    sys.exit(0)

#!/usr/bin/env python3
"""Switch translation units from asm to C++ with INCLUDE_ASM stubs. Run inside WSL.

    python3 tools/new_tu.py common/ctx common/light ...

For each TU: flips its text segment in SCUS_971.97.yaml to `cpp` (and rodata to `.rodata`), lists it in config/decomp_tus.txt, re-runs the
splat split once, then writes src/<tu>.cpp with one INCLUDE_ASM per function in address order (skipped if the file
already exists). Afterwards run `ninja` to check that the ROM still matches.
"""
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
tus = sys.argv[1:]
yaml = (ROOT / 'SCUS_971.97.yaml').read_text()
listed = (ROOT / 'config/decomp_tus.txt').read_text()
for tu in tus:
    pat = re.compile(r'(- \[0x[0-9A-Fa-f]+, )asm(, %s\])' % re.escape(tu))
    if not pat.search(yaml):
        print(f'{tu}: no asm text segment in yaml (already cpp?)')
    yaml = pat.sub(r'\1cpp\2', yaml)
    # splat's rodata migration needs the dotted type
    yaml = re.sub(r'(- \[0x[0-9A-Fa-f]+, )rodata(, %s\])' % re.escape(tu), r'\1.rodata\2', yaml)
    if not re.search(r'^%s(\s|$)' % re.escape(tu), listed, re.M):
        listed = listed.rstrip('\n') + f'\n{tu}\n'
(ROOT / 'SCUS_971.97.yaml').write_text(yaml)
(ROOT / 'config/decomp_tus.txt').write_text(listed)
subprocess.run([sys.executable, 'configure.py', '--split'], check=True, cwd=ROOT, stdout=subprocess.DEVNULL)

for tu in tus:
    out = ROOT / f'src/{tu}.cpp'
    if subprocess.run(['git', 'ls-files', '--error-unmatch', str(out.relative_to(ROOT))], cwd=ROOT, capture_output=True).returncode == 0:
        continue  # already hand-written (splat writes its own stubs, with real bodies for trivial functions; replace those)
    funcs = []
    for f in (ROOT / 'asm/nonmatchings' / tu).glob('*.s'):
        m = re.search(r'glabel [^
]*
\s*/\* [0-9A-F]+ ([0-9A-F]{8}) ', f.read_text())  # not the rodata lines above it
        if not m:
            print(f'WARNING {tu}: {f.name} has no code (data-only, e.g. a vtable): add its INCLUDE_ASM by hand, between the '
                  'functions whose rodata surrounds its address (see tools/wsl/check.sh)')
            continue
        funcs.append((int(m.group(1), 16), f.stem))
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text('#include "common.h"\n\n' + ''.join(f'INCLUDE_ASM("asm/nonmatchings/{tu}", {n});\n' for _, n in sorted(funcs)))
    print(f'{out}: {len(funcs)} functions')

#!/usr/bin/env python3
"""Run tools/difftest.py on every NON_MATCHING (equivalent) function of the given TUs.

    python3 tools/difftest_all.py game/Monster game/TheGame --runs 40

The argument list is guessed from the gcc 2.95 mangled name and the return type from the C++ definition. Functions that take
or return structs by value, or reference nested classes, are listed as 'unsupported'.
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import difftest  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
from difftest import parse_params, spec_from_mangled  # noqa: E402


def nm_functions(path):
    text = path.read_text()
    out = []
    for m in re.finditer(r'#ifdef NON_MATCHING\n(?:/\*.*?\*/\n)?(.*?)#else\nINCLUDE_ASM\("[^"]+", (\S+)\);\n#endif', text, re.S):
        body, mangled = m.group(1), m.group(2)
        sig = re.search(r'^([^\n(#/]*?)[\w:~]+\(', body, re.M)
        ret = 'void'
        if sig:
            r = sig.group(1).replace('static ', '').replace('const', '').strip()
            ret = 'void' if r in ('', 'void') else ('float' if r == 'float' else 'int')
        out.append((mangled, ret))
    return out


def main():
    runs = 40
    tus = []
    a = sys.argv[1:]
    while a:
        x = a.pop(0)
        if x == '--runs':
            runs = int(a.pop(0))
        else:
            tus.append(x)
    total = {'agree': 0, 'differ': 0, 'skipped': 0}
    for tu in tus:
        funcs = nm_functions(ROOT / f'src/{tu}.cpp')
        if not funcs:
            continue
        b = difftest.Bench(tu)
        if b.missing:
            print(f'{tu}: WARNING unresolved symbols (calls into them go to address 0): {b.missing}')
        for mangled, ret in funcs:
            spec = spec_from_mangled(mangled)
            if spec is None:
                print(f'{tu}: {mangled}: unsupported signature')
                continue
            if mangled not in b.asym or mangled not in b.sym:
                print(f'{tu}: {mangled}: not available')
                continue
            try:
                ok, bad, sk = b.test(mangled, spec, ret, runs, objsize=(0x122000 if tu.endswith('TheGame') else 0x20000))
            except SystemExit as e:
                print(f'{tu}: {mangled}: {e}')
                continue
            status = 'DIFFERS' if bad else ('agree' if ok else 'no run completed')
            print(f'{tu}: {mangled} [{",".join(spec)} -> {ret}]: {status} ({ok} ok, {bad} differ, {sk} skipped)', flush=True)
            total['agree'] += ok
            total['differ'] += bad
            total['skipped'] += sk
    print('total', total)


main()

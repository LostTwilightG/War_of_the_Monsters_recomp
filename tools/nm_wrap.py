#!/usr/bin/env python3
"""Wrap a hand-written function that does not match yet in `#ifdef NON_MATCHING`, falling back to INCLUDE_ASM.

    python3 tools/nm_wrap.py src/common/ColGrid.cpp removeCs__13ColGridCsListP3_cs "76% of words; loop peeled in retail"

The function is found by its unmangled name (text before the first `__`, or the whole name for C symbols) followed by
`(` at the start of a definition line, and runs to the next line that is exactly `}`.
"""
import re
import sys
from pathlib import Path

path, mangled, note = sys.argv[1:4]
qualified = sys.argv[4] if len(sys.argv) > 4 else None  # e.g. StaminaMeter::init when several classes share a method name
src = Path(path)
tu = src.with_suffix('').as_posix().split('src/', 1)[1]
name = mangled.split('__')[0] if '__' in mangled[1:] else mangled
if mangled.startswith('__'):
    name = mangled[2:].split('_')[0]
if qualified:
    name = qualified
lines = src.read_text().split('\n')
pat = re.compile(r'^[A-Za-z_].*[ *&]%s\(' % re.escape(name)) if qualified else re.compile(r'^[A-Za-z_].*[ :*&]%s\(' % re.escape(name))
start = next(i for i, l in enumerate(lines) if pat.match(l) and not l.rstrip().endswith(';'))
# include a preceding `extern "C"`-style linkage/`__asm__` marker lines directly above
while start > 0 and lines[start - 1].startswith('__asm__('):
    start -= 1
end = next(i for i in range(start, len(lines)) if lines[i] == '}')
body = lines[start:end + 1]
new = ['#ifdef NON_MATCHING', f'/* {note} */'] + body + ['#else', f'INCLUDE_ASM("asm/nonmatchings/{tu}", {mangled});', '#endif']
lines[start:end + 1] = new
src.write_text('\n'.join(lines))
print(f'wrapped {mangled} ({end - start + 1} lines)')

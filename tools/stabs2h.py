"""Convert the GNU stabs types in hieri.cpp's .mdebug into C++ headers.

Usage: python tools/stabs2h.py disc/SCUS_971.97 include

Writes <out>/ps2_sdk_types.h (SCE SDK / register bitfields) and <out>/hieri_types.h (engine types).
Methods (constructors, operator=) are dropped; only data members, enums and typedefs are kept.
Every struct gets a size check against the size stored in the stabs.
"""
import os, re, sys
sys.path.insert(0, os.path.dirname(__file__))
import mdebug_dump as m

BS = chr(92)
FILE = 'hieri.cpp'
ENGINE_FIRST = 'ptrdiff_t'  # first entry that belongs to the engine headers

BUILTIN = {1: 'int', 2: 'char', 3: 'long', 4: 'unsigned', 5: 'unsigned long', 6: 'long long',
           7: 'unsigned long long', 8: 'short', 9: 'unsigned short', 10: 'signed char',
           11: 'unsigned char', 12: '__int128_t', 13: '__uint128_t', 14: 'float', 15: 'double',
           16: 'long double', 21: 'bool', 22: 'void'}


def load_entries(path):
    d, h = m.load(path)
    fd = next(f for f in m.fdrs(d, h) if f['name'] == FILE)
    out, cur = [], ''
    for n, v, st, sc, idx in list(m.syms(d, h, fd))[4:]:
        if st != 0:
            continue
        cur += n
        if cur.endswith(BS):
            cur = cur[:-1]
            continue
        out.append(cur)
        cur = ''
    return out


class P:
    def __init__(self, s, types):
        self.s, self.i, self.types = s, 0, types

    def peek(self):
        return self.s[self.i] if self.i < len(self.s) else ''

    def take(self, n=1):
        r = self.s[self.i:self.i + n]
        self.i += n
        return r

    def until(self, ch):
        j = self.s.index(ch, self.i)
        r = self.s[self.i:j]
        self.i = j + 1
        return r

    def num(self):
        mo = re.compile(r'-?\d+').match(self.s, self.i)
        self.i = mo.end()
        return int(mo.group())

    def type(self):
        """Parse a type; returns a node: ('ref', n) | ('ptr'|'ref&', node) | ('arr', n, node) | ..."""
        n = self.num()
        if self.peek() != '=':
            return ('ref', n)
        self.take()
        node = self.defn(n)
        self.types[n] = node
        return ('ref', n)

    def defn(self, n):
        c = self.take()
        if c.isdigit() or c == '-':
            self.i -= 1
            return self.type()
        if c == '*':
            return ('ptr', self.type())
        if c == '&':
            return ('lref', self.type())
        if c == 'f':
            return ('func', self.type())
        if c == 'a':
            assert self.take() == 'r'
            self.num(); self.take()  # index type + ';'
            lo = self.num(); self.take()
            hi = self.num(); self.take()
            return ('arr', hi - lo + 1, self.type())
        if c == 'r':
            self.until(';'); self.until(';'); self.until(';')
            return ('builtin', n)
        if c == '@':
            assert self.take() == 's'
            self.until(';'); self.until(';')
            return ('bool',)
        if c == 'x':
            kind = self.take()
            return ('fwd', kind, self.until(':'))
        if c in 'su':
            return self.struct('struct' if c == 's' else 'union')
        if c == 'e':
            items = []
            while self.peek() != ';':
                nm = self.until(':')
                items.append((nm, self.num()))
                self.take()  # ','
            self.take()
            return ('enum', items)
        raise ValueError(f'unknown type code {c!r} at {self.i}: {self.s[max(0, self.i - 20):self.i + 30]}')

    def struct(self, kind):
        size = self.num()
        fields = []
        while True:
            if self.peek() == ';':
                self.take()
                break
            name = self.until(':')
            if self.peek() == ':':  # method group
                self.take()
                self.methods()
                continue
            if self.peek() == '/':
                raise ValueError('static member: ' + self.s[self.i - 30:self.i + 40])
            t = self.type()
            assert self.take() == ','
            off = self.num(); self.take()
            bits = self.num(); self.take()
            fields.append((name, t, off, bits))
        return (kind, size, fields)

    def methods(self):
        while True:
            mo = re.compile(r'[^;:]*').match(self.s, self.i)  # method type, ends at ';:' or ':'
            txt = mo.group()
            self.i = mo.end()
            if self.peek() == ';':
                self.take()
            for mo in re.finditer(r'(\d+)=([*&])(\d+)', txt):
                self.types[int(mo.group(1))] = ('ptr' if mo.group(2) == '*' else 'lref', ('ref', int(mo.group(3))))
            assert self.take() == ':', self.s[self.i - 40:self.i + 40]
            self.until(';')
            self.until('.')
            if self.peek() == ';':
                self.take()
                return


def parse(entries):
    types, names, aliases = {}, {}, []  # names: num -> tag/typedef name
    order = []
    for ei, e in enumerate(entries):
        mo = re.match(r'([^:]*):(Tt|T|t)(\d+)(=?)', e)
        if not mo:
            continue
        name, kind, n = mo.group(1), mo.group(2), int(mo.group(3))
        if name.startswith('t') and len(name) > 1 and name[1].isupper() and kind == 't':
            name = name[1:]  # anonymous typedef'd register structs come out as 'tNAME'
        if mo.group(4):
            p = P(e, types)
            p.i = mo.end(3) + 1
            node = p.defn(n)
            if p.i < len(e) and p.peek() not in '':
                pass
            types[n] = node
            if node[0] in ('struct', 'union', 'enum') and n not in names:
                names[n] = name
                order.append((name, n))
            elif node[0] == 'fwd':
                aliases.append((name, n, ei))
            elif name not in ('void',) and not (n in BUILTIN):
                aliases.append((name, n, ei))
        elif names.get(n) != name:
            aliases.append((name, n, ei))
    return types, names, order, aliases


class Emitter:
    def __init__(self, types, names, tname, type_align=None, field_align=None):
        self.types, self.names, self.tname = types, names, tname
        self.by_tag = {v: k for k, v in names.items()}
        self.done = set()
        self.type_align = type_align if type_align is not None else {}
        self.field_align = field_align if field_align is not None else {}

    def unref(self, t):
        while t and t[0] == 'ref':
            t = self.types.get(t[1])
        return t

    def builtin_of(self, n):
        """Follow typedef refs down to a builtin type number, or None."""
        while True:
            if n in BUILTIN:
                return n
            t = self.types.get(n)
            if t is None or t[0] != 'ref':
                return None
            n = t[1]

    def base(self, n):
        """C++ spelling of a named/builtin type number (typedef name preferred)."""
        if n in self.names:
            return self.names[n]
        if n in self.tname:
            return self.tname[n]
        if n in BUILTIN:
            return BUILTIN[n]
        t = self.types.get(n)
        if t is None:
            return f'/*type{n}*/int'
        if t[0] == 'ref':
            return self.base(t[1])
        if t[0] == 'bool':
            return 'bool'
        if t[0] == 'fwd':
            return t[2]
        return f'/*type{n}*/int'

    def named(self, n):
        t = self.types.get(n)
        return n in self.names or n in self.tname or n in BUILTIN or t is None

    def decl(self, node, name, indent):
        """Declaration of `name` with stabs type `node` (C declarator syntax)."""
        n = node[1]
        if self.named(n):
            return f'{self.base(n)} {name}'.rstrip()
        t = self.types[n]
        k = t[0]
        if k == 'ref':
            return self.decl(t, name, indent)
        if k in ('ptr', 'lref'):
            star = '*' if k == 'ptr' else '&'
            inner = None if self.named(t[1][1]) else self.unref(self.types.get(t[1][1]))
            if inner and inner[0] in ('func', 'arr'):
                return self.decl(t[1], f'({star}{name})', indent)
            return self.decl(t[1], star + name, indent)
        if k == 'arr':
            return self.decl(t[2], f'{name}[{t[1]}]', indent)
        if k == 'func':
            return self.decl(t[1], f'{name}()', indent)
        if k == 'bool':
            return f'bool {name}'
        if k == 'fwd':
            return f'{t[2]} {name}'
        if k in ('struct', 'union'):
            return self.body(t, indent, n) + f' {name}'
        if k == 'enum':
            return f'int /*anon enum*/ {name}'
        return f'int /*{k}*/ {name}'

    def body(self, t, indent, num=None):
        kind, size, fields = t
        pad = '    ' * (indent + 1)
        lines = [f'{kind} {{']
        for fname, ft, off, bits in fields:
            bn = self.builtin_of(ft[1])
            is_bit = bn is not None and bits != SIZES.get(bn, 4) * 8
            if is_bit:
                d = f'{self.base(ft[1])} {fname} : {bits}'
            else:
                d = self.decl(ft, fname, indent + 1)
                fa = self.field_align.get((num, fname))
                if fa:
                    d += f' __attribute__ ((aligned ({fa})))'
            lines.append(f'{pad}{d};  // 0x{off // 8:X}' + (f'.{off % 8}' if off % 8 or is_bit else ''))
        lines.append('    ' * indent + '}' + (f' __attribute__ ((aligned ({self.type_align[num]})))' if num in self.type_align else ''))
        return '\n'.join(lines)

    def deps(self, t, acc):
        """Named types needed by value (complete type) to define `t`."""
        if t[0] in ('struct', 'union'):
            for _, ft, _, _ in t[2]:
                self.deps(ft, acc)
        elif t[0] == 'arr':
            self.deps(t[2], acc)
        elif t[0] == 'ref':
            n = t[1]
            while True:
                if n in self.names:
                    acc.append(n)
                    return
                tt = self.types.get(n)
                if tt is None:
                    return
                if tt[0] == 'ref':
                    n = tt[1]
                elif tt[0] == 'fwd':
                    if tt[2] in self.by_tag:
                        acc.append(self.by_tag[tt[2]])
                    return
                else:
                    self.deps(tt, acc)
                    return

    def emit(self, n, sink, in_set):
        if n in self.done or n not in in_set:
            return
        self.done.add(n)
        t = self.types[n]
        if t[0] == 'enum':
            return
        acc = []
        self.deps(t, acc)
        for d in acc:
            self.emit(d, sink, in_set)
        name = self.names[n]
        sink.append(f'{self.body(t, 0, n).replace(t[0] + " {", t[0] + " " + name + " {", 1)};\n')
        sink.append(f'typedef char _size_{name}[sizeof({name}) == {t[1]} ? 1 : -1];\n\n')


def infer_alignment(types, names, tname):
    """The stabs hold no alignment attributes; recover them from the field offsets."""
    em = Emitter(types, names, tname)
    type_align, field_align = {}, {}

    def up(x, a):
        return (x + a - 1) // a * a

    def carrier_set(ft, parent, fname, want):
        n = ft[1]
        if n in names or n in tname:
            if type_align.get(n, 0) < want:
                type_align[n] = want
                return True
            return False
        if field_align.get((parent, fname), 0) < want:
            field_align[(parent, fname)] = want
            return True
        return False

    changed = [False]

    def sa(n):
        """(size, align) of type number n"""
        base = (4, 4)
        if n in BUILTIN:
            z = SIZES.get(n, 4)
            base = (z, min(z, 16))
        else:
            t = types.get(n)
            if t is None:
                base = (4, 4)
            elif t[0] == 'ref':
                base = sa(t[1])
            elif t[0] in ('ptr', 'lref', 'func', 'enum'):
                base = (4, 4)
            elif t[0] in ('bool', 'builtin'):
                base = (1, 1)
            elif t[0] == 'fwd':
                base = sa(em.by_tag[t[2]]) if t[2] in em.by_tag else (4, 4)
            elif t[0] == 'arr':
                z, a = sa(t[2][1])
                base = (z * t[1], a)
            else:
                base = layout(t, n)
        a = max(base[1], type_align.get(n, 1))
        return base[0], a

    def layout(t, n):
        kind, size, fields = t
        end, amax = 0, 1
        for fname, ft, off, bits in fields:
            z, a = sa(ft[1])
            bn = em.builtin_of(ft[1])
            if bn is not None and bits != SIZES.get(bn, 4) * 8:
                amax = max(amax, a)
                end = max(end, (off + bits + 7) // 8)
                continue
            req = off // 8
            nat = up(end, a) if kind == 'struct' else 0
            if req > nat:
                want = min(16, req & -req)
                if want > a:
                    changed[0] |= carrier_set(ft, n, fname, want)
                    a = want
            amax = max(amax, a)
            end = max(end, req + z)
        if up(end, amax) < size and n is not None:
            want = min(16, size & -size)
            if want > amax:
                changed[0] |= type_align.get(n, 0) < want
                type_align[n] = max(type_align.get(n, 0), want)
                amax = want
        return (size, amax)

    for _ in range(8):
        changed[0] = False
        for n in names:
            if types[n][0] in ('struct', 'union'):
                sa(n)
        if not changed[0]:
            break
    return type_align, field_align


SIZES = {2: 1, 10: 1, 11: 1, 8: 2, 9: 2, 3: 8, 5: 8, 6: 16, 7: 16, 12: 16, 13: 16, 15: 8, 16: 8, 21: 1}


def main():
    src, out = sys.argv[1], sys.argv[2]
    entries = load_entries(src)
    types, names, order, aliases = parse(entries)
    cut = next(i for i, e in enumerate(entries) if e.startswith(ENGINE_FIRST + ':'))

    for n in [n for n, nm in names.items() if ' ' in nm or nm.startswith('__')]:
        del names[n]  # compiler builtin structs

    # typedef names, skipping the compiler builtins and the tag names themselves
    tname, alias_list = {}, []
    for name, n, ei in aliases:
        if n in names or n in BUILTIN or n <= 25 or ' ' in name or name.startswith('__'):
            continue
        if n in tname:
            continue
        tname[n] = name
        alias_list.append((name, n, ei))

    type_align, field_align = infer_alignment(types, names, tname)
    opaque = sorted({t[2] for t in types.values() if t[0] == 'fwd'} - set(names.values()))

    sections = {'ps2_sdk_types.h': set(), 'hieri_types.h': set()}
    for ei, e in enumerate(entries):
        mo = re.match(r'[^:]*:(?:Tt|T|t)(\d+)=', e)
        if mo and int(mo.group(1)) in names:
            sections['ps2_sdk_types.h' if ei < cut else 'hieri_types.h'].add(int(mo.group(1)))

    for fname, pre, lo, hi in (('ps2_sdk_types.h', '', 0, cut),
                               ('hieri_types.h', '#include "ps2_sdk_types.h"\n', cut, len(entries))):
        nums = sections[fname]
        em = Emitter(types, names, tname, type_align, field_align)
        fwd = [f'{types[n][0]} {names[n]};' for n in sorted(nums) if types[n][0] in ('struct', 'union')]
        enums = [f'enum {names[n]} {{ ' + ', '.join(f'{a} = {b}' for a, b in types[n][1]) + ' };'
                 for n in sorted(nums) if types[n][0] == 'enum']
        if fname == 'hieri_types.h':
            fwd += [f'struct {o};' for o in opaque]
        sink = []
        for n in sorted(nums):
            em.emit(n, sink, nums)
        al = []
        for name, n, ei in alias_list:
            if not lo <= ei < hi:
                continue
            saved = tname.pop(n)
            try:
                al.append(f'typedef {em.decl(("ref", n), name, 0)}'
                          + (f' __attribute__ ((aligned ({type_align[n]})))' if n in type_align else '') + ';')
            finally:
                tname[n] = saved
        guard = fname.replace('.', '_').upper()
        with open(os.path.join(out, fname), 'w', newline='\n') as f:
            f.write(f'// Generated by tools/stabs2h.py from the stabs in {FILE} (.mdebug). Do not edit.\n')
            f.write(f'#ifndef {guard}\n#define {guard}\n\n{pre}\n')
            f.write('\n'.join(fwd) + '\n\n' + '\n'.join(enums) + '\n\n')
            f.write('\n'.join(al) + '\n\n')
            f.write(''.join(sink))
            f.write('#endif\n')
        print(fname, len(nums), 'types', len(al), 'aliases')


if __name__ == '__main__':
    main()

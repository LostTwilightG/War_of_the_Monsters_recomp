"""Recover translation-unit boundaries in .text from the retail ELF.

gcc 2.x emits a local `gcc2_compiled.` label at the start of every TU's .text, followed by
`__gnu_compiled_c` or `__gnu_compiled_cplusplus`. Library TUs (newlib, libgcc, SCE libs) also
have real file names in .mdebug. Game TUs have no names, so we derive one from the dominant class
or function-name prefix.

Writes config/tus.csv: start,end,lang,name,source,nfuncs
"""
import collections, csv, re, sys
from elftools.elf.elffile import ELFFile

sys.path.insert(0, 'tools')
import mdebug_dump

ELF = 'disc/SCUS_971.97'
TEXT_START, TEXT_END = 0x100000, 0x251020
GAME_END = 0x1E5360     # game/ TUs (alphabetical, from C:\CLEAN\MONSTERRT\game)
COMMON_START = 0x1EAA38  # common/ engine TUs (C:\CLEAN\MONSTERRT\COMMON) up to newlib
COMMON_END = 0x22D140

# Hand-picked names for game/common TUs where the heuristic is poor. Real file names are unknown;
# the game TUs are linked in case-insensitive alphabetical order, which guided these picks.
OVERRIDES = {
    0x00100000: 'crt0',
    0x00100100: 'game/ActionDispatch',
    0x00102BB8: 'game/AiAction',
    0x00105BE0: 'game/AiGrapple',
    0x00107A40: 'game/AiReflex',
    0x0010F6F0: 'game/AiSeek',
    0x001213F0: 'game/canyon2',
    0x0012B158: 'game/collision',
    0x0013B410: 'game/Grapple',
    0x0013CD38: 'game/GrappleOHAttack',
    0x0013DD00: 'game/GrappleThrow',
    0x00147B68: 'game/McFile',
    0x00149FC0: 'game/McPage',
    0x0014CA30: 'game/MilitaryFormation',
    0x0014EA90: 'game/MonsterStates',
    0x0015B3E0: 'game/Monster',
    0x00163CF8: 'game/MonsterAnimBlend',
    0x00165AC0: 'game/MonsterInit',
    0x001676C8: 'game/MonsterMc',
    0x0016D208: 'game/MonsterMeters',
    0x0016D7C0: 'game/MonsterTriggers',
    0x0016E158: 'game/MovementStates',
    0x001833C8: 'game/PlantBoss',
    0x0018D378: 'game/sanfran',
    0x001AC3E0: 'game/ShellFinished',
    0x001B1840: 'game/Sound',
    0x001C4A38: 'game/SpecialStates',
    0x001D1070: 'game/threemile',
    0x001DAF08: 'game/uidev',
    0x001EAA38: 'lib989snd/snd',
    0x001EE0C8: 'common/animation',
    0x001F2070: 'common/AnimCurve',
    0x001F6960: 'common/SoundBank',
    0x001F8A58: 'common/disp',
    0x001FE980: 'common/hdHat',
    0x001FEB30: 'common/HdIterator',
    0x001FF5F8: 'common/hdLos',
    0x001FF7F0: 'common/hdPoly',
    0x00200970: 'common/hdPrim',
    0x002017B0: 'common/hdSphere',
    0x00201D60: 'common/hdTraverse',
    0x00209D88: 'common/hieri',
    0x0020F340: 'common/MemoryStack',
    0x0020F440: 'common/MpegMovie',
    0x00210090: 'common/movies',
    0x00210938: 'common/partdraw',
    0x0021E090: 'common/videocb',
    0x0021E3C0: 'common/readbuf',
    0x00227380: 'common/videodec',
    0x0022C488: 'common/ColGrid',
}

# SCE library sources (from .mdebug) -> library directory.
SCE_LIBS = {
    'vu/vu': 'libgraph', 'graphdev': 'libgraph', 'libpad': 'libpad', 'libmc': 'libmc',
    'pack': 'libmpeg', 'mpeg': 'libmpeg', 'init': 'libmpeg', 'defhandler': 'libmpeg', 'var': 'libmpeg',
    'mpc': 'libmpeg', 'csc': 'libmpeg', 'bit': 'libmpeg', 'libipu': 'libipu', 'ipuinit': 'libipu',
    'eecdvd': 'libcdvd',
}
SCE_KERNEL_START = 0x002460C0  # klib.s .. tlbtrap.s: libkernl


def class_of(name):
    """Class from a gcc2-mangled method name: foo__6Pickup..., __12AiPutOutFire (ctor), _$_6Pickup (dtor)."""
    m = re.match(r'^(?:_\$_|.+?__C?|__)(?:Q(\d)|(\d+))', name)
    if not m:
        return None
    if m.group(1):  # nested: Q2 3Foo 3Bar -> Foo
        rest = name[m.end():]
        m2 = re.match(r'_?(\d+)', rest)
        if not m2:
            return None
        n = int(m2.group(1)); return rest[m2.end():m2.end() + n]
    n = int(m.group(2)); rest = name[m.end():]
    return rest[:n] if len(rest) >= n and n else None


def prefix_of(name):
    base = name.split('__')[0] if not name.startswith('__') else ''
    m = re.match(r'^_*([a-z]+|[A-Z][a-z]+)', base)
    return m.group(1) if m else None


def main():
    data, hdr = mdebug_dump.load(ELF)
    md_names = {}
    for fd in mdebug_dump.fdrs(data, hdr):
        if TEXT_START <= fd['adr'] < TEXT_END and (fd['cpd'] or fd['name'].endswith(('.s', '.S'))):
            md_names.setdefault(fd['adr'], fd['name'])

    with open(ELF, 'rb') as f:
        elf = ELFFile(f)
        text_idx = [i for i, s in enumerate(elf.iter_sections()) if s.name == '.text'][0]
        syms = list(elf.get_section_by_name('.symtab').iter_symbols())
    starts = {}
    for s in syms:
        if s['st_shndx'] != text_idx:
            continue
        if s.name == 'gcc2_compiled.':
            starts.setdefault(s['st_value'], 'c?')
        elif s.name == '__gnu_compiled_cplusplus':
            starts[s['st_value']] = 'c++'
        elif s.name == '__gnu_compiled_c':
            starts[s['st_value']] = 'c'
    for a in md_names:
        starts.setdefault(a, 'asm' if md_names[a].endswith(('.s', '.S')) else 'c')
    starts.setdefault(TEXT_START, 'asm')

    funcs = sorted((s['st_value'], s.name) for s in syms
                   if s['st_info']['type'] == 'STT_FUNC' and s['st_shndx'] == text_idx)
    addrs = sorted(starts)
    rows, used = [], collections.Counter()
    for i, a in enumerate(addrs):
        end = addrs[i + 1] if i + 1 < len(addrs) else TEXT_END
        fns = [n for fa, n in funcs if a <= fa < end]
        if not fns and a != TEXT_START:
            continue  # data-only TU (e.g. s_infconst.c); its .text is just alignment padding
        if a in OVERRIDES:
            name, src = OVERRIDES[a], 'manual'
        elif a in md_names:
            name, src = re.sub(r'^(\.\./)+', '', md_names[a]), 'mdebug'
            name = re.sub(r'^src/', '', name)
            name = re.sub(r'\.[^.]+$', '', name)
            if name in ('fp-bit', 'dp-bit'):
                name = f'gcc/{name}'
            elif 'libgcc2' in name and fns:
                name = f'{name}/{fns[0].strip("_")}'
            elif name in SCE_LIBS:
                name = f'sce/{SCE_LIBS[name]}/{name.split("/")[-1]}'
            elif a >= SCE_KERNEL_START and not name.startswith(('newlib', 'gcc')):
                name = f'sce/libkernl/{name}'
            if name.startswith('sce/') and used[name]:
                name = f'{name}_{fns[0]}'  # SCE builds one object per function from a shared source
        else:
            cls = collections.Counter(c for c in map(class_of, fns) if c)
            pre = collections.Counter(p for p in map(prefix_of, fns) if p)
            if cls and cls.most_common(1)[0][1] * 2 >= len(fns):
                name = cls.most_common(1)[0][0]
            elif pre:
                name = pre.most_common(1)[0][0]
            elif cls:
                name = cls.most_common(1)[0][0]
            else:
                name = f'unk_{a:08X}'
            folder = 'game' if a < GAME_END else 'common' if COMMON_START <= a < COMMON_END else 'misc'
            name, src = f'{folder}/{name}', 'guess'
        used[name] += 1
        if used[name] > 1:
            name = f'{name}_{used[name]}'
        rows.append((f'0x{a:08X}', f'0x{end:08X}', starts[a], name, src, len(fns)))

    for i in range(len(rows) - 1):  # skipped data-only TUs fold into the previous TU
        rows[i] = (rows[i][0], rows[i + 1][0]) + rows[i][2:]
    with open('config/tus.csv', 'w', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['start', 'end', 'lang', 'name', 'source', 'nfuncs'])
        w.writerows(rows)
    print(f'{len(rows)} TUs written to config/tus.csv')


if __name__ == '__main__':
    main()

# War of the Monsters (SCUS-97197): análise inicial

## Disco
- Volume `MONSTERS`, preparado por `INCOG INC` (Incognito Entertainment), publicado pela SCEA.
- `SYSTEM.CNF`: `BOOT2 = cdrom0:\SCUS_971.97;1`, `VER = 1.00`, `VMODE = NTSC`.
- ELF principal: `SCUS_971.97` (7,4 MB), SHA1 `1ed7f8457436a9170d8f85386e267369c7fcdd4f`.
- "ROM" gerada pelo splat (PT_LOAD em binário): SHA1 `250e3544aa99a04a2fd45974ff7ea38c31d0fc31`.
- IRX em `MOD/`: 989SND/989ERR (driver de som 989 Studios), LIBSD, MCMAN, MCSERV, MTAPMAN, PADMAN, SIO2MAN e `IOPRP24.IMG`.
- Formatos de asset: `.NGP/.PTR/.RTX/.TEX` (modelos e texturas de nível/monstro), `.AIC/.AIP` (IA),
  `.CHR`, `.MRS`, `.BNK` (som), `.VPK/.VAG` (áudio ADPCM), `.PSS` (vídeo MPEG2).

## ELF
- **Não está stripped**: `.symtab` com 9.669 símbolos, sendo 5.381 funções com tamanho (cobrem 98,8% do `.text`).
- C++ com mangling do **GCC 2.9x** (`regen__6Pickup`, `__tf...`, `_vt$...`) e exceções (`.gcc_except_table`).
- Toolchain da SCE: **ee-gcc 2.9x + newlib**. Libs da Sony versão **2.4.0** (`PsIIlib* 2400`).
- O prólogo salva registradores com `sq`/`lq` (padrão do ee-gcc).
- `.mdebug` (ECOFF + stabs) cobre libgcc, newlib e libs da SCE, e **um arquivo do jogo, `hieri.cpp`, com tipos completos**
  (layouts de struct/classe, como `HierLight`). Use `tools/mdebug_dump.py`.
- `.stab` contém só as linhas do microcode VU (`C:\CLEAN\MONSTERRT\COMMON\*.vsm/*.dsm`).
- `.vutext` (0x251020, 46 KB) e as seções `.DVP.overlay.*` guardam o microcode VU1/VU0.
- 294 classes do jogo nomeadas (Monster, MonsterSound, Ai, Shell, Pickup, TheGame, ...).
- 45 TUs têm construtores estáticos (`_GLOBAL_$I$...`), o que ajuda a achar os limites de arquivo.

## Layout do .text
| Endereço | Conteúdo |
|---|---|
| 0x100000 – ~0x1E5360 | código do jogo |
| ~0x1E5360 – ~0x1EAA00 | libgcc / runtime C++ (tinfo, exception, frame) |
| ~0x1EAA00 – 0x22D140 | mais código do jogo e engine (hier, pkta, ...) |
| 0x22D140 – 0x251020 | newlib (libm/libc), libs da SCE (libpad, libmc, libmpeg, libipu, libcdvd, libkernl) |

## Pendências
- [ ] Identificar a versão exata do compilador (testar ee-gcc 2.95.x / 2.96 no decomp.me com uma função simples).
- [ ] Dividir o `.text` em TUs (subsegments no yaml), separando libs da SCE e newlib (podem ser linkadas como blobs).
- [x] Ambiente Linux/WSL com binutils MIPS; rebuild do asm puro faz match do SHA1.
  Observa��es: `-mabi=o64` no GAS (o spimdisasm usa nomes de registradores o32) e se��es alinhadas a 0x80 (`align: 0x80` no yaml).
- [ ] Exportar tipos de `hieri.cpp` para headers.

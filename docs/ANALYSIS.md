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
- [x] Identificar o compilador: **SN Systems ProDG ee-gcc 2.95.2 (v2.73a) + assembler SN `ps2eeas`**, `-O2` (`-G8` no jogo e na engine). Veja "Toolchain".
- [x] Dividir o `.text` em TUs. São 336, e o rebuild continua fazendo match. Veja "Divisão em TUs" abaixo.
- [x] Dividir `.data`/`.rodata`/`.sdata`/`.sbss`/`.bss`/`.gcc_except_table` por TU (483 faixas). Veja "Divisão dos dados".
- [x] Ambiente Linux/WSL com binutils MIPS; rebuild do asm puro faz match do SHA1.
  Observações: `-mabi=o64` no GAS (o spimdisasm usa nomes de registradores o32) e seções alinhadas a 0x80 (`align: 0x80` no yaml).
- [ ] Exportar tipos de `hieri.cpp` para headers.

## Divisão em TUs
- O gcc 2.x emite um label local `gcc2_compiled.` no início do `.text` de cada TU, seguido de
  `__gnu_compiled_c` ou `__gnu_compiled_cplusplus`, o que dá limites exatos e a linguagem de cada TU.
  As TUs de biblioteca têm o nome real no `.mdebug`.
- `tools/find_tus.py` gera `config/tus.csv`, e `tools/apply_tus.py` reescreve os subsegments no yaml.
- Distribuição: `game/` 112, `common/` 71 (engine, `C:\CLEAN\MONSTERRT\COMMON`), `lib989snd/` 1,
  `gcc/` 21, `newlib/` 77, `sce/` 53 e `crt0`.
- As TUs de `game/` são linkadas em **ordem alfabética, sem diferenciar maiúsculas** (wildcard do makefile).
  Os nomes reais são desconhecidos: a coluna `source` do csv diz se o nome veio do `mdebug`,
  de escolha `manual` (em `OVERRIDES`, guiada pela ordem alfabética) ou de `guess` (classe ou prefixo dominante).
- Ajustes necessários para o match:
  - `subalign: 4`, porque o GAS alinha o `.text` de cada objeto a 16, mas no original as TUs C ficam em 8
    e os `.S` da newlib em 4.
  - `reloc_addrs.txt` (gerado por `tools/gen_relocs.py`) força imediatos crus em 1.606 pares `%hi/%lo`
    que o spimdisasm resolvia para o meio do `.text`. São offsets de struct grande (`lui 0x12`), como
    `0x120380`, e não ponteiros.
- `tools/symdiff.py` lista os símbolos que mudaram de endereço no ELF gerado, útil quando o checksum falha.

## Toolchain
- Teste de referência: `zipCrc32__FUlPCUcl` (`common/zip`) é o `crc32` do zlib e dá match exato
  (fonte em `tools/cctest/crc32_variants/localptr2.cpp`; o original usa um ponteiro local `tab`
  declarado depois do teste `buf == 0`).
- Todas as builds Sony 2.95.x (273a/274/3-107/3-114/3-136) e a SN v2.73a geram o **mesmo** `.s` aqui.
  O que decide é o **assembler**: o `ps2eeas` da SN expande `dli r,0xffffffff` como
  `addiu r,$0,-1; dsrl32 r,r,0`, e o GNU as (antigo ou moderno) usa `lui 0xffff`. Os símbolos
  `sn_reg_frame`/`sn_dereg_frame` (libsn) confirmam o ProDG.
- O `ps2eeas` não entende a sintaxe GNU do splat. Por isso o build compila com `-S`, reescreve as macros
  do jeito da SN com `tools/snfix.py` e monta com `mips-linux-gnu-as`. Isso permite `INCLUDE_ASM`.
  Macros sem expansão conhecida fazem o build falhar, em vez de gerar mismatch silencioso.
- `tools/cc.sh` é o pipeline de compilação, e `tools/setup_toolchain.sh` baixa o compilador para `tools/cc/`.
- `tools/ccmatch.py <src> "<flags>" [compiladores|project]` compila e compara cada função com o original.
  `tools/wsl/objdiff.sh <func> <obj>` mostra o diff lado a lado.

## Divisão dos dados
- `tools/find_data_tus.py` gera `config/data_tus.csv`, e `tools/apply_tus.py` grava os subsegments no yaml.
- Evidências: (1) os statics locais, agrupados por objeto na `.symtab` (exatos, e monotônicos em todas as
  seções); (2) as referências `%hi`/`%lo`/`%gp_rel` do código de cada TU. Uma DP escolhe a atribuição
  monotônica de maior peso.
- Os limites em trechos sem evidência (dados sem referência entre duas TUs) são arbitrários dentro do
  intervalo possível. Não afetam o match, mas podem precisar de ajuste ao descompilar.
- TUs em `config/decomp_tus.txt` usam `.rodata` (com ponto): a rodata usada por uma única função migra
  para o `.s` da função (INCLUDE_ASM), e a rodata compartilhada precisa ser definida no C/C++.

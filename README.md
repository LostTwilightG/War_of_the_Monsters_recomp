# WoTM decomp

Projeto de decompilação/recompilação de **War of the Monsters** (PS2, NTSC-U, SCUS-97197).

Você precisa de uma cópia própria do jogo. Nenhum arquivo do jogo vai para este repositório.

## Setup (por enquanto)
1. Coloque a ISO em `ISO/` e extraia para `disc/` (por exemplo, `7z x -odisc ISO/*.iso`).
2. `python -m pip install "splat64[mips]" pyelftools`
3. `python tools/elf2bin.py disc/SCUS_971.97 SCUS_971.97.rom`
4. `python tools/gen_symbol_addrs.py`
5. `python -m splat split SCUS_971.97.yaml`

Veja [docs/ANALYSIS.md](docs/ANALYSIS.md) para o que já se sabe sobre o executável.

<img width="2113" height="744" alt="War of The Monsters recompiled LOGO" src="https://github.com/user-attachments/assets/6160bebd-48f0-47b5-bd28-f74d23c662fd" />

# War of the Monsters Recompiled!

Projeto de decompilação/recompilação de **War of the Monsters** (PS2, NTSC-U, SCUS-97197).

Você precisa de uma cópia própria do jogo. Nenhum arquivo do jogo vai para este repositório.

## Setup (WSL/Ubuntu)
```sh
sudo apt install binutils-mips-linux-gnu ninja-build python3-venv
python3 -m venv ~/.venvs/wotm && ~/.venvs/wotm/bin/pip install "splat64[mips]" pyelftools
```
1. Coloque a ISO em `ISO/` e extraia para `disc/` (por exemplo, `7z x -odisc ISO/*.iso`).
2. `~/.venvs/wotm/bin/python configure.py --split` gera a ROM, os símbolos, roda o splat e escreve o `build.ninja`.
3. `ninja` monta, linka e confere o SHA1 contra o executável original (`build/SCUS_971.97.rom: OK`).

`tools/symdiff.py` lista símbolos fora do lugar e `tools/romdiff.py` mostra as palavras que diferem quando o checksum falha.

Veja [docs/ANALYSIS.md](docs/ANALYSIS.md) para o que já se sabe sobre o executável.

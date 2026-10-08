# Contribuindo

Obrigado por ajudar! O projeto é uma decompilação por correspondência; leia o [CLAUDE.md](CLAUDE.md) (serve para humanos também) e o [README](README.md) para o setup.

## Antes de começar
- Você precisa da **sua própria cópia** do jogo (NTSC-U, SCUS-97197). Nada do jogo entra no repo: nem ISO, nem `asm/`, nem trechos de assembly em issues/PRs.
- Faça o setup do README e confirme que `ninja` termina com `build/SCUS_971.97.rom: OK` antes de mexer em qualquer coisa.
- Combine com o dono qual área (TU) você vai pegar, para não trabalharmos na mesma. Headers compartilhados (`include/game/game.h`, `include/engine.h`) são o ponto mais provável de conflito: avise antes de alterá-los.

## Fluxo
1. Fork do repo (ou branch, se você tem acesso de escrita) a partir da `master` atualizada: `git switch -c <tu>-<assunto>`.
2. Um PR por TU ou por pequeno grupo de funções relacionadas. PRs pequenos são revisados rápido.
3. Antes de cada commit: `sh tools/wsl/gate.sh` tem que terminar em `ROM OK`.
4. **Não commite** `config/status.csv`, o bloco de progresso do README nem `docs/progress.svg`: são gerados e causam conflito em todo PR. O dono regenera depois do merge. Por isso **não ative** o hook `.githooks/pre-commit` (ele reescreve e dá `git add` nesses arquivos) e evite `git commit -a`.
5. Abra o PR contra `master` preenchendo o template. Não há CI (não dá para buildar sem a ROM), então a saída do `gate.sh` no PR é a prova.
6. Mantenha o branch atualizado com `git rebase origin/master`.

## O que revisamos
- A ROM bate (SHA1) com o build padrão.
- Funções que não batem estão em `#ifdef NON_MATCHING` com nota de pontuação (`tools/nm_wrap.py`).
- Para código equivalente: retorno usado pelos chamadores, sequência de chamadas, strides/tamanhos de struct e delay slots foram conferidos (lista de armadilhas no `CLAUDE.md`).
- Funções na ordem de endereço original; sem helpers `static` fora de `NON_MATCHING`.
- Nada de código de hardware; nada de mudança de comportamento disfarçada.

## Usando Claude Code
Ok e incentivado. O `CLAUDE.md` já carrega as regras. Lembre o seu Claude de **não dar push direto na `master`** e de rodar o gate antes de cada commit.

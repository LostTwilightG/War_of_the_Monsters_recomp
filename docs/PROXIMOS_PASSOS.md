# Próximos passos (atualizado em 2026-10-07 01:35)

Estado: `python3 tools/progress.py` (WSL Ubuntu, venv `~/.venvs/wotm`). `common`: 157 idênticas / 35 equivalentes / 117 hardware /
835 só assembly. `game`: 42 / 30 / 0 / 3109. Convenções em `docs/ANALYSIS.md` ("Convenções de status das funções").

## Como retomar
1. `wsl -d Ubuntu` (a distro padrão é a `docker-desktop`, que não serve). `cd /mnt/c/Users/TwistZero/WoTM`.
2. `sh tools/wsl/check.sh` deve dizer `ROM OK`. **Nunca** encadear com `| tail`: o pipe esconde a falha.
3. Para um TU novo: `python3 tools/new_tu.py game/<Nome>` e depois `sh tools/wsl/check.sh`. Se a ROM não bater por causa de
   vtable/jump table, `python3 tools/place_data.py <tu> '<simbolo>'`.
4. Para cada função: ler `asm/nonmatchings/<tu>/<func>.s`, escrever o C++ no `src/<tu>.cpp` no lugar da linha `INCLUDE_ASM`,
   `python3 tools/ccmatch.py src/<tu>.cpp '' project` (pontua e mostra o que não bate), `tools/wsl/variants.py` se valer a pena
   tentar variações; se não bater, `python3 tools/nm_wrap.py <arquivo> <simbolo> "<nota>" [Classe::metodo]`.
5. Commitar só depois do `check.sh` passar.

## Ordem sugerida
- Feitos nesta rodada (2026-10-07): StickShaker, PowerUpTool, MilitaryPickup, StartPointTool, PathTool, MonkeyChains (ver status.csv).
- Terminar os TUs pequenos de `game` já convertidos: TankVehicle (updateControls usa VU + Weapons), reset, AiPathTool, SubwayPickup,
  CarPickup (resto), Vehicle (resto), AiBrain.
- Converter e fazer os TUs seguintes de `game` por tamanho (`config/tus.csv`): Destructible, RigidDebris, Ai*, Monster*, Pickups.
- Priorizar lógica real (IA, monstros, fases, pickups). Não gastar esforço em código de hardware (`config/hw_funcs.txt`).
- Para funções de `game` o fluxo é: escrever C++ natural, compilar, pontuar, embrulhar se não bater, **sem afinar**.

## Headers compartilhados (include/)
Regra: uma classe/API usada por mais de um TU mora num header; não redeclarar parcialmente dentro do `.cpp`.
- `engine.h`: API do motor (matemática, timers, `_animHandle`/animation*, particleKillFx, hier/hd). `game/game.h` inclui.
- `game/game.h`: `TheGame` com campos nomeados (`m_gravity`, `m_gameMode`, `m_matchMode`, `m_phase`, `m_numSlots`, `m_numMonsters`, `m_levelIdx`),
  `gameSlotBase(idx)` (mantém a ordem `idx*0x11190 + 0xB80` do retail), `gameHud(i)`, `gameWeapons()`.
- `point_tool_kit.h` (base das ferramentas; PathTool/PowerUpTool/StartPointTool/AiPathTool derivam e chamam `PointToolKit::init/loadPoints/getPoint`),
  `task_manager.h`, `bidir_link.h`, `cs_pool.h` (tudo estático), `game/{shell,hit_history,pickup,hud,weapons,power_ups,start_points,stamina_meter}.h`.
- Mover uma classe para header pode mudar `sizeof` e deslocar campos de structs parciais que a embutem (aconteceu com `StaminaMeter` em `GrappleMonster`):
  depois de cada mudança rodar `sh tools/wsl/scoreall.sh <TUs>` e comparar com a linha de base, e `sh tools/wsl/gate.sh`.
- Pontuação antes/depois: `sh tools/wsl/scoreall.sh A B C > novo.txt; diff base.txt novo.txt`.
- Funções que o retail chama com `this` mesmo sem usá-lo (ex.: `StartPoints::getNumPoints`) só batem se declaradas não-estáticas; `isThisTypeFull` é estática.

## Armadilhas já vistas
- **Gate de commit**: `sh tools/wsl/gate.sh && git commit ...` (gate.sh sai com erro se a ROM não bater; `check.sh; git commit` ou `| tail` commitam builds quebrados). `check.sh | tail && git commit` commita mesmo com `BUILD FAILED` (aconteceu no PathTool).
- Ao reescrever o fim de um `.cpp` com script, conferir que as linhas `INCLUDE_ASM` finais (static init, `__tf`, ctor, `_GLOBAL_$I$`) continuam lá.
- `ccmatch.py` sem `-DNON_MATCHING` só compila as `INCLUDE_ASM` (tudo "MATCH"); para pontuar o C++ novo use `ccmatch.py src/x.cpp '-DNON_MATCHING' project`.
- Layout do `PointToolKit` nas ferramentas (PowerUpTool/StartPointTool/PathTool): pontos 0x40 cada, `numPoints` em 0x4000, ponteiro de dados em 0x4050; `init` = `PointToolKit::init(0)` + `game + idx*0x11190 + 0xB80`.
- `shell` é gp-relativo em alguns TUs (PowerUpTool, PathTool) e não em outros (StartPointTool): `__asm__("#SNFIX_SMALL shell")` só onde o retail usa gp.
- gas insere 2 `nop` extras num `.p2align 3` logo depois de uma sequência `li.s` (visto em StickShaker::DefaultSetup); o retail não tem. Sem causa achada, marcar como equivalente.
- Retorno `(x & 1) == 0` em vez de `!(x & 1)` muda `lw`/`xori` para `ld`/`andi` com campo de 64 bits (MilitaryPickup::kill).
- Cópia de `_fvector` por `lq/sq` com `jr` seguido de `nop` indica `asm volatile` com `lq/sq` no retail (MilitaryPickup::setFormationPos).
- Strings de uma função que passam de `INCLUDE_ASM` para C mudam o padding do `.rodata`: acrescentar `.word 0` em `.rodata` por asm.
- `switch` em C gera jump table; o retail alinha em 24 palavras (acrescentar `.word 0` x2).
- Classes com vtable: ctor, `__tf` e `_vt$...` ficam como `INCLUDE_ASM`; escrever os métodos sem `virtual`.
- Funções estáticas sem argumentos às vezes têm um `v` no fim do símbolo retail: usar `__asm__("nome__Classev")` no membro.
- O heredoc da ferramenta pode transformar `\n` em quebra de linha real dentro de scripts Python: para arquivos com regex, usar o Edit.

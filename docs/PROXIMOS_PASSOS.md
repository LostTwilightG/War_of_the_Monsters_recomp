# Próximos passos (atualizado em 2026-10-07 14:10)

## Onde estamos
- `sh tools/wsl/gate.sh` diz `ROM OK` (build + SHA1). Último estado medido (`python3 tools/progress.py`): `game` ~170 de 3181 funções
  decompiladas (idênticas + equivalentes), ~14 KB de 930 KB; `common` 192 de 1144. Convenções em `docs/ANALYSIS.md` ("Convenções de status das funções").
- Ritmo de hoje: ~2,6 KB/hora em funções pequenas. Decompilar o `game` inteiro nesse ritmo não fecha; por isso a estratégia mudou (abaixo).
- Objetivo do port: precisa de ~1000 funções / ~310 KB (lógica de jogo alcançável pelo laço de atualização), fora hardware. Lista em `config/callgraph.csv`.

## Estratégia (decidida com o usuário em 2026-10-07)
1. **Escopo por alcance**, não por tamanho: `python3 tools/callgraph.py Update__7TheGame,InitBeforeDbLoad__7TheGame,InitAfterDbLoad__7TheGame,ResetLevel__7TheGame --no-libs`
   grava `config/callgraph.csv` (função, TU, tamanho, profundidade). Só segue `jal`; chamadas virtuais (`jalr`) não entram, então é um piso.
   Trabalhar de cima para baixo nessa lista. Hardware (`config/hw_funcs.txt`) fica de fora; `game/Sound` e `game/StreamingSoundManager` NÃO são hardware
   (decisões de jogo) mas o port pode começar com áudio mudo, então são baixa prioridade.
2. **Equivalente natural, sem afinar**: escrever C++ natural, pontuar, embrulhar com `nm_wrap.py` se não bater. Só afinar quando é barato (ver armadilhas).
3. **m2c como rascunho** para funções com mais de ~30 instruções: `sh tools/m2c.sh <tu> <função>`. Conferir contra o assembly, trocar `unkNNN` por campos
   nomeados dos headers, ajustar tipos. Funções curtas: escrever direto. Nunca confiar no m2c sem ler o asm (os argumentos que ele mostra são ruído de registradores).
4. **Comparar comportamento com o PCSX2** assim que houver algo rodando (ainda sem testes; decisão do usuário).
5. **Unificar tipos conforme os usos se repetem**: promover campo a header só quando há evidência (mesmo offset, mesmo uso) em 2+ lugares ou o m2c/asm deixa claro.

## Como retomar
1. `wsl -d Ubuntu` (a distro padrão é a `docker-desktop`, que não serve). Venv: `~/.venvs/wotm/bin/python`. Raiz: `/mnt/c/Users/TwistZero/WoTM`.
2. **Gate de commit**: `sh tools/wsl/gate.sh && git commit ...` (de preferência o `git commit` rodando no Windows; o git do WSL não tem identidade).
   `check.sh; git commit` ou `check.sh | tail && git commit` commitam builds quebrados.
3. Converter TUs novos: `sh tools/wsl/convert_tus.sh TuA TuB ...` (um de cada vez, só mantém os que não quebram a ROM, posiciona vtables com `place_data.py`).
   Depois de um TU removido/revertido, regenerar com `~/.venvs/wotm/bin/python configure.py --split` (senão o `build.ninja` aponta para arquivo inexistente).
4. Por função: ler `asm/nonmatchings/<tu>/<func>.s` (ou rodar o m2c), escrever o C++ no lugar da linha `INCLUDE_ASM`,
   `sh tools/wsl/scoreall.sh <TU...>` (pontua com `-DNON_MATCHING`), `nm_wrap.py <arquivo> <simbolo> "<nota>" [Classe::metodo]` se não bater.
5. Antes de mexer em header compartilhado: `sh tools/wsl/scoreall.sh <todos os TUs de src/game> > ~/base.txt`; depois comparar com `diff`. Rodar `gate.sh`.

## Ferramentas (tools/)
- `wsl/gate.sh` (build + SHA1, sai com erro se falhar), `wsl/scoreall.sh` (pontuação por função com NON_MATCHING), `wsl/convert_tus.sh`, `wsl/check.sh`,
  `ccmatch.py`, `nm_wrap.py`, `new_tu.py`, `place_data.py`, `progress.py` (escreve `config/status.csv`), `callgraph.py`, `m2c.sh`, `wsl/variants.py`, `wsl/permute.py`.
- Scripts `.sh` devem ser criados por heredoc no bash; gravar com Python no Windows deixa CRLF e o `sh` do WSL quebra (`sed -i 's/\r$//'` conserta).
- Mod `/wotm` (HUD) carrega com `startup_command.bat` (no `.gitignore`), que inicia o Claude com `--plugin-dir ~/.claude/my-plugins/wotm-hud`.

## Headers compartilhados (include/)
Regra: uma classe/API usada por mais de um TU mora num header; não redeclarar parcialmente dentro do `.cpp`.
- `engine.h`: API do motor (matemática, timers, `_animHandle`/animation*, particleKillFx, hier/hd). `game/game.h` inclui.
- `game/game.h`: `Monster` (0x11190 bytes: `m_cs`, `m_playerNum`, `m_id`, `m_health`, `m_stamina`, `m_target`, `m_camUnify`), `TheGame`
  (`m_huds[4]`, `m_slots[16]` = os `Monster` em 0xB80, `m_monsters[]` = ponteiros, `m_gravity`, `m_gameMode`, `m_matchMode`, `m_levelId` (1 central, 2 vegas, 3 canyon2,
  5 airport, 6 threemile, 7 sanfran, 8/15 island, 9 tokyo, 10 ufo, 11 final boss, 26 bigshot, 27 crush), `m_numSlots`, `m_numMonsters`, `m_levelIdx`, `m_playerMask`),
  `Cameras` (`m_cameras`, `m_state`), acessores `gameSlotBase(idx)` (mantém a ordem `idx*0x11190 + 0xB80`), `gameHud(i)`, `gameWeapons()`.
- `point_tool_kit.h` (base de PathTool/PowerUpTool/StartPointTool/AiPathTool; derivados chamam `PointToolKit::init/loadPoints/getPoint`), `task_manager.h`, `bidir_link.h`,
  `cs_pool.h` (tudo estático), `game/{shell,hit_history,pickup,military_pickup,pickup_sound,vehicle_navigator,hud,weapons,power_ups,start_points,stamina_meter,crush_level,levels,streaming_sound}.h`.
- Mover uma classe para header pode mudar `sizeof` e deslocar campos de structs parciais que a embutem (aconteceu com `StaminaMeter`): sempre comparar antes/depois.
- Funções que o retail chama com `this` mesmo sem usá-lo (ex.: `StartPoints::getNumPoints`) só batem se declaradas não-estáticas; `isThisTypeFull` é estática.
- Ainda não unificados: `GamePad` (`GamePad.cpp` vê 6 ints, `GamePadClipPlayer.cpp` vê bytes), `GamePadClipPlayer`/`AiPadClips` (tipo do clipe diverge entre `AiGrapple` e os outros),
  `Ai`, `Destructibles`.

## Trabalho em andamento: TheGame
- `src/game/TheGame.cpp` convertido. `TheGame::Update` está escrito (embrulhado em `NON_MATCHING`, 23/252 palavras, equivalente a partir do m2c); falta conferir contra o comportamento.
- Próximos dentro de `TheGame`: `Update2`, `UpdatePadTweaks`, `SetGravity`, `GetNumAIsAlive`, `SetOkToUnify`, `InitAfterDbLoad`, `InitBeforeDbLoad`, `ResetLevel`, `gameResolveLifeAndDeath` (717 linhas), `gameCheckForCloseCombat`.
- Depois, descendo a árvore: `Monster::update`/`updatePosition`/`updateCinema` (TU `Monster`, 53 funções / ~18 KB), `AiNavigator`, uma fase completa (`tokyo`, já convertida), `PlantBoss`/`FinalBoss`.

## Teste diferencial (tools/difftest.py, tools/difftest_all.py)
Roda a função retail e a nossa (compilada com `-DNON_MATCHING`) num emulador MIPS (unicorn), com o mesmo estado aleatório, e compara retorno, chamadas a outras funções
(os callees viram `jr ra`; nome + argumentos relevantes, com strings comparadas pelo texto e ponteiros de função pelo nome), bytes alterados na arena e nos dados do retail e as variáveis
que o próprio TU define (alias para a cópia do retail). Serve para validar as funções "equivalentes (untuned)" sem PCSX2.
- Uma função: `~/.venvs/wotm/bin/python tools/difftest.py game/Monster isFullHealth__7Monster --args this --ret int --runs 60 [--objsize 0x122000]`
  (`--args` aceita `this,p,i,b,f`; `--alt OUTRAFUNC` é o controle negativo, deve dar MISMATCH; `--objsize` mantém ponteiros aleatórios fora do objeto, use 0x122000 para `TheGame`).
- Todas as equivalentes de TUs: `python3 tools/difftest_all.py game/Monster game/TheGame ... --runs 30` (adivinha a assinatura pelo nome mangled e o retorno pela definição).
- Resultado de 2026-10-07: ~95 funções equivalentes concordam; o teste achou e corrigimos 7 erros reais (Vehicle::setCs/setPos, StaminaMeter::credit, UpdatePadTweaks, TheGame::Update (2º TaskManager),
  MonkeyChains::init (FxTextureId é enum), AnimContact).
- Limites: não executa VU0/COP2 nem `min.s`/`madd.s`; funções com métodos virtuais ou estruturas encadeadas válidas são puladas ("skipped"); mais de 4 args inteiros ou 2 floats não são suportados;
  é evidência (só os caminhos que o estado aleatório alcança), não prova. Rodar em toda função equivalente nova antes de commitar.
- Armadilha do emulador: hook de escrita de memória quebra saltos com store no delay slot; por isso comparamos imagens de memória no fim, não escritas.

## Armadilhas já vistas
- Ao reescrever o fim de um `.cpp` com script, conferir que as linhas `INCLUDE_ASM` finais (static init, `__tf`, ctor, `_GLOBAL_$I$`) continuam lá.
- `ccmatch.py` sem `-DNON_MATCHING` só compila as `INCLUDE_ASM` (tudo "MATCH"); para pontuar o C++ novo use `-DNON_MATCHING` (o `scoreall.sh` já faz).
- Layout das ferramentas de pontos: pontos de 0x40 bytes, `numPoints` em 0x4000, campos próprios a partir de 0x4050; `levelData = gameSlotBase(game->m_levelIdx)`.
- `shell` é gp-relativo em alguns TUs (PowerUpTool, PathTool, AiPathTool) e não em outros (StartPointTool): `__asm__("#SNFIX_SMALL shell")` só onde o retail usa gp.
  Dentro de um delay slot (`.set nomacro`) o acesso a global é sempre gp, sem precisar do pragma (tokyo).
- gas insere 2 `nop` extras num `.p2align 3` logo depois de uma sequência `li.s` (StickShaker::DefaultSetup); o retail não tem. Sem causa achada: marcar como equivalente.
- `(x & 1) == 0` em vez de `!(x & 1)` muda `lw`/`xori` para `ld`/`andi` com campo de 64 bits (MilitaryPickup::kill).
- Cópia de `_fvector` por `lq/sq` com `jr` seguido de `nop` indica `asm volatile` com `lq/sq` no retail (setFormationPos, setTrans); registrador `$2` fixado com `register int t __asm__("$2")`.
- Chamada de função dentro dos argumentos da chamada final (operador vírgula) muda o agendamento do prólogo e fez `loadPoints` bater (PathTool/AiPathTool).
- Ponteiros intermediários (`VehicleNavigator *n = &nav; n->f14 = ...`) imitam os `addiu v1,v0,0x210` do retail.
- Strings de uma função que passam de `INCLUDE_ASM` para C mudam o padding do `.rodata`: acrescentar `.word 0` em `.rodata` por asm.
- `switch` em C gera jump table; o retail alinha em 24 palavras (acrescentar `.word 0` x2).
- Classes com vtable: ctor, `__tf` e `_vt$...` ficam como `INCLUDE_ASM`; escrever os métodos sem `virtual`.
- Funções estáticas sem argumentos às vezes têm um `v` no fim do símbolo retail: usar `__asm__("nome__Classev")` no membro.
- Mangling de matriz: `float (*m)[4]` vira `PA3_f` no gcc 2.95.
- O heredoc da ferramenta pode transformar `\n` em quebra de linha real dentro de scripts Python: para arquivos com regex, usar o Edit.
- `sh tools/wsl/gate.sh | tail` esconde o código de saída: para commitar só com ROM OK usar `sh tools/wsl/commit_if_ok.sh && git commit ...`.
- Global gp-relativo já definido no `.sdata` de um asm de dados (ex.: `WaterLevel`): declarar `extern float X; __asm__("#SNFIX_SMALL X");`, nunca definir de novo (duplicate symbol no link).
- Armazenar num campo de `struct` (ex.: `gameHud(i)->f0D0 = 0`) não invalida o `game` já carregado; armazenar via `*(int*)((char*)p + off)` invalida e o gcc recarrega. Por isso `Hud` ganhou campos reais.
- `EnemyInfo::s_info[i][j']` (include/game/enemy_info.h): tabela par-a-par, `j' = j - 1` quando `i < j`. `vecLenSq` (vecmath.h) = `mula.s/madda.s/madd.s` do retail.
- Em laços `for (j = 0; j < n; j++, m++)` guardar `n = game->m_numSlots` numa local (senão o gcc recarrega a cada volta).

## Testar o C++ novo no PCSX2 (meio asm, meio C++)
- O ROM do build normal � id�ntico ao retail (equivalentes ficam como `INCLUDE_ASM`). Para rodar as equivalentes: `sh tools/wsl/build_nm.sh` (WSL) compila uma c�pia em `~/wotm_nm`
  com `-DNON_MATCHING` e grava `build/pcsx2_test/SCUS_971.97_halfcpp.elf` e `..._control_matching.elf` (controle). Ambos com `p_paddr = p_vaddr` (o PCSX2 carrega por `p_paddr`).
- Rodar: `pcsx2-qt.exe -elf build\pcsx2_test\SCUS_971.97_halfcpp.elf -- "ISO\SCUS_971.97.War of the Monsters.iso"`. D� para automatizar: `-batch -nogui -logfile <log>` e matar depois de ~30 s;
  o log mostra `microVU1: Cached Prog`, `FMV started` etc. quando o jogo anda, e `Vif0: Unknown VifCmd` / `microVU0: Possible infinite compiling loop` quando os dados est�o errados.
- **Layout**: o linker script normal empilha as pe�as (`x.o(.sec)`) uma atr�s da outra e usa `SUBALIGN(4)`, ent�o qualquer fun��o equivalente maior/menor desloca todos os dados que v�m depois
  (248/252/280 bytes) e os buffers de DMA ficam desalinhados: foi o que quebrou o primeiro teste. `tools/gen_nm_ld.py` gera um script onde toda pe�a que n�o cresceu fica no endere�o retail
  (`. = <endere�o - base da se��o>`; `.cod_bss` fixo em seu endere�o) e as pe�as que cresceram (13, quase todas `.text`) v�o para `.nm_extra`, depois do bss.
- `CrushLevel` � compilado com `-G0` no build NM (as strings do c�digo novo cairiam em `.sdata`, onde n�o h� espa�o). `Monster.cpp`: `cloaker` � o s�mbolo `cloaker.2691`.
- Resultado at� agora: o halfcpp passa do boot e chega � FMV de abertura sem erro de VIF. Falta testar menu/fase.

### Bisseção do ELF com C++ novo (estado em 2026-10-08, ~02:00)
- Layout corrigido (gen_nm_ld.py); o halfcpp completo chega à fase mas trava/crasha. Resultados com variantes (build_nm.sh):
  `v3_game` (TheGame, MonsterMeters, StartPoints, AiGrapple, AiBrain, island, DodgeBall, CrushLevel) funciona; `v1_monster` e `a_update` (só `Monster::update`) travam no loading da fase
  (`Unrecognized op` no log, executando dados) -> **`Monster::update` tem bug**; `b_cinema`, `c_rest`, `v4_misc` ainda não testados pelo usuário.
- **Atualização 2026-10-08 (manhã)**: o erro `Jump to unaligned address (PC: 1)` do menu (`v2_common`, `bsA`, `t_hieri`) NÃO reproduz mais: refeitos com o `gen_nm_ld.py` atual,
  `t_hieri`, `h_flush`, `h_set`, `h_dma`, `t_zip`, `t_TaskManager`, `bsA` e o `halfcpp` completo passam da abertura/FMV sem `ReportErrorAsync` (45-60 s, 3x estável em `t_hieri`);
  os ELFs antigos eram de antes das correções de layout. Falta só o crash no loading da fase = `Monster::update` (não dá para automatizar: precisa de input no menu).
  Achado de build: `hierDmaHandler` chamava `hierFlushObjQ` sem protótipo quando só ela era `NON_MATCHING` (variante `h_dma` não linkava); protótipo adicionado em `hieri.cpp`.
  Script de teste automático: abrir `pcsx2-qt.exe -batch -nogui -logfile <log> -elf <elf> -- <iso>`, esperar até `ReportErrorAsync` ou 45 s, `taskkill`.
- **RESOLVIDO (2026-10-08)**: o crash no loading da fase NÃO era bug do `Monster::update` (as duas correções, `sb` em `m_x6874+0xC` e `f5C` como byte, continuam válidas; o difftest dá
  144/150 iguais, 0 divergências, e agora também confere os registradores callee-saved). A causa era **layout de memória**: o `.nm_extra` (peças que cresceram) ficava logo depois do bss, onde começa o
  heap do jogo, e depois de carregar uma fase a RAM de 32 MB inteira está em uso (nenhuma região livre acima de 0xA00000 nem em 0x1800000: DMA de pacotes sobrescreveu o início de `takeHit`).
  Correção: `patch_paddr.py --heap` empurra o início do heap para depois do `.nm_extra`, em **dois** lugares: a chamada `SetupHeap` do crt0 e a palavra `heap_ptr` de `libkernl/glue.data` (0x6E5D2C,
  o `_end` do malloc do SDK); o laço que zera o bss mantém o fim antigo. `build_nm.sh` passa `--heap` sozinho.
- **Resultado**: `halfcpp` (todas as equivalentes compiladas) entra na fase e roda uma partida inteira no PCSX2 (o CPU venceu o jogador parado: "AI WINS"). `a_update` e `m_noupd` também passam.
  `tools/pcsx2_auto/` (ver README) automatiza isso: `powershell tools/pcsx2_auto/play.ps1 halfcpp 60` troca os bindings por teclado, joga o menu até a fase, vigia o log e restaura o `.ini` com `emu.ps1 stop`.
  Próximo: testar as variantes `b_cinema`, `c_rest`, `v4_misc` (já cobertas pelo `halfcpp`), jogar de verdade (mover o monstro, ataques, outras fases) e comparar comportamento com o retail.
- difftest ganhou `--init '<python>'` (W/W16/W8/THIS/ARENA/STUB) para montar estado estruturado; `Monster::update` precisa de `m_state` -> objeto com vtable (`W(THIS+0x34,S); W(S+0x10,VT); W16(VT+0x18,0); W(VT+0x1C,STUB)`),
  mas ainda cai em escritas fora do mapa (retail e alt), falta ajustar mais ponteiros.
- O log do PCSX2 do usuário fica em `~/Documents/PCSX2/logs/emulog.txt`; erros em diálogo também aparecem lá como `ReportErrorAsync`.

### Bugs de comportamento do halfcpp achados jogando (2026-10-08)
Teste do usuário no halfcpp: veículos do chão sem glow verde e ◯ não pega; pedestres andando em fila; alguns prédios atravessáveis. Achados e corrigidos lendo as equivalentes contra o asm:
- **`PathNode` tinha 0x40 bytes, o retail usa 0x30** (links de 4 bytes a partir de 0x12, são 7 e não 9): `PathNet::getRandNode/getClosestNode` indexavam nós errados -> pedestres (e provavelmente os carros, que também seguem a PathNet) em fila. Agora `getRandNode` dá MATCH; há um `typedef char PathNodeSizeCheck[...]`.
- `AiBrain`: a classe base vazia `AiActionGroup` ocupa 1 byte no gcc 2.95 e empurrava `f_AAC` para 0xAB0 (retail 0xAAC). Sem herança agora (`AiBrain::reset` virou MATCH).
- `AiGrappleAttack::updateAction`: com `matchMode` 0/1 o retail sorteia `heavyPunch` (o nosso chamava `toss`).
- `Monster::isIdle`: último teste invertido (retail retorna 1 quando `t < state[2]`); só a câmera usa.
- **Lição**: "untuned" com muitas palavras diferentes pode esconder erro de layout/semântica (o difftest não pegou nenhum destes: estado aleatório, callees stubados). Ferramentas novas: `tools/nm_audit.py` (offsets e imediatos por função, ELF NM x retail; muito ruído de gp x lui), `tools/nm_calls.py` (sequência de chamadas; achou o `toss` extra) e `tools/ramdiff.py` (compara dois dumps de RAM do EE; com `NM_HEAP=0x8E0000 sh tools/wsl/build_nm.sh ...` o heap fica no mesmo lugar nos dois builds e os endereços dos objetos coincidem; dump por `play.ps1 <elf> <n> <arquivo>`).
- Comparação de RAM retail x halfcpp na fase (mesmo ponto): listas de pickups, `Interactives`, `ColGrid` (381 nós usados nos dois) e pools estão iguais; então o setup do nível está certo e o defeito do glow/pegar veículo é dinâmico. **Ainda não explicado**: glow/pegar veículo e prédios atravessáveis (retestar com o halfcpp novo; se persistir, reproduzir andando até um carro e ler `s_highlightPickup` na RAM).
- **Causa do glow/pegar pickup (achada com o dump do usuário, 2026-10-08)**: `Interactives::addInteractive` foi escrita como `void`, mas os callers do retail (`CarPickup::initAfterDbLoad`, `HeliPickup`, `Destructible`...) usam o `$v0` (o índice do slot) como id no `hierhead` (`(v0 & 0x7FF) << 7`). O `$v0` que sobrava era índice+1, então todo `getInteractive(idx)` devolvia o vizinho (no retail idx == posição no array nos 118 pickups; no halfcpp, +1 em 117). Isso quebra `getClosestPickup` (glow e ◯), a colisão da bola devolvida e provavelmente o dano contínuo em prédios (todos usam `getInteractive`). Agora `int addInteractive` devolve o slot. `tools/nm_retvals.py` varre as equivalentes `void` cujo valor de retorno é consumido por callers do retail (nenhum outro caso).
- Como achar isso de novo: dump da RAM com o jogo pausado (`ramdump.py`, pega a cópia da RAM com `game` != 0), `pick.py`/`cs.py`/`idx.py` (scratchpad) reproduzem a conta do `getClosestPickup` a partir do dump.

### Decomp por alcance (2026-10-08 tarde)
- Estado (`tools/progress.py`): `game` 466/3181 funções (28,4 KB de 930 KB), `common` 192/1144 (23,7 KB de 258 KB). Candidatas do `callgraph.csv` sem VU0 e com até 260 bytes: ~105 (AiNavigator 22, Pickup 12, Cameras 9, AiPathFinder 5...); o restante é VU0 (`lqc2`/`vsub`...), que exige `asm volatile` (ver `include/vecmath.h`) e não é coberto pelo difftest.
- `game/Pickup` convertido (27 funções: getters/setters, `drop`, `setVisualState`, `regenUpdate`, `getVel` x2, `initAfterDbLoad`, `update` batendo; `regen` equivalente). Truques: `(int)(bits >> n) & 1` gera o `dsll/dsra32` do retail; para testar um bit de `unsigned long long` num `if`, guardar `bits & m` num `unsigned long long` local (senão vem `dsll32/dsra32` extra); `lq/sq` por `asm volatile` com `$2` fixo.
- `game/AiNavigator`: layout (`monster` 0x0, `turn` 0xC, `strafe` 0x14, `mode` 0x18, `status` 0x1C, `ObstacleSensor` 0x30 (0x130 bytes), `AiPathFinder` 0x160 (0x28), alvo 0x188..0x19C, `PathInfo` 0x220/0x240 (0x20 cada)); 14 funções batendo (ctor, getters do sensor, wander/target/disable, seek/arrive por `DbInteractive`...) e 6 equivalentes (flee, init, orientTo, strafeTo, targetPin, updateTarget). `fabsf` (extern "C") gera `abs.s`; `max.s`/`rsqrt.s` só por `asm`; `permute.py` precisa de N <= nº de ordens possíveis (senão trava).
- Próximo: restante do AiNavigator (`PathInfo::init` x3, `nextPathPoint`, `seek`/`arrive` por vetor, `tag`, `updateFlee`, `isObstacleClimbable`...), depois `Cameras` e `AiPathFinder`. Validar sempre com o halfcpp (`play.ps1`) e jogando.

# Testar o C++ novo no PCSX2 (halfcpp) e bugs achados

(Dividido de `docs/PROXIMOS_PASSOS.md`; índice lá.)

## Testar o C++ novo no PCSX2 (meio asm, meio C++)
- O ROM do build normal é idêntico ao retail (equivalentes ficam como `INCLUDE_ASM`). Para rodar as equivalentes: `sh tools/wsl/build_nm.sh` (WSL) compila uma cópia em `~/wotm_nm`
  com `-DNON_MATCHING` e grava `build/pcsx2_test/SCUS_971.97_halfcpp.elf` e `..._control_matching.elf` (controle). Ambos com `p_paddr = p_vaddr` (o PCSX2 carrega por `p_paddr`).
- Rodar: `pcsx2-qt.exe -elf build\pcsx2_test\SCUS_971.97_halfcpp.elf -- "ISO\SCUS_971.97.War of the Monsters.iso"`. Dá para automatizar: `-batch -nogui -logfile <log>` e matar depois de ~30 s;
  o log mostra `microVU1: Cached Prog`, `FMV started` etc. quando o jogo anda, e `Vif0: Unknown VifCmd` / `microVU0: Possible infinite compiling loop` quando os dados estão errados.
- **Layout**: o linker script normal empilha as peças (`x.o(.sec)`) uma atrás da outra e usa `SUBALIGN(4)`, então qualquer função equivalente maior/menor desloca todos os dados que vêm depois
  (248/252/280 bytes) e os buffers de DMA ficam desalinhados: foi o que quebrou o primeiro teste. `tools/gen_nm_ld.py` gera um script onde toda peça que não cresceu fica no endereço retail
  (`. = <endereço - base da seção>`; `.cod_bss` fixo em seu endereço) e as peças que cresceram (13, quase todas `.text`) vão para `.nm_extra`, depois do bss.
- `CrushLevel` é compilado com `-G0` no build NM (as strings do código novo cairiam em `.sdata`, onde não há espaço). `Monster.cpp`: `cloaker` é o símbolo `cloaker.2691`.
- Resultado até agora: o halfcpp passa do boot e chega à FMV de abertura sem erro de VIF. Falta testar menu/fase.

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



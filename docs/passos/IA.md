# Notas da IA (AiAction / Ai / AiReflex / AiSeek)

(Dividido de `docs/PROXIMOS_PASSOS.md`; índice lá.)

## Notas da sessão da IA (AiAction / Ai / AiReflex / AiSeek)

- **Estrutura**: `include/game/ai.h` (classe `Ai`, campos já vistos), `ai_action.h` (`AiActionTuple`/`AiActionList`/`AiActionGroup`, chamadas virtuais à mão via `AI_VENT`/`VCALL_F`/`VCALL_V`, vptr em 0x44 nas tuplas e 0x84 nas listas) e `ai_support.h` (macros `MI/MF/MB/MP`, `NAV(ai)` = `AiNavigator` embutido em `Ai+0x80`, `FMAX/FMIN`, `StateButtSlam`, `GamePadClipPlayer`...). Cada ação (`AiPunchReflex`, `AiBlockReflex`...) deriva de `AiActionTuple` (0x48 bytes) e os campos próprios começam em 0x48. Ctor, vtable e `__tf` ficam em asm.
- **min.s / max.s**: o retail usa `max.s`/`min.s`, mas `-ffast-math` (única forma de o gcc 2.95 emitir isso) também reescreve todo `a < b` em `c.le` invertido. Em `Ai` e `AiAction` (`config/tu_flags.txt`) isso fecha; nos TUs com muita comparação use `FMAX`/`FMIN` (asm inline `max.s`), que não mexe nos outros compares.
- **Build NM com .sdata novo**: constantes float do código equivalente caem em `.sdata`; TUs cujo retail não tem `.sdata` precisam de `-G0` no build NM (`tools/wsl/build_nm.sh`, lista `for t in game/AiAction`). Sintoma: `defined in discarded section .sdata`.
- **Heredoc de Python**: nesta ferramenta uma barra invertida dupla dentro de heredoc vira uma só, e sequências como barra-1 ou barra-r viram caracteres de controle. Para scripts com regex ou `sed`, grave o arquivo com a ferramenta Write em vez de heredoc (um `build_nm.sh` ficou com `^A`/CR no meio por causa disso).
- **Fluxo usado para classes de ação**: escrever o C++ a partir do asm, `tools/wsl/scoreall.sh <TU>`; o que bate sai do `#ifdef NON_MATCHING`, o resto fica com nota `untuned: N/M words`. Funções com `switch` (jump table) ficam em NM mesmo quando batem, porque a tabela precisa vir do rodata do retail.
- **Offsets do Monster vistos de fora** (por offset cru, ainda sem campo nomeado): 0x34 estado atual (id em [0], flags em [1]: 4 = atacando, 0x10 = reação), 0x49 alvo válido, 0x4A ataques ligados, 0x280 no chão, 0x448 vida (max, cur), 0x460 stamina, 0x68A4 objeto segurado (kind em +0xA0), 0x6C34/0x6C38 botão apertado, 0x7978/0x7980 estados especiais, 0xDCE0 StateGrapple, 0xFA1C StateButtSlam.

### Onde parei na IA (2026-10-09, 00:45)
- `AiReflex`: todas as classes de ação já têm C++ equivalente (ctor, vtable e `__tf` seguem em asm).
- `AiSeek`: feitos SeekMonster, SeekPickup, SeekHealth, SeekStamina, SeekSpecial, SeekCloak, DodgeRam e DodgeStomp. **Faltam**: `AiDodgeThrow` (entry 0x264, update 0x21C, exit relevance 0x160), `AiDodgeSpecial` (`getExitRelevance`, `updateAction`; entry, enter, exit e `updateFleeSpot` já casam), `AiBatThrow`, `AiCatchThrow` e `AiSwarm` (estados, `getBestTarget`, `updateAction`).
- Depois do `AiSeek`: `SpecialStates` (151 funções) e os TUs convertidos em 2026-10-09 que ainda estão só como stub (`Grapple*`, `FinalBoss`, `PlantBoss`, `Debris`, `Destructible`, `collision`, `MonsterDynamics`, `MonsterAnimBlend`, `StateThrowBack`, `PowerUps`).
- O ELF `build/pcsx2_test/SCUS_971.97_next.elf` (todas as equivalências até aqui) ainda não foi testado em jogo.


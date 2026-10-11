# Runtime recompilado (PS2Recomp): estado e como retomar

(Dividido de `docs/PROXIMOS_PASSOS.md`; índice lá.)

## Runtime recompilado: estado e como retomar (2026-10-10)
**Resumo:** o jogo recompilado roda no Windows (MSVC) em tempo real (vsync 60/s, EE ~294M ciclos/s) com um combo de variáveis de teste, e um roteiro de pad leva do boot até uma fase, onde a
travessia nativa lê a hierarquia (`world`, 483 nós / 29 objetos). Ainda não há 3D nem texturas na tela: o VU1 e o GS emulados estão desligados de propósito e o desenho é papel da camada nativa.

**Onde está o código (fora do repo, por ser derivado do jogo / do PS2Recomp):**
- Clone de teste `C:\Users\TwistZero\wotm-recomp-win` (build MSVC em `build\`, `build.bat` recompila só o runner; o link LTCG leva ~10 min; `run.ps1` executa e captura).
- Mudanças de runtime (`PS2X_NO_VIF1`, `SKIP_CMOVIE`, `PEEK`, `PROF`, `STATS`, diagnósticos do escalonador) já estão versionadas na branch `wotm` do fork (`yanm1103/PS2Recomp`, submódulo `third_party/PS2Recomp`) no commit `a6b9af2`, **só local: falta o push no fork (decisão do dono)**. O `wotm_runtime_diagnostics.patch` e o `wotm_scene.inc` do clone de teste ficam como cópia de segurança (o patch é contra o upstream `2c5fbb9`, não aplica na branch `wotm`). O código gerado (`register_functions.cpp` etc.) nunca vai ao repo.

**Combo que funciona** (`PS2X_*` lidas pelo runner; `run.ps1` já põe `SKIP_MOVIES`, `STATS`, `NO_VU1`, `CD_IMAGE`):
`PS2X_NO_VIF1=1 PS2X_NO_GS=1 PS2X_SKIP_CMOVIE=1` (+ `PS2X_SCENE=1` para a travessia nativa). Roteiro de pad que chega a uma fase em ~50 s: `PS2X_PAD` = `start` (0.8 s) a cada 1,5 s de t=3 a 18,
depois `cross`@22, `down`@25, `cross`@28,31,34,37,40,43,47,51 (1 s cada). Rodar: `powershell -ExecutionPolicy Bypass -File run.ps1 -Scene 1 -Seconds 75 -Frames "3300" -Tag runN`.
Variáveis novas: `PS2X_NO_VIF1` (ignora a execução do DMA do VIF1/VU1 mas sinaliza a conclusão), `PS2X_SKIP_CMOVIE` (`CMovie::Play` no-op, `Update` devolve 1), `PS2X_PEEK="hex,hex"` (palavras da RDRAM no log `[peek]`),
`PS2X_PROF=1` (amostrador do PC ao vivo, grava `discroot/prof.<frame>`; mapear com `config/status.csv`), `PS2X_NOOP=addr,addr` (função vira no-op), e no stderr com `PS2X_STATS=1`: `[time]` (tempo de GS/upload/draw/idle),
`[ui]` (currScreen, betweenScreens, ...), `[pad]`, `[scene]`.

**O que se descobriu (e que desfaz conclusões anteriores):**
1. A "tela preta" era artefato do `TakeScreenshot` antes do `rlDrawRenderBatchActive()`; com a correção aparece imagem (fundo 2D chapado, sem textura).
2. A lentidão de ~40x vinha do DMA do VIF1 ser processado, com o VU1 interpretado, DENTRO do `sw 0x145 -> D1_CHCR` em `hierTraverseAsm` (0x207744, 99% das amostras). O EE ficava 100% ocupado, os ciclos virtuais andavam devagar e o
   vsync (que exige ciclo virtual E prazo de host) saía a ~12 Hz. O `PS2X_NO_VIF1` resolve; o GS em CPU (`processGIFPacket`) era a outra metade (86% do tempo com GS ligado): `PS2X_NO_GS`.
3. A thread de decodificação de filme girava 2M `switchThread` porque `CMovie::Play` (menu) não estava coberto pelo hook de skip.
4. `currScreen` 0x14 = `screenWaitForStart` (Press Start): só aceita `screenGetInput(1)==0x10` (Start) quando `betweenScreens==0`; antes ficava preso no fade (animação lenta pelo relógio virtual).
   Telas: 0x10 só faz `changeScreen(0x10->0x14)`, 1 menu, 99 (0x63) carregamento (`Shell::FadeScreen`, `DisplayLoadBackground`); dentro da fase `currScreen=1`.
5. **Câmera lenta na fase (34 vsync/s) era o laço do IOP**, não o VU0 (2026-10-10). Um amostrador de RIP do host (`PS2X_HOSTPROF=1`, `PS2X_HOSTPROF_SKIP=<s>` descarta o carregamento) mostrou ~18% da thread do guest em
   `IopKernel::beginNextReady/nextWakeCycle`: `EeScheduler::accountCycles` chamava `IopEmulator::runEeCycles` a cada checkpoint de EE (poucos ciclos), e cada chamada varre threads/timers/DMA do IOP. Acumular e rodar em lotes
   (`PS2X_IOP_BATCH`, em ciclos de IOP; padrão 256 = 2048 de EE, ~7 µs) leva a fase a **60 vsync/s** (batch 1: 31,8; 64: 48,7; 256: 60,1; 1024: 59,9) e a CPU de ~145% para ~50% de um núcleo. Os mesmos erros de som
   (`snd_BankLoad`) aparecem com e sem lote. O atalho do agendador do VU0 (`PS2X_VU0_VERIFY`: 0 divergências em 1,5M chamadas) não deu ganho mensurável; a reescrita do VU0 em C++ deixou de ser necessária. A mudança está só no clone
   do runtime (`wotm-recomp-win\PS2Recomp`), não no repo.
6. Endereços úteis: currScreen 0x6F8464, nextScreen 0x6F846C, betweenScreens 0x6F7E8C, screenFirstPass 0x6F7E80, targetAlpha 0x6F7E74, currAnimationIndex 0x6F7E88, fadingIn/Out 0x6F807C/0x6F8080, world 0x6F87C4.

**Próximos passos, em ordem:**
1. ~~Versionar o patch de runtime~~ (feito em 2026-10-10, `a6b9af2`); falta só o push no fork.
2. Travessia nativa de verdade: de círculos para malhas, lendo `.NGP/.PTR/.RTX/.TEX` (`docs/FORMATOS.md`) e desenhando via raylib; cortar na fronteira `hierTraverseAsm`/`pktAddVu1ObjAsm` (`config/hw_boundary.csv`). Texturas vêm por `pktAddVu1Tex`/`texmActivateTexture`.
3. Cobertura com gameplay (`PS2X_COVERAGE`, agora em tempo real) para realimentar `config/boot_coverage_asm.csv`; depois voltar à decompilação pela fila.
4. Ideia do dono: um `PS2X_FAST_BOOT` que pule esperas de abertura (cortar só o laço/fade, não a inicialização; ex. `Shell::FadeScreen` 0x1AB660, `screenTransition` 0x19E698, `uiIntro` 0x1DA3C8).
5. Pendentes antigos: PR #277 no PS2Recomp (SQRT.S/RSQRT.S), oracle para funções não-folha, `div.s` por zero e `min/max` com NaN no tradutor.

## Build rápido do runtime (sem LTCG) — 2026-10-10
O build de release do PS2Recomp liga `/GL` + `/LTCG` + IPO (`ps2xRuntime/cmake/ReleaseMode.cmake`): o `link.exe` reotimiza o programa inteiro e leva **~28 min** a cada mudança, mesmo num `.inc` pequeno. Medido no PC do dono (12 núcleos lógicos), na fase da CENTRAL: sem LTCG o jogo continua a **59,8 fps** (tempo real; o runtime segura 60 de propósito), com ~147% de um núcleo contra ~136% do build LTCG. Ou seja, sem perda visível. Reconstrução incremental: **13 s**; build completo do zero: ~4 min.
- Mudança (no clone de teste do runtime, fora deste repo): em `EnableFastReleaseMode`, logo depois do `message(...)`, `if(PS2X_FAST_LINK)` aplica só `/O2 /Ob2 /Oi /Gy /Gw /GF /Zc:inline /fp:fast /DNDEBUG /arch:AVX2 /GS- /Qspectre-` ao alvo e dá `return()` (sem `/GL`, `/LTCG`, IPO).
- Uso: `configure_dev.bat` (mesmo `configure.bat` com `-DPS2X_FAST_LINK=ON -B build_dev`), depois `build_dev.bat`; o executável fica em `build_dev\ps2xRuntime\ps2EntryRunner.exe`. `run_live.ps1` usa o `build_dev` por padrão (`-Build build` volta ao LTCG). O ninja não rastreia os `.inc`: depois de mudar `wotm_scene.inc`/`wotm_live.inc`, `touch ps2xRuntime/src/lib/ps2_runtime.cpp`.
- Medição reproduzível: `measure.ps1 -Exe <exe> -Tag <nome>` (roteiro de pad até a fase, 6 amostras de CPU e fps).
- Teclado (definido pelo dono; backend em `ps2_pad.cpp`, que antes so lia o teclado sem gamepad e agora soma): **setas = analogico esquerdo**, **I/J/K/L = D-pad** (cima/esq./baixo/dir.), **Espaco = X**, **Z = quadrado**, **X = triangulo**, **S = bola**, **Enter = Start**, **Backspace = Select**, **Q = L1**, **W = R1**, **Shift = L1+R1**, **A = L2**, **C = R2**.



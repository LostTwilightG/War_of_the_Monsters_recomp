# PCSX2 automático (teste do ELF meio C++ sem usar o gamepad)

- `play.ps1 <nome-do-elf> [n]`: fecha o PCSX2, abre `build/pcsx2_test/SCUS_971.97_<nome>.elf`, pula as cutscenes (Enter assim que `FMV started` aparece no log),
  entra em 1 player > free-for-all > Togera vs CPU > Midtown Park e vigia o log por `Unrecognized op|ReportErrorAsync|Trap exception` durante `n` x 2 s.
  Tira um print só da janela do PCSX2 (`play_<nome>.png`, ao lado do script) e imprime `OK` ou `FAIL: <linha do log>`.
- `emu.ps1 start|snap|key|status|stop`: peças do roteiro. `start` só abre o ELF; **os bindings de teclado ficam fixos no PCSX2.ini do usuário** (não são mais trocados/restaurados). O roteiro usa `ENTER` (Start) e `SPACE` (Cross; `X` é alias). Não rode o PCSX2 por outro lado enquanto um teste está aberto.
- **Controles (Pad1, sempre teclado)**: WASD = analógico esquerdo; mouse = analógico direito (e IJKL como alternativa); Q/E = L1/R1; Shift/X = L2/R2; Ctrl/V = L3/R3; Espaço = X; clique esquerdo ou U = Quadrado; clique direito ou O = Triângulo; F = Bola; Enter = Start; Backspace = Select; setas = D-pad. O hotkey TogglePause do PCSX2 foi movido de Espaço para Pause.
- **Abrir à mão**: `jogar.bat [nome]` na raiz do repositório (padrão `halfcpp`; ex. `jogar.bat bis_old`, `jogar.bat control_matching`).
- As teclas vão por `PostMessage` direto para a janela do PCSX2: não precisam de foco e não roubam o foco de quem está usando o PC.
  A captura usa `PrintWindow` na janela (nunca a tela inteira).
- Depois de um crash a VM fica pausada: dá para ler a RAM do EE do processo (achar o código retail em memória com `ReadProcessMemory`
  e comparar com o ELF; foi assim que se viu o heap sobrescrevendo o `.nm_extra`).

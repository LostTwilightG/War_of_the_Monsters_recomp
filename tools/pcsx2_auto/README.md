# PCSX2 automático (teste do ELF meio C++ sem usar o gamepad)

- `play.ps1 <nome-do-elf> [n]`: fecha o PCSX2, abre `build/pcsx2_test/SCUS_971.97_<nome>.elf`, pula as cutscenes (Enter assim que `FMV started` aparece no log),
  entra em 1 player > free-for-all > Togera vs CPU > Midtown Park e vigia o log por `Unrecognized op|ReportErrorAsync|Trap exception` durante `n` x 2 s.
  Tira um print só da janela do PCSX2 (`play_<nome>.png`, ao lado do script) e imprime `OK` ou `FAIL: <linha do log>`.
- `emu.ps1 start|snap|key|status|stop`: peças do roteiro. `start` faz backup de `Documents\PCSX2\inis\PCSX2.ini` e troca o Pad1 por teclado
  (X = Cross, Enter = Start, setas, C/V/Z); **`stop` restaura o .ini**. Não rode o PCSX2 por outro lado enquanto um teste está aberto.
- As teclas vão por `PostMessage` direto para a janela do PCSX2: não precisam de foco e não roubam o foco de quem está usando o PC.
  A captura usa `PrintWindow` na janela (nunca a tela inteira).
- Depois de um crash a VM fica pausada: dá para ler a RAM do EE do processo (achar o código retail em memória com `ReadProcessMemory`
  e comparar com o ELF; foi assim que se viu o heap sobrescrevendo o `.nm_extra`).

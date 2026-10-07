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
- Terminar os TUs pequenos de `game` já convertidos: StickShaker, PowerUpTool, MilitaryPickup, PathTool, StartPointTool,
  TankVehicle, MonkeyChains, reset, AiPathTool, SubwayPickup, CarPickup (resto), Vehicle (resto), AiBrain.
- Converter e fazer os TUs seguintes de `game` por tamanho (`config/tus.csv`): Destructible, RigidDebris, Ai*, Monster*, Pickups.
- Priorizar lógica real (IA, monstros, fases, pickups). Não gastar esforço em código de hardware (`config/hw_funcs.txt`).
- Para funções de `game` o fluxo é: escrever C++ natural, compilar, pontuar, embrulhar se não bater, **sem afinar**.

## Armadilhas já vistas
- Strings de uma função que passam de `INCLUDE_ASM` para C mudam o padding do `.rodata`: acrescentar `.word 0` em `.rodata` por asm.
- `switch` em C gera jump table; o retail alinha em 24 palavras (acrescentar `.word 0` x2).
- Classes com vtable: ctor, `__tf` e `_vt$...` ficam como `INCLUDE_ASM`; escrever os métodos sem `virtual`.
- Funções estáticas sem argumentos às vezes têm um `v` no fim do símbolo retail: usar `__asm__("nome__Classev")` no membro.
- O heredoc da ferramenta pode transformar `\n` em quebra de linha real dentro de scripts Python: para arquivos com regex, usar o Edit.

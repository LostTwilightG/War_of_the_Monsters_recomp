# Oracle por recompilação (retail × nossa função)

Compara a função **retail** com a **nossa equivalente** executando as duas, recompiladas para C++ com o
[PS2Recomp](https://github.com/ran-j/PS2Recomp), no mesmo estado inicial. Cobre o que o `tools/difftest.py` (unicorn) não executa:
VU0/COP2 (`lqc2`, `vsub.xyz`…) e `min.s/max.s/madd.s/mula.s…`. Dos 329 `equivalent`, 62 usam uma dessas coisas.

**Nada disto vai para o repo exceto estes scripts.** O C++ gerado é código do jogo (regra 1 do `CLAUDE.md`): fica em `~/wotm-recomp/` no WSL.
O PS2Recomp é GPL; por isso o patch dele não está aqui, só descrito abaixo.

## Preparar (uma vez, no WSL Ubuntu)
1. `git clone https://github.com/ran-j/PS2Recomp ~/wotm-recomp/PS2Recomp`
2. **Corrigir o tradutor de FPU** (`ps2xRecomp/src/lib/fpu_translator.cpp`): no R5900 `SQRT.S fd, ft` lê a fonte em **`ft`** (usa `|ft|`) e
   `RSQRT.S fd, fs, ft` faz `fs / sqrt(|ft|)`. O upstream gera `sqrt(fs)` e `1/sqrt(fs)`, o que erra 108 funções com `sqrt.s` e 76 com `rsqrt.s`.
   Sem isso o oracle acusa divergências falsas (ou esconde verdadeiras).
3. Recompilador + analisador: `cmake -S . -B out/build -G Ninja -DCMAKE_BUILD_TYPE=Release -DPS2X_BUILD_RUNTIME=OFF -DPS2X_BUILD_TEST=OFF -DPS2X_BUILD_STUDIO=OFF && cmake --build out/build`
4. Runtime (só para a `libps2_runtime.a`; precisa de `libx11-dev libgl-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libasound2-dev` e das libs do FFmpeg):
   `cmake -S . -B out/rt -G Ninja -DCMAKE_BUILD_TYPE=Release -DPS2X_BUILD_RECOMP=OFF -DPS2X_BUILD_ANALYZER=OFF -DPS2X_BUILD_TEST=OFF -DPS2X_BUILD_STUDIO=OFF -DPS2X_ENABLE_DEBUG_UI=OFF -DCMAKE_CXX_FLAGS='-msse4.1 -mavx -w' && cmake --build out/rt --target ps2EntryRunner`
   (o GCC precisa de `-msse4.1 -mavx`; o CMake do runtime só os passa para o MSVC)

## Gerar as duas saídas (em `~/wotm-recomp/work/`)
```
cp <repo>/disc/SCUS_971.97 game.elf
ps2_analyzer game.elf config.toml                       # troque  output = "./output_fix/"  no TOML
ps2_recomp config.toml
sh tools/wsl/build_nm.sh nmall                          # no repo: ELF com todas as equivalentes (build/pcsx2_test/SCUS_971.97_nmall.elf)
cp <repo>/build/pcsx2_test/SCUS_971.97_nmall.elf nm_all.elf
ps2_analyzer nm_all.elf config_nm_all.toml              # output = "./output_nm_all/"
ps2_recomp config_nm_all.toml
```

## Rodar
`python3 tools/recomp_oracle/oracle_all.py [filtro ...]` (filtros = pedaços do nome mangled). Resultado em `~/wotm-recomp/work/oracle_all.csv`.
Para cada função candidata gera um harness, com argumentos derivados do nome mangled (`p` ponteiro, `i` inteiro, `f` float em `$f12…`),
200 estados aleatórios numa arena de 16 KB (palavras = float, ponteiro para a própria arena, inteiro pequeno ou lixo), a imagem do ELF retail carregada
(globais) e `$gp`/`$vf0` corretos. Classes:

| classe | significado |
|---|---|
| `exact` | retorno, arena e memória global idênticos |
| `approx` | iguais com tolerância de 1e-3 (ordem de operações float) |
| `retonly` | memória igual; só `f0/v0` diferem (função `void` ou retorno que o harness não sabe tipar): conferir o tipo no fonte |
| `diff` | **memória diferente: divergência real**, investigar |

Limites: só funções **folha** (sem `jal`; 35 das 60 candidatas têm chamadas), assinaturas simples (sem `Q…`, templates, ponteiro de função, `…`),
e funções cujo laço depende do dado aleatório podem estourar o tempo (`Monster::getClosestMonster`). É evidência, não prova.

## Varredura de `sqrt.s` mal codificado
`python3 tools/recomp_oracle/scan_sqrt.py <elf>`: conta `sqrt.s` na forma R5900 (`fs=0`) e na forma errada (`ft=0, fs≠0`). O retail tem 127 e nenhuma errada.
Ver `docs/PROXIMOS_PASSOS.md` (armadilha do `sqrtf`; usar `eeSqrtf` de `include/vecmath.h` em código NM).

## Achados (2026-10-09)
- `AiPathFinder::computeCostEstimate` e mais 10 usos de `sqrtf`: `sqrt.s` codificado na forma MIPS32 → no EE lia `$f0`. Corrigido (`eeSqrtf`). 0/300 → 300/300.
- `StateClimb::handlePreemption`: gravava 4 bytes onde o retail grava 1 (`sb`). Corrigido. 0/200 → 200/200.
- Das 24 funções do lote que rodaram (200 estados cada): 18 só `exact`; 2 só `approx` (`AiNavigator::orientTo`, `strafeTo`); 3 mistas sem `diff`
  (`Ai::creditRelevance(Pickup)`, `AiPathNet::getClosestNode`, `PathNet::getClosestNode`); 1 só `retonly` (`HeliVehicle::updateElevator`, `void`). Nenhuma `diff`.
  Não rodou: `Monster::getClosestMonster` (estouro de tempo com dado aleatório). As outras 35 candidatas têm chamadas (não-folha) e 1 assinatura não suportada.

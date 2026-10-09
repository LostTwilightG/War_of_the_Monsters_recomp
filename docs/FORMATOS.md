# Formatos de arquivo do jogo (o que o código decompilado revela)

Fonte: `Shell::LoadLevelFiles`, `LoadLevelDB`, `LoadMonstersDB`, `LoadResTexture`, `LoadTexture` (`src/game/Shell.cpp`),
`dbsRelocateFileZero`, `dbsRelocateViaPtrListFile` (`src/common/dbs.cpp`), `dbInitDb`, `dbProcInteractive` (`src/game/db.cpp`).
Tudo abaixo foi lido do código; o que está marcado (?) ainda é suposição.

## Arquivos por fase e por monstro
| Extensão | Conteúdo | Carga |
|---|---|---|
| `lvl\<fase>.NGP` | imagem do nível (hierarquia de nós), comprimida em zip | `zipInflateAll(path, 0xA00000)`; depois `fileOnlyNgpFile` |
| `lvl\<fase>.TEX` | texturas do nível | `getNextNgpLoadAddr` + `fileOnlyTexFile` |
| `lvl\<fase>.RTX` | texturas de recurso (`res`) do nível | `fileOnlyResFile` |
| `lvl\<fase>.PTR` | lista de correções do arquivo zero (ver abaixo) | `dbsRelocateFileZero` |
| `<monstro><costume>.ngp/.tex/.rtx` | um monstro (modelo, textura, recursos); `<monstro>` vem de `MonsterLongNames`, `<costume>` é o número do traje | `fileReadf(path, getNextNgpLoadAddr/Res/Tex())` |
| `mon\<nome>.ptr` | correções de ponteiros do monstro `<nome>` | `dbsRelocateViaPtrListFile` |
| `shell\shell|load|ui|shella|preshell.ptr` (+ `.ngp/.tex/.rtx`) | telas do shell (menus, loading, UI) | idem arquivo zero |
| `host0:monster.ngp/.tex/.rtx` | caminho de desenvolvimento (PC host), usado quando o índice da fase é >= 0x1D |

Nomes dos monstros (`MonsterLongNames`, 8 bytes por entrada, índice = monstro): `null, monkey, robot, lizard, shogun, mantis, spider,
energy, lava, dragon, rock, alien, ogre, assboss, jelly, final, temp16, default`. A seleção de cada vaga é `(monstro << 5) | variante`
em `Shell::m_monsterSel[]` (4 jogadores, depois IAs); o traje fica em `m_costume[]`. No modo 6 (mini-games?) carrega os monstros 1..5 e 7..11.
Quirk do retail: para o 3º e o 4º jogador o caminho do arquivo usa o monstro do jogador 2.

### Nomes de arquivo (`Shell::formatFilename`, `formatFilename1`)
Todos começam pelo prefixo da raiz do disco (`D_006F81E0`, string vazia no retail), então os caminhos ficam `\LVL\...` e `\MON\...`.
| Chamada | Tipo | Caminho |
|---|---|---|
| `formatFilename(dst, nome, ext, tipo)` | `SH_FILE_0` (fase) | `\LVL\<nome>.<ext>` |
| | `SH_FILE_PLAYER` | `\MON\<nome>.<ext>` |
| | `SH_FILE_AI` | `\MON\<nome>0.<ext>` ou `\MON\<nome>1.<ext>` (abaixo) |
| `formatFilename1(dst, nome, traje, ext, tipo)` | `SH_FILE_PLAYER`/`SH_FILE_AI` | `\MON\<nome><traje>.<ext>` (traje em decimal) |
| | `SH_FILE_0` | `\LVL\<nome>.<ext>` |
| `Shell::formatFilename(dst, dir, nome, ext)` (estática) | arquivos dos point tools no host | `<dir>/<nome>.<ext>` |

Sufixo das IAs em `formatFilename(..., SH_FILE_AI)`: `1` se `m_mode` é 0 ou 1 e `m_levelNum == 3`; senão `0` se `m_mode != 1`; no modo 1, se a
IA da vaga 4 ou 5 usa o mesmo monstro do jogador 1 (`m_monsterSel[0]`), `1` quando o jogador 1 está no traje 0 e **nada é escrito** quando
ele está em outro traje (quirk do retail: o `dst` fica como estava); sem esse conflito, `0`. Ou seja, a IA carrega a outra pele para não
ficar igual ao jogador 1.

O `LoadLevelFiles` procura `<raiz>\LVL\<fase>.NGP;1` e `.TEX;1` no CD (`fileCdSearchFile`) para saber os tamanhos (barra de progresso) e
carrega, nesta ordem: nível (`.ngp`), monstros, texturas de recurso, texturas; depois `dbsRelocateFileZero` e `dbInitDb`.

## Imagem `.NGP`
Pré-ligada para o endereço **0xA00000** (a de nível é descompactada lá). Palavra 0 = N (número de nós-raiz), palavras 1..N = ponteiros
para os nós-raiz da hierarquia (`_hierhead`; opcode nos 6 bits baixos da primeira palavra, id do objeto nos 14 bits altos).

## Correções (`.PTR`)
Três (arquivo zero) ou quatro (monstros) listas, cada uma `contagem` seguida de `contagem` offsets em bytes dentro da imagem (0 = não usado):
- **arquivo zero** (`dbsRelocateFileZero`): as duas primeiras listas são puladas; a terceira lista os doublewords com `TEX0` do GS, e soma
  `vram / 64` ao campo `TBP0` (14 bits, bits 37..50 do doubleword).
- **monstro** (`dbsRelocateViaPtrListFile`, `idx` > 0): (cabeçalho) palavras 1..N += `endereço_real - 0xA00000`; (1) ponteiros (mesma soma);
  (2) ids de textura de 16 bits += maior id do arquivo anterior; (3) palavras do GS: `TBP0` += fim das texturas do arquivo anterior, e as
  marcadas 0x1B/0x2C/0x24 somam ao campo baixo de 14 bits o fim dos recursos do arquivo anterior.

## Ids de objeto do nível (`dbProcInteractive`)
Id = 14 bits altos da primeira palavra do nó. Os nós de opcode 0x1F e 0x27 são sempre destrutíveis.
| Faixa | Significado |
|---|---|
| 1 | raiz do mundo (`viewSetWorldEpNode`) |
| 2 | céu (`viewSetSkyEntry` x4) |
| 18 | textura da fonte |
| 0x20..0x400 | monstros (`TheGame::MonsterParse`) |
| 0xBB8 | grupo de pedestres (`PedGroup`, 0x190 bytes na pilha de memória; byte `(head>>7)&0xFF` = tamanho) |
| 0x7D0..0x897 | ignorados aqui |
| 0xFA0..0x11F7 | armas (`Weapons::AddWeaponEpNode`) |
| 0x11F8..0x12BF | efeitos: 0x11F8/0x1203 `HomingBug`, 0x11FC bola da `OgreMace`, 0x11F9..0x11FA/0x11FD/0x11FE/0x11FF/0x1201 `SpecFxAnim` |
| 0x12C0..0x13EB | pickups (`LevelPickups::createPickup`); o id vira o "hat id" do pickup |
| 0x13EC..0x144F | power-ups |
| 0x1450..0x1B57 | destrutíveis (`Destructibles::AddDestruct`) |
| 0x1B58..0x1C1F | texturas de partícula |
| 0x1C20..0x2133 | elementos do HUD |
| 0x2134..0x2327 | scripts/ações (0x2134/0x2135/0x2136 só em certos modos; 0x2260.. scripts do usuário) |
| 0x2328..0x270F | pontos de fim do shell (`Shell::AddEpNode`) |
| 0x2710..0x3E7F | pontos de fim por fase (threemile, tokyo 0x2743, canyon2, airport, FinalBoss, BigShot) |

Os pickups guardam o id do slot de `Interactives` no `hierhead`: `Interactives::addInteractive` devolve esse índice.

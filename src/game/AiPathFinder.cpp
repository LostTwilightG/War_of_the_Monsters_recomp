#include "common.h"

INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", __12AiPathFinderR9AiPathNetR7Monster);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computePath__12AiPathFinderPP6AiPathR8_fvectorR6AiPath);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", traceSuccessor__12AiPathFinderR6AiPathR10AiPathNodePQ212AiPathFinder4NodeT1);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computeCostEstimate__12AiPathFinderR6AiPathR10AiPathNodeT1);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computeCostActual__12AiPathFinderR6AiPathR10AiPathNodeT1);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", compileResultForReturn__12AiPathFinderPP6AiPathPQ212AiPathFinder4Node);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", add__Q212AiPathFinder8OpenListPQ212AiPathFinder4Node);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", adjust__Q212AiPathFinder8OpenListPQ212AiPathFinder4Node);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", popBest__Q212AiPathFinder8OpenList);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", find__Q212AiPathFinder8OpenListP6AiPath);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", init__Q212AiPathFinder10GlobalListi);
class BidirLink {
public:
    int next, prev;

    void init(void);
};
void BidirLink::init(void)
{
    prev = 0;
    next = 0;
}

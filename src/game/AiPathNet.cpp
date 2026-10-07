#include "common.h"

int mathfRand(int lo, int hi);

struct AiPathNode {
    char data[0x30];
};
struct AiPath {
    char data[0x14];
};

class AiPathNet {
public:
    unsigned short numNodes;
    unsigned short numPaths;
    char pad4[0xC];
    AiPathNode nodes[0x12C];
    AiPath paths[1];

    AiPathNode *getRandNode(void);
    AiPath *getRandPath(void);
};

INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getRandPath__10AiPathNodeUs);
AiPathNode *AiPathNet::getRandNode(void)
{
    return &nodes[mathfRand(0, numNodes - 1)];
}
AiPath *AiPathNet::getRandPath(void)
{
    return &paths[mathfRand(0, numPaths - 1)];
}
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getClosestNode__9AiPathNetR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getClosestPath__9AiPathNetR8_fvectorUc);
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getPath__9AiPathNetR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", isIn__9AiPathNetR8_fvectorR6AiPath);
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getRangeSq__9AiPathNetR8_fvectorR6AiPath);

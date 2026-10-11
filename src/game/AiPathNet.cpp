#include "common.h"
#include "engine.h"


struct AiPathNode {
    char data[0x30];

    unsigned short getRandPath(unsigned short from);
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
    AiPathNode *getClosestNode(_fvector &pos);
    AiPath *getPath(_fvector &pos);
    int isIn(_fvector &pos, AiPath &path);
};
extern float closestStart __asm__("D_006F7980");
__asm__("#SNFIX_SMALL D_006F7980");

#ifdef NON_MATCHING
/* A random neighbouring path of this node (list of u16 at +0x10, count at +0x28), never the one we came from when there is a choice. */
unsigned short AiPathNode::getRandPath(unsigned short from)
{
    unsigned char count = *(unsigned char *)(data + 0x28);
    unsigned short *list = (unsigned short *)(data + 0x10);

    switch (count) {
    case 1:
        return list[0];
    case 0:
        return from;
    default: {
        int r = mathfRand(0, count - 1) & 0xFF;
        int idx = r;

        if (list[r] == from)
            idx = (r + 1) % count & 0xFF;
        return list[idx];
    }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getRandPath__10AiPathNodeUs);
#endif
AiPathNode *AiPathNet::getRandNode(void)
{
    return &nodes[mathfRand(0, numNodes - 1)];
}
AiPath *AiPathNet::getRandPath(void)
{
    return &paths[mathfRand(0, numPaths - 1)];
}
#ifdef NON_MATCHING
AiPathNode *AiPathNet::getClosestNode(_fvector &pos)
{
    float best = closestStart;
    AiPathNode *res = &nodes[0];
    float *p = (float *)&pos;
    int i;

    for (i = 0; i < numNodes; i++) {
        float *n = (float *)&nodes[i];
        float dx = p[0] - n[0];
        float dy = p[1] - n[1];
        float dz = p[2] - n[2];
        float d = dx * dx + dy * dy + dz * dz;

        if (d < best) {
            best = d;
            res = &nodes[i];
        }
    }
    return res;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getClosestNode__9AiPathNetR8_fvector);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getClosestPath__9AiPathNetR8_fvectorUc);
#ifdef NON_MATCHING
/* The path whose volume contains `pos`, or 0. */
AiPath *AiPathNet::getPath(_fvector &pos)
{
    int i;

    for (i = 0; i < numPaths; i++) {
        if (isIn(pos, paths[i]))
            return &paths[i];
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getPath__9AiPathNetR8_fvector);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", isIn__9AiPathNetR8_fvectorR6AiPath);
INCLUDE_ASM("asm/nonmatchings/game/AiPathNet", getRangeSq__9AiPathNetR8_fvectorR6AiPath);

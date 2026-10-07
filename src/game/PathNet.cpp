#include "common.h"
#include "hieri_types.h"

int mathfRand(int lo, int hi);
extern float D_006F7DC4; /* initial "closest distance squared" for node searches */

struct PathLink {
    unsigned short id;
    unsigned short data;
} __attribute__((packed));

class PathNode {
public:
    _fvector pos;
    unsigned char type;
    unsigned char numPaths;
    PathLink paths[9];

    void removePath(unsigned short id);
};

class PathNet {
public:
    unsigned short numNodes;
    char pad2[0xE];
    PathNode nodes[1];

    PathNode *getRandNode(void);
    PathNode *getClosestNode(_fvector &pos, int type);
};

void PathNode::removePath(unsigned short id)
{
    unsigned i;

    for (i = 0; i < numPaths; i++) {
        if (paths[i].id == id) {
            numPaths--;
            for (; i < numPaths; i++)
                paths[i] = paths[i + 1];
            return;
        }
    }
}
#ifdef NON_MATCHING
/* 9/16 words, untuned */
PathNode *PathNet::getRandNode(void)
{
    return &nodes[mathfRand(0, numNodes - 1)];
}
#else
INCLUDE_ASM("asm/nonmatchings/game/PathNet", getRandNode__7PathNet);
#endif
#ifdef NON_MATCHING
/* 6/41 words, untuned: retail uses VU0 inline vector math */
PathNode *PathNet::getClosestNode(_fvector &pos, int type)
{
    PathNode *best = nodes;
    float bestDist = D_006F7DC4;
    PathNode *n = nodes;
    int left = numNodes;

    for (; left != 0; left--, n++) {
        if (type < 0 || n->type == type) {
            _fvector d;
            float dist;

            d.x = pos.x - n->pos.x;
            d.y = pos.y - n->pos.y;
            d.z = pos.z - n->pos.z;
            dist = d.x * d.x + d.y * d.y + d.z * d.z;
            if (dist < bestDist) {
                bestDist = dist;
                best = n;
            }
        }
    }
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/PathNet", getClosestNode__7PathNetR8_fvectori);
#endif
INCLUDE_ASM("asm/nonmatchings/game/PathNet", getClosestPath__7PathNetR8_fvectorRii);
INCLUDE_ASM("asm/nonmatchings/game/PathNet", getClosestPath__7PathNetR8_fvectorT1Rii);

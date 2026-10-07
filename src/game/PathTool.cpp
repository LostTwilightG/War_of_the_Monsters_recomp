#include "common.h"
#include "game/shell.h"
#include "hieri_types.h"
#include "game/game.h"

extern char D_006F7DC8[];
__asm__("#SNFIX_SMALL shell");

struct PathLinkOut {
    unsigned short id;
    unsigned char a;
    unsigned char b;
};

struct PathNodeOut {
    _fvector pos;
    unsigned char type;
    unsigned char numPaths;
    PathLinkOut paths[6];
};

struct PathNetOut {
    unsigned short numNodes;
    char pad2[0xE];
    PathNodeOut nodes[256];
};

struct PathPoint {
    _fvector pos;
    int numPaths;
    unsigned short ids[1][2];
    char pad18[0x2C - 0x18];
    unsigned char a[6];
    unsigned char type;
    unsigned char b[6];
    char pad39[0x40 - 0x39];
};

/* PointToolKit is the base in retail; its layout is flattened here. */
class PathTool {
public:
    PathPoint points[256];
    int numPoints;
    char pad4004[0x4050 - 0x4004];
    PathNetOut *net;
    char *name;
    char pad4058[4];
    void *levelData;

    void init(int i) __asm__("init__8PathTooli");
    void loadPoints(char *n, bool b);
    void exportPoints(void);
};
void PointToolKitInit(PathTool *t, int i) __asm__("init__12PointToolKiti");
PathPoint *PointToolKitGetPoint(PathTool *t, int i) __asm__("getPoint__12PointToolKiti");
void PointToolKitLoad(PathTool *t, char *name, bool b) __asm__("loadPoints__12PointToolKitPcb");

INCLUDE_ASM("asm/nonmatchings/game/PathTool", _vt$8PathTool);
void PathTool::loadPoints(char *n, bool b)
{
    char path[0x20];
    char *fmt = D_006F7DC8;

    PointToolKitLoad(this, (Shell::formatFilename(path, fmt, n ? n : shell->GetLevelName(), name), path), b);
}
void PathTool::init(int i)
{
    PointToolKitInit(this, 0);
    levelData = gameSlotBase(game->m_levelIdx);
}
#ifdef NON_MATCHING
/* 7/99 words, untuned */
void PathTool::exportPoints(void)
{
    PathPoint *p = PointToolKitGetPoint(this, 0);
    PathNodeOut *n;
    int count = 256;

    if (numPoints < 256)
        count = numPoints;
    net->numNodes = count;
    if (net->numNodes == 0) {
        net->nodes[0].numPaths = 1;
        net->nodes[0].paths[0].id = 1;
        net->nodes[0].paths[0].a = 250;
        net->nodes[0].paths[0].b = 250;
        net->nodes[0].pos.x = -100.0f;
        net->nodes[0].pos.z = 250.0f;
        net->nodes[0].pos.y = 0.0f;
        net->nodes[1].numPaths = 1;
        net->nodes[1].paths[0].id = 0;
        net->nodes[1].paths[0].a = 250;
        net->nodes[1].paths[0].b = 250;
        net->nodes[1].pos.x = 100.0f;
        net->nodes[1].pos.z = 250.0f;
        net->nodes[1].pos.y = 0.0f;
        net->numNodes = 2;
    } else {
        int i, j;

        n = net->nodes;
        for (i = net->numNodes; i != 0; i--, p++, n++) {
            n->pos = p->pos;
            n->type = p->type;
            n->numPaths = p->numPaths < 7 ? p->numPaths : 6;
            for (j = 0; j < n->numPaths; j++) {
                n->paths[j].id = p->ids[j][0];
                n->paths[j].a = p->a[j];
                n->paths[j].b = p->b[j];
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/PathTool", exportPoints__8PathTool);
#endif
INCLUDE_ASM("asm/nonmatchings/game/PathTool", __static_initialization_and_destruction_0_0017F2F0);
INCLUDE_ASM("asm/nonmatchings/game/PathTool", __tf8PathTool);
INCLUDE_ASM("asm/nonmatchings/game/PathTool", __8PathToolP7PathNetPcT2);
INCLUDE_ASM("asm/nonmatchings/game/PathTool", _GLOBAL_$I$_8PathTool$instance);

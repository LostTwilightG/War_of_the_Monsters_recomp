#include "common.h"
#include "hieri_types.h"
#include "cs_pool.h"
#include "engine.h"
#include "game/shell.h"

struct _dbheader;
struct _HierCollisionGrid;
struct _animmgr;
class ScreenPolys {
public:
    static void clearSources(void) __asm__("clearSources__11ScreenPolysv");
};

void hdSaveColGrid(_HierCollisionGrid *g);
void animationInitGlobal(void);
void animationInitTweakers(void);
void animationSetGlobal(_hierhead *h, int file);
void animationSetInstance(_animmgr *a, int i);
int getNgpFilesLoaded(void);
char *getNgpAddr(int i);
void dbsRelocateViaPtrListFile(int idx, _vramAddrs vram);
void dbProcInteractive(_hierhead *h, _fvector *v, float (*m)[4]);
void dbsTraverse(_hierhead **h, void (*fn)(_hierhead *, _fvector *, float (*)[4]), _fvector *v);

extern short g_dbsDisableNodeMask;
extern char *game;


/* Initialises the loaded .NGP images: image k (0 = the level, then the monsters) starts with N, followed by N pointers to root nodes.
   Animation roots (opcode 7) are registered first; every other root is walked by dbsTraverse (dbProcInteractive); for monster images
   the animation roots are bound to the game's animation manager. */
#ifdef NON_MATCHING
/* 74/120 words: untuned, from the m2c draft */
void dbInitDb(_dbheader *db, _vramAddrs vram)
{
    _fvector v;
    int file;
    int i;

    hdSaveColGrid(0);
    switch (shell->m_mode) {
    case 1:
        g_dbsDisableNodeMask = 0x80;
        break;
    case 2:
    case 4:
    case 5:
        g_dbsDisableNodeMask = 0x10;
        break;
    case 3:
    case 6:
    case 7:
    case 8:
    case 9:
        g_dbsDisableNodeMask = 0x20;
        break;
    default:
        g_dbsDisableNodeMask = 0;
        break;
    }
    ScreenPolys::clearSources();
    animationInitGlobal();
    animationInitTweakers();
    for (file = 0; file < getNgpFilesLoaded(); file++) {
        int *img = (int *)getNgpAddr(file);
        dbsRelocateViaPtrListFile(file, vram);
        for (i = 0; i < img[0]; i++) {
            _hierhead *root = (_hierhead *)img[1 + i];

            if (*(unsigned *)root % 64 == 7)
                animationSetGlobal(root, file);
        }
        for (i = 0; i < img[0]; i++) {
            _hierhead *root = (_hierhead *)img[1 + i];

            if (*(unsigned *)root % 64 != 7)
                dbsTraverse((_hierhead **)&img[1 + i], dbProcInteractive, &v);
            else if (file != 0)
                animationSetInstance((_animmgr *)root, *(int *)(game + 0x120448));
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/db", dbInitDb__FP9_dbheader10_vramAddrs);
#endif
INCLUDE_ASM("asm/nonmatchings/game/db", dbProcInteractive__FP9_hierheadP8_fvectorPA3_f);
_cs *dbGetCSForModel(_hierhead *h)
{
    _cs *cs = CsPool::csActivate();

    cs->drawMe = 1;
    hierSetCsEpNode(cs, h);
    return cs;
}
_cs *dbGetHPCSForModel(_hierhead *h)
{
    _cs *cs = CsPool::csHPActivate();

    cs->drawMe = 1;
    hierSetCsEpNode(cs, h);
    return cs;
}

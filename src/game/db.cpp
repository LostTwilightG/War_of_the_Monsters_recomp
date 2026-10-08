#include "common.h"
#include "hieri_types.h"
#include "cs_pool.h"
#include "engine.h"
#include "game/shell.h"
#include "memory_stack.h"

struct _dbheader;
void *operator new(unsigned, void *);
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

class Weapons {
public:
    void AddWeaponEpNode(int idx, _hierhead *h);
};
class TheGame {
public:
    void MonsterParse(_hierhead *h, _fvector *pos);
};
extern TheGame *game;
class Destructibles {
public:
    void AddDestruct(int id, _hierhead *h, _fvector *pos, float (*m)[4]);
};
extern Destructibles *destructibles;
class PedGroup {
public:
    PedGroup(unsigned char n, _fvector &pos);
};
class HomingBug {
public:
    static void addInstance(_hierhead *h);
};
class SpecFxAnim {
public:
    static void addInstance(_hierhead *h, int i);
};
class OgreMace {
public:
    static _hierhead *s_ballEp;
};
enum ePickupType { PICKUP_TYPE_0 };
class LevelPickups {
public:
    static void createPickup(_hierhead *h, _fvector &pos, float (*m)[4], bool b, ePickupType t);
};
class PowerUps {
public:
    static PowerUps instance;
    void addPowerUpEpNode(_hierhead *h);
};
class Hud {
public:
    static void parseHudElements(_hierhead *h);
};
class ActHead;
class Destructible;
struct _hierScript;
class ActionDispatch {
public:
    static void addAction(ActHead *a, float (*m)[4], _fvector *v, Destructible *d);
    static void addUserDefinedScipt(_hierScript *s);
};
class FinalBoss {
public:
    void addEpNode(_hierhead *h);
};
extern FinalBoss finalBoss;
class BigShotLevel {
public:
    static BigShotLevel instance;
    void addEpNode(_hierhead *h);
};
void viewSetWorldEpNode(_hierhead *h);
void viewSetSkyEntry(_hierhead *h, int i);
void fontSetTexId(_hierhead *h);
void particleParseTexture(int i, _hierhead *h);
void threeMileAddEpNode(_hierhead *h);
void tokyoAddEpNode(_hierhead *h);
void canyon2AddEpNode(_hierhead *h);
void airportAddEpNode(_hierhead *h);
extern int g_noScripts;
extern MemoryStack gMemStackDb __asm__("_11MemoryStack$global");



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
                animationSetInstance((_animmgr *)root, *(int *)((char *)game + 0x120448));
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/db", dbInitDb__FP9_dbheader10_vramAddrs);
#endif
/* Level object dispatcher: the high 14 bits of a node's head word are the object id, which selects what the node means.
     1 world root, 2 sky, 18 font texture, 0x20..0x400 monsters, 0xBB8 pedestrian group, 0xFA0..0x11F7 weapons,
     0x11F8..0x12BF effects (bugs, special fx, ogre mace ball), 0x12C0..0x13EB pickups, 0x13EC..0x144F power-ups,
     0x1450..0x1B57 and 0x7D0..0x897 (excluded) destructibles, 0x1B58..0x1C1F particle textures, 0x1C20..0x2133 HUD elements,
     0x2134..0x2327 scripts/actions, 0x2328..0x270F shell end points, 0x2710..0x3E7F per-level end points.
   Opcode 0x1F and 0x27 nodes are always destructibles. */
#ifdef NON_MATCHING
/* 6/275 words: untuned, from the m2c draft (branch order differs) */
void dbProcInteractive(_hierhead *h, _fvector *pos, float (*m)[4])
{
    unsigned head = *(unsigned *)h;
    int op = head & 0x3F;
    unsigned id = head >> 18;

    if (op == 0x1F || op == 0x27) {
        destructibles->AddDestruct(id, h, pos, m);
        return;
    }
    if (id - 0x7D0 < 0xC8)
        return;
    if ((int)id < 0x20) {
        switch (id) {
        case 1:
            viewSetWorldEpNode(h);
            break;
        case 2:
            viewSetSkyEntry(h, 0);
            viewSetSkyEntry(h, 1);
            viewSetSkyEntry(h, 2);
            viewSetSkyEntry(h, 3);
            break;
        case 18:
            fontSetTexId(h);
            break;
        }
    } else if ((int)id < 0x401) {
        game->MonsterParse(h, pos);
    } else if ((int)id < 0xFA0) {
        if (id == 0xBB8) {
            char *mem = (char *)(((int)gMemStackDb.low + 0xF) & ~0xF);

            gMemStackDb.low = mem + 0x190;
            new (mem) PedGroup((head >> 7) & 0xFF, *pos);
        }
    } else if ((int)id < 0x11F8) {
        ((Weapons *)((char *)game + 0x112490))->AddWeaponEpNode(id, h);
    } else if ((int)id < 0x12C0) {
        switch (id) {
        case 0x11F8:
        case 0x1203:
            HomingBug::addInstance(h);
            break;
        case 0x11FC:
            OgreMace::s_ballEp = h;
            break;
        default:
            if (id - 0x11F9 < 2 || id == 0x11FD || id == 0x11FE || id == 0x11FF || id == 0x1201)
                SpecFxAnim::addInstance(h, *(int *)((char *)game + 0x120448));
            break;
        }
    } else if ((int)id < 0x13EC) {
        LevelPickups::createPickup(h, *pos, m, false, PICKUP_TYPE_0);
    } else if ((int)id < 0x1450) {
        PowerUps::instance.addPowerUpEpNode(h);
    } else if ((int)id < 0x1B58) {
        destructibles->AddDestruct(id, h, pos, m);
    } else if ((int)id < 0x1C20) {
        particleParseTexture(id - 0x1B58, h);
    } else if ((int)id < 0x2134) {
        Hud::parseHudElements(h);
    } else if ((int)id < 0x2328) {
        if (g_noScripts == 0) {
            int mode = *(int *)((char *)game + 0x1203C8);

            if ((id == 0x2134 && mode == 2) || (id == 0x2135 && mode == 3) || (id == 0x2136 && mode == 1))
                ActionDispatch::addAction(*(ActHead **)((char *)h + 0xC), (float (*)[4])((char *)h + 0x10), (_fvector *)((char *)h + 0x50), 0);
            if (id - 0x2260 < 0xC8)
                ActionDispatch::addUserDefinedScipt((_hierScript *)h);
        }
    } else if ((int)id < 0x2710) {
        shell->AddEpNode(id - 0x2328, h);
    } else if ((int)id < 0x3E80) {
        if ((int)id < 0x2729)
            threeMileAddEpNode(h);
        else if (id == 0x2743)
            tokyoAddEpNode(h);
        else if ((int)id < 0x278D)
            canyon2AddEpNode(h);
        else if ((int)id < 0x27A6)
            airportAddEpNode(h);
        else if ((int)id < 0x27BF)
            finalBoss.addEpNode(h);
        else if ((int)id < 0x27C9)
            BigShotLevel::instance.addEpNode(h);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/db", dbProcInteractive__FP9_hierheadP8_fvectorPA3_f);
#endif
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

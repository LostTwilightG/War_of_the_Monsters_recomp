#include "common.h"
#include "hieri_types.h"

extern int D_006F8C08;
extern int D_006F8C0C;
extern int D_006F8C10;
extern int D_006F8C18;
extern int D_006F8C1C;
extern int D_006F8C14;
extern int D_006F8C20;
extern _fvector D_0070B5C0;
float mathfRandf(float lo, float hi);
float mathfHeadingFromPointToPoint(_fvector *a, _fvector *b);

struct _animHandle {
    int a, b, c, d;
};
void animationStart(_animHandle h, bool b);
int mathfRand(int lo, int hi);
extern _animHandle D_0070B590;
extern _animHandle D_0070B5A0;
extern _animHandle D_0070B5B0;
extern int D_006F8C18;
extern int D_006F8C1C;
extern int D_006F8C08;
extern int D_006F8C0C;

class Hud {
public:
    void addMessage(int a, int b);
};

class Weapons {
public:
    void CreateLavaBall(_fvector *pos, _fvector *dir, _fvector *target, int a, int b);
};
class TheGameW {
public:
    char pad0[0x112490];
    Weapons weapons;
};
extern TheGameW *game;
void islandFireLava(_fvector *target, int owner);
__asm__("#SNFIX_SMALL D_006F8C20");

void islandInitBefore(void)
{
    D_006F8C08 = 0;
    D_006F8C0C = 0;
    D_006F8C10 = 0;
    D_006F8C18 = 0;
    D_006F8C1C = 0;
    D_006F8C20 = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/island", islandInitAfter__Fv);
INCLUDE_ASM("asm/nonmatchings/game/island", islandUpdate__Fv);
#ifdef NON_MATCHING
/* 7/113 words, untuned: written from the asm with raw game offsets */
void islandEruptVolcano(int type, int id)
{
    int i;

    if (D_006F8C0C > 0)
        return;
    D_006F8C14 = id;
    D_006F8C08 = 90;
    D_006F8C10 = 0;
    *(int *)((char *)game + 0x121610 + 0xF0) = 1;
    D_006F8C0C = mathfRand(600, 1200);
    if (type == 0x2729) {
        D_006F8C18 = 1;
        animationStart(D_0070B590, true);
    } else if (type == 0x272A) {
        D_006F8C1C = 1;
        animationStart(D_0070B5A0, true);
    } else {
        D_006F8C20 = 1;
        animationStart(D_0070B5B0, true);
    }
    for (i = 0; i < *(int *)((char *)game + 0x1203D4); i++) {
        char *pl = (char *)game + 0xB80 + i * 0x11190;

        if (*(int *)(pl + 0x20) == D_006F8C14) {
            ((Hud *)((char *)game + *(int *)(pl + 0x6CD8) * 0x2E0))->addMessage(0xE, 1);
        } else {
            _fvector v;

            v.x = *(float *)(pl + 0x3E60);
            v.y = *(float *)(pl + 0x3E64);
            v.z = *(float *)(pl + 0x3E68) + 50.0f;
            islandFireLava(&v, *(int *)(pl + 0x20));
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/island", islandEruptVolcano__Fii);
#endif
void islandFireLava(_fvector *target, int owner)
{
    _fvector pos, dir;

    pos.x = D_0070B5C0.x + mathfRandf(-300.0f, 300.0f);
    pos.y = D_0070B5C0.y + mathfRandf(-300.0f, 300.0f);
    pos.z = D_0070B5C0.z;
    dir.x = mathfRandf(0.0f, 30.0f) * 0.017453292f;
    dir.y = 0.0f;
    if (D_006F8C10 < 10) {
        dir.z = mathfHeadingFromPointToPoint(&pos, target);
        game->weapons.CreateLavaBall(&pos, &dir, target, D_006F8C14, owner);
    }
    D_006F8C10++;
}
INCLUDE_ASM("asm/nonmatchings/game/island", __static_initialization_and_destruction_0_00147B18);
INCLUDE_ASM("asm/nonmatchings/game/island", _GLOBAL_$I$islandInitBefore__Fv);

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
INCLUDE_ASM("asm/nonmatchings/game/island", islandEruptVolcano__Fii);
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

#include "common.h"

extern int D_006F8C08;
extern int D_006F8C0C;
extern int D_006F8C10;
extern int D_006F8C18;
extern int D_006F8C1C;
extern int D_006F8C20;
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
INCLUDE_ASM("asm/nonmatchings/game/island", islandFireLava__FP8_fvectori);
INCLUDE_ASM("asm/nonmatchings/game/island", __static_initialization_and_destruction_0_00147B18);
INCLUDE_ASM("asm/nonmatchings/game/island", _GLOBAL_$I$islandInitBefore__Fv);

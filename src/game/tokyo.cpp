#include "common.h"
#include "hieri_types.h"

extern int tidalWaveActive;
extern int tidalWaveCount;
extern int tidalSource;
extern _hierhead *D_006F8CFC;
extern int D_006F8D00;

INCLUDE_ASM("asm/nonmatchings/game/tokyo", _vt$8UfoTokyo);
int tokyoGetTidalWaveSource(void)
{
    return tidalSource;
}
void tokyoInitBefore(void)
{
    tidalWaveActive = 0;
    tidalWaveCount = 0;
    tidalSource = 0;
    D_006F8CFC = 0;
    D_006F8D00 = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/tokyo", tokyoInitAfter__Fv);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", tokyoUpdate__Fv);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", tokyoStartTidalWave__FP7Monster);
void tokyoAddEpNode(_hierhead *h)
{
    D_006F8CFC = h;
}
INCLUDE_ASM("asm/nonmatchings/game/tokyo", init__8UfoTokyo);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", update__8UfoTokyo);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", takeHit__8UfoTokyoP8_fvectorfi);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", __static_initialization_and_destruction_0_001D7390);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", func_001D73E0);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", func_001D73F0);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", func_001D7418);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", __tf8UfoTokyo);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", func_001D7478);
INCLUDE_ASM("asm/nonmatchings/game/tokyo", _GLOBAL_$I$tidalWaveAnim);

#include "common.h"

void ufoInitBefore(void)
{
}
extern int ufoFlag __asm__("D_006F8D04");
extern int movieActive;
extern int movieCount;
__asm__("#SNFIX_SMALL movieActive");
__asm__("#SNFIX_SMALL movieCount");

void ufoInitAfter(void)
{
    ufoFlag = 0;
    movieActive = 0;
    movieCount = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/ufo", ufoUpdate__Fv);

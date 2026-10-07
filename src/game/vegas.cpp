#include "common.h"
#include "game/level_pickups.h"

extern int vegasSpawnMilitary;
extern int whichAction_006F8550;
__asm__("#SNFIX_SMALL vegasSpawnMilitary");
__asm__("#SNFIX_SMALL whichAction_006F8550");

void vegasInitBefore(void)
{
    vegasSpawnMilitary = 0;
}
void vegasInitAfter(void)
{
    whichAction_006F8550 = 0;
    LevelPickups::turnOffMilitary();
}
INCLUDE_ASM("asm/nonmatchings/game/vegas", vegasUpdate__Fv);

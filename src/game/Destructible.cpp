#include "common.h"

INCLUDE_ASM("asm/nonmatchings/game/Destructible", Init__13Destructibles);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", AddDestruct__13DestructiblesiP9_hierheadP8_fvectorPA3_f);
void Update__13Destructibles(void *self) __asm__("Update__13Destructibles");
void Update__13Destructibles(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/Destructible", rebuildCanyon2Pillars__13Destructibles);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", rebuildCapitolPillars__13Destructibles);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", takeHit__12DestructibleP8_fvectorfi);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", GenericTakeHit__12DestructibleP8_fvectorfii);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", ChangeState__12DestructibleibT2);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", SwitchState__12Destructible);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", DispatchAction__12DestructibleP7ActHeadPA3_f);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", doDebris__12DestructibleP9ActDebrisPA3_f);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", shedMonsters__12Destructible);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", func_00131038);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", func_00131048);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", getVel__13DbInteractive);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", _vt$12Destructible);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", __tf12Destructible);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", SwitchState__12DestructiblePv);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", getHealth__12Destructible);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", CurrentState__12Destructible);
int getHitType__12Destructible(void *self) __asm__("getHitType__12Destructible");
int getHitType__12Destructible(void *self)
{
    return *(int *)((char *)self + 0x30);
}
INCLUDE_ASM("asm/nonmatchings/game/Destructible", getId1__12Destructible);
INCLUDE_ASM("asm/nonmatchings/game/Destructible", func_00131120);

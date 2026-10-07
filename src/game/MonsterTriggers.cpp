#include "common.h"
#include "game/game.h"

struct TriggerHead;
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", checkTriggerTree__Fv);
void doDeathVolume(Monster *m, TriggerHead *t)
{
    m->takeDamage(m->m_health, false, 0);
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doSoundVolume__FP7MonsterP11TriggerHead);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doDamageVolume__FP7MonsterP11TriggerHead);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doHealthVolume__FP7MonsterP11TriggerHead);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doStaminaVolume__FP7MonsterP11TriggerHead);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doFireVolume__FP7MonsterP11TriggerHead);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doShockVolume__FP7MonsterP11TriggerHead);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doImpaleVolume__FP7MonsterP11TriggerHead);
void doCamUnifyVolume(Monster *m, TriggerHead *t)
{
    m->m_camUnify = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doCamVolume__FP7MonsterP11TriggerHead);

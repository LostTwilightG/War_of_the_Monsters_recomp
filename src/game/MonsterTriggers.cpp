#include "common.h"

struct TriggerHead;
class Monster {
public:
    char pad0[0x44C];
    float health;
    char pad450[0x7970 - 0x450];
    int camUnify;

    void takeDamage(float dmg, bool b, Monster *src);
};

INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", checkTriggerTree__Fv);
void doDeathVolume(Monster *m, TriggerHead *t)
{
    m->takeDamage(m->health, false, 0);
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
    m->camUnify = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doCamVolume__FP7MonsterP11TriggerHead);

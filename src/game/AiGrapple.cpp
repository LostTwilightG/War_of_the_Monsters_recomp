#include "common.h"

struct DbInteractive;

class StaminaMeter {
public:
    char pad0[8];
    float maxLevel;
    char padC[4];
    float level;

    float getMaxLevel(void);
};

struct GrappleMonster {
    char pad0[0x460];
    StaminaMeter stamina;
    char pad474[0x68B4 - 0x474];
    void *target;
};

class Ai {
public:
    GrappleMonster *monster;
    char pad4[0x4C - 4];
    int state;

    void setFocus(DbInteractive *d);
};

class AiGrappleThrow {
public:
    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void exitAction(Ai &ai);
};

class AiGrappleAttack {
public:
    float getEntryRelevance(Ai &ai);
    void exitAction(Ai &ai);
};

INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", update__20AiGrappledActionListR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", update__21AiGrapplingActionListR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", __14AiGrappleThrow);
#ifdef NON_MATCHING
/* 2/27 words, untuned: FP register allocation */
float AiGrappleThrow::getEntryRelevance(Ai &ai)
{
    StaminaMeter *m = &ai.monster->stamina;
    float lvl = m->level;
    float r = lvl / m->getMaxLevel();
    float t = 1.0f - r * r;

    return t * 0.95f + 0.05f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", getEntryRelevance__14AiGrappleThrowR2Ai);
#endif
#ifdef NON_MATCHING
/* 2/15 words, untuned: retail leaves nops in the delay slots */
float AiGrappleThrow::getExitRelevance(Ai &ai)
{
    if (ai.monster->target) {
        if (ai.state == 1)
            return 1.0f;
    }
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", getExitRelevance__14AiGrappleThrowR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", enterAction__14AiGrappleThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", updateAction__14AiGrappleThrowR2Ai);
void AiGrappleThrow::exitAction(Ai &ai)
{
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", __15AiGrappleAttack);
float AiGrappleAttack::getEntryRelevance(Ai &ai)
{
    return 1.0f;
}
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", getExitRelevance__15AiGrappleAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", enterAction__15AiGrappleAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", updateAction__15AiGrappleAttackR2Ai);
void AiGrappleAttack::exitAction(Ai &ai)
{
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", __tf20AiGrappledActionList);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", __tf21AiGrapplingActionList);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", __tf14AiGrappleThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", __tf15AiGrappleAttack);
float StaminaMeter::getMaxLevel(void)
{
    return maxLevel;
}
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", func_00106230);
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", func_00106270);

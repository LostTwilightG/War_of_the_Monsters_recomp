#include "common.h"
#include "game/stamina_meter.h"
#include "game/game.h"
#include "engine.h"

struct DbInteractive;

class GamePadClipPlayer;
class GamePad;
class AiActionTuple {
public:
    int getFieldsSinceEval(void);
};
class AiPadClips {
public:
    static int getThrow(void) __asm__("getThrow__10AiPadClipsv");
};
class GamePadClipPlayer {
public:
    int clip;
    int update(GamePad &pad);
    void rewind(void);
};

class Ai {
public:
    Monster *monster;
    char pad4[0x44 - 4];
    char *pad;
    GamePadClipPlayer clipPlayer;
    int state;

    void setFocus(DbInteractive *d);
    int getButtonMashDelay(void);
    int getReflexDelay(void);
    void targetPin(void);
    void lightPunch(void);
    void heavyPunch(void);
    void toss(void);
};

class AiGrappleThrow {
public:
    char pad0[0x48];
    int timer;

    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void exitAction(Ai &ai);
};

class AiGrappleAttack {
public:
    char pad0[0x48];
    int timer;

    void updateAction(Ai &ai);
    void enterAction(Ai &ai);
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
    StaminaMeter *m = &ai.monster->m_stamina;
    float lvl = m->cur;
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
    if (ai.monster->m_target) {
        if (ai.state == 1)
            return 1.0f;
    }
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", getExitRelevance__14AiGrappleThrowR2Ai);
#endif
void AiGrappleThrow::enterAction(Ai &ai)
{
    int d = ai.getReflexDelay();

    d -= ((AiActionTuple *)this)->getFieldsSinceEval();
    timer = (d > -1) ? d : 0;
    ai.setFocus((DbInteractive *)ai.monster->m_target);
    ai.clipPlayer.clip = AiPadClips::getThrow();
    ai.clipPlayer.rewind();
}
void AiGrappleThrow::updateAction(Ai &ai)
{
    if (--timer > 0)
        return;
    ai.targetPin();
    if (!ai.clipPlayer.update(*(GamePad *)ai.pad))
        ai.clipPlayer.rewind();
}
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
void AiGrappleAttack::enterAction(Ai &ai)
{
    int d;

    ai.setFocus((DbInteractive *)ai.monster->m_target);
    d = ai.getReflexDelay();
    d -= ((AiActionTuple *)this)->getFieldsSinceEval();
    timer = (d > -1) ? d : 0;
}
#ifdef NON_MATCHING
/* 11/57 words, untuned */
void AiGrappleAttack::updateAction(Ai &ai)
{
    if (--timer > 0)
        return;
    timer = ai.getButtonMashDelay();
    if (game->m_matchMode >= 0 && game->m_matchMode < 2) {
        if (mathfRand(0, 5) == 0) {
            ai.pad[0xE] = 0xFF;
            ai.toss();
        } else {
            ai.lightPunch();
        }
    } else {
        int r = mathfRand(1, 100);

        if (r < 51)
            ai.lightPunch();
        else if (r < 61)
            ai.heavyPunch();
        else {
            ai.pad[0xE] = 0xFF;
            ai.toss();
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiGrapple", updateAction__15AiGrappleAttackR2Ai);
#endif
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

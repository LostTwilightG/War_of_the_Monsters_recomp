#include "common.h"

#include "engine.h"
#include "game/game.h"
#include "game/ai.h"
#include "game/ai_action.h"
#include "game/enemy_info.h"
#include "game/pad_flags.h"

#define MI(m, o) (*(int *)((char *)(m) + (o)))
#define MF(m, o) (*(float *)((char *)(m) + (o)))
#define MB(m, o) (*(signed char *)((char *)(m) + (o)))
#define MP(m, o) ((char *)(m) + (o))
#define STATE_ID(m) (*(m)->m_state)
#define NAV(ai) ((AiNavigator *)((char *)&(ai) + 0x80))
#define LEVEL_ID (*(int *)((char *)game + 0x1203D0))
#define MATCH_MODE (*(int *)((char *)game + 0x1203CC))

class AiNavigator {
public:
    enum Status { STATUS_0, STATUS_1, STATUS_2, STATUS_3 };
    enum FailureHint { HINT_0 };
    char pad0[0x18];
    int status; /* 0x18 */
    void target(_fvector &p, float r);
    void disable(Status s, FailureHint h);
};
class HealthMeter {
public:
    float getMaxLevel(void);
};
class StateStompAttack {
public:
    int transitionFeasible(void);
};
extern float LOOK_AHEAD_T;
__asm__("#SNFIX_SMALL LOOK_AHEAD_T");
extern int s_attackOtherAi __asm__("_2Ai$s_attackOtherAi");
__asm__("#SNFIX_SMALL _2Ai$s_attackOtherAi");
extern int s_ccAi __asm__("_2Ai$s_ccAi");
__asm__("#SNFIX_SMALL _2Ai$s_ccAi");
extern int minDepth __asm__("minDepth.2436");
__asm__("#SNFIX_SMALL minDepth.2436");
extern int maxDepth __asm__("maxDepth.2437");
__asm__("#SNFIX_SMALL maxDepth.2437");

/* Reflex tuples: AiActionTuple (0x48 bytes) followed by their own state. */
class AiPunchReflex : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    float m_range;     /* 0x4C: 300.0 */
    float m_reach;     /* 0x50: squared against the closest approach */
    int m_timer;       /* 0x54: fields until the next punch */
    int m_depth;       /* 0x58: combo length the AI is aiming for */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getTargetScore(Ai &ai, Monster *m);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void selectPunch(Ai &ai);
    void punch(Ai &ai, int kind);
    void exitAction(Ai &ai);
};

INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __13AiPunchReflex);
#ifdef NON_MATCHING
/* untuned: 39/54 words, size 0xd0 vs 0xd8 */
float AiPunchReflex::getEntryRelevance(Ai &ai)
{
    if (ai.monster->m_attacksEnabled && !ai.monster->isHolding() && !(ai.monster->m_state[1] & 4)) {
        float best;
        StaminaMeter *sm;
        float st;

        m_target = getBestTarget(ai, best);
        sm = &ai.monster->m_stamina;
        st = sm->cur;
        st = st / sm->getMaxLevel();
        best = best * (st * st);
        if (ai.monster->m_typeBits == 0x1E0 || ai.okayToCC(m_target, best))
            return best;
    }
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__13AiPunchReflexR2Ai);
#endif
Monster *AiPunchReflex::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    Monster **p = (Monster **)((char *)&ai + 4);

    best = 0.0f;
    for (; *p; p++) {
        float s = getTargetScore(ai, *p);

        if (best < s) {
            best = s;
            bestM = *p;
        }
    }
    return bestM;
}
#ifdef NON_MATCHING
/* untuned: 5/88 words, size 0x154 vs 0x160 */
float AiPunchReflex::getTargetScore(Ai &ai, Monster *m)
{
    float f, health;

    if (m_reach * m_reach < ai.getClosestStillApproach(*(DbInteractive *)m, LOOK_AHEAD_T))
        return 0.0f;
    if (!((EnemyInfo::Info *)EnemyInfo::getInfo(*ai.monster, *m))->los)
        return 0.0f;
    if (ai.overPit(*m))
        return 0.0f;
    if (MATCH_MODE != 2 && ai.monster->m_typeBits != 0x1E0) {
        float dy = MF(EnemyInfo::getInfo(*ai.monster, *m), 8);

        if (dy < 0.0f ? -dy > 50.0f : dy > 50.0f)
            return 0.0f;
    }
    if (!m->m_unk49)
        return 0.0f;
    f = 1.0f;
    if (!m->m_attacksEnabled)
        f = 2.0f;
    if (m->m_playerNum == 2) {
        health = m->m_health / ((HealthMeter *)((char *)m + 0x448))->getMaxLevel();
        health = health * 0.5f + 0.0f;
        f = f * health;
    }
    return f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getTargetScore__13AiPunchReflexR2AiP7Monster);
#endif
#ifdef NON_MATCHING
/* untuned: 29/75 words, size 0x128 vs 0x12c */
float AiPunchReflex::getExitRelevance(Ai &ai)
{
    Monster *m = ai.monster;

    if (STATE_ID(m) == 3 && !(MI(m, 0x10DF8) < m_depth))
        return 0.0f;
    if (m->isHolding())
        return 0.0f;
    if (!s_attackOtherAi && ai.pinningAi())
        return 0.0f;
    if (!ai.okayToCC(m_target, 1.0f))
        return 0.0f;
    if (STATE_ID(ai.monster) == 3 || m_timer >= 0)
        return getTargetScore(ai, m_target);
    {
        float score = getTargetScore(ai, m_target);

        if (score == 0.0f)
            return 0.0f;
        VCALL_V(this, 0x18, ai);
        return score;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiPunchReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 12/58 words, size 0xe4 vs 0xe8 */
void AiPunchReflex::enterAction(Ai &ai)
{
    int d;
    AiNavigator *nav = NAV(ai);

    ai.setFocus((DbInteractive *)m_target);
    s_ccAi = (int)&ai;
    {
        AiVEntry *e = AI_VENT(m_target, 0x10, 0x10);
        _fvector *pos = ((_fvector * (*)(void *))e->fn)((char *)m_target + e->delta);

        nav->target(*pos, 0.2f);
    }
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
    m_depth = 100;
    if (MATCH_MODE < 2) {
        if (ai.isMinion())
            m_depth = mathfRand(minDepth, maxDepth);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__13AiPunchReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 57/70 words, size 0x118 vs 0x118 */
void AiPunchReflex::updateAction(Ai &ai)
{
    Monster *m = ai.monster;
    int pressed;

    if (MF(EnemyInfo::getInfo(*m, *m_target), 0x14) > 0.0f)
        ai.targetPin();
    if (--m_timer > 0)
        return;
    m = ai.monster;
    if (MI(m, 0x6C38) == 0)
        pressed = (*(PadFlags *)MP(m, 0x5040))[0]->f32 != 0;
    else
        pressed = MI(m, 0x6C34) != 0;
    if (!pressed)
        return;
    if (ai.getClosestStillApproach(*(DbInteractive *)m_target, LOOK_AHEAD_T) < m_reach * m_reach) {
        if (LEVEL_ID == 7 && STATE_ID(ai.monster) == 3 && MI(ai.monster, 0x10E70) == 0)
            return;
        m_timer = ai.getButtonMashDelay();
        selectPunch(ai);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__13AiPunchReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 10/250 words, size 0x3e8 vs 0x3e0 */
void AiPunchReflex::selectPunch(Ai &ai)
{
    int mode = MATCH_MODE;
    Monster *m;
    EnemyInfo::Info *info;
    char *rec = 0;

    if (mode < 2) {
        if (ai.isMinion())
            mode = 0;
    }
    m = ai.monster;
    if (((StateStompAttack *)MP(m, 0x10CF0))->transitionFeasible()) {
        ai.heavyPunch();
        return;
    }
    info = (EnemyInfo::Info *)EnemyInfo::getInfo(*m, *m_target);
    m = ai.monster;
    if (STATE_ID(m) == 3) {
        rec = MP(m, 0x8470 + 0x20 + MI(m, 0x10DF8) * 0x420 + MI(m, 0x10DFC) * 0xB0);
    } else {
        m_lastEnter = (m_lastEnter & ~1) | mathfRand(0, 1);
    }
    if (mode > 0) {
        m = m_target;
        if (STATE_ID(m) == 3) {
            if (MF(m, 0x8470 + MI(m, 0x10DF8) * 0x420 + MI(m, 0x10DFC) * 0xB0 + 0x8C) > 100.0f) {
                ai.lightPunch();
                return;
            }
        }
        if (STATE_ID(m) == 0x29) {
            if (MF(m, 0xAE90 + 0x20 + MI(m, 0xAE90 + 0x2D48) * 0x480 + MI(m, 0xAE90 + 0x2D4C) * 0xC0 + 0x6C) > 100.0f) {
                ai.lightPunch();
                return;
            }
        }
        if (STATE_ID(m) == 0x1B) {
            punch(ai, 1);
            return;
        }
        if (mode >= 2) {
            float d = MF(info, 8);

            if (d > 125.0f) {
                punch(ai, 2);
                return;
            }
            if (d > 75.0f) {
                punch(ai, mathfRand(0, 5) ? 1 : 2);
                return;
            }
        }
    }
    if (mode == 1) {
        int r = mathfRand(0, 100);

        if (r < 40)
            punch(ai, 0);
        else if (r < 45)
            punch(ai, 1);
        else if (r < 50)
            punch(ai, 2);
        else if (r < 55)
            punch(ai, 3);
        else if (r < 60)
            punch(ai, 4);
        else
            punch(ai, 5);
        return;
    }
    if (mode == 2) {
        if (!rec) {
            punch(ai, mathfRand(0, 1) ? 0 : 5);
        } else {
            int list[6];
            int n = 0;
            int i;

            for (i = 0; i < 6; i++) {
                if (*(float *)(rec + 0x94 + i * 4) > 0.0f)
                    list[n++] = i;
            }
            if (n > 0)
                punch(ai, list[mathfRand(0, n - 1)]);
        }
        return;
    }
    if (m_lastEnter & 1)
        ai.lightPunch();
    else
        ai.heavyPunch();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", selectPunch__13AiPunchReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* matches with NON_MATCHING enabled; kept out of the retail build because the switch jump table must come from the retail rodata */
void AiPunchReflex::punch(Ai &ai, int kind)
{
    switch (kind) {
    case 0:
        ai.heavyPunch();
        break;
    case 1:
        ai.pad[0xE] = 0xFF;
        ai.heavyPunch();
        break;
    case 2:
        ai.pad[0xF] = 0xFF;
        ai.heavyPunch();
        break;
    case 3:
        ai.pad[0xE] = 0xFF;
        ai.lightPunch();
        break;
    case 4:
        ai.pad[0xF] = 0xFF;
        ai.lightPunch();
        break;
    case 5:
        ai.lightPunch();
        break;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", punch__13AiPunchReflexR2Aii);
#endif
#ifdef NON_MATCHING
/* untuned: 9/17 words */
void AiPunchReflex::exitAction(Ai &ai)
{
    AiNavigator *nav = NAV(ai);

    ai.setFocus(0);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__13AiPunchReflexR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __11AiBatReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__11AiBatReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__11AiBatReflexR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getTargetScore__11AiBatReflexR2AiP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__11AiBatReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__11AiBatReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__11AiBatReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", selectSwipe__11AiBatReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", swipe__11AiBatReflexR2Aii);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__11AiBatReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __16AiFireProjectile);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__16AiFireProjectileR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__16AiFireProjectileR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getTargetRelevance__16AiFireProjectileR2AiP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__16AiFireProjectileR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__16AiFireProjectileR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__16AiFireProjectileR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__16AiFireProjectileR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __15AiGrappleReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__15AiGrappleReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__15AiGrappleReflexR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__15AiGrappleReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__15AiGrappleReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__15AiGrappleReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__15AiGrappleReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __13AiBlockReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__13AiBlockReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiBlockReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__13AiBlockReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__13AiBlockReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__13AiBlockReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __15AiCounterReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__15AiCounterReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__15AiCounterReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__15AiCounterReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__15AiCounterReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__15AiCounterReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __14AiThrashReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__14AiThrashReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__14AiThrashReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__14AiThrashReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__14AiThrashReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__14AiThrashReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __15AiDisposeReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__15AiDisposeReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__15AiDisposeReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__15AiDisposeReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__15AiDisposeReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__15AiDisposeReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __13AiSmashReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__13AiSmashReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiSmashReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__13AiSmashReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__13AiSmashReflexR2Ai);
void exitAction__13AiSmashReflexR2Ai(void *self) __asm__("exitAction__13AiSmashReflexR2Ai");
void exitAction__13AiSmashReflexR2Ai(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __14AiThrowRecover);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__14AiThrowRecoverR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__14AiThrowRecoverR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__14AiThrowRecoverR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__14AiThrowRecoverR2Ai);
void exitAction__14AiThrowRecoverR2Ai(void *self) __asm__("exitAction__14AiThrowRecoverR2Ai");
void exitAction__14AiThrowRecoverR2Ai(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __13AiThrowPickup);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__13AiThrowPickupR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__13AiThrowPickupR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiThrowPickupR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__13AiThrowPickupR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__13AiThrowPickupR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__13AiThrowPickupR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __13AiStompReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__13AiStompReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiStompReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__13AiStompReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__13AiStompReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__13AiStompReflexR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __11AiRamAttack);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__11AiRamAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__11AiRamAttackR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__11AiRamAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__11AiRamAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__11AiRamAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__11AiRamAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __14AiCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__14AiCrowdControlR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__14AiCrowdControlR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__14AiCrowdControlR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__14AiCrowdControlR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__14AiCrowdControlR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__14AiCrowdControlR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __15AiSpecialAttack);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__15AiSpecialAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__15AiSpecialAttackR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__15AiSpecialAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__15AiSpecialAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__15AiSpecialAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__15AiSpecialAttackR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __14AiDetonateHead);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__14AiDetonateHeadR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__14AiDetonateHeadR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__14AiDetonateHeadR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__14AiDetonateHeadR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__14AiDetonateHeadR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__14AiDetonateHeadR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __7AiTaunt);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__7AiTauntR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__7AiTauntR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__7AiTauntR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__7AiTauntR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", exitAction__7AiTauntR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __12AiPutOutFire);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__12AiPutOutFireR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__12AiPutOutFireR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__12AiPutOutFireR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__12AiPutOutFireR2Ai);
void exitAction__12AiPutOutFireR2Ai(void *self) __asm__("exitAction__12AiPutOutFireR2Ai");
void exitAction__12AiPutOutFireR2Ai(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$12AiPutOutFire);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$7AiTaunt);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$14AiDetonateHead);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$15AiSpecialAttack);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$14AiCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$11AiRamAttack);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$13AiStompReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$13AiThrowPickup);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$14AiThrowRecover);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$13AiSmashReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$15AiDisposeReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$14AiThrashReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$15AiCounterReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$13AiBlockReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$15AiGrappleReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$16AiFireProjectile);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$11AiBatReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", _vt$13AiPunchReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf13AiPunchReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf11AiBatReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf16AiFireProjectile);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf15AiGrappleReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf13AiBlockReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf15AiCounterReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf14AiThrashReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf15AiDisposeReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf13AiSmashReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf14AiThrowRecover);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf13AiThrowPickup);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf13AiStompReflex);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf11AiRamAttack);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf14AiCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf15AiSpecialAttack);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf14AiDetonateHead);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf7AiTaunt);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __tf12AiPutOutFire);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getMaxLevel__11HealthMeter);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", func_0010DB98);
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", inFov__9EnemyInfof);
int inLos__9EnemyInfo(void *self) __asm__("inLos__9EnemyInfo");
int inLos__9EnemyInfo(void *self)
{
    return *(int *)((char *)self + 0x20);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", func_0010DBC8);

#include "common.h"

#include "engine.h"
#include "game/game.h"
#include "game/ai.h"
#include "game/ai_action.h"
#include "game/enemy_info.h"
#include "game/pad_flags.h"

#include "game/level_pickups.h"
/* Blocking an incoming attack: picks the opponent (or thrown pickup) whose attack is about to land. */
class AiBlockReflex : public AiActionTuple {
public:
    void *m_target;    /* 0x48: Monster (type tag 1) or a thrown pickup's interactive */
    float m_reach;     /* 0x4C: 500.0 */
    int m_timer;       /* 0x50 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};

/* Swinging a held weapon (bat, pole...). Same shape as AiPunchReflex but the reach comes from the weapon. */
class AiBatReflex : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    float m_range;     /* 0x4C: 100.0 */
    float m_reachBase; /* 0x50: reach without the weapon */
    float m_reach;     /* 0x54: reachBase + weapon length, squared against the closest approach */
    int m_timer;       /* 0x58 */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getTargetScore(Ai &ai, Monster *m);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void selectSwipe(Ai &ai);
    void swipe(Ai &ai, int kind);
    void exitAction(Ai &ai);
};

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
void AiPunchReflex::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __11AiBatReflex);
#ifdef NON_MATCHING
/* untuned: 13/68 words, size 0xfc vs 0x110 */
float AiBatReflex::getEntryRelevance(Ai &ai)
{
    char *held;
    int kind;
    float best;
    StaminaMeter *sm;
    float st;

    if (MI(ai.monster, 0x68A4) == 0 || !ai.monster->m_attacksEnabled || (ai.monster->m_state[1] & 4))
        return 0.0f;
    held = (char *)MI(MI(ai.monster, 0x68A4), 0);
    kind = MI(held, 0xA0);
    if (kind == 0x10)
        return 0.0f;
    m_reach = m_reachBase + (MF(held, 0xB4) - MF(held, 0xC4));
    m_target = getBestTarget(ai, best);
    sm = &ai.monster->m_stamina;
    st = sm->cur;
    st = st / sm->getMaxLevel();
    best = best * (st * st);
    if (kind == 3)
        best = best * 0.01f;
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__11AiBatReflexR2Ai);
#endif
Monster *AiBatReflex::getBestTarget(Ai &ai, float &best)
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
/* untuned: 9/74 words, size 0x128 vs 0x120 */
float AiBatReflex::getTargetScore(Ai &ai, Monster *m)
{
    float f, health;
    float dy = MF(EnemyInfo::getInfo(*ai.monster, *m), 8);

    if (dy < 0.0f ? -dy > 50.0f : dy > 50.0f)
        return 0.0f;
    if (m_reach * m_reach < ai.getClosestStillApproach(*(DbInteractive *)m, LOOK_AHEAD_T))
        return 0.0f;
    if (MATCH_MODE < 2 && !m->m_unk49)
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
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getTargetScore__11AiBatReflexR2AiP7Monster);
#endif
#ifdef NON_MATCHING
/* untuned: 11/68 words, size 0x108 vs 0x110 */
float AiBatReflex::getExitRelevance(Ai &ai)
{
    if (!MI(ai.monster, 0x68A4))
        return 0.0f;
    if (!s_attackOtherAi && ai.pinningAi())
        return 0.0f;
    if (!EnemyInfo::getInfo(*ai.monster, *m_target)->los)
        return 0.0f;
    if (ai.overPit(*m_target))
        return 0.0f;
    if (STATE_ID(ai.monster) == 0x29 || m_timer >= 0)
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
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__11AiBatReflexR2Ai);
#endif
void AiBatReflex::enterAction(Ai &ai)
{
    int d;
    AiNavigator *nav = NAV(ai);

    ai.setFocus((DbInteractive *)m_target);
    {
        AiVEntry *e = AI_VENT(m_target, 0x10, 0x10);
        _fvector *pos = ((_fvector * (*)(void *))e->fn)((char *)m_target + e->delta);

        nav->target(*pos, 0.2f);
    }
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
#ifdef NON_MATCHING
/* untuned: 60/73 words, size 0x124 vs 0x124 */
void AiBatReflex::updateAction(Ai &ai)
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
        if (LEVEL_ID == 7 && STATE_ID(ai.monster) == 0x29 && MI(ai.monster, 0xAE90 + 0x2DC0) == 0)
            return;
        selectSwipe(ai);
        m_timer = ai.getButtonMashDelay();
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__11AiBatReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 4/226 words, size 0x388 vs 0x37c */
void AiBatReflex::selectSwipe(Ai &ai)
{
    Monster *m;
    EnemyInfo::Info *info;
    char *rec = 0;
    int mode;

    m = ai.monster;
    if (((StateStompAttack *)MP(m, 0x10CF0))->transitionFeasible()) {
        ai.heavyPunch();
        return;
    }
    info = EnemyInfo::getInfo(*m, *m_target);
    m = ai.monster;
    if (STATE_ID(m) == 0x29)
        rec = MP(m, 0xAE90 + 0x20 + MI(m, 0xAE90 + 0x2D48) * 0x480 + MI(m, 0xAE90 + 0x2D4C) * 0xC0);
    mode = MATCH_MODE;
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
            swipe(ai, 1);
            return;
        }
        if (MATCH_MODE >= 2) {
            if (MF(info, 8) > 100.0f) {
                swipe(ai, 2);
                return;
            }
        }
    }
    mode = MATCH_MODE;
    if (mode == 1) {
        int r = mathfRand(0, 100);

        if (r < 40)
            swipe(ai, 0);
        else if (r < 45)
            swipe(ai, 1);
        else if (r < 50)
            swipe(ai, 2);
        else if (r < 55)
            swipe(ai, 3);
        else if (r < 60)
            swipe(ai, 4);
        else
            swipe(ai, 5);
        return;
    }
    if (mode == 2) {
        if (!rec) {
            swipe(ai, mathfRand(0, 1) ? 0 : 5);
        } else {
            int list[6];
            int n = 0;
            int i;

            for (i = 0; i < 6; i++) {
                if (*(float *)(rec + 0xA8 + i * 4) > 0.0f)
                    list[n++] = i;
            }
            if (n > 0)
                swipe(ai, list[mathfRand(0, n - 1)]);
        }
        return;
    }
    if (m_lastEnter & 1)
        ai.lightPunch();
    else
        ai.heavyPunch();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", selectSwipe__11AiBatReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* matches with NON_MATCHING enabled; kept out of the retail build because the switch jump table must come from the retail rodata */
void AiBatReflex::swipe(Ai &ai, int kind)
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
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", swipe__11AiBatReflexR2Aii);
#endif
void AiBatReflex::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
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
#ifdef NON_MATCHING
/* untuned: 6/360 words, size 0x5a0 vs 0x598 */
float AiBlockReflex::getEntryRelevance(Ai &ai)
{
    float best;
    Monster **p;
    int found;
    void *pickup;

    if (!ai.monster->m_unk49)
        return 0.0f;
    if ((ai.monster->m_state[1] & 4) && (ai.monster->m_state[0] == 0x27 || ai.monster->m_state[0] == 0x28))
        return 0.0f;
    m_target = 0;
    best = 0.0f;
    for (p = (Monster **)((char *)&ai + 4); *p; p++) {
        Monster *o = *p;
        EnemyInfo::Info *info = EnemyInfo::getInfo(*o, *ai.monster);
        float rel;
        float reach;
        int id;

        if (!(o->m_state[1] & 4))
            continue;
        if (o->m_state == (int *)MI(o, 0x7980))
            continue;
        if (!o->m_attacksEnabled)
            continue;
        rel = 1.0f;
        id = o->m_state[0];
        if ((unsigned)(id - 3) < 0x3D) {
            switch (id - 3) {
            case 0: {
                char *base = (char *)o + 0x8470;

                if (MI(base, 0x299C)) {
                    char *rec = base + MI(base, 0x2988) * 0x420 + 0x20 + MI(base, 0x298C) * 0xB0;

                    if ((float)(ai.getReflexDelay() * 2) < MF(rec, 0x24))
                        continue;
                }
                if (MATCH_MODE > 0) {
                    if (MF(base, MI(base, 0x298C) * 0xB0 + MI(base, 0x2988) * 0x420 + 0x8C) > 100.0f)
                        continue;
                }
                if (info->dot < -0.7f)
                    continue;
                if (m_reach * m_reach < ai.getClosestStillApproach(*(DbInteractive *)o, 0.5f))
                    continue;
                rel *= ai.getFovRelevance(info->dot, -0.7f);
                break;
            }
            case 38: {
                char *base = (char *)o + 0xAE90;

                if (MI(base, 0x2D5C)) {
                    char *rec = base + MI(base, 0x2D48) * 0x480 + 0x20 + MI(base, 0x2D4C) * 0xC0;

                    if ((float)(ai.getReflexDelay() * 2) < MF(rec, 0x24))
                        continue;
                }
                if (MATCH_MODE > 0) {
                    if (MF(base + MI(base, 0x2D48) * 0x480 + 0x20 + MI(base, 0x2D4C) * 0xC0, 0x6C) > 100.0f)
                        continue;
                }
                if (info->dot < -0.7f)
                    continue;
                if (MI(o, 0x68A4) == 0) {
                    reach = m_reach;
                } else {
                    char *w = (char *)MI(MI(o, 0x68A4), 0);

                    reach = m_reach + (MF(w, 0xB4) - MF(w, 0xC4)) + 25.0f;
                }
                if (reach * reach < ai.getClosestStillApproach(*(DbInteractive *)o, 0.5f))
                    continue;
                rel *= ai.getFovRelevance(info->dot, -1.0f);
                break;
            }
            case 6:
            case 51:
            case 58:
                continue;
            case 7:
            case 23:
            case 37:
            case 47:
            case 60:
                if (info->dot < 0.0f)
                    continue;
                /* fall through */
            default:
                if (m_reach * m_reach < ai.getClosestStillApproach(*(DbInteractive *)o, 0.5f))
                    continue;
                break;
            }
        } else {
            if (m_reach * m_reach < ai.getClosestStillApproach(*(DbInteractive *)o, 0.5f))
                continue;
        }
        if (best < rel) {
            m_target = o;
            best = rel;
        }
    }
    if (best != 0.0f)
        return best;
    found = 0;
    pickup = LevelPickups::getClosestThrownPickup(*(_fvector *)MP(MI(ai.monster, 0xC), 0x10), m_reach * 3.0f);
    if (pickup) {
        int kind = MI(*(void **)pickup, 0xA0);

        if (kind != 7 && kind != 0xE) {
            if (ai.getClosestStillApproach(*(DbInteractive *)*(void **)pickup, LOOK_AHEAD_T) < 75.0f * 75.0f)
                found = 1;
        }
    }
    if (!found)
        return best;
    best = 1.0f;
    if (MATCH_MODE < 2) {
        AiVEntry *e1 = AI_VENT(*(void **)pickup, 0x10, 0x10);
        float *a = ((float *(*)(void *))e1->fn)((char *)*(void **)pickup + e1->delta);
        AiVEntry *e2 = AI_VENT(ai.monster, 0x10, 0x10);
        float *b = ((float *(*)(void *))e2->fn)((char *)ai.monster + e2->delta);
        float dx = a[0] - b[0];
        float dy = a[1] - b[1];
        float dz = a[2] - b[2];
        float inv = 1.0f / sqrtf(dx * dx + dy * dy + dz * dz);
        float *fwd = (float *)MP(MI(ai.monster, 0xC), 0x30);

        dx *= inv;
        dy *= inv;
        dz *= inv;
        best = ai.getFovRelevance(dx * fwd[0] + dy * fwd[1] + dz * fwd[2], -1.0f);
        m_target = *(void **)pickup;
    } else
        m_target = *(void **)pickup;
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__13AiBlockReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 20/44 words, size 0xb0 vs 0xa0 */
float AiBlockReflex::getExitRelevance(Ai &ai)
{
    void *old = m_target;
    float r = VCALL_F(this, 8, ai);

    m_target = old;
    if (r <= 0.0f) {
        m_timer--;
        if (m_timer < -ai.getReflexDelay())
            return 0.0f;
    }
    {
        float f = m_entryRel;

        return f > 1.0f ? f : 1.0f;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiBlockReflexR2Ai);
#endif
void AiBlockReflex::enterAction(Ai &ai)
{
    int d;

    if (*(int *)m_target == 1) {
        AiNavigator *nav = NAV(ai);

        ai.setFocus((DbInteractive *)m_target);
        {
            AiVEntry *e = AI_VENT(m_target, 0x10, 0x10);
            _fvector *pos = ((_fvector * (*)(void *))e->fn)((char *)m_target + e->delta);

            nav->target(*pos, 0.1f);
        }
    }
    if (STATE_ID(ai.monster) == 0x37) {
        m_timer = 0;
    } else {
        d = ai.getReflexDelay() - getFieldsSinceEval();
        m_timer = d > -1 ? d : 0;
    }
}
#ifdef NON_MATCHING
/* untuned: 26/37 words, size 0x90 vs 0x94 */
void AiBlockReflex::updateAction(Ai &ai)
{
    if (*(int *)m_target == 1) {
        if (MF(EnemyInfo::getInfo(*ai.monster, *(Monster *)m_target), 0x14) > 0.7f)
            ai.targetPin();
    }
    if (m_timer > 0)
        m_timer--;
    else
        ai.block();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__13AiBlockReflexR2Ai);
#endif
void AiBlockReflex::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
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

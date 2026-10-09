#include "common.h"
#include "vecmath.h"

#include "engine.h"
#include "game/game.h"
#include "game/ai.h"
#include "game/ai_action.h"
#include "game/ai_support.h"
#include "game/enemy_info.h"
#include "game/pad_flags.h"

#include "game/level_pickups.h"

float mathfClosestApproach(_fvector &a, _fvector &b, _fvector &c, _fvector &d);
class StateGrapple {
public:
    int transitionFeasible(void);
};
extern float LOW_GRAPPLE_STAMINA;
__asm__("#SNFIX_SMALL LOW_GRAPPLE_STAMINA");
/* The monster at +0x10594 (the "head" this monster was ordered to detonate) */
#define HEAD(ai) ((char *)MI((ai).monster, 0x10594))

/* Detonating a head (special-level AI): fire at the opponent the head would hit. */
class AiDetonateHead : public AiActionTuple {
public:
    int m_timer;   /* 0x48 */
    float m_range; /* 0x4C: 250.0 */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Grabbing an opponent. */
class AiGrappleReflex : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    float m_range;     /* 0x4C: 120.0 */
    int m_timer;       /* 0x50 */
    int m_counter;     /* 0x54: frames the grab button was seen held */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Shooting a projectile (fire breath, missiles ...) at the best opponent. */
class AiFireProjectile : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    float m_minRange;  /* 0x4C: 300.0 */
    float m_maxRange;  /* 0x50: 1000.0 */
    int m_timer;       /* 0x54 */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getTargetRelevance(Ai &ai, Monster *m);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};

/* EnemyInfo helpers living in other TUs (this = an EnemyInfo::Info record). */
int infoInFov(EnemyInfo::Info *info, float cosAngle) __asm__("inFov__9EnemyInfof");
int infoInLos(EnemyInfo::Info *info) __asm__("inLos__9EnemyInfo");

/* Charging at an opponent in a straight line. */
class AiRamAttack : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    int m_timer;       /* 0x4C */
    float m_minRange;  /* 0x50: 300.0 */
    float m_maxRange;  /* 0x54: 600.0 */
    float m_lateral;   /* 0x58: 50.0, how far off the charge line the target may be */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Crowd control (the area attack that pushes opponents away). */
class AiCrowdControl : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    int m_timer;       /* 0x4C */
    float m_range;     /* 0x50: 600.0 */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* The monster's special attack. */
class AiSpecialAttack : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    int m_timer;       /* 0x4C */
    int m_start;       /* 0x50: field the attack began at, -1 before */
    float m_minRange;  /* 0x54: 250.0 */
    float m_maxRange;  /* 0x58: 1000.0 */
    int m_duration;    /* 0x5C: fields the button is held (150) */
    int m_needLos;     /* 0x60 */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};

/* Taunting a far-away opponent that can see us. */
class AiTaunt : public AiActionTuple {
public:
    int m_timer;        /* 0x48 */
    Monster *m_target;  /* 0x4C */
    float m_range;      /* 0x50: 400.0 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Running to put out a fire on the monster. */
class AiPutOutFire : public AiActionTuple {
public:
    int m_timer;       /* 0x48 */
    float m_prevBurn;  /* 0x4C: burn counter of the previous evaluation */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
};

/* Butt-stomping a nearby monster. */
class AiStompReflex : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    int m_timer;       /* 0x4C */
    float m_range;     /* 0x50: 200.0 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Smashing a pickup/vehicle the monster is standing next to. */
class AiSmashReflex : public AiActionTuple {
public:
    int m_timer; /* 0x48 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
};
/* Getting back up after being thrown. */
class AiThrowRecover : public AiActionTuple {
public:
    int m_timer; /* 0x48 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
};
/* Throwing the held object at the best opponent. */
class AiThrowPickup : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    float m_range;     /* 0x4C: 1000.0 */
    int m_timer;       /* 0x50 */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};

extern float GET_UP_ATTACK_RANGE;
__asm__("#SNFIX_SMALL GET_UP_ATTACK_RANGE");

/* Mashing buttons to break free when held, burning or knocked down. */
class AiThrashReflex : public AiActionTuple {
public:
    int m_timer; /* 0x48 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Throwing away whatever the monster is holding (used over pits). */
class AiDisposeReflex : public AiActionTuple {
public:
    int m_timer;      /* 0x48 */
    char pad4C[4];
    float m_aim[4];   /* 0x50: point to walk to before the throw */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};

class StateCounter {
public:
    int transitionFeasible(void);
};
class StatePunch {
public:
    char *getCurrentConfig(void);
};
/* Counter-attacking: the AI watches an attacker's windup and times its own counter inside the attacker's hit window. */
class AiCounterReflex : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    float m_reach;     /* 0x4C: 500.0 */
    int m_timer;       /* 0x50 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
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

class StateStompAttack {
public:
    int transitionFeasible(void);
};
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
#ifdef NON_MATCHING
/* untuned: 1/94 words, size 0x178 vs 0x174 */
float AiFireProjectile::getEntryRelevance(Ai &ai)
{
    float best;
    float *a, *b;
    float dx, dy, dz;
    int kind;
    StaminaMeter *sm;
    float st;

    if (!ai.monster->m_attacksEnabled)
        return 0.0f;
    a = (float *)MP(ai.monster, 0x3E30);
    b = (float *)MP(MI(ai.monster, 0x6BF8), 0x10);
    dx = a[0] - b[0];
    dy = a[1] - b[1];
    dz = a[2] - b[2];
    if (dx * dx + dy * dy + dz * dz < 300.0f * 300.0f)
        return 0.0f;
    kind = 0;
    if (MI(ai.monster, 0x68A4)) {
        kind = MI(MI(MI(ai.monster, 0x68A4), 0), 0xA0);
        if (kind == 6 || kind == 0xD || kind == 0x10)
            return 0.0f;
    }
    m_target = getBestTarget(ai, best);
    sm = &ai.monster->m_stamina;
    st = sm->cur;
    st = st / sm->getMaxLevel();
    best = best * ((st * st) * (st * st));
    if (MATCH_MODE > 0 && m_target && kind != 0) {
        if (*(short *)MP(MI(MI(ai.monster, 0x68A4), 0), 0xDE) > 0)
            return 100.0f;
    }
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__16AiFireProjectileR2Ai);
#endif
Monster *AiFireProjectile::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    Monster **p = (Monster **)((char *)&ai + 4);

    best = 0.0f;
    for (; *p; p++) {
        float s = getTargetRelevance(ai, *p);

        if (best < s) {
            best = s;
            bestM = *p;
        }
    }
    return bestM;
}
#ifdef NON_MATCHING
/* untuned: 59/68 words, size 0x110 vs 0x110 */
float AiFireProjectile::getTargetRelevance(Ai &ai, Monster *m)
{
    EnemyInfo::Info *info = EnemyInfo::getInfo(*ai.monster, *m);
    float r, t;

    if (!info->los || MB(m, 0xEF))
        return 0.0f;
    if (m_maxRange < info->dist)
        return 0.0f;
    if (info->dist < m_minRange)
        return 0.0f;
    if (!m->m_unk49)
        return 0.0f;
    if (STATE_ID(m) == 0x1B)
        return 0.0f;
    if (m->m_state[1] & 0x10)
        return 0.0f;
    r = ai.getFovRelevance(info->dot, 0.0f);
    t = (info->dist - m_minRange) * 0.5f;
    t = t / (m_maxRange - m_minRange);
    t = t + 0.5f;
    r = r * t;
    if (!m->m_attacksEnabled)
        r = r + r;
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getTargetRelevance__16AiFireProjectileR2AiP7Monster);
#endif
#ifdef NON_MATCHING
/* untuned: 10/49 words, size 0xc4 vs 0xb4 */
float AiFireProjectile::getExitRelevance(Ai &ai)
{
    int special;

    if (ai.overPit(*ai.monster)) {
        float v = m_ivalue + 1.0f;

        m_ivalue = v < m_ivalueMax ? v : m_ivalueMax;
        return 1.0f;
    }
    special = 0;
    if (MATCH_MODE > 0 && MI(ai.monster, 0x68A4)) {
        if (*(short *)MP(MI(MI(ai.monster, 0x68A4), 0), 0xDE) > 0)
            special = 1;
    }
    if (special)
        return 100.0f;
    return getTargetRelevance(ai, m_target);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__16AiFireProjectileR2Ai);
#endif
void AiFireProjectile::enterAction(Ai &ai)
{
    int d;

    ai.setFocus((DbInteractive *)m_target);
    NAV(ai)->seek(*(_fvector *)MP(m_target, 0x3E30), 0.0f);
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
#ifdef NON_MATCHING
/* untuned: 6/117 words, size 0x1d4 vs 0x1cc */
void AiFireProjectile::updateAction(Ai &ai)
{
    EnemyInfo::Info *info = EnemyInfo::getInfo(*ai.monster, *m_target);
    int special;

    if (ai.overPit(*ai.monster))
        return;
    if (info->dot > 0.7f)
        ai.targetPin();
    if (ai.isMinion()) {
        AiNavigator *nav = NAV(ai);

        MF(nav, 4) = (info->dist - m_minRange) / (m_maxRange - m_minRange);
        if (EnemyInfo::getInfo(*ai.monster, *m_target)->dist < m_minRange + 50.0f && !MB(ai.monster, 0x280)) {
            if (nav->status)
                nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
        } else if (m_minRange + 75.0f < EnemyInfo::getInfo(*ai.monster, *m_target)->dist) {
            nav->seek(*(_fvector *)MP(m_target, 0x3E30), 0.0f);
        }
    }
    if (--m_timer > 0)
        return;
    if (m_timer != 0) {
        m_timer = ai.getButtonMashDelay();
        return;
    }
    special = 0;
    if (MATCH_MODE > 0 && MI(ai.monster, 0x68A4)) {
        if (*(short *)MP(MI(MI(ai.monster, 0x68A4), 0), 0xDE) > 0)
            special = 1;
    }
    if (special)
        ai.lightPunch();
    else
        ai.fireProjectile();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__16AiFireProjectileR2Ai);
#endif
void AiFireProjectile::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
    MF(nav, 4) = 1.0f;
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __15AiGrappleReflex);
#ifdef NON_MATCHING
/* untuned: 31/50 words, size 0xc4 vs 0xc8 */
float AiGrappleReflex::getEntryRelevance(Ai &ai)
{
    float best;

    if (!ai.monster->m_attacksEnabled)
        return 0.0f;
    if (Cameras::GetUnifiedTime() < 180.0f)
        return 0.0f;
    if (ai.monster->isHolding())
        return 0.0f;
    if (STATE_ID(ai.monster) == 6)
        return 0.0f;
    if (!((StateGrapple *)MP(ai.monster, 0xDCE0))->transitionFeasible())
        return 0.0f;
    if (MF(ai.monster, 0x470) < LOW_GRAPPLE_STAMINA)
        return 0.0f;
    m_target = getBestTarget(ai, best);
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__15AiGrappleReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 32/89 words, size 0x15c vs 0x164 */
Monster *AiGrappleReflex::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    Monster **p = (Monster **)((char *)&ai + 4);

    best = 0.0f;
    for (; *p; p++) {
        Monster *o = *p;
        float w;

        if (!o->m_unk49)
            continue;
        if (o->m_state[1] & 0x10)
            continue;
        if (!EnemyInfo::getInfo(*o, *ai.monster)->los)
            continue;
        w = 1.0f;
        if (o->m_state[1] & 4)
            w = 0.5f;
        else if (STATE_ID(o) == 0x1B)
            w = 2.0f;
        if (o->m_playerNum == 2) {
            float h = o->m_health / ((HealthMeter *)((char *)o + 0x448))->getMaxLevel();

            h = h * 0.5f;
            w = w * h;
        }
        if (!o->m_attacksEnabled)
            w = w + w;
        if (best < w) {
            best = w;
            bestM = o;
        }
    }
    return bestM;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__15AiGrappleReflexR2AiRf);
#endif
#ifdef NON_MATCHING
/* untuned: 42/46 words, size 0xb8 vs 0xb8 */
float AiGrappleReflex::getExitRelevance(Ai &ai)
{
    if (MI(&ai, 0x9C) == 2)
        return 0.0f;
    if (!EnemyInfo::getInfo(*ai.monster, *m_target)->los)
        return 0.0f;
    if (m_counter > 0 || m_timer < -5) {
        if (STATE_ID(ai.monster) != 6)
            return 0.0f;
    }
    if (!s_attackOtherAi && ai.pinningAi())
        return 0.0f;
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__15AiGrappleReflexR2Ai);
#endif
void AiGrappleReflex::enterAction(Ai &ai)
{
    float r;
    int d;

    r = MF(ai.monster, 0x404) + MF(ai.monster, 0x3F4);
    r = r + MF(m_target, 0x404);
    r = r + MF(m_target, 0x3F4);
    ai.setFocus((DbInteractive *)m_target);
    NAV(ai)->seek(*(DbInteractive *)m_target, r + 20.0f);
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_counter = 0;
    m_timer = d > -1 ? d : 0;
}
#ifdef NON_MATCHING
/* untuned: 43/51 words, size 0xcc vs 0xcc */
void AiGrappleReflex::updateAction(Ai &ai)
{
    if (MF(EnemyInfo::getInfo(*ai.monster, *m_target), 0x14) > 0.7f)
        ai.targetPin();
    if (MI(&ai, 0x9C) != 1)
        return;
    if (--m_timer > 0)
        return;
    if (((StateGrapple *)MP(ai.monster, 0xDCE0))->transitionFeasible()) {
        if (*(unsigned short *)((char *)(*(PadFlags *)MP(ai.monster, 0x5040))[1] + 0x2E) != 0)
            m_counter++;
        else
            ai.grab();
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__15AiGrappleReflexR2Ai);
#endif
void AiGrappleReflex::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
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
        float inv = 1.0f / eeSqrtf(dx * dx + dy * dy + dz * dz);
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
#ifdef NON_MATCHING
/* untuned: 18/238 words, size 0x3b8 vs 0x3b8 */
float AiCounterReflex::getEntryRelevance(Ai &ai)
{
    float best;
    Monster **p;

    if (MATCH_MODE == 0)
        return 0.0f;
    if (ai.monster->m_state[1] & 4)
        return 0.0f;
    if (!((StateCounter *)MP(ai.monster, 0x7DD8))->transitionFeasible())
        return 0.0f;
    m_target = 0;
    best = 0.0f;
    for (p = (Monster **)((char *)&ai + 4); *p; p++) {
        Monster *o = *p;
        EnemyInfo::Info *info;
        char *sc;
        int mid, span, lo, hi, ref, ok;
        float r;

        if (!(o->m_state[1] & 4))
            continue;
        if (!o->m_attacksEnabled)
            continue;
        info = EnemyInfo::getInfo(*ai.monster, *o);
        if (m_reach < info->dist)
            continue;
        if (!(info->dot > 0.6f))
            continue;
        span = 0;
        sc = MP(ai.monster, 0x7DD8);
        mid = ((ai.m_reflexMin + ai.m_reflexMax) >> 1) + MI(sc, 0x3C);
        hi = MI(sc, 0x40) - MI(sc, 0x3C);
        if (o->m_state[0] == 3) {
            char *base = MP(o, 0x8470);
            int w0, w1, w2;

            if (MATCH_MODE < 2 && MI(base, 0x2988) < 2) {
                if (!(MF(base, MI(base, 0x298C) * 0xB0 + MI(base, 0x2988) * 0x420 + 0x8C) > 100.0f)) {
                    char *cfg = ((StatePunch *)base)->getCurrentConfig();

                    if (MF(cfg, 0x68) != 0.0f) {
                        ok = 1;
                    } else {
                        cfg = ((StatePunch *)base)->getCurrentConfig();
                        ok = MF(cfg, 0x64) != 0.0f;
                    }
                    if (!ok)
                        continue;
                }
            }
            w0 = MI(base, 0x29F0);
            w1 = MI(base, 0x29F4);
            w2 = MI(base, 0x29EC);
            if (mid < w0 + (w1 - w0) / 2 - w2)
                continue;
            span = w1 - w2;
        } else if (o->m_state[0] == 0x29) {
            char *base = MP(o, 0xAE90);
            int w0, w1, w2;

            if (MATCH_MODE < 2 && MI(base, 0x2D48) < 2) {
                char *rec = base + MI(base, 0x2D48) * 0x480 + 0x20 + MI(base, 0x2D4C) * 0xC0;

                if (!(MF(rec, 0x6C) > 100.0f)) {
                    ok = MF(rec, 0x68) != 0.0f || MF(rec, 0x64) != 0.0f;
                    if (!ok)
                        continue;
                }
            }
            w0 = MI(base, 0x2DB0);
            w1 = MI(base, 0x2DB4);
            w2 = MI(base, 0x2DAC);
            if (mid < w0 + (w1 - w0) / 2 - w2)
                continue;
            span = w1 - w2;
        }
        if (!(mid - hi < span))
            continue;
        r = ai.getFovRelevance(info->dot, 0.0f);
        r = r * 0.5f + 0.5f;
        if (span < 4)
            r = r * 0.5f;
        if (best < r) {
            m_target = o;
            best = r;
        }
    }
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__15AiCounterReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 14/23 words, size 0x5c vs 0x50 */
float AiCounterReflex::getExitRelevance(Ai &ai)
{
    if (m_timer < -5) {
        if (STATE_ID(ai.monster) != 0x37)
            return 0.0f;
    }
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__15AiCounterReflexR2Ai);
#endif
void AiCounterReflex::enterAction(Ai &ai)
{
    int d;

    ai.setFocus((DbInteractive *)m_target);
    NAV(ai)->target(*(_fvector *)MP(MI(m_target, 0xC), 0x10), 0.1f);
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
void AiCounterReflex::updateAction(Ai &ai)
{
    if (MF(EnemyInfo::getInfo(*ai.monster, *m_target), 0x14) > 0.3f)
        ai.targetPin();
    if (ai.monster->isBlocking())
        ai.block();
    if (--m_timer >= 0) {
        if (m_timer < 2)
            ai.counter();
    }
}
void AiCounterReflex::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __14AiThrashReflex);
float AiThrashReflex::getEntryRelevance(Ai &ai)
{
    Monster *m = ai.monster;
    int id = m->m_state[0];

    if (MI(m, 0x68B0) != 0 || MI(m, 0x130) == 2 || id == 0x2D || id == 0x2E)
        return 1.0f;
    return 0.0f;
}
float AiThrashReflex::getExitRelevance(Ai &ai)
{
    return VCALL_F(this, 8, ai);
}
void AiThrashReflex::enterAction(Ai &ai)
{
    int d;

    ai.setFocus((DbInteractive *)MI(ai.monster, 0x68B0));
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
#ifdef NON_MATCHING
/* untuned: 21/81 words, size 0x144 vs 0x118 */
void AiThrashReflex::updateAction(Ai &ai)
{
    Monster *m;

    if (--m_timer > 0)
        return;
    m = ai.monster;
    if (m->m_state[1] & 0x10) {
        AiVEntry *e = AI_VENT(m, 0x10, 0x20);
        float *v = ((float *(*)(void *))e->fn)((char *)m + e->delta);

        if ((v[2] < 0.0f ? -v[2] : v[2]) < 10.0f) {
            int mode = MATCH_MODE;
            int punch;

            if (mode == 1) {
                punch = (m_lastEnter & 3) == 0;
            } else if (mode >= 2 || mode != 0) {
                punch = ai.monster->getClosestMonster(GET_UP_ATTACK_RANGE) != 0;
            } else {
                punch = 0;
            }
            if (punch)
                ai.heavyPunch();
            else
                ai.thrash();
        } else
            ai.thrash();
    } else
        ai.thrash();
    m_timer = ai.getButtonMashDelay();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__14AiThrashReflexR2Ai);
#endif
void AiThrashReflex::exitAction(Ai &ai)
{
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __15AiDisposeReflex);
float AiDisposeReflex::getEntryRelevance(Ai &ai)
{
    AiNavigator *nav;

    if (MI(ai.monster, 0x130) != 0 || !ai.monster->isHolding())
        return 0.0f;
    nav = NAV(ai);
    if (MI(nav, 0x1C) == 2 && MI(nav, 0x20) == 3)
        return 1.0f;
    if (LEVEL_ID != 7)
        return 0.0f;
    if (!MB(ai.monster, 0x280))
        return 0.0f;
    if (!(MF(MI(ai.monster, 0xC), 0x18) < 100.0f))
        return 0.0f;
    if (ai.overPit(*ai.monster))
        return 1.0f;
    return 0.0f;
}
#ifdef NON_MATCHING
/* untuned: 11/22 words, size 0x58 vs 0x4c */
float AiDisposeReflex::getExitRelevance(Ai &ai)
{
    if (!ai.monster->isHolding())
        return 0.0f;
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__15AiDisposeReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 14/70 words, size 0x118 vs 0xf4 */
void AiDisposeReflex::enterAction(Ai &ai)
{
    Monster *m = ai.monster;
    int d;
    int isKind3 = 0;
    float scale;
    float *fwd;
    float *pos;

    if (MI(m, 0x68A4))
        isKind3 = MI(MI(MI(m, 0x68A4), 0), 0xA0) == 3;
    scale = isKind3 ? -50.0f : 50.0f;
    fwd = (float *)MP(MI(m, 0xC), 0x30);
    m_aim[0] = fwd[0] * scale;
    m_aim[1] = fwd[1] * scale;
    m_aim[2] = fwd[2] * scale;
    m_aim[3] = fwd[3];
    pos = (float *)MP(MI(ai.monster, 0xC), 0x10);
    m_aim[0] += pos[0];
    m_aim[1] += pos[1];
    m_aim[2] += pos[2];
    NAV(ai)->target(*(_fvector *)m_aim, 0.7f);
    ((GamePadClipPlayer *)((char *)&ai + 0x48))->clip = AiPadClips::getThrow();
    ((GamePadClipPlayer *)((char *)&ai + 0x48))->rewind();
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__15AiDisposeReflexR2Ai);
#endif
void AiDisposeReflex::updateAction(Ai &ai)
{
    GamePadClipPlayer *cp;

    if (--m_timer > 0)
        return;
    if (MI(&ai, 0x9C) != 1)
        return;
    cp = (GamePadClipPlayer *)((char *)&ai + 0x48);
    if (!cp->update(*(GamePad *)ai.pad))
        cp->rewind();
}
void AiDisposeReflex::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __13AiSmashReflex);
#ifdef NON_MATCHING
/* untuned: 18/63 words, size 0xfc vs 0xf4 */
float AiSmashReflex::getEntryRelevance(Ai &ai)
{
    float dx, dy, dz;
    float *a;
    float *b;

    if (MI(ai.monster, 0x130) != 0 || ai.monster->isHolding())
        return 0.0f;
    if (!(*(unsigned long long *)MP(ai.monster, 0x6C50) & (0xFFE0ULL << 27)))
        return 0.0f;
    if (!ai.monster->m_stamina.hasEnough(12.0f))
        return 0.0f;
    a = (float *)MP(ai.monster, 0x3E30);
    b = (float *)MP(MI(ai.monster, 0x6BF8), 0x10);
    dx = a[0] - b[0];
    dy = a[1] - b[1];
    dz = a[2] - b[2];
    if (!(dx * dx + dy * dy + dz * dz < 200.0f * 200.0f))
        return 0.0f;
    if (MI(NAV(ai), 0x1C) != 2)
        return 0.0f;
    if (MI(NAV(ai), 0x20) == 3)
        return 1.0f;
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__13AiSmashReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 16/20 words, size 0x50 vs 0x50 */
float AiSmashReflex::getExitRelevance(Ai &ai)
{
    if (m_timer < -2) {
        if (STATE_ID(ai.monster) != 3)
            return 0.0f;
    }
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiSmashReflexR2Ai);
#endif
void AiSmashReflex::enterAction(Ai &ai)
{
    int d = ai.getReflexDelay() - getFieldsSinceEval();

    m_timer = d > -1 ? d : 0;
}
void AiSmashReflex::updateAction(Ai &ai)
{
    if (--m_timer > 0)
        return;
    ai.pad[0xF] = 0xFF;
    ai.heavyPunch();
}
void exitAction__13AiSmashReflexR2Ai(void *self) __asm__("exitAction__13AiSmashReflexR2Ai");
void exitAction__13AiSmashReflexR2Ai(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __14AiThrowRecover);
float AiThrowRecover::getEntryRelevance(Ai &ai)
{
    char *st = (char *)ai.monster->m_state;
    int ok;

    if (!(MI(st, 4) & 0x10))
        return 0.0f;
    ok = 0;
    if (MI(st, 0x38)) {
        if (MI(st, 0x530))
            ok = MI(st, 0x2A4) == 0;
    }
    if (!ok)
        return 0.0f;
    if (MF(MI(ai.monster, 0xC), 0x48) < 0.9f)
        return 0.0f;
    if (MATCH_MODE < 2) {
        if (mathfRand(0, 5) > 0)
            return 0.0f;
    }
    return 1.0f;
}
#ifdef NON_MATCHING
/* untuned: 1/13 words, size 0x34 vs 0x30 */
float AiThrowRecover::getExitRelevance(Ai &ai)
{
    if (m_timer < 0)
        return 0.0f;
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__14AiThrowRecoverR2Ai);
#endif
void AiThrowRecover::enterAction(Ai &ai)
{
    int d;

    ai.setFocus(0);
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
void AiThrowRecover::updateAction(Ai &ai)
{
    if (--m_timer > 0)
        return;
    ai.block();
}
void exitAction__14AiThrowRecoverR2Ai(void *self) __asm__("exitAction__14AiThrowRecoverR2Ai");
void exitAction__14AiThrowRecoverR2Ai(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __13AiThrowPickup);
float AiThrowPickup::getEntryRelevance(Ai &ai)
{
    float best;

    if (!ai.monster->m_attacksEnabled || !MI(ai.monster, 0x68A4))
        return 0.0f;
    m_target = getBestTarget(ai, best);
    if (MI(ai.monster, 0x68A4)) {
        int kind = MI(MI(MI(ai.monster, 0x68A4), 0), 0xA0);

        if (kind == 6 || kind == 0x10)
            best = best * 0.2f;
    }
    return best;
}
#ifdef NON_MATCHING
/* untuned: 48/115 words, size 0x1cc vs 0x1c8 */
Monster *AiThrowPickup::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    Monster **p = (Monster **)((char *)&ai + 4);
    int heavy;

    best = 0.0f;
    heavy = MI(MI(MI(ai.monster, 0x68A4), 0), 0xA0) == 3;
    for (; *p; p++) {
        Monster *o = *p;
        EnemyInfo::Info *info = EnemyInfo::getInfo(*ai.monster, *o);
        float w;
        float rel;

        if (!info->los)
            continue;
        if (m_range < info->dist)
            continue;
        if (!o->m_unk49)
            continue;
        if (o->m_state[1] & 0x10)
            continue;
        w = 1.0f;
        if (o->m_playerNum == 2)
            w = (o->m_health / ((HealthMeter *)((char *)o + 0x448))->getMaxLevel()) * 0.25f;
        if (!o->m_attacksEnabled)
            w = w + w;
        if (heavy) {
            if (info->dist < 500.0f)
                w = w * 0.0f;
            else
                w = w * 10.0f;
        }
        rel = ai.getFovRelevance(info->dot, -1.0f);
        w = w * (rel * 0.25f + 0.75f);
        if (best < w) {
            best = w;
            bestM = o;
        }
    }
    return bestM;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__13AiThrowPickupR2AiRf);
#endif
#ifdef NON_MATCHING
/* untuned: 6/26 words, size 0x64 vs 0x68 */
float AiThrowPickup::getExitRelevance(Ai &ai)
{
    if (!MI(ai.monster, 0x68A4))
        return 0.0f;
    if (!s_attackOtherAi && ai.pinningAi())
        return 0.0f;
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiThrowPickupR2Ai);
#endif
void AiThrowPickup::enterAction(Ai &ai)
{
    int d;

    NAV(ai)->target(*(_fvector *)MP(MI(m_target, 0xC), 0x10), 0.1f);
    ai.setFocus((DbInteractive *)m_target);
    ((GamePadClipPlayer *)((char *)&ai + 0x48))->clip = AiPadClips::getThrow();
    ((GamePadClipPlayer *)((char *)&ai + 0x48))->rewind();
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
#ifdef NON_MATCHING
/* untuned: 46/55 words, size 0xdc vs 0xdc */
void AiThrowPickup::updateAction(Ai &ai)
{
    Monster *m;
    int pressed;
    GamePadClipPlayer *cp;

    if (MF(EnemyInfo::getInfo(*ai.monster, *m_target), 0x14) > 0.7f || ai.overPit(*ai.monster))
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
    cp = (GamePadClipPlayer *)((char *)&ai + 0x48);
    if (!cp->update(*(GamePad *)ai.pad))
        cp->rewind();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__13AiThrowPickupR2Ai);
#endif
void AiThrowPickup::exitAction(Ai &ai)
{
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __13AiStompReflex);
#ifdef NON_MATCHING
/* untuned: 31/92 words, size 0x16c vs 0x170 */
float AiStompReflex::getEntryRelevance(Ai &ai)
{
    float rel;
    EnemyInfo::Info *info;
    StaminaMeter *sm;
    float st;

    if (!ai.monster->m_attacksEnabled)
        return 0.0f;
    if (!MB(ai.monster, 0x280))
        return 0.0f;
    m_target = ai.monster->getClosestMonster2D(m_range);
    if (!m_target)
        return 0.0f;
    if (!m_target->m_unk49)
        return 0.0f;
    if (m_target->m_state[1] & 0x10)
        return 0.0f;
    if (!((StateButtSlam *)MP(ai.monster, 0xFA1C))->transitionFeasible())
        return 0.0f;
    rel = 1.0f;
    info = EnemyInfo::getInfo(*ai.monster, *m_target);
    if (m_target->m_playerNum == 2) {
        float h = m_target->m_health;

        h = h / ((HealthMeter *)((char *)m_target + 0x448))->getMaxLevel();
        rel = h * 0.5f;
    }
    if (!m_target->m_attacksEnabled)
        rel = rel + rel;
    {
        float d = info->dist2D / m_range;

        rel = rel * (1.0f - d * d);
    }
    sm = &ai.monster->m_stamina;
    st = sm->cur;
    rel = rel * 1.0f;
    st = st / sm->getMaxLevel();
    rel = rel * (st * st);
    if (ai.monster->isSpecialAvailable())
        rel = rel * 0.0f;
    return rel;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__13AiStompReflexR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 2/14 words, size 0x38 vs 0x34 */
float AiStompReflex::getExitRelevance(Ai &ai)
{
    if (!MB(ai.monster, 0x280))
        return 0.0f;
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__13AiStompReflexR2Ai);
#endif
void AiStompReflex::enterAction(Ai &ai)
{
    AiNavigator *nav;
    int d;

    ai.setFocus((DbInteractive *)m_target);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
void AiStompReflex::updateAction(Ai &ai)
{
    if (--m_timer > 0)
        return;
    ai.buttStomp();
}
void AiStompReflex::exitAction(Ai &ai)
{
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __11AiRamAttack);
#ifdef NON_MATCHING
/* untuned: 29/40 words, size 0x9c vs 0xa0 */
float AiRamAttack::getEntryRelevance(Ai &ai)
{
    float best;
    StaminaMeter *sm;
    float st;

    if (!ai.monster->m_attacksEnabled || MB(ai.monster, 0x280) || *(unsigned char *)MP(MI(ai.monster, 0xC), 0xE) == 0)
        return 0.0f;
    m_target = getBestTarget(ai, best);
    sm = &ai.monster->m_stamina;
    st = sm->cur;
    st = st / sm->getMaxLevel();
    return best * (st * st);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__11AiRamAttackR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 15/188 words, size 0x2f0 vs 0x2c4 */
Monster *AiRamAttack::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    Monster **p = (Monster **)((char *)&ai + 4);

    best = 0.0f;
    for (; *p; p++) {
        Monster *o = *p;
        EnemyInfo::Info *info;
        float d, lat, w;
        float *v, *fwd;
        int ok;

        if (STATE_ID(o) == 0x1B)
            continue;
        if (o->m_state == (int *)MI(o, 0x7978)) {
            if (*(unsigned char *)MP(ai.monster, 0x18) != 6)
                continue;
        }
        info = EnemyInfo::getInfo(*ai.monster, *o);
        d = info->dist2D;
        if (m_maxRange < d)
            continue;
        if (d < m_minRange)
            continue;
        ok = 0;
        if (infoInFov(info, 0.0f))
            ok = infoInLos(info) != 0;
        if (!ok)
            continue;
        {
            float dy = MF(info, 8);

            if (dy < 0.0f ? -dy > 50.0f : dy > 50.0f)
                continue;
        }
        if (!o->m_unk49)
            continue;
        if (o->m_state[1] & 0x10)
            continue;
        if (MATCH_MODE < 2) {
            if (MI(EnemyInfo::getInfo(*o, *ai.monster), 0x24) == 0)
                continue;
        }
        v = (float *)MP(o, 0x260);
        fwd = (float *)MP(MI(ai.monster, 0xC), 0x20);
        lat = fwd[0] * v[0] + fwd[1] * v[1] + fwd[2] * v[2];
        lat = lat < 0.0f ? -lat : lat;
        if (m_lateral < lat)
            continue;
        w = 1.0f;
        if (o->m_playerNum == 2)
            w = (o->m_health / ((HealthMeter *)((char *)o + 0x448))->getMaxLevel()) * 0.5f;
        if (!o->m_attacksEnabled)
            w = w + w;
        {
            float t = (d - m_minRange) / (m_maxRange - m_minRange);

            w = w * ((1.0f - t * t) * 0.5f + 0.5f);
        }
        w = w * (1.0f - lat / m_lateral);
        w = w * ai.getCommonGroundProbability(*o);
        if (best < w) {
            best = w;
            bestM = o;
        }
    }
    return bestM;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__11AiRamAttackR2AiRf);
#endif
#ifdef NON_MATCHING
/* untuned: 4/75 words, size 0x12c vs 0x124 */
float AiRamAttack::getExitRelevance(Ai &ai)
{
    EnemyInfo::Info *info;
    int ok;
    int id;

    if (MI(&ai, 0x4C) != 1) {
        id = STATE_ID(ai.monster);
        if (id != 0x28 && id != 0x27)
            return 0.0f;
    }
    info = EnemyInfo::getInfo(*ai.monster, *m_target);
    ok = 0;
    if (infoInFov(info, 0.0f))
        ok = infoInLos(info) != 0;
    if (!ok)
        return 0.0f;
    if (!s_attackOtherAi && ai.pinningAi())
        return 0.0f;
    id = STATE_ID(ai.monster);
    if ((id == 0x27 && *(unsigned char *)MP(ai.monster, 0x7ACC)) || id == 0x28)
        m_bias = 10.0f;
    else
        m_bias = 1.5f;
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__11AiRamAttackR2Ai);
#endif
void AiRamAttack::enterAction(Ai &ai)
{
    int d = ai.getReflexDelay() - getFieldsSinceEval();

    m_timer = d > -1 ? d : 0;
    ai.setFocus((DbInteractive *)m_target);
    NAV(ai)->target(*(_fvector *)MP(MI(m_target, 0xC), 0x10), 0.1f);
    ((GamePadClipPlayer *)((char *)&ai + 0x48))->clip = AiPadClips::getDash();
    ((GamePadClipPlayer *)((char *)&ai + 0x48))->rewind();
}
void AiRamAttack::updateAction(Ai &ai)
{
    if (MF(EnemyInfo::getInfo(*ai.monster, *m_target), 0x14) > 0.7f)
        ai.targetPin();
    if (--m_timer > 0)
        return;
    if (((GamePadClipPlayer *)((char *)&ai + 0x48))->update(*(GamePad *)ai.pad))
        return;
    if (EnemyInfo::getInfo(*ai.monster, *m_target)->dist2D < 300.0f) {
        if (STATE_ID(ai.monster) == 0x27 && *(unsigned char *)MP(ai.monster, 0x7ACC) == 1)
            ai.pad[8] = 0xFF;
    }
    ai.pad[0xE] = 0xFF;
}
void AiRamAttack::exitAction(Ai &ai)
{
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __14AiCrowdControl);
float AiCrowdControl::getEntryRelevance(Ai &ai)
{
    float best;

    if (ai.monster->m_attacksEnabled && ai.monster->isSpecialAvailable()) {
        m_target = getBestTarget(ai, best);
        return best;
    }
    return 0.0f;
}
#ifdef NON_MATCHING
/* untuned: 4/116 words, size 0x1c4 vs 0x1d0 */
Monster *AiCrowdControl::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    Monster **p = (Monster **)((char *)&ai + 4);
    float radius;
    char *base = MP(ai.monster, 0xF3F0);

    best = 0.0f;
    radius = MF(base + (MI(base, 0x3C) - 1) * 4, 0x28C);
    for (; *p; p++) {
        Monster *o = *p;
        EnemyInfo::Info *info = EnemyInfo::getInfo(*ai.monster, *o);
        float w, d;

        if (m_range < info->dist)
            continue;
        if (25.0f < MF(info, 8))
            continue;
        if (!o->m_unk49)
            continue;
        if (o->m_state[1] & 0x10)
            continue;
        if (MATCH_MODE < 2) {
            if (MI(EnemyInfo::getInfo(*o, *ai.monster), 0x24) == 0)
                continue;
        }
        d = info->dist / radius;
        w = 1.0f - d * d;
        if (o->m_playerNum == 2)
            w = w * ((o->m_health / ((HealthMeter *)((char *)o + 0x448))->getMaxLevel()) * 0.5f);
        if (!o->m_attacksEnabled)
            w = w + w;
        if (best < w) {
            best = w;
            bestM = o;
        }
    }
    return bestM;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__14AiCrowdControlR2AiRf);
#endif
#ifdef NON_MATCHING
/* untuned: 9/12 words, size 0x30 vs 0x30 */
float AiCrowdControl::getExitRelevance(Ai &ai)
{
    if (m_timer > 0)
        return FMAX(1.0f, m_entryRel);
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__14AiCrowdControlR2Ai);
#endif
void AiCrowdControl::enterAction(Ai &ai)
{
    int d;

    ai.setFocus((DbInteractive *)m_target);
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
void AiCrowdControl::updateAction(Ai &ai)
{
    if (--m_timer > 0)
        return;
    ai.crowdControl();
}
void AiCrowdControl::exitAction(Ai &ai)
{
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __15AiSpecialAttack);
#ifdef NON_MATCHING
/* untuned: 24/43 words, size 0xa8 vs 0xac */
float AiSpecialAttack::getEntryRelevance(Ai &ai)
{
    float best;

    if (!ai.monster->isSpecialAvailable())
        return 0.0f;
    if (!ai.monster->m_attacksEnabled)
        return 0.0f;
    if (ai.monster->isHoldingLarge())
        return 0.0f;
    if (ai.monster->m_state[1] & 4)
        return 0.0f;
    if (MB(ai.monster, 0x280))
        return 0.0f;
    if (STATE_ID(ai.monster) == 0x1F)
        return 0.0f;
    m_target = getBestTarget(ai, best);
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__15AiSpecialAttackR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 15/140 words, size 0x218 vs 0x230 */
Monster *AiSpecialAttack::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    Monster **p = (Monster **)((char *)&ai + 4);

    best = 0.0f;
    for (; *p; p++) {
        Monster *o = *p;
        EnemyInfo::Info *info = EnemyInfo::getInfo(*ai.monster, *o);
        float w;

        if (m_maxRange < info->dist)
            continue;
        if (info->dist < m_minRange)
            continue;
        if (m_needLos) {
            if (!info->los)
                continue;
        }
        if (!o->m_unk49)
            continue;
        if (MB(o, 0xEF)) {
            int ok = 0;

            if (infoInFov(info, 0.92f))
                ok = infoInLos(info) != 0;
            if (!ok)
                continue;
            if (!(info->dist < 500.0f))
                continue;
        }
        if (MATCH_MODE < 2) {
            if (MI(EnemyInfo::getInfo(*o, *ai.monster), 0x24) == 0)
                continue;
        }
        w = ai.getFovRelevance(info->dot, -1.0f);
        {
            float t = (info->dist - m_minRange) / (m_maxRange - m_minRange);

            w = w * (1.0f - t * t);
        }
        if (o->m_playerNum == 2)
            w = w * ((o->m_health / ((HealthMeter *)((char *)o + 0x448))->getMaxLevel()) * 0.5f);
        if (!o->m_attacksEnabled)
            w = w + w;
        if (best < w) {
            best = w;
            bestM = o;
        }
    }
    return bestM;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__15AiSpecialAttackR2AiRf);
#endif
#ifdef NON_MATCHING
/* untuned: 5/122 words, size 0x1e8 vs 0x1ac */
float AiSpecialAttack::getExitRelevance(Ai &ai)
{
    EnemyInfo::Info *info;

    if (m_timer > 0)
        return FMAX(1.0f, m_entryRel);
    if (ai.monster->m_state == (int *)MI(ai.monster, 0x7980))
        return FMAX(1.0f, m_entryRel);
    info = EnemyInfo::getInfo(*ai.monster, *m_target);
    if (MI(ai.monster, 0x6C3C) != 0 && info->dot > 0.0f) {
        if (ai.monster->m_typeBits == 0x40) {
            AiVEntry *e = AI_VENT(ai.monster, 0x10, 0x10);
            float *a = ((float *(*)(void *))e->fn)((char *)ai.monster + e->delta);
            float *b = (float *)MP(MI(ai.monster, 0x6BF8), 0x10);
            float dx = a[0] - b[0];
            float dy = a[1] - b[1];
            float dz = a[2] - b[2];

            if (1000.0f * 1000.0f < dx * dx + dy * dy + dz * dz)
                ai.block();
        }
        return 0.0f;
    }
    if (!ai.monster->isSpecialAvailable())
        return 0.0f;
    if (MI(&ai, 0x9C) == 2)
        return 0.0f;
    if (m_start > 0) {
        if (m_duration < timerGetFieldCount() - m_start)
            return 0.0f;
    }
    if (!s_attackOtherAi && ai.pinningAi())
        return 0.0f;
    if (m_needLos == 0 || info->los != 0)
        return FMAX(1.0f, m_entryRel);
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__15AiSpecialAttackR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 29/38 words, size 0x98 vs 0x98 */
void AiSpecialAttack::enterAction(Ai &ai)
{
    int d = ai.getReflexDelay() - getFieldsSinceEval();

    m_maxFields = m_duration + 180;
    m_start = -1;
    m_timer = d > -1 ? d : 0;
    ai.setFocus((DbInteractive *)m_target);
    NAV(ai)->target(*(_fvector *)MP(MI(m_target, 0xC), 0x10), 0.3f);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", enterAction__15AiSpecialAttackR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 52/61 words, size 0xf4 vs 0xf4 */
void AiSpecialAttack::updateAction(Ai &ai)
{
    Monster *m;
    int pressed;

    if (MI(&ai, 0x9C) != 1)
        return;
    if (MF(EnemyInfo::getInfo(*ai.monster, *m_target), 0x14) > 0.7f)
        ai.targetPin();
    if (--m_timer >= 0)
        return;
    m = ai.monster;
    if (MI(m, 0x6C38) == 0)
        pressed = (*(PadFlags *)MP(m, 0x5040))[0]->f32 != 0;
    else
        pressed = MI(m, 0x6C34) != 0;
    if (!pressed)
        return;
    if (m_start < 0)
        m_start = timerGetFieldCount();
    if (timerGetFieldCount() - m_start < m_duration)
        ai.specialAttack();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__15AiSpecialAttackR2Ai);
#endif
void AiSpecialAttack::exitAction(Ai &ai)
{
    AiNavigator *nav = NAV(ai);

    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __14AiDetonateHead);
#ifdef NON_MATCHING
/* untuned: 19/33 words, size 0x80 vs 0x84 */
float AiDetonateHead::getEntryRelevance(Ai &ai)
{
    char *head;
    float best;

    if (MATCH_MODE == 0)
        return 0.0f;
    head = HEAD(ai);
    if (!head)
        return 0.0f;
    if (!MI(head, 0xC))
        return 0.0f;
    if (!*(unsigned char *)MP(MI(head, 0xC), 0xC))
        return 0.0f;
    if (MI(ai.monster, 0x68A4) == MI(head, 0x14))
        return 0.0f;
    getBestTarget(ai, best);
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__14AiDetonateHeadR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 8/164 words, size 0x1fc vs 0x290 */
Monster *AiDetonateHead::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    char *head = HEAD(ai);
    Monster **p = (Monster **)((char *)&ai + 4);

    best = 0.0f;
    for (; *p; p++) {
        Monster *o = *p;
        float *hp = (float *)MP(MI(head, 0xC), 0x10);
        AiVEntry *eo = AI_VENT(o, 0x10, 0x10);
        float *op = ((float *(*)(void *))eo->fn)((char *)o + eo->delta);
        float dx = hp[0] - op[0];
        float dy = hp[1] - op[1];
        float dz = hp[2] - op[2];
        float r;
        _fvector *a, *b, *c, *d;

        if (m_range < eeSqrtf(dx * dx + dy * dy + dz * dz))
            continue;
        {
            AiVEntry *e1 = AI_VENT(head, 0x10, 0x10);
            AiVEntry *e2 = AI_VENT(head, 0x10, 0x20);
            AiVEntry *e3 = AI_VENT(o, 0x10, 0x10);
            AiVEntry *e4 = AI_VENT(o, 0x10, 0x20);

            a = ((_fvector * (*)(void *))e1->fn)(head + e1->delta);
            b = ((_fvector * (*)(void *))e2->fn)(head + e2->delta);
            c = ((_fvector * (*)(void *))e3->fn)((char *)o + e3->delta);
            d = ((_fvector * (*)(void *))e4->fn)((char *)o + e4->delta);
        }
        r = mathfClosestApproach(*a, *b, *c, *d);
        if (MATCH_MODE < 2) {
            if (1.0f < r)
                continue;
        } else {
            if (0.0f < r)
                continue;
        }
        best = 1.0f;
        bestM = o;
    }
    return bestM;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getBestTarget__14AiDetonateHeadR2AiRf);
#endif
#ifdef NON_MATCHING
/* untuned: 9/12 words, size 0x30 vs 0x30 */
float AiDetonateHead::getExitRelevance(Ai &ai)
{
    if (m_timer > 0)
        return FMAX(1.0f, m_entryRel);
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__14AiDetonateHeadR2Ai);
#endif
void AiDetonateHead::enterAction(Ai &ai)
{
    int d;

    ai.setFocus((DbInteractive *)HEAD(ai));
    NAV(ai)->target(*(_fvector *)MP(MI(HEAD(ai), 0xC), 0x10), 0.1f);
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
void AiDetonateHead::updateAction(Ai &ai)
{
    if (--m_timer > 0)
        return;
    ai.specialAttack();
}
void AiDetonateHead::exitAction(Ai &ai)
{
    AiNavigator *nav = NAV(ai);

    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __7AiTaunt);
float AiTaunt::getEntryRelevance(Ai &ai)
{
    EnemyInfo::Info *info;

    if (!MB(ai.monster, 0xF4))
        return 0.0f;
    if (ai.monster->m_state[1] & 4)
        return 0.0f;
    m_target = ai.monster->getClosestMonsterWithLos(5000.0f);
    if (!m_target)
        return 0.0f;
    info = EnemyInfo::getInfo(*m_target, *ai.monster);
    if (!(m_range < info->dist))
        return 0.0f;
    if (MI(info, 0x24) != 0)
        return 1.0f;
    return 0.0f;
}
#ifdef NON_MATCHING
/* untuned: 6/20 words, size 0x50 vs 0x4c */
float AiTaunt::getExitRelevance(Ai &ai)
{
    if (m_timer >= 0 || STATE_ID(ai.monster) == 0x2A)
        return FMAX(1.0f, m_entryRel);
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__7AiTauntR2Ai);
#endif
void AiTaunt::enterAction(Ai &ai)
{
    int d;

    ai.setFocus((DbInteractive *)m_target);
    NAV(ai)->target(*(_fvector *)MP(MI(m_target, 0xC), 0x10), 0.3f);
    d = ai.getReflexDelay() - getFieldsSinceEval();
    m_timer = d > -1 ? d : 0;
}
#ifdef NON_MATCHING
/* untuned: 35/44 words, size 0xb0 vs 0xb0 */
void AiTaunt::updateAction(Ai &ai)
{
    Monster *m;
    int pressed;

    if (MF(EnemyInfo::getInfo(*ai.monster, *m_target), 0x14) > 0.7f)
        ai.targetPin();
    m = ai.monster;
    if (MI(m, 0x6C38) == 0)
        pressed = (*(PadFlags *)MP(m, 0x5040))[0]->f32 != 0;
    else
        pressed = MI(m, 0x6C34) != 0;
    if (pressed) {
        if (--m_timer <= 0)
            ai.taunt();
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", updateAction__7AiTauntR2Ai);
#endif
void AiTaunt::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", __12AiPutOutFire);
#ifdef NON_MATCHING
/* untuned: 1/69 words, size 0xfc vs 0x114 */
float AiPutOutFire::getEntryRelevance(Ai &ai)
{
    float rel = 0.0f;
    float burn = MF(ai.monster, 0x6CB8);

    if (0.0f < burn) {
        if (m_prevBurn - burn < 20.0f) {
            if (ai.monster->m_attacksEnabled) {
                if (MF(ai.monster, 0x470) > 100.0f) {
                    float h;

                    rel = burn * 0.5f / 300.0f + 0.5f;
                    h = MF(ai.monster, 0x44C);
                    h = h / ((HealthMeter *)MP(ai.monster, 0x448))->getMaxLevel();
                    rel = rel * ((1.0f - h * h) * 0.5f + 0.5f);
                }
            }
        }
    }
    m_prevBurn = MF(ai.monster, 0x6CB8);
    return rel;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getEntryRelevance__12AiPutOutFireR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 1/28 words, size 0x64 vs 0x70 */
float AiPutOutFire::getExitRelevance(Ai &ai)
{
    if (0.0f < MF(ai.monster, 0x6CB8)) {
        if (MI(&ai, 0x4C) == 1 || STATE_ID(ai.monster) == 0x27)
            return FMAX(1.0f, m_entryRel);
    }
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiReflex", getExitRelevance__12AiPutOutFireR2Ai);
#endif
void AiPutOutFire::enterAction(Ai &ai)
{
    int d = ai.getReflexDelay() - getFieldsSinceEval();

    m_timer = d > -1 ? d : 0;
    ((GamePadClipPlayer *)((char *)&ai + 0x48))->clip = AiPadClips::getDash();
    ((GamePadClipPlayer *)((char *)&ai + 0x48))->rewind();
}
void AiPutOutFire::updateAction(Ai &ai)
{
    if (--m_timer > 0)
        return;
    if (!((GamePadClipPlayer *)((char *)&ai + 0x48))->update(*(GamePad *)ai.pad))
        ai.pad[0xE] = 0xFF;
}
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

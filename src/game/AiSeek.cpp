#include "common.h"

#include "engine.h"
#include "game/game.h"
#include "game/ai.h"
#include "game/ai_action.h"
#include "game/ai_support.h"
#include "game/enemy_info.h"
#include "game/pad_flags.h"
#include "game/level_pickups.h"

float mathfClosestApproach(_fvector &a, _fvector &b, _fvector &c, _fvector &d, float t, float &outDist);
void mathfVectorCrossUp(_fvector *out, _fvector *in);

/* Sidestepping an opponent that is ramming us. */
class AiDodgeRam : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    char pad4C[4];
    float m_dir[4];    /* 0x50: point to run to; [3] (0x5C) keeps the signed length while it is being built */
    int m_timer;       /* 0x60 */
    float m_range;     /* 0x64: 1000.0 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Jumping away from an opponent that is about to stomp us. */
class AiDodgeStomp : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    char pad4C[4];
    float m_dir[4];    /* 0x50 */
    int m_timer;       /* 0x60 */
    float m_range;     /* 0x64: 1000.0 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};

/* Running from an opponent's special attack. */
class AiDodgeSpecial : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    char pad4C[4];
    float m_dir[4];    /* 0x50: flee spot */
    int m_timer;       /* 0x60 */
    float m_range;     /* 0x64 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateFleeSpot(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};

extern float stamMed __asm__("med.2444");
__asm__("#SNFIX_SMALL med.2444");

/* Walking to a stamina / special / cloak power-up: the three share everything but the entry relevance and the credit rule. */
class AiSeekItem : public AiActionTuple {
public:
    void *m_target;    /* 0x48: PowerUp */
    float m_range;     /* 0x4C: 1000.0 */
    int m_stomped;     /* 0x50 */
};
class AiSeekStamina : public AiSeekItem {
public:
    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
class AiSeekSpecial : public AiSeekItem {
public:
    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
class AiSeekCloak : public AiSeekItem {
public:
    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
#ifdef NON_MATCHING
static float seekItemExit(AiSeekItem *t, Ai &ai)
{
    AiNavigator *nav = NAV(ai);

    if (nav->status == 0) {
        if (t->m_stomped == 0 && nav->mode == 2 && nav->substate == nav->mode && STATE_ID(ai.monster) != 0x26 && ai.monster->m_stamina.hasEnough(15.0f))
            return 1.0f;
        return 0.0f;
    }
    if (*(unsigned char *)MP(MI(t->m_target, 8), 0xC) != 0 || (*(unsigned short *)t->m_target & 0x20))
        return FMAX(1.0f, t->m_entryRel);
    return 0.0f;
}
static void seekItemEnter(AiSeekItem *t, Ai &ai)
{
    float *pos = (float *)MP(MI(t->m_target, 8), 0x10);

    NAV(ai)->tag(*(_fvector *)pos, ai.getPowerUpGrabRange());
    t->m_stomped = 0;
}
static void seekItemUpdate(AiSeekItem *t, Ai &ai)
{
    AiNavigator *nav = NAV(ai);

    if (nav->mode == 2 && nav->substate == nav->mode) {
        if (STATE_ID(ai.monster) != 0x26) {
            if (((StateButtSlam *)MP(ai.monster, 0xFA1C))->transitionFeasible()) {
                AiVEntry *e = AI_VENT(ai.monster, 0x10, 0x20);
                float *v = ((float *(*)(void *))e->fn)((char *)ai.monster + e->delta);

                if (0.0f < v[2]) {
                    ai.buttStomp();
                    t->m_stomped = 1;
                    return;
                }
            }
            if (ai.monster->m_state != (int *)MI(ai.monster, 0x7978)) {
                if (*(unsigned short *)((char *)(*(PadFlags *)MP(ai.monster, 0x5040))[0] + 0x2A) != 0)
                    return;
            }
            ai.jump();
        }
    }
}
/* special: 0 = credit when the stamina bar is not full, 1 = when the special is not available */
static void seekItemExitAction(AiSeekItem *t, Ai &ai, int special)
{
    AiNavigator *nav;

    if (t->m_target) {
        if (MI(&ai, 0x9C) == 2) {
            if (*(unsigned char *)MP(MI(t->m_target, 8), 0xC) == 1) {
                int skip;

                if (special)
                    skip = ai.monster->isSpecialAvailable();
                else
                    skip = MF(ai.monster, 0x468) <= MF(ai.monster, 0x470);
                if (!skip)
                    ai.creditRelevance((PowerUp *)t->m_target, -0.5f);
            }
        } else if (MI(&ai, 0x9C) == 1) {
            if (*(unsigned char *)MP(MI(t->m_target, 8), 0xC) == 0)
                ai.creditRelevance((PowerUp *)t->m_target, 1.0f);
        }
    }
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
#endif

extern float PICK_UP_GRAB_RANGE;
__asm__("#SNFIX_SMALL PICK_UP_GRAB_RANGE");
extern float high __asm__("high.2424");
__asm__("#SNFIX_SMALL high.2424");
extern float med __asm__("med.2425");
__asm__("#SNFIX_SMALL med.2425");
extern float D_006F79F4;
extern float D_006F79F8;
extern void *LevelPickups_highlight[] __asm__("_12LevelPickups$s_highlightPickup");

/* Walking up to a monster (to be in range for the other actions). */
class AiSeekMonster : public AiActionTuple {
public:
    Monster *m_target; /* 0x48 */
    float m_standoff;  /* 0x4C: 20.0, gap to keep between the bodies */
    float m_maxRange;  /* 0x50: 10000.0 */

    float getEntryRelevance(Ai &ai);
    Monster *getBestTarget(Ai &ai, float &best);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Walking to a pickup (car, rock ...) and grabbing it. */
class AiSeekPickup : public AiActionTuple {
public:
    char pad48[8];
    float m_aim[4];    /* 0x50: where the pickup is heading */
    void *m_pickup;    /* 0x60 */
    int m_counter;     /* 0x64: frames the grab button was seen held */
    int m_timer;       /* 0x68 */
    float m_range;     /* 0x6C: 1000.0 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};
/* Walking to a health power-up. */
class AiSeekHealth : public AiActionTuple {
public:
    void *m_target;    /* 0x48: PowerUp */
    float m_range;     /* 0x4C: 4000.0 */
    int m_stomped;     /* 0x50 */

    float getEntryRelevance(Ai &ai);
    float getExitRelevance(Ai &ai);
    void enterAction(Ai &ai);
    void updateAction(Ai &ai);
    void exitAction(Ai &ai);
};

INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __12AiSeekHealth);
#ifdef NON_MATCHING
/* untuned: 9/96 words, size 0x174 vs 0x180 */
float AiSeekHealth::getEntryRelevance(Ai &ai)
{
    float best;
    float hp;

    hp = MF(ai.monster, 0x44C);
    hp = hp / ((HealthMeter *)MP(ai.monster, 0x448))->getMaxLevel();
    if (LEVEL_ID == 6 && ai.isMinion()) {
        if (1.0f <= hp)
            return 0.0f;
        m_target = ai.getBestHealthPowerup(best, m_range);
        if (!m_target)
            return 0.0f;
        return best;
    }
    if (1.0f <= hp)
        return 0.0f;
    m_target = ai.getBestPowerupByType(PowerUp::TYPE_2, best, 1000.0f);
    if (m_target)
        return 1.0f;
    if (hp < med) {
        m_target = ai.getBestHealthPowerup(best, m_range);
        if (!m_target)
            return 0.0f;
        return 1.0f;
    }
    if (hp < high) {
        m_target = ai.getBestHealthPowerup(best, m_range);
        best = best * (1.0f - hp * hp);
        return m_target ? best : 0.0f;
    }
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__12AiSeekHealthR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 64/86 words, size 0x158 vs 0x154 */
float AiSeekHealth::getExitRelevance(Ai &ai)
{
    AiNavigator *nav = NAV(ai);

    if (nav->status == 0) {
        if (m_stomped == 0 && nav->mode == 2 && nav->substate == nav->mode && STATE_ID(ai.monster) != 0x26 && ai.monster->m_stamina.hasEnough(15.0f))
            return 1.0f;
        if (MI(&ai, 0x9C) != 1)
            return 0.0f;
        if (*(unsigned char *)MP(MI(m_target, 8), 0xC) != 0)
            return 0.0f;
        if (!(0.0f < VCALL_F(this, 8, ai)))
            return 0.0f;
        VCALL_V(this, 0x18, ai);
        return 1.0f;
    }
    if (*(unsigned char *)MP(MI(m_target, 8), 0xC) != 0 || (*(unsigned short *)m_target & 0x20))
        return FMAX(1.0f, m_entryRel);
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__12AiSeekHealthR2Ai);
#endif
void AiSeekHealth::enterAction(Ai &ai)
{
    float *pos = (float *)MP(MI(m_target, 8), 0x10);

    NAV(ai)->tag(*(_fvector *)pos, ai.getPowerUpGrabRange());
    m_stomped = 0;
}
void AiSeekHealth::updateAction(Ai &ai)
{
    AiNavigator *nav = NAV(ai);

    if (nav->mode == 2 && nav->substate == nav->mode) {
        if (STATE_ID(ai.monster) != 0x26) {
            if (((StateButtSlam *)MP(ai.monster, 0xFA1C))->transitionFeasible()) {
                AiVEntry *e = AI_VENT(ai.monster, 0x10, 0x20);
                float *v = ((float *(*)(void *))e->fn)((char *)ai.monster + e->delta);

                if (0.0f < v[2]) {
                    ai.buttStomp();
                    m_stomped = 1;
                    return;
                }
            }
            if (ai.monster->m_state != (int *)MI(ai.monster, 0x7978)) {
                if (*(unsigned short *)((char *)(*(PadFlags *)MP(ai.monster, 0x5040))[0] + 0x2A) != 0)
                    return;
            }
            ai.jump();
        }
    }
}
#ifdef NON_MATCHING
/* untuned: 12/51 words, size 0xc8 vs 0xcc */
void AiSeekHealth::exitAction(Ai &ai)
{
    AiNavigator *nav;

    if (m_target) {
        if (MI(&ai, 0x9C) == 2) {
            if (*(unsigned char *)MP(MI(m_target, 8), 0xC) == 1) {
                if (!(MF(ai.monster, 0x448) <= MF(ai.monster, 0x44C)))
                    ai.creditRelevance((PowerUp *)m_target, -0.5f);
            }
        } else if (MI(&ai, 0x9C) == 1) {
            if (*(unsigned char *)MP(MI(m_target, 8), 0xC) == 0)
                ai.creditRelevance((PowerUp *)m_target, 1.0f);
        }
    }
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__12AiSeekHealthR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __13AiSeekStamina);
#ifdef NON_MATCHING
/* untuned: 36/110 words, size 0x1a8 vs 0x1b8 */
float AiSeekStamina::getEntryRelevance(Ai &ai)
{
    float best;
    float st;
    StaminaMeter *sm;

    sm = &ai.monster->m_stamina;
    st = sm->cur;
    st = st / sm->getMaxLevel();
    if (LEVEL_ID == 6 && ai.isMinion()) {
        if (1.0f <= st)
            return 0.0f;
        m_target = ai.getBestStaminaPowerup(best, m_range);
        if (!m_target)
            return 0.0f;
        return best;
    }
    if (ai.monster->m_stamina.maxLevel <= ai.monster->m_stamina.cur)
        return 0.0f;
    if (0.75f < st && ai.monster->isSpecialAvailable())
        return 0.0f;
    m_target = ai.getBestPowerupByType((PowerUp::Type)5, best, 1000.0f);
    if (m_target)
        return 1.0f;
    if (st < stamMed) {
        m_target = ai.getBestStaminaPowerup(best, m_range);
        if (!m_target)
            return 0.0f;
        return 1.0f;
    }
    m_target = ai.getBestStaminaPowerup(best, m_range);
    best = best * (1.0f - st * st);
    return m_target ? best : 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__13AiSeekStaminaR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 1/48 words, size 0x1c vs 0xc0 */
float AiSeekStamina::getExitRelevance(Ai &ai)
{
    return seekItemExit(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__13AiSeekStaminaR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/24 words, size 0x1c vs 0x60 */
void AiSeekStamina::enterAction(Ai &ai)
{
    seekItemEnter(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__13AiSeekStaminaR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/59 words, size 0x1c vs 0xec */
void AiSeekStamina::updateAction(Ai &ai)
{
    seekItemUpdate(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__13AiSeekStaminaR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/52 words, size 0x1c vs 0xd0 */
void AiSeekStamina::exitAction(Ai &ai)
{
    seekItemExitAction(this, ai, 0);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__13AiSeekStaminaR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __13AiSeekSpecial);
#ifdef NON_MATCHING
/* untuned: 8/25 words, size 0x64 vs 0x5c */
float AiSeekSpecial::getEntryRelevance(Ai &ai)
{
    float best;

    if (!ai.monster->isSpecialAvailable()) {
        m_target = ai.getBestPowerupByType((PowerUp::Type)8, best, m_range);
        return best;
    }
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__13AiSeekSpecialR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 1/48 words, size 0x1c vs 0xc0 */
float AiSeekSpecial::getExitRelevance(Ai &ai)
{
    return seekItemExit(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__13AiSeekSpecialR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/24 words, size 0x1c vs 0x60 */
void AiSeekSpecial::enterAction(Ai &ai)
{
    seekItemEnter(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__13AiSeekSpecialR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/59 words, size 0x1c vs 0xec */
void AiSeekSpecial::updateAction(Ai &ai)
{
    seekItemUpdate(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__13AiSeekSpecialR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/49 words, size 0x1c vs 0xc4 */
void AiSeekSpecial::exitAction(Ai &ai)
{
    seekItemExitAction(this, ai, 1);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__13AiSeekSpecialR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __11AiSeekCloak);
#ifdef NON_MATCHING
/* untuned: 6/21 words, size 0x54 vs 0x50 */
float AiSeekCloak::getEntryRelevance(Ai &ai)
{
    float best;

    if (!MB(ai.monster, 0xEF)) {
        m_target = ai.getBestPowerupByType((PowerUp::Type)9, best, m_range);
        return best;
    }
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__11AiSeekCloakR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 1/48 words, size 0x1c vs 0xc0 */
float AiSeekCloak::getExitRelevance(Ai &ai)
{
    return seekItemExit(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__11AiSeekCloakR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/24 words, size 0x1c vs 0x60 */
void AiSeekCloak::enterAction(Ai &ai)
{
    seekItemEnter(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__11AiSeekCloakR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/59 words, size 0x1c vs 0xec */
void AiSeekCloak::updateAction(Ai &ai)
{
    seekItemUpdate(this, ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__11AiSeekCloakR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/49 words, size 0x1c vs 0xc4 */
void AiSeekCloak::exitAction(Ai &ai)
{
    seekItemExitAction(this, ai, 1);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__11AiSeekCloakR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __12AiSeekPickup);
#ifdef NON_MATCHING
/* untuned: 15/29 words, size 0x70 vs 0x74 */
float AiSeekPickup::getEntryRelevance(Ai &ai)
{
    float best;

    if (ai.monster->isHolding())
        return 0.0f;
    if (!ai.monster->getClosestMonsterWithLos(D_006F79F4))
        return 0.0f;
    m_pickup = ai.getBestPickup(best, m_range);
    if (!m_pickup)
        return 0.0f;
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__12AiSeekPickupR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 8/69 words, size 0x114 vs 0x100 */
float AiSeekPickup::getExitRelevance(Ai &ai)
{
    AiNavigator *nav;

    if (m_timer > 0)
        return FMAX(1.0f, m_entryRel);
    nav = NAV(ai);
    if (nav->mode == 2)
        return 0.0f;
    if (!MI(m_pickup, 0xC))
        return 0.0f;
    if (MI(m_pickup, 0xD8))
        return 0.0f;
    if (m_counter > 0 && STATE_ID(ai.monster) != 5)
        return 0.0f;
    if (nav->mode == 1) {
        if (!LevelPickups_highlight[MI(ai.monster, 0x28)])
            return 0.0f;
    }
    if (LEVEL_ID == 7) {
        if (!ai.monster->getClosestMonsterWithLos(D_006F79F8))
            return 0.0f;
    }
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__12AiSeekPickupR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 1/55 words, size 0xdc vs 0xb8 */
void AiSeekPickup::enterAction(Ai &ai)
{
    float *pos;
    float *vel;

    ai.setFocus((DbInteractive *)m_pickup);
    NAV(ai)->arrive(*(_fvector *)m_aim, PICK_UP_GRAB_RANGE, PICK_UP_GRAB_RANGE, 0.9f);
    {
        AiVEntry *e = AI_VENT(m_pickup, 0x10, 0x20);

        pos = (float *)MP(MI(m_pickup, 0xC), 0x10);
        vel = ((float *(*)(void *))e->fn)((char *)m_pickup + e->delta);
    }
    m_aim[0] = pos[0] + vel[0] * 0.5f;
    m_aim[1] = pos[1] + vel[1] * 0.5f;
    m_aim[2] = pos[2] + vel[2] * 0.5f;
    m_timer = ai.getReflexDelay();
    m_counter = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__12AiSeekPickupR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 0/74 words, size 0x128 vs 0x104 */
void AiSeekPickup::updateAction(Ai &ai)
{
    char *cs = (char *)MI(m_pickup, 0xC);
    float *pos, *vel;
    void *hl;

    if (!cs)
        return;
    {
        AiVEntry *e = AI_VENT(m_pickup, 0x10, 0x20);

        pos = (float *)(cs + 0x10);
        vel = ((float *(*)(void *))e->fn)((char *)m_pickup + e->delta);
    }
    m_aim[0] = pos[0] + vel[0] * 0.5f;
    m_aim[1] = pos[1] + vel[1] * 0.5f;
    m_aim[2] = pos[2] + vel[2] * 0.5f;
    hl = LevelPickups_highlight[MI(ai.monster, 0x28)];
    if (--m_timer > 0)
        return;
    if (!hl)
        return;
    if (*(void **)hl != m_pickup && MI(&ai, 0x9C) != 1)
        return;
    if (*(unsigned short *)((char *)(*(PadFlags *)MP(ai.monster, 0x5040))[1] + 0x2E) != 0)
        m_counter++;
    else
        ai.grab();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__12AiSeekPickupR2Ai);
#endif
void AiSeekPickup::exitAction(Ai &ai)
{
    AiNavigator *nav;

    if (m_pickup) {
        if (MI(&ai, 0x9C) == 2 || (m_counter > 0 && STATE_ID(ai.monster) == 0x1F)) {
            if (!ai.monster->isHolding()) {
                if (MI(m_pickup, 0xC))
                    ai.creditRelevance((Pickup *)m_pickup, -0.5f);
            }
        }
    }
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
    ai.setFocus(0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __13AiSeekMonster);
#ifdef NON_MATCHING
/* untuned: 32/44 words, size 0xb0 vs 0xb0 */
float AiSeekMonster::getEntryRelevance(Ai &ai)
{
    float best;
    StaminaMeter *sm;
    float st;

    m_target = getBestTarget(ai, best);
    sm = &ai.monster->m_stamina;
    st = sm->cur;
    st = st / sm->getMaxLevel();
    best = best * ((st * st) * 0.9f + 0.1f);
    if (MI(ai.monster, 0x68C0) != 0)
        best = best * 10.0f;
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__13AiSeekMonsterR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 16/197 words, size 0x304 vs 0x314 */
Monster *AiSeekMonster::getBestTarget(Ai &ai, float &best)
{
    Monster *bestM = 0;
    Monster **p = (Monster **)((char *)&ai + 4);
    float myRadius = MF(ai.monster, 0x404) + MF(ai.monster, 0x3F4);
    int heavy = 0;

    if (MI(ai.monster, 0x68A4))
        heavy = MI(MI(MI(ai.monster, 0x68A4), 0), 0xA0) == 3;
    best = 0.0f;
    for (; *p; p++) {
        Monster *o = *p;
        EnemyInfo::Info *info = EnemyInfo::getInfo(*ai.monster, *o);
        int los;
        float w, t, h;

        if (m_maxRange < info->dist)
            continue;
        if (o->m_playerNum == 2 && !s_attackOtherAi)
            continue;
        if (ai.overPit(*o))
            continue;
        los = info->los;
        if (los) {
            if (-50.0f < MF(info, 8)) {
                if (info->dist2D < m_standoff + myRadius + MF(o, 0x404) + MF(o, 0x3F4))
                    continue;
            }
        }
        t = (info->dist - m_standoff) / (m_maxRange - m_standoff);
        w = (1.0f - t) * 0.5f + 0.5f;
        if (!los)
            w = w * 0.5f;
        if (!(o->m_flags & 2))
            w = w * 0.2f;
        if (!o->m_attacksEnabled)
            w = w + w;
        if (!o->m_unk49)
            w = w * 0.5f;
        h = o->m_health / ((HealthMeter *)((char *)o + 0x448))->getMaxLevel();
        h = (1.0f - h) * 0.5f + 0.5f;
        w = w * h;
        w = w * (ai.getFovRelevance(info->dot, -1.0f) * 0.25f + 0.75f);
        if (ai.monster->isHolding()) {
            if (heavy && info->dist < 600.0f)
                w = w * 0.01f;
            else if (info->los)
                w = w + w;
            else
                w = w * 0.5f;
        }
        if (o->m_playerNum == 2) {
            if (MI(EnemyInfo::getInfo(*ai.monster, *o), 0x24) != 0)
                w = w * 0.5f;
            else
                w = w * 0.0f;
        }
        if (best < w) {
            best = w;
            bestM = o;
        }
    }
    return bestM;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getBestTarget__13AiSeekMonsterR2AiRf);
#endif
#ifdef NON_MATCHING
/* untuned: 18/55 words, size 0xd8 vs 0xdc */
float AiSeekMonster::getExitRelevance(Ai &ai)
{
    Monster **p;

    if (MI(&ai, 0x98) == 0)
        return 0.0f;
    if (LEVEL_ID == 7 && MF(MI(m_target, 0xC), 0x18) < -200.0f && ai.overPit(*m_target))
        return 0.0f;
    p = ai.m_opponents;
    if (!*p)
        return 0.0f;
    do {
        if (*p++ == m_target)
            return FMAX(1.0f, m_entryRel);
    } while (*p);
    return 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__13AiSeekMonsterR2Ai);
#endif
void AiSeekMonster::enterAction(Ai &ai)
{
    ai.setFocus((DbInteractive *)m_target);
    NAV(ai)->seek(*ai.m_focus, 50.0f);
}
#ifdef NON_MATCHING
/* untuned: 23/57 words, size 0xe4 vs 0xe4 */
void AiSeekMonster::updateAction(Ai &ai)
{
    EnemyInfo::Info *info = EnemyInfo::getInfo(*ai.monster, *m_target);
    float reach = m_standoff + MF(ai.monster, 0x404) + MF(ai.monster, 0x3F4);
    AiNavigator *nav;

    reach = reach + MF(m_target, 0x404);
    reach = reach + MF(m_target, 0x3F4);
    nav = NAV(ai);
    if (info->los != 0 && -50.0f < MF(info, 8) && info->dist2D < reach) {
        if (nav->status)
            nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
        return;
    }
    if (nav->mode != 0)
        nav->seek(*ai.m_focus, 50.0f);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__13AiSeekMonsterR2Ai);
#endif
void AiSeekMonster::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __7AiSwarm);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__7AiSwarmR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getBestTarget__7AiSwarmR2AiRf);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__7AiSwarmR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__7AiSwarmR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__7AiSwarmR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterState__7AiSwarmR2AiQ27AiSwarm5State);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__7AiSwarmR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __12AiDodgeThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__12AiDodgeThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__12AiDodgeThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__12AiDodgeThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__12AiDodgeThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__12AiDodgeThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __12AiCatchThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__12AiCatchThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__12AiCatchThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__12AiCatchThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__12AiCatchThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__12AiCatchThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __10AiBatThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__10AiBatThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__10AiBatThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__10AiBatThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateAction__10AiBatThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__10AiBatThrowR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __10AiDodgeRam);
#ifdef NON_MATCHING
/* untuned: 2/130 words, size 0x208 vs 0x1fc */
float AiDodgeRam::getEntryRelevance(Ai &ai)
{
    float best = 0.0f;
    Monster **p = (Monster **)((char *)&ai + 4);

    m_target = 0;
    for (; *p; p++) {
        Monster *o = *p;
        EnemyInfo::Info *info = EnemyInfo::getInfo(*ai.monster, *o);
        float v[4];
        float *fwd, *pos;
        float dist;
        float w;
        EnemyInfo::Info *info2;
        int id;

        if (m_range < info->dist)
            continue;
        if (!info->los)
            continue;
        id = STATE_ID(o);
        if (id != 0x27 && id != 0x28)
            continue;
        fwd = (float *)MP(MI(o, 0xC), 0x30);
        v[0] = fwd[0] * 1200.0f;
        v[1] = fwd[1] * 1200.0f;
        v[2] = fwd[2] * 1200.0f;
        v[3] = fwd[3];
        pos = (float *)MP(MI(ai.monster, 0xC), 0x10);
        {
            AiVEntry *e = AI_VENT(ai.monster, 0x10, 0x20);
            _fvector *vel = ((_fvector * (*)(void *))e->fn)((char *)ai.monster + e->delta);

            mathfClosestApproach(*(_fvector *)pos, *vel, *(_fvector *)MP(MI(o, 0xC), 0x10), *(_fvector *)v, 1.5f, dist);
        }
        if (75.0f * 75.0f < dist)
            continue;
        info2 = EnemyInfo::getInfo(*o, *ai.monster);
        w = 1.0f;
        if (MATCH_MODE < 2)
            w = ai.getFovRelevance(info2->dot, 0.8f);
        w = w * ((1.0f - info2->dist / m_range) * 0.5f + 0.5f);
        if (best < w) {
            m_target = o;
            best = w;
        }
    }
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__10AiDodgeRamR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 7/36 words, size 0x88 vs 0x90 */
float AiDodgeRam::getExitRelevance(Ai &ai)
{
    int id = STATE_ID(m_target);

    if (id != 0x28 && id != 0x27)
        return 0.0f;
    if (MI(&ai, 0x98) == 0)
        return 0.0f;
    if (EnemyInfo::getInfo(*m_target, *ai.monster)->dot <= 0.0f)
        return 0.0f;
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__10AiDodgeRamR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 13/92 words, size 0x170 vs 0x124 */
void AiDodgeRam::enterAction(Ai &ai)
{
    float len;
    float *vel;
    float s;
    float *pos;

    ai.setFocus((DbInteractive *)m_target);
    m_timer = ai.getReflexDelay();
    mathfVectorCrossUp((_fvector *)m_dir, (_fvector *)MP(MI(m_target, 0xC), 0x30));
    len = sqrtf(m_dir[0] * m_dir[0] + m_dir[1] * m_dir[1] + m_dir[2] * m_dir[2]);
    m_dir[3] = len;
    vel = (float *)MP(ai.monster, 0x260);
    if (0.0f < vel[0] * m_dir[0] + vel[1] * m_dir[1] + vel[2] * m_dir[2])
        m_dir[3] = -len;
    s = 300.0f / m_dir[3];
    m_dir[0] *= s;
    m_dir[1] *= s;
    m_dir[2] *= s;
    pos = (float *)MP(MI(ai.monster, 0xC), 0x10);
    m_dir[0] += pos[0];
    m_dir[1] += pos[1];
    m_dir[2] += pos[2];
    NAV(ai)->tag(*(_fvector *)m_dir, 50.0f);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__10AiDodgeRamR2Ai);
#endif
void AiDodgeRam::updateAction(Ai &ai)
{
    if (EnemyInfo::getInfo(*ai.monster, *m_target)->dot > 0.0f)
        ai.targetPin();
    if (--m_timer < 0)
        ai.pad[6] = 0xFF;
}
void AiDodgeRam::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __12AiDodgeStomp);
#ifdef NON_MATCHING
/* untuned: 12/61 words, size 0xf4 vs 0xbc */
float AiDodgeStomp::getEntryRelevance(Ai &ai)
{
    float dy;

    m_target = ai.monster->getClosestMonster2D(m_range);
    if (!m_target)
        return 0.0f;
    if (STATE_ID(m_target) != 0x26)
        return 0.0f;
    dy = MF(MI(m_target, 0x1A3C), 0x18) - MF(MI(ai.monster, 0x1A3C), 0x18);
    if (100.0f < (dy < 0.0f ? -dy : dy))
        return 0.0f;
    return 1.0f - EnemyInfo::getInfo(*m_target, *ai.monster)->dist2D / m_range;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__12AiDodgeStompR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 20/23 words, size 0x5c vs 0x5c */
float AiDodgeStomp::getExitRelevance(Ai &ai)
{
    if (STATE_ID(m_target) != 0x26)
        return 0.0f;
    if (MI(&ai, 0x98) == 0)
        return 0.0f;
    return FMAX(1.0f, m_entryRel);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getExitRelevance__12AiDodgeStompR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned: 1/68 words, size 0x110 vs 0xc0 */
void AiDodgeStomp::enterAction(Ai &ai)
{
    float *info;
    float *pos;
    float s;

    ai.setFocus((DbInteractive *)m_target);
    m_timer = ai.getReflexDelay();
    info = (float *)EnemyInfo::getInfo(*m_target, *ai.monster);
    m_dir[0] = info[0];
    m_dir[1] = info[1];
    m_dir[2] = info[2];
    m_dir[3] = info[3];
    m_dir[2] = 0.0f;
    pos = (float *)MP(MI(ai.monster, 0xC), 0x10);
    s = 300.0f / sqrtf(m_dir[0] * m_dir[0] + m_dir[1] * m_dir[1]);
    m_dir[0] = pos[0] + m_dir[0] * s;
    m_dir[1] = pos[1] + m_dir[1] * s;
    m_dir[2] = pos[2] + m_dir[2] * s;
    NAV(ai)->tag(*(_fvector *)m_dir, 50.0f);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__12AiDodgeStompR2Ai);
#endif
void AiDodgeStomp::updateAction(Ai &ai)
{
    if (EnemyInfo::getInfo(*ai.monster, *m_target)->dot > 0.0f)
        ai.targetPin();
    if (--m_timer < 0)
        ai.pad[6] = 0xFF;
}
void AiDodgeStomp::exitAction(Ai &ai)
{
    AiNavigator *nav;

    ai.setFocus(0);
    nav = NAV(ai);
    if (nav->status)
        nav->disable(AiNavigator::STATUS_3, AiNavigator::HINT_0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __14AiDodgeSpecial);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", getEntryRelevance__14AiDodgeSpecialR2Ai);
float AiDodgeSpecial::getExitRelevance(Ai &ai)
{
    if (m_timer < 0 && MI(&ai, 0x98) == 0)
        return 0.0f;
    if (m_range < EnemyInfo::getInfo(*ai.monster, *m_target)->dist)
        return 0.0f;
    return VCALL_F(this, 8, ai);
}
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", enterAction__14AiDodgeSpecialR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", updateFleeSpot__14AiDodgeSpecialR2Ai);
void AiDodgeSpecial::updateAction(Ai &ai)
{
    updateFleeSpot(ai);
    if (EnemyInfo::getInfo(*ai.monster, *m_target)->dot > 0.0f)
        ai.targetPin();
    if (--m_timer == 0)
        NAV(ai)->tag(*(_fvector *)m_dir, 50.0f);
    else if (m_timer < 0)
        ai.pad[6] = 0xFF;
}
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", exitAction__14AiDodgeSpecialR2Ai);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$14AiDodgeSpecial);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$12AiDodgeStomp);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$10AiDodgeRam);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$10AiBatThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$12AiCatchThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$12AiDodgeThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$7AiSwarm);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$13AiSeekMonster);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$12AiSeekPickup);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$11AiSeekCloak);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$13AiSeekSpecial);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$13AiSeekStamina);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", _vt$12AiSeekHealth);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf12AiSeekHealth);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf13AiSeekStamina);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf13AiSeekSpecial);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf11AiSeekCloak);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf12AiSeekPickup);
void setRange__12AiSeekPickupf(void *self, float v) __asm__("setRange__12AiSeekPickupf");
void setRange__12AiSeekPickupf(void *self, float v)
{
    *(float *)((char *)self + 0x6C) = v;
}
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf13AiSeekMonster);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf7AiSwarm);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf12AiDodgeThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf12AiCatchThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf10AiBatThrow);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf10AiDodgeRam);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf12AiDodgeStomp);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", __tf14AiDodgeSpecial);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", func_00113C58);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", func_00113C60);
INCLUDE_ASM("asm/nonmatchings/game/AiSeek", func_00113C68);

#include "common.h"
#include "engine.h"
#include "game/ai.h"
#include "game/ai_action.h"

#define VCALL_F(self, off, ai) (((float (*)(void *, Ai &))AI_VENT(self, 0x44, off)->fn)((char *)(self) + AI_VENT(self, 0x44, off)->delta, ai))
#define VCALL_V(self, off, ai) (((void (*)(void *, Ai &))AI_VENT(self, 0x44, off)->fn)((char *)(self) + AI_VENT(self, 0x44, off)->delta, ai))
#define EPS 1.0e-10f

INCLUDE_ASM("asm/nonmatchings/game/AiAction", __13AiActionTuplef);
#ifdef NON_MATCHING
/* store order */
void AiActionTuple::init(float weight)
{
    reset();
    m_weight = weight;
    m_bias = 1.5f;
    m_ivalueMax = 1.0f;
    m_ivalue = 1.0f;
    m_ivalueMin = 0.0f;
    m_ivalueDrop = 0.0f;
    m_decayTime = 0.0f;
    m_recoverTime = 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", init__13AiActionTuplef);
#endif
#ifdef NON_MATCHING
/* untuned */
void AiActionTuple::reset(void)
{
    m_lastExit = -1;
    m_lastEnter = -1;
    m_runs = 0;
    m_entryRel = 0.0f;
    m_relevance = 0.0f;
    if (m_recoverTime > EPS)
        m_ivalue = mathfRandf(0.1f, 1.0f);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", reset__13AiActionTuple);
#endif
#ifdef NON_MATCHING
/* untuned: FPR allocation */
void AiActionTuple::updateIValueMod(void)
{
    if (m_lastEnter >= 0) {
        float t = m_decayTime;

        if (t > EPS) {
            float v = m_ivalue - 1.0f / (t * 60.0f);

            m_ivalue = v > m_ivalueMin ? v : m_ivalueMin;
        }
    } else {
        float t = m_recoverTime;

        if (t > EPS) {
            float v = m_ivalue + 1.0f / (t * 60.0f);

            m_ivalue = v < m_ivalueMax ? v : m_ivalueMax;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", updateIValueMod__13AiActionTuple);
#endif
#ifdef NON_MATCHING
/* untuned: scheduling/branch layout */
float AiActionTuple::getRelevance(Ai &ai)
{
    if (m_lastEnter >= 0) {
        if (timerGetFieldCount() - m_lastEnter < m_maxFields) {
            float r = VCALL_F(this, 0x10, ai);

            m_entryRel = r;
            m_relevance = m_ivalue * m_weight * r * m_bias;
        } else {
            m_relevance = 0.0f;
            m_entryRel = 0.0f;
        }
    } else {
        int mask;

        if (*ai.m_gate == 0)
            mask = 3;
        else if (*(signed char *)((char *)ai.monster + 0x280) || ai.overPit(*ai.monster))
            mask = 1;
        else
            mask = 2;
        if (m_ivalue * m_weight > EPS) {
            if (m_flags & mask)
                m_entryRel = VCALL_F(this, 8, ai);
            else
                m_entryRel = 0.0f;
        } else
            m_entryRel = 0.0f;
        m_relevance = m_ivalue * m_weight * m_entryRel;
        m_lastEval = timerGetFieldCount();
    }
    return m_relevance;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", getRelevance__13AiActionTupleR2Ai);
#endif
int AiActionTuple::getFieldsSinceEval(void)
{
    return timerGetFieldCount() - m_lastEval;
}
void AiActionTuple::execute(Ai &ai)
{
    if (m_lastEnter < 0) {
        if (m_lastEval < timerGetFieldCount()) {
            if (VCALL_F(this, 8, ai) == 0.0f)
                return;
        }
        VCALL_V(this, 0x18, ai);
        m_lastEnter = timerGetFieldCount();
        m_runs++;
    }
    VCALL_V(this, 0x20, ai);
}
#ifdef NON_MATCHING
/* 8/26 words, untuned */
void AiActionTuple::deactivate(Ai &ai)
{
    VCALL_V(this, 0x28, ai);
    {
        float v = m_ivalue - m_ivalueDrop;

        m_ivalue = v > m_ivalueMin ? v : m_ivalueMin;
    }
    m_lastEnter = -1;
    m_lastExit = timerGetFieldCount();
    m_relevance = 0.0f;
    m_entryRel = 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", deactivate__13AiActionTupleR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiAction", __12AiActionList);
void AiActionList::init(Strategy st)
{
    m_count = 0;
    setStrategy(st);
}
#ifdef NON_MATCHING
/* loop shape differs */
void AiActionList::reset(void)
{
    unsigned n = m_count;
    AiActionTuple **p = m_tuples;

    while (n--)
        (*p++)->reset();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", reset__12AiActionList);
#endif
#ifdef NON_MATCHING
/* member-pointer constants; branch layout */
void AiActionList::setStrategy(Strategy st)
{
    if (st == FIRST_POSITIVE)
        m_run = &AiActionList::runFirstPositive;
    else if (st == MAX_POSITIVE)
        m_run = &AiActionList::runMaxPositive;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", setStrategy__12AiActionListQ212AiActionList8Strategy);
#endif
#ifdef NON_MATCHING
/* 7/8 words: index masking order */
void AiActionList::addTuple(AiActionTuple &t)
{
    m_tuples[m_count++] = &t;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", addTuple__12AiActionListR13AiActionTuple);
#endif
void AiActionList::addTuple(AiActionTuple &t, float weight)
{
    t.init(weight);
    addTuple(t);
}
#ifdef NON_MATCHING
/* untuned */
AiActionTuple *AiActionList::runFirstPositive(Ai &ai)
{
    AiActionTuple *found = 0;
    AiActionTuple **p = m_tuples;
    unsigned i = 0;
    unsigned skip = timerGetFieldCount() % m_count;

    if (m_count) {
        do {
            AiActionTuple *t = *p;
            float r;

            t->updateIValueMod();
            if (!found) {
                if (i == skip || t->m_lastEnter >= 0)
                    r = t->getRelevance(ai);
                else
                    r = t->m_relevance;
                if (r > 0.0f)
                    found = t;
            }
            i++;
            p++;
        } while (i < m_count);
    }
    return found;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", runFirstPositive__12AiActionListR2Ai);
#endif
#ifdef NON_MATCHING
/* untuned */
AiActionTuple *AiActionList::runMaxPositive(Ai &ai)
{
    AiActionTuple *best = 0;
    AiActionTuple **p = m_tuples;
    unsigned i = 0;
    unsigned start = m_window;
    float bestR = 0.0f;

    m_window = start + 5;
    if (m_count) {
        do {
            AiActionTuple *t = *p;
            float r;

            t->updateIValueMod();
            if (i >= start && i < m_window)
                r = t->getRelevance(ai);
            else if (t->m_lastEnter >= 0)
                r = t->getRelevance(ai);
            else
                r = t->m_relevance;
            if (bestR < r) {
                bestR = r;
                best = t;
            }
            i++;
            p++;
        } while (i < m_count);
    }
    if (m_window >= m_count)
        m_window = 0;
    return best;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", runMaxPositive__12AiActionListR2Ai);
#endif
#ifdef NON_MATCHING
/* loop shape differs */
void AiActionList::clearIValues(void)
{
    unsigned n = m_count;
    AiActionTuple **p = m_tuples;

    while (n--)
        (*p++)->m_ivalue = 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", clearIValues__12AiActionList);
#endif
#ifdef NON_MATCHING
/* same as AiActionList::update */
AiActionTuple *AiThrashActionList::update(Ai &ai)
{
    return (this->*m_run)(ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", update__18AiThrashActionListR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiAction", __13AiActionGroup);
#ifdef NON_MATCHING
/* store order */
void AiActionGroup::init(void)
{
    m_count = 0;
    m_active = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", init__13AiActionGroup);
#endif
#ifdef NON_MATCHING
/* loop shape differs */
void AiActionGroup::reset(void)
{
    int n = m_count;
    AiActionList **p = m_lists;

    m_active = 0;
    while (n--)
        (*p++)->reset();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", reset__13AiActionGroup);
#endif
void AiActionGroup::addList(AiActionList &l)
{
    m_lists[m_count++] = &l;
}
void AiActionGroup::update(Ai &ai)
{
    int n = m_count;
    AiActionList **p = m_lists;

    while (n) {
        AiActionList *l = *p;
        AiVEntry *e = AI_VENT(l, 0x84, 8);
        AiActionTuple *t = ((AiActionTuple * (*)(void *, Ai &)) e->fn)((char *)l + e->delta, ai);

        if (t) {
            if (m_active != t) {
                if (m_active)
                    m_active->deactivate(ai);
                m_active = t;
            }
            t->execute(ai);
            return;
        }
        n--;
        p++;
    }
    if (m_active) {
        m_active->deactivate(ai);
        m_active = 0;
    }
}
#ifdef NON_MATCHING
/* loop shape differs */
void AiActionGroup::clearIValues(void)
{
    int n = m_count;
    AiActionList **p = m_lists;

    while (n--)
        (*p++)->clearIValues();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", clearIValues__13AiActionGroup);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiAction", _vt$12AiActionList);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", _vt$13AiActionTuple);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", __tf13AiActionTuple);
void enterAction__13AiActionTupleR2Ai(void *self) __asm__("enterAction__13AiActionTupleR2Ai");
void enterAction__13AiActionTupleR2Ai(void *self)
{
}
void exitAction__13AiActionTupleR2Ai(void *self) __asm__("exitAction__13AiActionTupleR2Ai");
void exitAction__13AiActionTupleR2Ai(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/AiAction", __tf12AiActionList);
#ifdef NON_MATCHING
/* retail list is virtual so the member-pointer call carries the vtable check; ours is non-virtual */
AiActionTuple *AiActionList::update(Ai &ai)
{
    return (this->*m_run)(ai);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiAction", update__12AiActionListR2Ai);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E75F8);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", __tf18AiThrashActionList);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7620);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7630);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7648);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7658);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7668);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7678);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7688);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7698);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E76A8);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E76B8);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E76C8);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E76D8);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E76E8);
INCLUDE_ASM("asm/nonmatchings/game/AiAction", D_006E7700);

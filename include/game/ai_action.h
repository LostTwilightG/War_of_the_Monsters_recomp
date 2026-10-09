#ifndef AI_ACTION_H
#define AI_ACTION_H

class Ai;
int timerGetFieldCount(void);

/* The AI decision system. An AiActionTuple is one behaviour (punch, block, throw ...) with an "interest" value (m_ivalue) that decays while the
   behaviour runs and recovers while it rests. AiActionLists pick one tuple per frame (first positive or maximum relevance); the AiActionGroup keeps
   the winner of the highest priority list running (enter/update/exit).
   All three are virtual in retail (vtable/ctor/__tf stay as asm), so they are declared without `virtual`: the tuple's overridable hooks are reached through
   the vtable by hand (vptr at 0x44 for tuples, 0x84 for lists), see AI_VCALL. Vtable entries are {this delta, 0, function}; the hooks are entries at
   +0x08 getEntryRelevance, +0x10 getExitRelevance, +0x18 enterAction, +0x20 updateAction, +0x28 exitAction. */
struct AiVEntry {
    short delta;
    short idx;
    void *fn;
};

#define AI_VENT(self, vptrOff, off) ((AiVEntry *)(*(char **)((char *)(self) + (vptrOff)) + (off)))

class AiActionTuple {
public:
    float m_weight;         /* 0x00: scale of the relevance */
    float m_ivalue;         /* 0x04: current interest, 0..1 (decays while active, recovers while idle) */
    float m_ivalueMin;      /* 0x08 */
    float m_ivalueMax;      /* 0x0C */
    float m_ivalueDrop;     /* 0x10: subtracted from the interest when the behaviour ends */
    float m_decayTime;      /* 0x14: seconds for the interest to fall to the minimum while active (<= 0: constant) */
    float m_recoverTime;    /* 0x18: seconds for the interest to climb back to the maximum while idle */
    float m_bias;           /* 0x1C: multiplier applied while the behaviour is running (hysteresis) */
    int m_lastEnter;        /* 0x20: field the behaviour started at, -1 when not running */
    int m_lastExit;         /* 0x24: field the behaviour ended at */
    int m_lastEval;         /* 0x28: field of the last entry-relevance evaluation */
    int m_maxFields;        /* 0x2C: longest run, in fields (3600) */
    int m_runs;             /* 0x30: how many times it started */
    float m_entryRel;       /* 0x34: last entry/exit relevance */
    float m_relevance;      /* 0x38: m_ivalue * m_weight * m_entryRel */
    int m_flags;            /* 0x3C: situations the behaviour can start in (1 = on the ground, 2 = ..., 3 = any) */
    const char *m_name;     /* 0x40 */
    void *m_vptr;           /* 0x44 */

    void init(float weight);
    void reset(void);
    void updateIValueMod(void);
    float getRelevance(Ai &ai);
    int getFieldsSinceEval(void);
    void execute(Ai &ai);
    void deactivate(Ai &ai);
};

class AiActionList {
public:
    enum Strategy { FIRST_POSITIVE, MAX_POSITIVE };
    AiActionTuple *(AiActionList::*m_run)(Ai &ai);  /* 0x00: member pointer to the selection function (8 bytes in gcc 2.95) */
    AiActionTuple *m_tuples[30];                    /* 0x08 */
    unsigned short m_count;                         /* 0x80 */
    unsigned short m_window;                        /* 0x82: runMaxPositive re-evaluates idle tuples [m_window, m_window + 5) each frame */
    void *m_vptr;                                   /* 0x84 */

    void init(Strategy s);
    void reset(void);
    void setStrategy(Strategy s);
    void addTuple(AiActionTuple &t);
    void addTuple(AiActionTuple &t, float weight);
    AiActionTuple *runFirstPositive(Ai &ai);
    AiActionTuple *runMaxPositive(Ai &ai);
    void clearIValues(void);
    AiActionTuple *update(Ai &ai);
};

class AiThrashActionList : public AiActionList {
public:
    AiActionTuple *update(Ai &ai);
};

class AiActionGroup {
public:
    AiActionTuple *m_active;    /* 0x00 */
    AiActionList *m_lists[10];  /* 0x04: highest priority first */
    int m_count;                /* 0x2C */

    void init(void);
    void reset(void);
    void addList(AiActionList &l);
    void update(Ai &ai);
    void clearIValues(void);
};

#endif

#ifndef AI_SUPPORT_H
#define AI_SUPPORT_H

/* Small helpers shared by the AI action TUs (AiReflex, AiSeek ...): raw field access into Monster/Ai, the AiNavigator view, the pad clip player. */

#define MI(m, o) (*(int *)((char *)(m) + (o)))
#define MF(m, o) (*(float *)((char *)(m) + (o)))
#define MB(m, o) (*(signed char *)((char *)(m) + (o)))
#define MP(m, o) ((char *)(m) + (o))
#define STATE_ID(m) (*(m)->m_state)
/* max.s / min.s without -ffast-math (which also rewrites every float compare) */
#define FMAX(a, b) ({ float _r; __asm__("max.s %0,%1,%2" : "=f"(_r) : "f"(a), "f"(b)); _r; })
#define FMIN(a, b) ({ float _r; __asm__("min.s %0,%1,%2" : "=f"(_r) : "f"(a), "f"(b)); _r; })
/* The AiNavigator is embedded in Ai at 0x80 */
#define NAV(ai) ((AiNavigator *)((char *)&(ai) + 0x80))
#define LEVEL_ID (*(int *)((char *)game + 0x1203D0))
#define MATCH_MODE (*(int *)((char *)game + 0x1203CC))

class _fvector;
class DbInteractive;
class GamePad;

class AiNavigator {
public:
    enum Status { STATUS_0, STATUS_1, STATUS_2, STATUS_3 };
    enum FailureHint { HINT_0 };
    char pad0[0x18];
    int status;     /* 0x18 */
    int mode;       /* 0x1C: 0 idle, 1 seeking/arriving, 2 arrived/in range */
    int substate;   /* 0x20 */
    void target(_fvector &p, float r);
    void seek(DbInteractive &d, float r);
    void seek(_fvector &p, float r);
    void arrive(_fvector &p, float a, float b, float c);
    void arrive(DbInteractive &it, float a, float b, float c);
    void tag(_fvector &p, float r);
    void disable(Status s, FailureHint h);
};
class StateButtSlam {
public:
    int transitionFeasible(void);
};
class HealthMeter {
public:
    float getMaxLevel(void);
};
class GamePadClipPlayer {
public:
    int clip;
    int update(GamePad &pad);
    void rewind(void);
};
class AiPadClips {
public:
    static int getThrow(void) __asm__("getThrow__10AiPadClipsv");
    static int getDash(void) __asm__("getDash__10AiPadClipsv");
};

extern float LOOK_AHEAD_T;
__asm__("#SNFIX_SMALL LOOK_AHEAD_T");
extern int s_attackOtherAi __asm__("_2Ai$s_attackOtherAi");
__asm__("#SNFIX_SMALL _2Ai$s_attackOtherAi");
extern int s_ccAi __asm__("_2Ai$s_ccAi");
__asm__("#SNFIX_SMALL _2Ai$s_ccAi");

#endif

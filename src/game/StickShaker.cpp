#include "common.h"
#include "game/hit_history.h"
#include "task_manager.h"
#include "hieri_types.h"
#include "game/game.h"
#include "vecmath.h"

extern "C" float sqrtf(float);

struct ActuatorData {
    int strength;
    unsigned char mode;
    int c;
    int d;
};

void inputSetActuator(int pad, ActuatorData *a);

struct StickShakerConfig;

/* Layout of the retail config: ring count, 20 rings of two actuators, 20 reach thresholds, time scale. */
template <class T>
class ExpandingRing {
public:
    int numRings;
    char pad4[0x20];
    ActuatorData rings[20][2];
    float reach[20];
    float timeScale;
};

class StickShaker {
public:
    int active;
    unsigned time;
    char pad8[8];
    _fvector pos;
    ExpandingRing<StickShakerConfig> cfg;
    HitHistory hits;
    char pad320[0x360 - 0x320];

    static StickShaker m_shakePool[20];

    static void DefaultSetup(ExpandingRing<StickShakerConfig> &c);
    static void Init(void);
    void Activate(_fvector *p, ExpandingRing<StickShakerConfig> c, bool b);
    int update(void);
    static unsigned update(void *p);
};

#ifdef NON_MATCHING
/* 60/60 words identical; gas pads the first loop head with 2 nops (8 bytes), retail does not: untuned */
void StickShaker::DefaultSetup(ExpandingRing<StickShakerConfig> &c)
{
    int i, j;

    c.numRings = 10;
    for (i = 0; i < 10; i++) {
        float f;

        c.reach[i] = (float)(i + 1) * 50.0f;
        f = 1.0f - (float)i * 0.1f;
        for (j = 0; j < 2; j++) {
            ActuatorData &a = c.rings[i][j];

            a.strength = (int)(f * 60.0f);
            a.mode = (j == 0) ? 1 : (int)(f * 255.0f);
            a.c = 0;
            a.d = 0;
        }
    }
    c.timeScale = 54.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/StickShaker", DefaultSetup__11StickShakerRt13ExpandingRing1Z17StickShakerConfig);
#endif
void StickShaker::Init(void)
{
    int i;
    StickShaker *p = m_shakePool;

    for (i = 7; i >= 0; i--, p++)
        p->active = 0;
}
#ifdef NON_MATCHING
/* 65/145 words, untuned */
void StickShaker::Activate(_fvector *p, ExpandingRing<StickShakerConfig> c, bool b)
{
    StickShaker *s = m_shakePool;
    int i;

    for (i = 0; i < 20 && s->active; i++)
        s++;
    if (i == 20)
        return;
    s->active = 1;
    s->time = 0;
    s->pos = *p;
    s->cfg = c;
    s->hits.reset();
    TaskManager::global.add(update, s, 1);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/StickShaker", Activate__11StickShakerP8_fvectorGt13ExpandingRing1Z17StickShakerConfigb);
#endif
#ifdef NON_MATCHING
/* 16/142 words, untuned: retail inlines the dot product/sqrt as VU asm */
int StickShaker::update(void)
{
    float t, d;
    int i;

    time += timerGetFieldsLastFrame();
    t = cfg.timeScale * (float)time;
    if (!(t <= cfg.reach[cfg.numRings - 1])) {
        active = 0;
        return 0;
    }
    for (i = 0; i < game->m_numMonsters; i++) {
        _fvector diff;
        char *mon = (char *)game->m_monsters[i];

        vecSub(&diff, &pos, (_fvector *)(*(char **)(mon + 0xC) + 0x10));
        d = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
        if (d <= t && hits.newHit(*(int *)(mon + 0x20), false)) {
            ActuatorData *a;
            int k;

            if (cfg.reach[cfg.numRings - 1] < d) {
                a = &cfg.rings[0][0];
            } else {
                for (k = 0; k < cfg.numRings && !(d < cfg.reach[k]); k++)
                    ;
                a = &cfg.rings[k][0];
            }
            inputSetActuator(i, a);
        }
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/StickShaker", update__11StickShaker);
#endif
INCLUDE_ASM("asm/nonmatchings/game/StickShaker", __static_initialization_and_destruction_0_001CF398);
unsigned StickShaker::update(void *p)
{
    return ((StickShaker *)p)->update();
}
INCLUDE_ASM("asm/nonmatchings/game/StickShaker", _GLOBAL_$I$DefaultSetup__11StickShakerRt13ExpandingRing1Z17StickShakerConfig);

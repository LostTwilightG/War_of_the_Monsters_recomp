#include "common.h"
#include "game/stamina_meter.h"
#include "game/game.h"

class HealthMeter {
public:
    float max;
    float cur;
    float recent[3];
    int idx;

    HealthMeter();
    void init(void);
    void reset(void);
    void creditFull(void);
    void credit(float amount);
    void drain(float amount);
};

extern float gLowStaminaThreshold;

HealthMeter::HealthMeter()
{
    init();
}
void HealthMeter::init(void)
{
    max = 100.0f;
}
void HealthMeter::reset(void)
{
    int i;

    cur = max;
    for (i = 2; i >= 0; i--)
        recent[i] = 0;
    idx = 0;
}
void HealthMeter::creditFull(void)
{
    credit(max);
}
void HealthMeter::credit(float amount)
{
    cur += amount * (max * 0.01f);
    if (max < cur)
        cur = max;
}
void HealthMeter::drain(float amount)
{
    if (--idx < 0)
        idx = 2;
    recent[idx] = amount;
    cur -= amount;
    if (cur < 0)
        cur = 0;
}
StaminaMeter::StaminaMeter()
{
    init();
}
#ifdef NON_MATCHING
/* 10/18 words, untuned: store order */
void StaminaMeter::init(void)
{
    enabled = 1;
    max = 100.0f;
    regenRate = 0.08f;
    maxLevel = 130.0f;
    fC = 0.5f;
    exhaustTime = 360;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", init__12StaminaMeter);
#endif
void StaminaMeter::reset(void)
{
    cur = max;
    exhausted = 0;
    exhaustTimer = 0;
}
void StaminaMeter::update(int dt)
{
    if (exhausted) {
        exhaustTimer -= dt;
        if (exhaustTimer <= 0) {
            exhausted = 0;
            recoverPool = 0;
            cur = gLowStaminaThreshold * max;
        }
    }
    if (enabled)
        creditBaseOnly((float)dt * regenRate);
}
void StaminaMeter::creditFull(void)
{
    credit(maxLevel);
}
#ifdef NON_MATCHING
/* 23/47 words, untuned: branch layout */
void StaminaMeter::creditBaseOnly(float amount)
{
    if (cur < max) {
        if (!exhausted) {
            float room = max - cur;

            if (amount < room)
                room = amount;
            cur += room;
            return;
        } else {
            float room = max - recoverPool;

            if (amount < room)
                room = amount;
            recoverPool += room;
            if (gLowStaminaThreshold * max <= recoverPool) {
                cur = recoverPool;
                recoverPool = 0;
                exhausted = 0;
                exhaustTimer = 0;
            } else {
                float t = (float)exhaustTime;

                exhaustTimer -= (int)(t * (room / t));
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", creditBaseOnly__12StaminaMeterf);
#endif
#ifdef NON_MATCHING
/* 13/48 words, untuned: duplicate of creditBaseOnly in retail */
void StaminaMeter::credit(float amount)
{
    if (cur < maxLevel) {
        if (!exhausted) {
            float room = maxLevel - cur;

            if (amount < room)
                room = amount;
            cur += room;
            return;
        } else {
            float room = max - recoverPool;

            if (amount < room)
                room = amount;
            recoverPool += room;
            if (gLowStaminaThreshold * max <= recoverPool) {
                cur = recoverPool;
                recoverPool = 0;
                exhausted = 0;
                exhaustTimer = 0;
            } else {
                float t = (float)exhaustTime;

                exhaustTimer -= (int)(t * (room / t));
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", credit__12StaminaMeterf);
#endif
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", drain__12StaminaMeterfbT2);
int StaminaMeter::hasEnough(float amount)
{
    if (amount <= cur)
        return 1;
    if (exhausted)
        return 0;
    exhaustTimer = exhaustTime;
    exhausted = 1;
    cur = 0;
    if (game->m_gameMode == 1 && game->m_levelId == 1 && *(int *)(owner + 0x18) == 1)
        gameHud(0)->addTextBoxMessage(0x19);
    return 1;
}

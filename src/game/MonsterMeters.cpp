#include "common.h"

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

class StaminaMeter {
public:
    float max;
    float f4;
    float f8;
    float fC;
    float f10;
    int f14;
    int f18;
    int f1C;
    int f20;

    StaminaMeter();
    void init(void);
    void reset(void);
    void creditFull(void);
    void credit(float amount);
};

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
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", init__12StaminaMeter);
void StaminaMeter::reset(void)
{
    f10 = max;
    f20 = 0;
    f18 = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", update__12StaminaMeteri);
void StaminaMeter::creditFull(void)
{
    credit(f8);
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", creditBaseOnly__12StaminaMeterf);
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", credit__12StaminaMeterf);
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", drain__12StaminaMeterfbT2);
INCLUDE_ASM("asm/nonmatchings/game/MonsterMeters", hasEnough__12StaminaMeterf);

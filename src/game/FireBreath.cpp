#include "common.h"

struct Monster;
void particleKillFx(int &);

int timerGetFieldsLastFrame(void);

class MonsterSound {
public:
    void updateFireBreath(void);
};

class StateFireBreath {
public:
    void handleApplyMint(void);
};

class FireBreath {
public:
    int state;
    Monster *owner;
    char pad8[0x8C - 8];
    int fx0;
    int fx1;
    int fx2;
    float timeLeft;

    FireBreath(Monster &m);
    void Init(void);
    unsigned Update(void);
    void ApplyMint(void);
    int TestCollis(void);
    static unsigned Update(void *p);
};

FireBreath::FireBreath(Monster &m)
{
    owner = &m;
    state = 0;
    fx0 = -1;
    fx1 = -1;
    fx2 = -1;
}
void FireBreath::Init(void)
{
    state = 0;
    if (fx0 != -1)
        ApplyMint();
}
INCLUDE_ASM("asm/nonmatchings/game/FireBreath", Activate__10FireBreathfffffff);
unsigned FireBreath::Update(void)
{
    unsigned r = 0;

    if (state) {
        timeLeft -= (float)timerGetFieldsLastFrame();
        if (timeLeft > 0.0f) {
            TestCollis();
            r = 1;
        } else {
            ApplyMint();
            state = 0;
        }
    }
    ((MonsterSound *)((char *)owner + 0x1A7C))->updateFireBreath();
    return r;
}
void FireBreath::ApplyMint(void)
{
    ((StateFireBreath *)((char *)owner + 0xF974))->handleApplyMint();
    particleKillFx(fx0);
    particleKillFx(fx1);
    particleKillFx(fx2);
}
INCLUDE_ASM("asm/nonmatchings/game/FireBreath", TestCollis__10FireBreath);
unsigned FireBreath::Update(void *p)
{
    return ((FireBreath *)p)->Update();
}

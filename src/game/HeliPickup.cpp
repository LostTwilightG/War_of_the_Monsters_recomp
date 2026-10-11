#include "common.h"
#include "engine.h"
#include "game/pickup.h"
#include "game/pickup_fx.h"
#include "cs_pool.h"

class HeliSound {
public:
    void terminateHeliSound(void);
};

class HeliPickup : public Pickup {
public:
    enum State { STATE_0, STATE_1, STATE_2, STATE_3, STATE_4, STATE_5 };

    char padE0[0x1C0 - 0xE0];
    _fvector vel;     /* 0x1C0 */
    char pad1D0[0x2E0 - 0x1D0];
    int fx;           /* 0x2E0 */

    _fvector *getVel(void);
    void drop(void);
    void grab(int i);
    void enterState(State s);
    void regen(void);
    void kill(void);
};

INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", __10HeliPickupb);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", initAfterDbLoad__10HeliPickup);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", takeHit__10HeliPickupP8_fvectorfi);
void HeliPickup::grab(int i)
{
    Pickup::grab(i);
    particleKillFx(fx);
    enterState(STATE_5);
}
void HeliPickup::drop(void)
{
    heldState = 0;
    Pickup::setVisualState(0);
    Pickup::hatCheck();
    hdReparentCsGrid(cs);
}
void HeliPickup::kill(void)
{
    cs->drawMe = 0;
    cs->testCollision = 0;
    particleCreateFx(&cs->trans, 0xB, 8.0f, 0, 0.0f);
    PICKUP_SOUNDS()->playDestructibleSound(0x65, &cs->trans);
    ((HeliSound *)((char *)this + 0x2E4))->terminateHeliSound();
    particleKillFx(fx);
    flags &= 0xFFFD;
    if ((bits & 1) == 0) {
        CsPool::csDeactivate(cs);
        cs = 0;
    }
}
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", update__10HeliPickup);
void HeliPickup::regen(void)
{
    Pickup::regen();
    cs->testCollision = 1;
    *(int *)((char *)this + 0x178) = 0;
    *(int *)((char *)this + 0x184) = 0;
    *(int *)((char *)this + 0x17C) = 0;
    fx = -1;
    enterState(STATE_0);
}
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", updateAttackBehavior__10HeliPickupR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", enterState__10HeliPickupQ210HeliPickup5State);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", _vt$10HeliPickup);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", func_0013F610);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", func_0013F620);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", __tf10HeliPickup);
_fvector *HeliPickup::getVel(void)
{
    return &vel;
}

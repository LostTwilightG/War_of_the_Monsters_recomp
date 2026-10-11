#include "common.h"
#include "engine.h"
#include "game/pickup.h"
#include "game/pickup_fx.h"
#include "cs_pool.h"

class GenericSound {
public:
    void terminateGenericSound(void);
};

class UfoPickup : public Pickup {
public:
    enum State { STATE_0, STATE_1, STATE_2, STATE_3, STATE_4, STATE_5, STATE_6, STATE_7 };

    char padE0[0x1E0 - 0xE0];
    _fvector vel;     /* 0x1E0 */

    _fvector *getVel(void);
    void drop(void);
    void grab(int i);
    void enterState(State s);
    void kill(void);
    void regen(void);
};

INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", __9UfoPickup);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", initAfterDbLoad__9UfoPickup);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", takeHit__9UfoPickupP8_fvectorfi);
void UfoPickup::grab(int i)
{
    Pickup::grab(i);
    enterState(STATE_7);
}
void UfoPickup::drop(void)
{
    heldState = 0;
    Pickup::setVisualState(0);
    Pickup::hatCheck();
    hdReparentCsGrid(cs);
}
void UfoPickup::kill(void)
{
    int f210 = *(int *)((char *)this + 0x210);

    cs->drawMe = 0;
    cs->testCollision = 0;
    particleCreateFx(&cs->trans, 0xB, 12.0f, 0, 0.0f);
    PICKUP_SOUNDS()->playDestructibleSound(0x66, &cs->trans);
    ((GenericSound *)((char *)this + 0x21C))->terminateGenericSound();
    flags &= 0xFFFD;
    if ((bits & 1) == 0 && *(int *)((char *)this + 0x210) == 0) {
        CsPool::csDeactivate(cs);
        cs = 0;
    }
    if (*(int *)((char *)this + 0x210) != 0)
        regenTimer = 1;
}
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", update__9UfoPickup);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", fire__9UfoPickupR8_fvector);
void UfoPickup::regen(void)
{
    Pickup::regen();
    cs->testCollision = 1;
    *(int *)((char *)this + 0x1F0) = 0;
    *(int *)((char *)this + 0x1F8) = 0;
    *(int *)((char *)this + 0x1F4) = 0;
    if (*(int *)((char *)this + 0x210) != 0) {
        *(int *)((char *)this + 0x214) = 0;
        enterState(STATE_5);
        cs->trans.z = 3000.0f;
        return;
    }
    enterState(STATE_0);
}
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", updateAttackBehavior__9UfoPickup);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", enterState__9UfoPickupQ29UfoPickup5State);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", _vt$9UfoPickup);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", func_001D9348);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", func_001D9358);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", __tf9UfoPickup);
_fvector *UfoPickup::getVel(void)
{
    return &vel;
}

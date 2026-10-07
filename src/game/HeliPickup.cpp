#include "common.h"
#include "engine.h"
#include "game/pickup.h"

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
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", kill__10HeliPickup);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", update__10HeliPickup);
INCLUDE_ASM("asm/nonmatchings/game/HeliPickup", regen__10HeliPickup);
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

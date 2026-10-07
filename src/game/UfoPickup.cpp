#include "common.h"
#include "engine.h"
#include "game/pickup.h"

class UfoPickup : public Pickup {
public:
    enum State { STATE_0, STATE_1, STATE_2, STATE_3, STATE_4, STATE_5, STATE_6, STATE_7 };

    char padE0[0x1E0 - 0xE0];
    _fvector vel;     /* 0x1E0 */

    _fvector *getVel(void);
    void drop(void);
    void grab(int i);
    void enterState(State s);
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
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", kill__9UfoPickup);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", update__9UfoPickup);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", fire__9UfoPickupR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/UfoPickup", regen__9UfoPickup);
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

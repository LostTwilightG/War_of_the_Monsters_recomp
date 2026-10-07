#include "common.h"
#include "engine.h"
#include "game/military_pickup.h"

class GamePad;

class MissileTruckPickup : public MilitaryPickup {
public:
    char pad190[0x290 - 0x190];
    int f290;

    void takeHit(_fvector *pos, float dmg, int x);
    void drop(void);
    void updateFollowBehavior(GamePad &pad);
    void updateAttackBehavior(GamePad &pad);
    void enterState(MilitaryPickup::State s);
    void resignFormation(void);
};

INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", __18MissileTruckPickup);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", initAfterDbLoad__18MissileTruckPickup);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", setTrans__18MissileTruckPickupR8_fvector);
void MissileTruckPickup::takeHit(_fvector *pos, float dmg, int x)
{
    if (dmg <= 0.1f)
        return;
    health -= dmg;
    if (health <= 0.0f)
        flags &= 0xFFF7;
}
void MissileTruckPickup::drop(void)
{
    heldState = 0;
    Pickup::setVisualState(0);
    Pickup::hatCheck();
    hdReparentCsGrid(cs);
}
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", kill__18MissileTruckPickup);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", fire__18MissileTruckPickupR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", update__18MissileTruckPickup);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", updateLeadBehavior__18MissileTruckPickupR7GamePad);
void MissileTruckPickup::updateFollowBehavior(GamePad &pad)
{
    updateAttackBehavior(pad);
}
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", updateAttackBehavior__18MissileTruckPickupR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", enterState__18MissileTruckPickupQ214MilitaryPickup5State);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", _vt$18MissileTruckPickup);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", leadFormation__18MissileTruckPickup);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", followFormation__18MissileTruckPickup);
void MissileTruckPickup::resignFormation(void)
{
    f290 = 0;
    enterState(STATE_0);
}
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", func_0014E508);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", func_0014E518);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", __tf18MissileTruckPickup);

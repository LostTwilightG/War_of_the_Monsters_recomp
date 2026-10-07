#include "common.h"
#include "engine.h"
#include "game/military_pickup.h"

class GamePad;

class TankPickup : public MilitaryPickup {
public:
    char pad190[0x2A0 - 0x190];
    int f2A0;

    void takeHit(_fvector *pos, float dmg, int x);
    void drop(void);
    void updateFollowBehavior(GamePad &pad);
    void updateAttackBehavior(GamePad &pad);
    void enterState(MilitaryPickup::State s);
    void resignFormation(void);
};

INCLUDE_ASM("asm/nonmatchings/game/TankPickup", __10TankPickup);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", initAfterDbLoad__10TankPickup);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", setTrans__10TankPickupR8_fvector);
void TankPickup::takeHit(_fvector *pos, float dmg, int x)
{
    if (dmg <= 0.1f)
        return;
    health -= dmg;
    if (health <= 0.0f)
        flags &= 0xFFF7;
}
void TankPickup::drop(void)
{
    heldState = 0;
    Pickup::setVisualState(0);
    Pickup::hatCheck();
    hdReparentCsGrid(cs);
}
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", kill__10TankPickup);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", fire__10TankPickupR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", update__10TankPickup);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", updateLeadBehavior__10TankPickupR7GamePad);
void TankPickup::updateFollowBehavior(GamePad &pad)
{
    updateAttackBehavior(pad);
}
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", updateAttackBehavior__10TankPickupR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", enterState__10TankPickupQ214MilitaryPickup5State);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", _vt$10TankPickup);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", leadFormation__10TankPickup);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", followFormation__10TankPickup);
void TankPickup::resignFormation(void)
{
    f2A0 = 0;
    enterState(STATE_0);
}
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", func_001D0398);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", func_001D03A8);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", __tf10TankPickup);

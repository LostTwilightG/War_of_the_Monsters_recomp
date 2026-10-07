#include "common.h"
#include "engine.h"
#include "game/military_pickup.h"
#include "game/vehicle_navigator.h"

class GamePad;

class TankPickup : public MilitaryPickup {
public:
    char pad190[0x1C8 - 0x190];
    float speed;               /* 0x1C8 */
    char pad1CC[0x210 - 0x1CC];
    VehicleNavigator nav;      /* 0x210 */
    char pad2A0[0x2A0 - 0x2A0];
    int mode;                  /* 0x2A0 */

    void leadFormation(void);
    void followFormation(void);
    void setTrans(_fvector &p);
    void takeHit(_fvector *pos, float dmg, int x);
    void drop(void);
    void updateFollowBehavior(GamePad &pad);
    void updateAttackBehavior(GamePad &pad);
    void enterState(MilitaryPickup::State s);
    void resignFormation(void);
};

INCLUDE_ASM("asm/nonmatchings/game/TankPickup", __10TankPickup);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", initAfterDbLoad__10TankPickup);
void TankPickup::setTrans(_fvector &p)
{
    register int t __asm__("$2");

    __asm__ volatile("lq %1, 0(%2)
	sq %1, %0" : "=m"(cs->trans), "=r"(t) : "r"(&p));
    hdReparentCsGrid(cs);
    nav.init();
    enterState((MilitaryPickup::State)state);
}
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
#ifdef NON_MATCHING
/* 16/18 words, untuned: store scheduling */
void TankPickup::leadFormation(void)
{
    VehicleNavigator *n = &nav;

    speed = 88.0f;
    n->f14 = 0.5f;
    n->f10 = 0.5f;
    mode = 1;
    enterState(STATE_0);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", leadFormation__10TankPickup);
#endif
void TankPickup::followFormation(void)
{
    VehicleNavigator *n = &nav;

    speed = 146.66667f;
    n->f14 = 1.0f;
    n->f10 = 0;
    mode = 2;
    enterState(STATE_2);
}
void TankPickup::resignFormation(void)
{
    mode = 0;
    enterState(STATE_0);
}
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", func_001D0398);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", func_001D03A8);
INCLUDE_ASM("asm/nonmatchings/game/TankPickup", __tf10TankPickup);

#include "common.h"
#include "engine.h"
#include "game/military_pickup.h"
#include "game/vehicle_navigator.h"
#include "game/pickup_fx.h"

class GamePad;

class MissileTruckPickup : public MilitaryPickup {
public:
    char pad190[0x1B8 - 0x190];
    float speed;               /* 0x1B8 */
    char pad1BC[0x200 - 0x1BC];
    VehicleNavigator nav;      /* 0x200 */
    char pad290[0x290 - 0x290];
    int mode;                  /* 0x290 */

    void leadFormation(void);
    void followFormation(void);
    void setTrans(_fvector &p);
    void kill(void);
    void takeHit(_fvector *pos, float dmg, int x);
    void drop(void);
    void updateFollowBehavior(GamePad &pad);
    void updateAttackBehavior(GamePad &pad);
    void enterState(MilitaryPickup::State s);
    void resignFormation(void);
};

INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", __18MissileTruckPickup);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", initAfterDbLoad__18MissileTruckPickup);
void MissileTruckPickup::setTrans(_fvector &p)
{
    register int t __asm__("$2");

    __asm__ volatile("lq %1, 0(%2)
	sq %1, %0" : "=m"(cs->trans), "=r"(t) : "r"(&p));
    hdReparentCsGrid(cs);
    nav.init();
    enterState((MilitaryPickup::State)state);
}
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
void MissileTruckPickup::kill(void)
{
    cs->drawMe = 0;
    cs->testCollision = 0;
    particleCreateFx(&cs->trans, 0xB, 8.0f, 0, 0.0f);
    PICKUP_SOUNDS()->playDestructibleSound(0x65, &cs->trans);
    MilitaryPickup::kill();
}
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
#ifdef NON_MATCHING
/* 16/18 words, untuned: store scheduling */
void MissileTruckPickup::leadFormation(void)
{
    VehicleNavigator *n = &nav;

    speed = 88.0f;
    n->f14 = 0.5f;
    n->f10 = 0.5f;
    mode = 1;
    enterState(STATE_0);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", leadFormation__18MissileTruckPickup);
#endif
void MissileTruckPickup::followFormation(void)
{
    VehicleNavigator *n = &nav;

    speed = 146.66667f;
    n->f14 = 1.0f;
    n->f10 = 0;
    mode = 2;
    enterState(STATE_2);
}
void MissileTruckPickup::resignFormation(void)
{
    mode = 0;
    enterState(STATE_0);
}
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", func_0014E508);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", func_0014E518);
INCLUDE_ASM("asm/nonmatchings/game/MissileTruckPickup", __tf18MissileTruckPickup);

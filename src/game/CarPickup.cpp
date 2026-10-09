#include "common.h"
#include "game/pickup.h"
#include "hieri_types.h"
#include "engine.h"
#include "game/vehicle_navigator.h"

/* CarPickup is a virtual class (vtable, ctor and __tf stay as asm). The members below are written without the
   `virtual` keyword so that this file does not emit a second vtable; fields are reached by raw offset. */

class CarSound {
public:
    void terminateCarSound(void);
};

class CarPickup : public Pickup {
public:
    void takeHit(_fvector *pos, float dmg, int x);
    void kill(void);
    void grab(int i);
    void drop(void);
    char *getVehicle(void);
    void regen(void);
};

INCLUDE_ASM("asm/nonmatchings/game/CarPickup", _vt$9CarPickup);
INCLUDE_ASM("asm/nonmatchings/game/CarPickup", __9CarPickup);
INCLUDE_ASM("asm/nonmatchings/game/CarPickup", initAfterDbLoad__9CarPickup);
void CarPickup::takeHit(_fvector *pos, float dmg, int x)
{
    health -= dmg;
}
void CarPickup::kill(void)
{
    cs->drawMe = 0;
    cs->testCollision = 0;
    ((CarSound *)((char *)this + 0x170))->terminateCarSound();
    Pickup::kill();
}
void CarPickup::grab(int i)
{
    Pickup::grab(i);
    *(int *)((char *)this + 0x260) = 0;
    ((CarSound *)((char *)this + 0x170))->terminateCarSound();
}
void CarPickup::drop(void)
{
    heldState = 0;
    Pickup::setVisualState(0);
    Pickup::hatCheck();
    hdReparentCsGrid(cs);
}
INCLUDE_ASM("asm/nonmatchings/game/CarPickup", update__9CarPickup);
void CarPickup::regen(void)
{
    Pickup::regen();
    cs->testCollision = 1;
    ((VehicleNavigator *)((char *)this + 0x1D0))->init();
    *(int *)((char *)this + 0x260) = 1;
}
INCLUDE_ASM("asm/nonmatchings/game/CarPickup", updateInputs__9CarPickupR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/CarPickup", func_001228C8);
INCLUDE_ASM("asm/nonmatchings/game/CarPickup", func_001228D8);
INCLUDE_ASM("asm/nonmatchings/game/CarPickup", __tf9CarPickup);
char *CarPickup::getVehicle(void)
{
    return (char *)this + 0x180;
}

#include "common.h"
#include "hieri_types.h"
#include "engine.h"

int getHatField(void);

/* Vehicle layout is only partly known; the class is virtual in retail (vtable/ctor stay asm), so the members
   below are declared non-virtual and fields are plain members at their retail offsets. */
class Vehicle {
public:
    _cs *cs;
    char pad4[0x1C];
    float throttle;
    char pad24[4];
    float throttleMax;
    char pad2C[8];
    int hatField;
    char pad38[8];
    QwData orient;
    float f4C;

    void setCs(_cs *c);
    void setPos(_fvector &p);
    void updateThrottle(float input);
};

INCLUDE_ASM("asm/nonmatchings/game/Vehicle", getHatField__Fv);
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", __7Vehicle);
#ifdef NON_MATCHING
/* 7/20 words, untuned */
void Vehicle::setCs(_cs *c)
{
    hatField = getHatField();
    cs = c;
    orient = *(QwData *)((char *)c + 0x40);
    f4C = c->trans.z;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", setCs__7VehicleP3_cs);
#endif
#ifdef NON_MATCHING
/* 0/8 words, untuned: retail copies the vector with one lq/sq */
void Vehicle::setPos(_fvector &p)
{
    *(QwData *)&cs->trans = *(QwData *)&p;
    f4C = p.z;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", setPos__7VehicleR8_fvector);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", updateControls__7VehicleR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", updatePosition__7Vehicle);
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", updateCollision__7Vehicle);
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", updateSteering__7Vehiclef);
#ifdef NON_MATCHING
/* 1/27 words, untuned: constant materialisation order */
void Vehicle::updateThrottle(float input)
{
    float target = input * throttleMax;
    float rate = 0.1f;

    if (throttle < target)
        rate = 0.04f;
    throttle = smoothEasyInTC(throttle, target, rate, 0.01f);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", updateThrottle__7Vehiclef);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Vehicle", orientUpToNormal__7VehicleR8_fvectorf);

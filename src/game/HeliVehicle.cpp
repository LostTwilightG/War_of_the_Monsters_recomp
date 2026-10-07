#include "common.h"
#include "hieri_types.h"
#include "vecmath.h"
#include "game/game.h"

int timerGetUpdateRate(void);
void mathfNormalizeQuaternion(_fvector *dst, _fvector *src);
void mathfQuaternionToMatrix4x4(float (*m)[4], _fvector *q);
void hdReparentCsGrid(_cs *cs);

extern float s_drag;
extern float s_elevatorThrust;
__asm__("#SNFIX_SMALL s_drag");
__asm__("#SNFIX_SMALL s_elevatorThrust");

float smoothEasyInTC(float cur, float target, float rate, float eps);
void mathfRotAxisToQuaternion(_fvector *dst, _fvector *axis, float angle);
void mathfConcatQuaternions(_fvector *dst, _fvector *a, _fvector *b);
extern "C" float fabsf(float);

class PointMass {
public:
    _fvector *pos;
    char pad[12];
    _fvector vel;
    _fvector force;

    void init(float mass, _fvector *p);
    void resolveForcesEuler(float dt);
};

class HeliVehicle {
public:
    _cs *cs;
    char pad4[0xC];
    _fvector quat;
    PointMass pm;
    float headingRate;
    char pad54[8];
    float turnRate;

    void setCs(_cs *c);
    void updatePosition(void);
    void updateCollision(void);
    void updateElevator(float f);
    void updateHeading(float f);
};

INCLUDE_ASM("asm/nonmatchings/game/HeliVehicle", __11HeliVehicle);
void HeliVehicle::setCs(_cs *c)
{
    cs = c;
    pm.init(1.0f, &c->trans);
}
INCLUDE_ASM("asm/nonmatchings/game/HeliVehicle", setPos__11HeliVehicleR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/HeliVehicle", updateControls__11HeliVehicleR7GamePad);
#ifdef NON_MATCHING
/* 8/47 words, untuned: register use for pm/force pointers */
void HeliVehicle::updatePosition(void)
{
    _fvector tmp;
    PointMass *p = &pm;
    _fvector *fp = &pm.force;
    float rate;

    vecScale(&tmp, &pm.vel, -s_drag);
    vecAdd(&pm.force, fp, &tmp);
    rate = 1.0f / (float)timerGetUpdateRate();
    p->force.z += *(float *)((char *)game + 0x1203C4);
    p->resolveForcesEuler(rate);
    pm.force.x = 0.0f;
    fp->z = 0.0f;
    fp->y = 0.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/HeliVehicle", updatePosition__11HeliVehicle);
#endif
void HeliVehicle::updateCollision(void)
{
    mathfNormalizeQuaternion(&quat, &quat);
    mathfQuaternionToMatrix4x4((float (*)[4])((char *)cs + 0x20), &quat);
    hdReparentCsGrid(cs);
}
#ifdef NON_MATCHING
/* 22/24 words, untuned: add operand order */
void HeliVehicle::updateElevator(float f)
{
    _fvector tmp;
    float t = f * s_elevatorThrust;

    t = -*(float *)((char *)game + 0x1203C4) + t;
    vecScale(&tmp, (_fvector *)((char *)cs + 0x40), t);
    vecAdd(&pm.force, &pm.force, &tmp);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/HeliVehicle", updateElevator__11HeliVehiclef);
#endif
#ifdef NON_MATCHING
/* 3/49 words, untuned: prologue/abs scheduling */
void HeliVehicle::updateHeading(float f)
{
    _fvector q;

    if (fabsf(f) < 1e-10f) {
        headingRate = 0.0f;
    } else {
        smoothEasyInTC(headingRate, f, 0.01f, 0.001f);
        f *= -turnRate / (float)timerGetUpdateRate();
        mathfRotAxisToQuaternion(&q, (_fvector *)((char *)cs + 0x40), f);
        mathfConcatQuaternions(&quat, &quat, &q);
        headingRate = f;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/HeliVehicle", updateHeading__11HeliVehiclef);
#endif
INCLUDE_ASM("asm/nonmatchings/game/HeliVehicle", updateTilt__11HeliVehicleff);
INCLUDE_ASM("asm/nonmatchings/game/HeliVehicle", orientUpToNormal__11HeliVehicleR8_fvectorf);

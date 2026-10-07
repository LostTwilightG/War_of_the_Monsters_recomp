#include "common.h"
#include "hieri_types.h"
#include "vecmath.h"

extern "C" float sqrtf(float);

class PointMass {
public:
    _fvector *pos;
    char pad[12];
    FVECTOR vel;
    FVECTOR force;

    void init(float mass, _fvector *p);
    void applyImpulse(_fvector &impulse);
    void applyDampeningForce(float d);
    void clampForce(float maxForce);
    void resolveForcesEuler(float dt);
};

class Spring {
public:
    float restLength;
    float stiffness;
    float damping;
    PointMass *a;
    PointMass *b;

    void init(float rest, float k, float damp);
    void applyForces(void);
};

#ifdef NON_MATCHING
/* 2/14 words: retail zeroes vel/force through derived pointers in a different order */
void PointMass::init(float mass, _fvector *p)
{
    pos = p;
    vel.x = 0;
    vel.y = 0;
    vel.z = 0;
    force.x = 0;
    force.y = 0;
    force.z = 0;
    force.w = 1.0f / mass;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/PointMass", init__9PointMassfP8_fvector);
#endif
void PointMass::applyImpulse(_fvector &impulse)
{
    _fvector tmp;

    vecScale(&tmp, &impulse, force.w);
    vecAdd(&vel, &vel, &tmp);
}
void PointMass::applyDampeningForce(float d)
{
    _fvector tmp;

    vecScale(&tmp, &vel, -d);
    vecAdd(&force, &force, &tmp);
}
INCLUDE_ASM("asm/nonmatchings/common/PointMass", clampForce__9PointMassf);
#ifdef NON_MATCHING
/* 18/26 words: pointer temporaries for &force/&vel get swapped registers */
void PointMass::resolveForcesEuler(float dt)
{
    _fvector tmp;

    vecScale(&tmp, &force, dt * force.w);
    vecAdd(&vel, &vel, &tmp);
    vecScale(&tmp, &vel, dt);
    vecAdd(pos, pos, &tmp);
}
#else
INCLUDE_ASM("asm/nonmatchings/common/PointMass", resolveForcesEuler__9PointMassf);
#endif
void Spring::init(float rest, float k, float damp)
{
    restLength = rest;
    stiffness = k;
    damping = damp;
    b = 0;
    a = 0;
}
INCLUDE_ASM("asm/nonmatchings/common/PointMass", applyForces__6Spring);

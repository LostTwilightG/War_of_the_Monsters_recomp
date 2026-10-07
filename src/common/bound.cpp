#include "common.h"
#include "hieri_types.h"

void boundEulerAngle(float *p);

void boundEulerAngles(_fvector *v)
{
    boundEulerAngle(&v->x);
    boundEulerAngle(&v->y);
    boundEulerAngle(&v->z);
}
#ifdef NON_MATCHING
/* 29/50 words: retail keeps |x| in $f1 and the constant in $f0, with a dead mov.s in the branch delay slot */
void boundEulerAngle(float *p)
{
    float x = *p;

    if (!(__builtin_fabsf(x) < 3.1415927f)) {
        float t = x * 0.15915494f;

        t -= (float)(int)t;
        if (t > 0.5f)
            t -= 1.0f;
        else if (t <= -0.5f)
            t += 1.0f;
        *p = t * 6.2831855f;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/bound", boundEulerAngle__FPf);
#endif
void boundBetweenMinusMaxAndMax(float *p, float max)
{
    if (max < __builtin_fabsf(*p)) {
        if (0.0f < *p)
            *p = max;
        else
            *p = -max;
    }
}

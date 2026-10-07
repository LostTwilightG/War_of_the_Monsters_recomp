#include "common.h"

struct _smoothDat {
    float *p0;
    float f4;
    int f8;
    float fC;
    float f10;
    float f14;
    int f18;
    float *p1C;
    int f20;
};

#ifdef NON_MATCHING
/* 1/8 words: store order of the struct fields differs */
void smoothInit(_smoothDat *d, float *p, float a, float b, float c, float e, float *q, bool flag)
{
    d->f20 = flag;
    d->p0 = p;
    d->f4 = a;
    d->fC = b;
    d->f10 = c;
    d->f14 = e;
    d->p1C = q;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/smooth", smoothInit__FP10_smoothDatPfffffT1b);
#endif
INCLUDE_ASM("asm/nonmatchings/common/smooth", smoothEasyIn__Fffff);
INCLUDE_ASM("asm/nonmatchings/common/smooth", smoothEasyInAngle__Fffff);
#ifdef NON_MATCHING
/* 17/26 words: retail uses bc1f with the result set in the delay slot (no branch-likely) */
int smoothCanIncreaseToMax(float a, float b, float c)
{
    int r = 1;

    if (!(a < b)) {
        float d = a - b;
        float n = (float)(int)(d / b + 1.0f);

        r = (d * 0.5f + b) * n < c;
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/smooth", smoothCanIncreaseToMax__Ffff);
#endif
#ifdef NON_MATCHING
/* 17/26 words: same as smoothCanIncreaseToMax */
int smoothCanDecreaseToMinusMax(float a, float b, float c)
{
    int r = 1;

    if (!(b < a)) {
        float d = a - b;
        float n = (float)(int)(d / b + 1.0f);

        r = c < (d * 0.5f + b) * n;
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/smooth", smoothCanDecreaseToMinusMax__Ffff);
#endif
INCLUDE_ASM("asm/nonmatchings/common/smooth", smoothEasyOutEasyIn__FP10_smoothDat);
INCLUDE_ASM("asm/nonmatchings/common/smooth", smoothEasyInTC__Fffff);
INCLUDE_ASM("asm/nonmatchings/common/smooth", smoothEasyInAngleTC__Fffff);

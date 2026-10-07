#include "common.h"
#include "hieri_types.h"

extern _ParticleType fx[];
extern int splats;
void particleDestroyEffect(_ParticleType *p);
void pedSplat(_ParticleType *p, _fvector *pos);

__asm__("#SNFIX_SMALL splats");

void pedsTurnOff(_fvector *pos, bool b)
{
    if (b && splats != -1)
        pedSplat(&fx[splats], pos);
    *(int *)&pos->w = -1;
}
INCLUDE_ASM("asm/nonmatchings/common/peds", stampPacketInit__FP13_ParticleType);
INCLUDE_ASM("asm/nonmatchings/common/peds", pedsCreate__FiffUiPP8_fvectorii);
INCLUDE_ASM("asm/nonmatchings/common/peds", pedsUpdateFOVbox__FP13_ParticleType);
void pedsKill(_ParticleType **pp)
{
    particleDestroyEffect(*pp);
    *pp = 0;
    particleDestroyEffect(&fx[splats]);
}

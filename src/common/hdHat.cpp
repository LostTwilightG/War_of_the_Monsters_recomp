#include "common.h"

void hdInitHatTimer__Fv(void *self) __asm__("hdInitHatTimer__Fv");
void hdInitHatTimer__Fv(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/common/hdHat", hdSetDefaultHeight__Ff);
INCLUDE_ASM("asm/nonmatchings/common/hdHat", hdHatTest__FP8_fvectorT0fbf);

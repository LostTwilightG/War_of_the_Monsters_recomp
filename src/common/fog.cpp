#include "common.h"

INCLUDE_ASM("asm/nonmatchings/common/fog", fogInit__Fv);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogInitStats__Fv);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogSetChanged__Fv);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogUpdate__Fi);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogCalcFogValues__FfffPfT3);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogGetFogValues__FP6QwData);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogGetFogParms__FPfN30PiN24);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogGetFogMinRange__Fv);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogSetFogParms__Fffffiii);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogSetVolumeParms__Fiffffiii);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogGetFarClipRange__Fv);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogGetIsSlavedToBg__Fv);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogGetTODFogValues__FP6QwData);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogGetTODFogParms__FPfN30PiN24);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogSetTODFogParms__Fffffiii);
INCLUDE_ASM("asm/nonmatchings/common/fog", fogAdjust__Ff);

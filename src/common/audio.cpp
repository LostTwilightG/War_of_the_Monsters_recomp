#include "common.h"

INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecCreate__FP8AudioDecPUcii);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecReset__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecDelete__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecBeginPut__FP8AudioDecPPUcPiT1T2);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecEndPut__FP8AudioDeci);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecIsPreset__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecStart__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecSendToIOP__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", iopGetArea__FPiN30P8AudioDeci);
INCLUDE_ASM("asm/nonmatchings/common/audio", sendToIOP2area__FiiiiPUciT4i);
INCLUDE_ASM("asm/nonmatchings/common/audio", sendToIOP__FiPUci);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecPause__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecResume__FP8AudioDec);

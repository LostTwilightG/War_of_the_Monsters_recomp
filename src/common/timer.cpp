#include "common.h"

INCLUDE_ASM("asm/nonmatchings/common/timer", timerInit__Fi);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerEndOfRealTimeProcessing__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerSetFieldCount__Fi);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerSetFrameStart__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timer1Handler__FPUi);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerWaitForMinUpdateRate__FPUi);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetCurTics__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetUpdateRate__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetFieldCount__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetFieldsLastFrame__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerSetMinUpdateRate__Fi);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetMinUpdateRate__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetFrameStart__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetFrameTime__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetAverageFrameTime__Fv);
INCLUDE_ASM("asm/nonmatchings/common/timer", timerGetOverloadScale__Fv);

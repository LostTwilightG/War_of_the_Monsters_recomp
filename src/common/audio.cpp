#include "common.h"

struct AudioDec {
    int state;
    char pad4[0x40];
    int f44;
    int f48;
    char pad4C[4];
    int f50;
    int f54;
};

extern "C" int snd_PauseMovieSound(void);
int audioDecStart(AudioDec *a);

INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecCreate__FP8AudioDecPUcii);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecReset__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecDelete__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecBeginPut__FP8AudioDecPPUcPiT1T2);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecEndPut__FP8AudioDeci);
int audioDecIsPreset(AudioDec *a)
{
    return !(a->f54 < a->f48);
}
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecStart__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", audioDecSendToIOP__FP8AudioDec);
INCLUDE_ASM("asm/nonmatchings/common/audio", iopGetArea__FPiN30P8AudioDeci);
INCLUDE_ASM("asm/nonmatchings/common/audio", sendToIOP2area__FiiiiPUciT4i);
INCLUDE_ASM("asm/nonmatchings/common/audio", sendToIOP__FiPUci);
int audioDecPause(AudioDec *a)
{
    a->state = 3;
    a->f50 = snd_PauseMovieSound() - a->f44;
    return 1;
}
int audioDecResume(AudioDec *a)
{
    return audioDecStart(a);
}

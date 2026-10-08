#include "common.h"

extern "C" int AddDmacHandler(int, int (*)(int), int);

extern int objsInPacket;
extern int objsInAlphaPacket;
extern int dmaVu1[];

int hierDmaHandler(int);
void hierFlushObjQ(void);

/* this TU was built without optimization (-O0, see config/tu_flags.txt) */
#ifdef NON_MATCHING
/* 35/43 words at -O0: retail has no jump-to-epilogue and leaves jr delay slots empty */
void hierFlushObjQ(void)
{
    if (objsInPacket || objsInAlphaPacket) {
        *(volatile unsigned *)0x10009020 = 0;
        *(volatile unsigned *)0x10009030 = dmaVu1[dmaVu1[3]] & 0xFFFFFFF;
        *(volatile unsigned *)0x10009000 = 0x145;
        objsInPacket = 0;
        objsInAlphaPacket = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hieri", hierFlushObjQ__Fv);
#endif
#ifdef NON_MATCHING
/* 7/18 words: same -O0 epilogue/delay-slot differences */
void hierSetDmaIntHandler(void)
{
    AddDmacHandler(1, hierDmaHandler, 0);
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hieri", hierSetDmaIntHandler__Fv);
#endif
#ifdef NON_MATCHING
/* 11/22 words: same -O0 epilogue/delay-slot differences */
int hierDmaHandler(int x)
{
    if (x == 1)
        hierFlushObjQ();
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hieri", hierDmaHandler__Fi);
#endif

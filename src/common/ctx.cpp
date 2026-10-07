#include "common.h"

typedef unsigned int u128 __attribute__((mode(TI)));

extern unsigned long ctxDef[14];

void ctxSetUpGSCtx(void)
{
    *(u128 *)ctxDef = 0;
    ((unsigned *)ctxDef)[2] = 0x11000000;
    ((unsigned *)ctxDef)[3] = 0x50000006;
    ctxDef[2] = 0x1000000000008005UL;
    ctxDef[3] = 0xE;
    ctxDef[4] = 0x44;
    ctxDef[5] = 0x42;
    ctxDef[6] = 0x5040C;
    ctxDef[7] = 0x47;
    ctxDef[9] = 8;
    ctxDef[11] = 0x49;
    ctxDef[12] = 0x1000000;
    ctxDef[8] = 0;
    ctxDef[10] = 0;
    ctxDef[13] = 0x4E;
}
unsigned long *ctxGetDefaultGSCtx(void)
{
    return ctxDef;
}

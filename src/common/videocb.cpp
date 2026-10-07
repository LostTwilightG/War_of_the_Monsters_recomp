#include "common.h"

extern "C" void *memcpy(void *, const void *, unsigned);

INCLUDE_ASM("asm/nonmatchings/common/videocb", videoCallback__FP7sceMpegP16sceMpegCbDataStrPv);
INCLUDE_ASM("asm/nonmatchings/common/videocb", pcmCallback__FP7sceMpegP16sceMpegCbDataStrPv);
#ifdef NON_MATCHING
/* 19/76 words: retail recomputes m1+m2 in every branch instead of keeping it in a register */
int copy2area(unsigned char *d1, int n1, unsigned char *d2, int n2, unsigned char *s1, int m1, unsigned char *s2, int m2)
{
    int rem;

    if (n1 + n2 < m1 + m2)
        return 0;
    if (m1 >= n1) {
        memcpy(d1, s1, n1);
        memcpy(d2, s1 + n1, m1 - n1);
        memcpy(d2 + m1 - n1, s2, m2);
        return m1 + m2;
    }
    rem = n1 - m1;
    if (m2 >= rem) {
        memcpy(d1, s1, m1);
        memcpy(d1 + m1, s2, rem);
        memcpy(d2, s2 + rem, m2 - rem);
        return m1 + m2;
    }
    memcpy(d1, s1, m1);
    memcpy(d1 + m1, s2, m2);
    return m1 + m2;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/videocb", copy2area__FPUciT0iT0iT0i);
#endif

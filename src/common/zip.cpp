#include "common.h"

/* Inflate (gzip/zlib-style) used to decompress game data. */

extern unsigned long crc_32_tab[];

/* zlib crc32 */
#define DO1(buf) crc = tab[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf) DO1(buf); DO1(buf);
#define DO4(buf) DO2(buf); DO2(buf);
#define DO8(buf) DO4(buf); DO4(buf);

unsigned long zipCrc32(unsigned long crc, const unsigned char *buf, long len)
{
    if (buf == NULL) return 0L;
    unsigned long *tab = crc_32_tab;
    crc = crc ^ 0xffffffffL;
    while (len >= 8) {
        DO8(buf);
        len -= 8;
    }
    if (len) do {
        DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}

INCLUDE_ASM("asm/nonmatchings/common/zip", zipFlush__FUl);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipCheckHeader__Fv);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipInflateCodes__FP4huftT0ii);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipFreeHuffmanTable__FP4huft);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipBuildHuffmanTable__FPCUiUiUiPCUsT3PP4huftPi);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipInflateBlockStored__Fv);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipInflateBlockFixed__Fv);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipInflateBlockDynamic__Fv);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipInflateBlock__FPi);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipInflateAll__FPcPv);

INCLUDE_ASM("asm/nonmatchings/common/zip", zipGetChar__Fv);

INCLUDE_ASM("asm/nonmatchings/common/zip", __static_initialization_and_destruction_0_0022B7A0);

INCLUDE_ASM("asm/nonmatchings/common/zip", _GLOBAL_$I$whichHalfMeg);

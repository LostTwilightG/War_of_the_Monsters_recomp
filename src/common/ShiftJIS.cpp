#include "common.h"

extern "C" {
int printf(const char *, ...);
unsigned strlen(const char *);
}

struct AsciiSpecial {
    unsigned short sjis;
    unsigned short ascii;
};
struct AsciiTable {
    unsigned short base;
    unsigned short sub;
};
struct Str8 {
    char s[8];
};

extern AsciiSpecial ascii_special[33];
extern AsciiTable ascii_table[3];
extern Str8 D_006F8960;

class ShiftJIS {
public:
    static int Sjis2Ascii(unsigned char *p);
    static void Sjis2AsciiString(unsigned char *src, char *dst);
    static int IsSjis(unsigned char *p);
    static int IsAscii(char *p);
    static unsigned short Ascii2Sjis(unsigned char c);
    static void AsciiString2Sjis(unsigned char *src, unsigned short *dst);
    static short SwapShort(unsigned short v);
};

int ShiftJIS::Sjis2Ascii(unsigned char *p)
{
    unsigned c0 = p[0];
    unsigned c1 = p[1];
    int r = 0;

    if ((unsigned char)(c0 + 0x7F) >= 2)
        goto bad;
    if (c0 == 0x82) {
        if ((unsigned)(c1 - 0x4F) < 0xB || (unsigned)(c1 - 0x60) < 0x1B)
            r = (char)(c1 - 0x1F);
        else if ((unsigned char)(c1 + 0x7F) < 0x1B)
            r = (char)(c1 - 0x20);
        else
            goto bad;
    } else {
        int i;

        for (i = 0; i < 33; i++) {
            if ((c1 & 0xFFFF) == (ascii_special[i].sjis & 0xFF)) {
                r = (char)ascii_special[i].ascii;
                break;
            }
        }
        if (i != 33)
            goto done;
bad:
        return 0;
    }
done:
    return r;
}
#ifdef NON_MATCHING
/* 4/41 words: retail hoists the D_006F8960 address into a callee-saved register */
void ShiftJIS::Sjis2AsciiString(unsigned char *src, char *dst)
{
    int i = 0;
    int n = strlen((char *)src) >> 1;

    for (; i < n; i++) {
        int c = Sjis2Ascii(src);

        if (c == 0) {
            *(Str8 *)dst = D_006F8960;
            i = 7;
            break;
        }
        src += 2;
        dst[i] = c;
    }
    dst[i] = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/ShiftJIS", Sjis2AsciiString__8ShiftJISPUcPc);
#endif
int ShiftJIS::IsSjis(unsigned char *p)
{
    unsigned c = *p;

    if ((unsigned char)(c + 0x7F) < 0x1F || (unsigned char)(c + 0x20) < 0x10)
        return 1;
    return c < 0x81 ? 0 : -1;
}
int ShiftJIS::IsAscii(char *p)
{
    unsigned long c = *(unsigned char *)p;

    return (c >> 7) ^ 1;
}
#ifdef NON_MATCHING
/* 41/67 words: branch layout of the classification chain differs */
unsigned short ShiftJIS::Ascii2Sjis(unsigned char c)
{
    int group = 0;
    int special = 0;

    if ((unsigned)(c - 0x20) < 0x10)
        special = 1;
    else if ((unsigned)(c - 0x30) < 0xA)
        group = 0;
    else if ((unsigned)(c - 0x3A) < 7)
        special = 0xB;
    else if ((unsigned)(c - 0x41) < 0x1A)
        group = 1;
    else if ((unsigned)(c - 0x5B) < 6)
        special = 0x25;
    else if ((unsigned)(c - 0x61) < 0x1A)
        group = 2;
    else if ((unsigned)(c - 0x7B) < 4)
        special = 0x3F;
    else {
        printf("bad ASCII code 0x%x\n", c);
        return 0;
    }
    if (special)
        return ascii_special[c - (special + 0x1F)].sjis;
    return ascii_table[group].base + c - ascii_table[group].sub;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/ShiftJIS", Ascii2Sjis__8ShiftJISUc);
#endif
#ifdef NON_MATCHING
/* 6/40 words: loop shape and byte-swap scheduling differ */
void ShiftJIS::AsciiString2Sjis(unsigned char *src, unsigned short *dst)
{
    int i = 0;
    int n = strlen((char *)src);
    unsigned short *p = dst;

    for (; i < n; p++) {
        unsigned short v = Ascii2Sjis(src[i]);

        i++;
        *p = ((v >> 8) & 0xFF) | ((v & 0xFF) << 8);
    }
    dst[i] = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/ShiftJIS", AsciiString2Sjis__8ShiftJISPUcPUs);
#endif
short ShiftJIS::SwapShort(unsigned short v)
{
    return (v << 8) | (v >> 8);
}

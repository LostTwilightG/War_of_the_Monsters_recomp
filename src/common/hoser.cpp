#include "common.h"

struct HoserBookmark {
    int state;
    int a;
    int b;
    int pad;
    float mat[4][4];
    int vec[4];
};
struct HoserInfo {
    int f0;
    int f4;
    int f8;
    int fC;
    char pad10[0x38];
    int count;
    int cur;
    int f50;
    int f54;
    int f58;
    char pad5C[0x24];
    HoserBookmark bm[50];
};

extern int D_006F8D80;
extern int D_006F8D84;
extern int D_006F8D88;
extern HoserInfo D_00778010;
void mathfUnitMatrix(float (*m)[4]);

int hoserFindFreeBookmark(HoserInfo *info);

INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserInfoInit__FP9HoserInfo);
int hoserFindFreeBookmark(HoserInfo *info)
{
    int r = -1;
    int i;

    for (i = 0; i < 50; i++) {
        if (info->bm[i].state == -1) {
            r = i;
            i = 50;
        }
    }
    return r;
}
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserAddBookmark__FP9HoserInfoPA3_fP8_fvector);
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserDeleteBookmark__FP9HoserInfo);
#ifdef NON_MATCHING
/* 32/55 words: gcc turns the loop into a count-down; retail keeps an end-pointer compare */
int hoserDeleteAllBookmarks(HoserInfo *info)
{
    int i;

    info->cur = 0;
    info->f50 = 0;
    info->f54 = 0;
    info->f58 = 0;
    info->bm[0].state = 1;
    info->bm[0].b = -1;
    info->count = 1;
    info->bm[0].a = -1;
    mathfUnitMatrix(info->bm[0].mat);
    info->bm[0].vec[0] = 0;
    info->bm[0].vec[1] = 0;
    info->bm[0].vec[2] = 0;
    for (i = 1; i < 50; i++) {
        info->bm[i].state = -1;
        info->bm[i].a = -1;
        info->bm[i].b = -1;
        mathfUnitMatrix(info->bm[i].mat);
        info->bm[i].vec[0] = 0;
        info->bm[i].vec[1] = 0;
        info->bm[i].vec[2] = 0;
    }
    D_006F8D80 = 0;
    D_006F8D84 = 0;
    D_006F8D88 = 0;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserDeleteAllBookmarks__FP9HoserInfo);
#endif
int hoserGotoBookmark(HoserInfo *info)
{
    return 0;
}
int hoserNextBookmark(HoserInfo *info)
{
    D_006F8D80 = info->bm[info->cur].a;
    if (D_006F8D80 != -1)
        info->cur = D_006F8D80;
    return D_006F8D80;
}
int hoserPrevBookmark(HoserInfo *info)
{
    D_006F8D84 = info->bm[info->cur].b;
    if (D_006F8D84 != -1)
        info->cur = D_006F8D84;
    return D_006F8D84;
}
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserClosestBookmark__FP9HoserInfoP8_fvector);
HoserInfo *hoserGetHoserInfo(void)
{
    return &D_00778010;
}

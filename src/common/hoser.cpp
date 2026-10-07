#include "common.h"

struct HoserBookmark {
    int state;
    int a;
    int b;
    int pad;
    char mat[0x40];
    char vec[0x10];
};
struct HoserInfo {
    char head[0x80];
    HoserBookmark bm[50];
};

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
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserDeleteAllBookmarks__FP9HoserInfo);
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserGotoBookmark__FP9HoserInfo);
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserNextBookmark__FP9HoserInfo);
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserPrevBookmark__FP9HoserInfo);
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserClosestBookmark__FP9HoserInfoP8_fvector);
INCLUDE_ASM("asm/nonmatchings/common/hoser", hoserGetHoserInfo__Fv);

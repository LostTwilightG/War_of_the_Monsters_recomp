#include "common.h"

struct VoData {
    char d[0x118000];
};
struct VoTag {
    int state;
    char pad[0x85740 - 4];
};
struct VoBuf {
    VoData *data;
    VoTag *tags;
    volatile int head;
    volatile int count;
    int size;
};

extern "C" int DIntr(void);
extern "C" int EIntr(void);

int voBufIsFull(VoBuf *b);
int voBufIsEmpty(VoBuf *b);

int voBufCreate(VoBuf *b, VoData *d, VoTag *t, int n)
{
    int i;

    b->count = 0;
    b->data = d;
    b->tags = t;
    b->size = n;
    b->head = 0;
    for (i = 0; i < n; i++)
        b->tags[i].state = 0;
    return 1;
}
void voBufDelete(VoBuf *b)
{
}
#ifdef NON_MATCHING
/* 2/5 words: retail leaves the jr delay slot empty */
int voBufReset(VoBuf *b)
{
    b->count = 0;
    b->head = 0;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/vo", voBufReset__FP5VoBuf);
#endif
int voBufIsFull(VoBuf *b)
{
    return b->count == b->size;
}
#ifdef NON_MATCHING
/* 23/30 words: retail leaves the EIntr delay slot empty */
void voBufIncCount(VoBuf *b)
{
    DIntr();
    b->tags[b->head].state = 2;
    b->count++;
    b->head = (b->head + 1) % b->size;
    EIntr();
}
#else
INCLUDE_ASM("asm/nonmatchings/common/vo", voBufIncCount__FP5VoBuf);
#endif
#ifdef NON_MATCHING
/* 12/17 words: mult operand registers differ */
VoData *voBufGetData(VoBuf *b)
{
    if (voBufIsFull(b))
        return 0;
    return b->data + b->head;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/vo", voBufGetData__FP5VoBuf);
#endif
int voBufIsEmpty(VoBuf *b)
{
    return b->count == 0;
}
VoTag *voBufGetTag(VoBuf *b)
{
    if (voBufIsEmpty(b))
        return 0;
    return &b->tags[(b->head - b->count + b->size) % b->size];
}
void voBufDecCount(VoBuf *b)
{
    if (b->count > 0)
        b->count--;
}

#include "common.h"
#include "hieri_types.h"

typedef unsigned long long u128 __attribute__((mode(TI)));

extern "C" void FlushCache(int);
QwData *viewGetDb(int view);

class GsPkt {
public:
    QwData *cur;
    char pad[0x32C];
    QwData *last;

    void frameInit(int view, int flag);
    void AddCallToQueue(QwData *addr);
    void AddCallToLastQueue(QwData *addr);
    void DmaGsPktQueue(void);
};

#ifdef NON_MATCHING
/* 37/46 words: register choice for src/loop counter and tail constants differ */
void GsPkt::frameInit(int view, int flag)
{
    QwData *p;
    QwData *db;
    QwData *src;
    int i;

    *(u128 *)((char *)this + 0x10) = 0;
    *(unsigned *)((char *)this + 0x10) = 0x10000009;
    cur = (QwData *)((char *)this + 0x10);
    p = (QwData *)((char *)this + 0x20);
    db = viewGetDb(view);
    src = db + 0x1F;
    if (!flag)
        src = db + 8;
    for (i = 0; i < 9; i++) {
        *(u128 *)p = *(u128 *)&src[i];
        if (i == 0)
            p->ui16[0] = 0x8008;
        p++;
    }
    *(u128 *)p = 0;
    *(unsigned *)p = 0x70000000;
    cur = p;
    last = (QwData *)((char *)this + 0x340);
    *(u128 *)((char *)this + 0x340) = 0;
    *(unsigned *)((char *)this + 0x340) = 0x70000000;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/GsPkt", frameInit__5GsPktii);
#endif
void GsPkt::AddCallToQueue(QwData *addr)
{
    QwData *p = cur;

    *(u128 *)p = 0;
    *(unsigned *)p = 0x50000000;
    ((unsigned *)p)[1] = (unsigned)addr & 0xFFFFFFF;
    p++;
    *(u128 *)p = 0;
    *(unsigned *)p = 0x70000000;
    cur = p;
}
void GsPkt::AddCallToLastQueue(QwData *addr)
{
    QwData *p = last;

    *(u128 *)p = 0;
    *(unsigned *)p = 0x50000000;
    ((unsigned *)p)[1] = (unsigned)addr & 0xFFFFFFF;
    p++;
    *(u128 *)p = 0;
    *(unsigned *)p = 0x70000000;
    last = p;
}
void GsPkt::DmaGsPktQueue(void)
{
    FlushCache(0);
    *(volatile unsigned *)0x1000A020 = 0;
    *(volatile unsigned *)0x1000E010 = 4;
    *(volatile unsigned *)0x1000A030 = (unsigned)((char *)this + 0x10) & 0xFFFFFFF;
    *(volatile unsigned *)0x1000A000 = 0x145;
}

#include "common.h"
#include "hieri_types.h"
struct HierStackEnt {
    float eo[3];
    _hierhead **pp;
    short i10;
    short i12;
    unsigned w14;
    unsigned *p18;
    float f1c;
    _animCharInstance *anim;
    short s24;
    short s26;
    int pad28[2];
};
#define SPAD_HIERSTACK (*(HierStackEnt **)0x70000030)
#define SPAD_HIERDEPTH (*(int *)0x70000034)
void vu0GetEoAsm(QwData *);
void hierPush(_hierhead **pp, int a, unsigned b, unsigned c, unsigned *d, float f, _animCharInstance *anim, int e, int g)
{
    HierStackEnt *ent = SPAD_HIERSTACK;

    vu0GetEoAsm((QwData *)ent);
    ent->s26 = g;
    ent->pp = pp;
    ent->i10 = a;
    ent->i12 = c;
    ent->w14 = b;
    ent->p18 = d;
    ent->f1c = f;
    ent->anim = anim;
    ent->s24 = e;
    if (SPAD_HIERDEPTH < 0x96) {
        SPAD_HIERDEPTH++;
        SPAD_HIERSTACK++;
    }
}

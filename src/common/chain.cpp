#include "common.h"
#include "hieri_types.h"

struct FxListNode {
    int active;
    int prev;
    int next;
};

extern FxListNode fxList[100];
extern _ParticleType fx[];
extern int fxActiveTop;
extern int fxInactiveTop;

void chainPacketInit(_ParticleType *p, float a, float b);
void chainPacketBuild(_ParticleType *p, int a);
void particleDestroyEffect(_ParticleType *p);

_ParticleType *chainCreate(float x, float y, int unused, unsigned colorIdx, FxTextureId tex, float z)
{
    int idx = fxInactiveTop;
    _ParticleType *p;

    if (idx != -1) {
        fxInactiveTop = fxList[idx].next;
        if (fxInactiveTop != -1)
            fxList[fxInactiveTop].prev = -1;
        if (fxActiveTop != -1)
            fxList[fxActiveTop].prev = idx;
        fxList[idx].next = fxActiveTop;
        fxList[idx].prev = -1;
        fxList[idx].active = 1;
        p = &fx[idx];
        p->vis.alphaContext = 0x44;
        p->vis.attribs = 12;
        p->vis.texInfoIndex = tex;
        *(float *)((char *)p + 0x2D8C) = 1.0f;
        p->info.minSize = x;
        p->vis.colorIndex = colorIdx;
        fxActiveTop = idx;
        p->numParticles = 0;
        p->newParticles = 0;
        p->info.lifetime = 0;
        p->info.attribs = 0;
        p->vis.lightColor = 0;
        p->prevViewMat = 0;
        p->master = 0;
        p->slave = 0;
        p->backPointer = 0;
        chainPacketInit(p, z, y);
        return p;
    }
    return 0;
}
FVECTOR *chainAddSegment(_ParticleType *p, int a)
{
    FVECTOR *seg = 0;

    if (p) {
        seg = p->pos + p->numParticles;
        chainPacketBuild(p, a);
        p->newParticles = p->numParticles;
    }
    return seg;
}
void chainKill(_ParticleType **pp)
{
    particleDestroyEffect(*pp);
    *pp = 0;
}

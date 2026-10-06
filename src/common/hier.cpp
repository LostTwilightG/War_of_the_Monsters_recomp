#include "common.h"
#include "hieri_types.h"

struct Mat4 {
    float m[4][4];
} __attribute__ ((aligned (16)));

extern _hierSkelBone **skelStack;
extern float (*skelMatStack)[4][4];
extern int g_cameraLosPointsPerView[5];
extern _fvector g_cameraLosPoints[5][2];
extern "C" float sqrtf(float);
extern unsigned *lastGsCtx;
typedef void (*TravCb)(_cs *, unsigned, unsigned, float (&)[4][4], _fvector *);
extern TravCb gTraversalCallback;
extern unsigned FxToKill[32];
void particleKillFx(int &);
void hierPush(_hierhead **, int, unsigned, unsigned, unsigned *, float, _animCharInstance *, int, int);
extern int eoCnt;
extern int lastEoCnt;
extern float shadowLodRangeSq;
void psInitBlockerDMA(void);
_cs *psIsBlockerCsActive(int);
float psGetBlockerLodRangeSq(int);
void psSetBlkLitPos(_fvector *, int);
void psBldBlockerPkt(_fvector *, int, int);
_cs *psBlockerHas2ndCS(int);
void psEndBlkObjs(int);
void psEndBlkDma(void);
extern _worldctx worldCtx[5];
extern int g_doFOVGroups;
extern int g_dontTraverseObjects;
extern int tracingShadow;
extern char pointLights[0x8A0];
void *psGetLightDir(void);
float fogGetFarClipRange(void);
extern "C" void hierCacheForAsm(_fvector *trans, void *lightDir, int pl, float farClip, float one);
extern "C" void hierCacheWorldMatsAsm(_worldctx *wc, float (*light)[4]);
extern "C" void hierTraverseAsm(void *root, void *ctx, int arg);
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
void mathfUnitMatrix(float (*m)[4]);
void viewSetRot(float (*m)[4][4], int view);
void viewSetTrans(_fvector *v, int view);
void viewSetSkyTrans(_fvector *v, int view);
extern int hierCamDirty __asm__("dirty.476");
extern float hierCamFixAxes[4][4] __asm__("fixAxes.477");
extern float D_00777F80[4][4];
extern _fvector D_00777FC0;
void mathfMulVec(float (*m)[4], _fvector *v, _fvector *out);
void vu0MulMatrix3x3(float (*dst)[4], float (*a)[4], float (*b)[4]);
void vu0MulMatrix3x3_1(float (*dst)[4], float (*a)[4], float (*b)[4]);
void mathfMulMatrix(float (*dst)[4], float (*a)[4], float (*b)[4]);
_hierSkelBone *skelGetRoot(int);
float (*skelGetSkelMat(int))[4][4];
extern "C" void sceGsSyncPath(int, int);
extern "C" void FlushCache(int);
extern char D_0025B890[];
extern char D_00257C10[];
void vu0GetEoAsm(QwData *);
_fvector *viewGetTrans(int);
_lightenv *lightGetEnv(int);
void mathfUnitMatrix(float (*m)[4]);
extern _worldctx *world;
extern _cs *gCs;
extern float (*matStack)[4];
extern float (*fovNorms)[4];
extern float (*fovNorms1)[4];
extern float (*lightDir)[4];
struct CsNode {
    _cs *cs;
    CsNode *next;
};
class CsPool {
public:
    static CsNode m_activeList;
    static CsNode m_HPActiveList;
};
extern "C" int hierCsUpdateAsm(_cs *cs, _fvector *eo);
extern "C" void hierTraverseAsm(void *root, void *ctx, int arg);
void vu0UnitMatrix(float (*m)[4]);
void vu0CopyMatrix(float (*dst)[4], float (*src)[4]);
float (*plightGetParaLight(void))[4];

unsigned *hierGetLastGSCtx(void)
{
    return lastGsCtx;
}
void addSkeleton(_hierSkelBone *bone, int idx)
{
    skelStack[idx] = bone;
}
void addSkelMat(float (*mat)[4][4], int idx)
{
    char *dst = (char *)(idx * 64) + (unsigned)skelMatStack;

    __asm__ volatile("lq $8, 0x0(%0)
	"
                     "lq $9, 0x10(%0)
	"
                     "lq $10, 0x20(%0)
	"
                     "lq $11, 0x30(%0)
	"
                     "sq $8, 0x0(%1)
	"
                     "sq $9, 0x10(%1)
	"
                     "sq $10, 0x20(%1)
	"
                     "sq $11, 0x30(%1)"
                     : : "r"(mat), "r"(dst) : "$8", "$9", "$10", "$11", "memory");
}
#ifdef NON_MATCHING
/* register allocation differs: base pointer lands in $v1 instead of $v0 */
_hierSkelBone *skelGetRoot(int idx)
{
    return (unsigned)idx < 5 ? skelStack[idx] : 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hier", skelGetRoot__Fi);
#endif
float (*skelGetSkelMat(int idx))[4][4]
{
    return (unsigned)idx < 0xA0 ? &skelMatStack[idx] : 0;
}
INCLUDE_ASM("asm/nonmatchings/common/hier", hier__Fii);
void hierTraceSky(int arg)
{
    _fvector eo;
    _cs *sky = world->skyCs;

    if (sky && sky->epNode) {
        hierCsUpdateAsm(sky, &eo);
        hierTraverseAsm(world->skyCs, &eo, arg);
    }
}
void hierTraceWorld(int arg)
{
    if (world->ep) {
        vu0UnitMatrix(matStack);
        vu0CopyMatrix(fovNorms, (float (*)[4])world->fovNorms);
        vu0CopyMatrix(fovNorms1, (float (*)[4])world->fovNorms[1]);
        vu0CopyMatrix(lightDir, plightGetParaLight());
        hierTraverseAsm(world, &world->eo, arg);
    }
}
#ifdef NON_MATCHING
/* 49/57 words: $s2/$s3 swapped (list end vs ~(1 << mask)) */
void hierTraceCsList(int mask)
{
    _fvector eo;
    CsNode *n;
    CsNode *const end = &CsPool::m_activeList;

    for (n = end->next; n != end; n = n->next) {
        if (!n->cs->drawMe || !n->cs->epNode)
            continue;
        gCs = n->cs;
        gCs->inFov &= ~(1 << mask);
        if (gCs->scaleMe)
            *(float *)0x700003A0 = 1.0f / gCs->scale.x;
        hierCsUpdateAsm(gCs, &eo);
        hierTraverseAsm(n->cs, &eo, mask);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hier", hierTraceCsList__Fi);
#endif
#ifdef NON_MATCHING
/* 49/57 words: $s2/$s3 swapped (list end vs ~(1 << mask)) */
void hierTraceHPCsList(int mask)
{
    _fvector eo;
    CsNode *n;
    CsNode *const end = &CsPool::m_HPActiveList;

    for (n = end->next; n != end; n = n->next) {
        if (!n->cs->drawMe || !n->cs->epNode)
            continue;
        gCs = n->cs;
        gCs->inFov &= ~(1 << mask);
        if (gCs->scaleMe)
            *(float *)0x700003A0 = 1.0f / gCs->scale.x;
        hierCsUpdateAsm(gCs, &eo);
        hierTraverseAsm(n->cs, &eo, mask);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hier", hierTraceHPCsList__Fi);
#endif
void hierLoadVu1Ucode(void)
{
    FlushCache(0);
    sceGsSyncPath(0, 0);
    *(volatile unsigned *)0x10009020 = 0;
    *(volatile unsigned *)0x10009030 = (unsigned)D_00257C10 & 0xFFFFFFF;
    *(volatile unsigned *)0x1000E010 = 2;
    *(volatile unsigned *)0x10009000 = 0x145;
}
void hierLoadVu0Ucode(void)
{
    sceGsSyncPath(0, 0);
    *(volatile unsigned *)0x10008020 = 0;
    *(volatile unsigned *)0x10008030 = (unsigned)D_0025B890 & 0xFFFFFFF;
    *(volatile unsigned *)0x1000E010 = 1;
    *(volatile unsigned *)0x10008000 = 0x145;
}
void hierCsUpdate(_cs *cs, _worldctx *wc, _lightenv *le, _fvector *out)
{
    _fvector v;

    v.x = wc->eo.x + cs->trans.x;
    v.y = wc->eo.y + cs->trans.y;
    v.z = wc->eo.z + cs->trans.z;
    mathfMulVec((float (*)[4])&cs->mat, &v, out);
    vu0CopyMatrix(matStack, (float (*)[4])&cs->mat);
    vu0MulMatrix3x3(fovNorms, (float (*)[4])&cs->mat, (float (*)[4])wc->fovNorms[0]);
    vu0MulMatrix3x3(fovNorms1, (float (*)[4])&cs->mat, (float (*)[4])wc->fovNorms[1]);
    vu0MulMatrix3x3_1(lightDir, (float (*)[4])&cs->mat, plightGetParaLight());
}
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0MulMatrix3x3_1__FPA3_fN20);
#ifdef NON_MATCHING
/* 45/59 words: $s register order and store order differ */
void hierPush(_hierhead **pp, int a, unsigned b, unsigned c, unsigned *d, float f, _animCharInstance *anim, int e, int g)
{
    HierStackEnt *ent = SPAD_HIERSTACK;

    vu0GetEoAsm((QwData *)ent);
    ent->s24 = e;
    ent->anim = anim;
    ent->f1c = f;
    ent->p18 = d;
    ent->w14 = b;
    ent->i12 = c;
    ent->i10 = a;
    ent->pp = pp;
    ent->s26 = g;
    if (SPAD_HIERDEPTH < 0x96) {
        SPAD_HIERDEPTH++;
        SPAD_HIERSTACK++;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hier", hierPush__FPP9_hierheadiUiUiPUifP17_animCharInstanceii);
#endif
extern "C" void hierTranslateSkel(short *depth, short *matIdx, _animCharInstance *anim, _fvector *trans)
{
    Mat4 a;
    Mat4 b;
    _hierSkelBone *root = skelGetRoot(*depth);

    mathfUnitMatrix(a.m);
    __asm__ volatile("lqc2 $vf11, 0x0(%1)
	"
                     "lqc2 $vf12, 0x0(%2)
	"
                     "vadd.xyz $vf11, $vf11, $vf12
	"
                     "sqc2 $vf11, %0"
                     : "=m"(a.m[3]) : "r"(&a.m[3]), "r"(trans));
    float (*sm)[4][4] = skelGetSkelMat(*matIdx);
    mathfMulMatrix(b.m, a.m, (float (*)[4])sm);
    addSkelMat((float (*)[4][4])&b, ++*matIdx);
    if (root->skelOutputMatIdx >= 0)
        mathfMulMatrix((float (*)[4])anim->animMatrixPtr[root->skelOutputMatIdx], (float (*)[4])&root->restPoseInv, b.m);
    --*depth;
}
extern "C" void hierRotateSkel(short *depth, short *matIdx, _animCharInstance *anim, _hierSkelBone *node)
{
    Mat4 m;
    _hierSkelBone *root = skelGetRoot(*depth);
    float (*sm)[4][4] = skelGetSkelMat(*matIdx);

    mathfMulMatrix(m.m, (float (*)[4])&node->restPoseInv, (float (*)[4])sm);
    addSkelMat((float (*)[4][4])&m, ++*matIdx);
    if (root->skelOutputMatIdx >= 0)
        mathfMulMatrix((float (*)[4])anim->animMatrixPtr[root->skelOutputMatIdx], (float (*)[4])&root->restPoseInv, m.m);
    --*depth;
}
INCLUDE_ASM("asm/nonmatchings/common/hier", hierAnimTransNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierStartRotMatPRH);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierGetRotMatPRH);
__asm__("#SNFIX_SMALL gTraversalCallback");
extern "C" void hierAttachPtNode(_hierhead *node, int view, _cs *cs, int idx)
{
    QwData eo;
    _fvector out;
    float (*m)[4];

    vu0GetEoAsm(&eo);
    m = (float (*)[4])((char *)(idx * 64) + (unsigned)matStack);
    __asm__ volatile("lqc2 $vf11, %1
	"
                     "lqc2 $vf12, 0x0(%2)
	"
                     "lqc2 $vf13, 0x10(%2)
	"
                     "lqc2 $vf14, 0x20(%2)
	"
                     "vmulax.xyz $ACC, $vf12, $vf11x
	"
                     "vmadday.xyz $ACC, $vf13, $vf11y
	"
                     "vmaddz.xyz $vf11, $vf14, $vf11z
	"
                     "sqc2 $vf11, %0"
                     : "=m"(out) : "m"(eo), "r"(m));
    _fvector *t = viewGetTrans(view);
    __asm__ volatile("lqc2 $vf11, 0x0(%1)
	"
                     "lqc2 $vf12, 0x0(%2)
	"
                     "vadd.xyz $vf11, $vf11, $vf12
	"
                     "sqc2 $vf11, %0"
                     : "=m"(out) : "r"(&out), "r"(t));
    gTraversalCallback(cs, node->id1, node->id2, *(float (*)[4][4])((char *)matStack + idx * 64), &out);
}
INCLUDE_ASM("asm/nonmatchings/common/hier", hierScaleNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierAnimScaleNode);
extern "C" void hierSkelBoneNode(_hierSkelBone *bone, unsigned short *boneIdx, short *matIdx)
{
    Mat4 m;

    addSkeleton(bone, (short)++*boneIdx);
    if (bone->isRootNode) {
        ++*matIdx;
        mathfUnitMatrix(m.m);
        addSkelMat((float (*)[4][4])&m, *matIdx);
    }
}
INCLUDE_ASM("asm/nonmatchings/common/hier", hierLightNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierParticleNode);
void hierShadow(int ctx, int arg)
{
    _fvector eo;
    _cs *cs;
    int i;

    tracingShadow = 1;
    eoCnt = 0;
    lastGsCtx = 0;
    world = &worldCtx[ctx];
    gCs = 0;
    psInitBlockerDMA();
    for (i = 0; i < 10; i++) {
        cs = psIsBlockerCsActive(i);
        if (cs && cs->drawMe && cs->epNode) {
            lastEoCnt = 999999;
            shadowLodRangeSq = psGetBlockerLodRangeSq(i);
            gCs = cs;
            psSetBlkLitPos(&world->eo, i);
            psBldBlockerPkt(&world->eo, ctx, arg);
            hierCsUpdate(cs, world, cs->lightEnv, &eo);
            hierTraverseAsm(cs, &eo, ctx);
            cs = psBlockerHas2ndCS(i);
            if (cs) {
                hierCsUpdate(cs, world, cs->lightEnv, &eo);
                hierTraverseAsm(cs, &eo, ctx);
            }
            psEndBlkObjs(i);
        }
    }
    psEndBlkDma();
}
INCLUDE_ASM("asm/nonmatchings/common/hier", hierCheckVifComplete__Fv);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierWaitForVif1__Fv);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0HierRotateAsm__FPA3_A3_fi);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierGetRotatesAsm__Fi);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0FovTestResAsm__Fv);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0MulEoForObj__FPA3_f);
#ifdef NON_MATCHING
/* 42/98 words: gcc cross-jumps the two hierPush call tails, retail keeps them separate (0x17c vs 0x188) */
int hierLod(_hierlod *lod, float dist, _hierhead **out, _hierstack *stk)
{
    int i;
    float t;

    for (i = lod->numLods - 1; i >= 0; i--) {
        if (dist < lod->lods[i].switchOutDis) {
            *out = lod->lods[i].child;
            if (lod->ctx)
                stk->gsCtx = lod->ctx;
            if (lod->lods[i].fade > 0.0f) {
                if (lod->lods[i].fade < dist) {
                    t = 1.0f - (dist - lod->lods[i].fade) / (lod->lods[i].switchOutDis - lod->lods[i].fade);
                    if (i - 1 >= 0) {
                        if (t > 0.5f) {
                            *out = lod->lods[i - 1].child;
                            stk->fade = (1.0f - t) * 2.0f;
                            hierPush(&lod->lods[i].child, stk->matIdx, 1, stk->eoCnt, stk->gsCtx, 1.0f, stk->animCharInst, stk->skelIndex, stk->skelMatIndex);
                            return 1;
                        }
                        *out = lod->lods[i - 1].child;
                        stk->fade = 1.0f;
                        hierPush(&lod->lods[i].child, stk->matIdx, 1, stk->eoCnt, stk->gsCtx, t * 2.0f, stk->animCharInst, stk->skelIndex, stk->skelMatIndex);
                        return 1;
                    }
                    stk->fade = t;
                }
            }
            return 1;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hier", hierLod__FP8_hierlodfPP9_hierheadP10_hierstack);
#endif
INCLUDE_ASM("asm/nonmatchings/common/hier", hierCtrlNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierInit__Fv);
void hierDisableFov(void)
{
}
void hierSetCsEpNode(_cs *cs, _hierhead *ep)
{
    cs->epNode = ep;
}
void hierSetCsDrawMe(_cs *cs, unsigned char drawMe)
{
    if (cs)
        cs->drawMe = drawMe;
}
void hierSetTraversalCallback(TravCb cb)
{
    gTraversalCallback = cb;
}
#ifdef NON_MATCHING
/* 30/51 words: store scheduling and constant register choice differ */
void hierInitCs(_cs *cs)
{
    cs->epNode = 0;
    cs->lightEnv = lightGetEnv(0);
    cs->drawMe = 0;
    cs->testCollision = 0;
    cs->inFov = 0;
    cs->scaleMe = 0;
    cs->next = 0;
    cs->rot.x = 0;
    cs->rot.y = 0;
    cs->rot.z = 0;
    cs->color.chan.a = 0x80;
    cs->color.chan.r = 0x80;
    cs->color.chan.g = 0x80;
    cs->color.chan.b = 0x80;
    mathfUnitMatrix((float (*)[4])&cs->mat);
    cs->cloakWeight = 16.0f;
    cs->minCellRow = 0x7FFFFFFF;
    cs->maxCellRow = 0x80000001;
    cs->colorQuad.fVec[3] = 1.0f;
    cs->lightMe = 1;
    cs->trans.x = 0;
    cs->trans.y = 0;
    cs->trans.z = 0;
    cs->minCellCol = 0x7FFFFFFF;
    cs->maxCellCol = 0x80000001;
    cs->colorQuad.fVec[0] = 1.0f;
    cs->colorQuad.fVec[1] = 1.0f;
    cs->colorQuad.fVec[2] = 1.0f;
    cs->ptLightMe = 0;
    cs->cloakMe = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hier", hierInitCs__FP3_cs);
#endif
void hierSetSwitch(_hierswitch *sw, int which)
{
    if (sw && sw->head.opcode == 6 && which < sw->numKids)
        sw->whichChild = which;
}
#ifdef NON_MATCHING
/* 27/40 words: 1.0f is materialized via $v0 (not $at) and store scheduling differs */
void hierSetCameraLosPoint(_fvector *pos, int view, int idx)
{
    _fvector *dst = &g_cameraLosPoints[view][idx];
    _fvector *t = viewGetTrans(view);
    register float x __asm__("$f2");
    register float y __asm__("$f3");
    register float z __asm__("$f0");
    register float len __asm__("$f4");
    float inv;

    __asm__ volatile("lqc2 $vf11, 0x0(%1)
	"
                     "lqc2 $vf12, 0x0(%2)
	"
                     "vsub.xyz $vf11, $vf11, $vf12
	"
                     "sqc2 $vf11, 0x0(%0)"
                     : : "r"(dst), "r"(pos), "r"(t) : "memory");
    __asm__ volatile("lwc1 %0, 0x0(%4)
	"
                     "lwc1 %1, 0x4(%4)
	"
                     "lwc1 %2, 0x8(%4)
	"
                     "mula.s %0, %0
	"
                     "madda.s %1, %1
	"
                     "madd.s %3, %2, %2
	"
                     ".word 0x46040104"
                     : "=f"(x), "=f"(y), "=f"(z), "=f"(len) : "r"(dst) : "memory");
    inv = 1.0f / len;
    dst->z = z * inv;
    dst->x = x * inv;
    dst->y = y * inv;
    dst->w = len;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/hier", hierSetCameraLosPoint__FP8_fvectorii);
#endif
void hierSetLosPointsPerView(int idx, int points)
{
    g_cameraLosPointsPerView[idx] = points;
}
__asm__("#SNFIX_SMALL dirty.476");
extern "C" void hierSetCamera(float (*mat)[4][4], _fvector *vec, int view)
{
    if (mat) {
        hierCamDirty = 1;
        __asm__ volatile("lqc2 $vf11, 0x0(%1)
	"
                         "lqc2 $vf12, 0x0(%2)
	"
                         "lqc2 $vf13, 0x10(%2)
	"
                         "lqc2 $vf14, 0x20(%2)
	"
                         "vmulax.xyz $ACC, $vf12, $vf11x
	"
                         "vmadday.xyz $ACC, $vf13, $vf11y
	"
                         "vmaddz.xyz $vf11, $vf14, $vf11z
	"
                         "sqc2 $vf11, %0"
                         : "=m"(D_00777FC0) : "r"(vec), "r"(mat));
        _fvector *t = viewGetTrans(view);
        __asm__ volatile("lqc2 $vf11, 0x0(%1)
	"
                         "lqc2 $vf12, 0x0(%2)
	"
                         "vadd.xyz $vf11, $vf11, $vf12
	"
                         "sqc2 $vf11, %0"
                         : "=m"(D_00777FC0) : "r"(&D_00777FC0), "r"(t));
        mathfMulMatrix(D_00777F80, hierCamFixAxes, (float (*)[4])mat);
    } else if (hierCamDirty) {
        hierCamDirty = 0;
        viewSetRot((float (*)[4][4])D_00777F80, view);
        viewSetTrans(&D_00777FC0, view);
        viewSetSkyTrans(&D_00777FC0, view);
    }
}
extern "C" void hierKillLocator(int idx, _hierhead *node)
{
    unsigned *slot = &FxToKill[idx];

    particleKillFx((int &)*slot);
    *slot = (unsigned)node;
    node->id1 = idx;
}
void hierRegisterLocator(unsigned id, int fx)
{
    unsigned idx;

    if (fx != -1) {
        idx = id - 0x1D4C;
        if (idx < 32)
            FxToKill[idx] = fx;
    }
}
void hierClearLocatorListForReplay(void)
{
    int i;

    for (i = 0; i < 32; i++) {
        unsigned fx = FxToKill[i];

        if (fx >= 100) {
            if (fx != 0xFFFFFFFF) {
                _hierhead *node = (_hierhead *)fx;

                node->id1 = node->id1 + 0x1D4C;
            }
        }
        FxToKill[i] = 0xFFFFFFFF;
    }
}
__asm__("#SNFIX_SMALL g_doFOVGroups");
__asm__("#SNFIX_SMALL g_dontTraverseObjects");
void hierTraverseCSForLocators(_cs *cs, bool flag)
{
    _fvector eo;

    if (cs->epNode) {
        matStack = (float (*)[4])0x70000400;
        fovNorms = (float (*)[4])0x70000900;
        g_doFOVGroups = flag;
        fovNorms1 = (float (*)[4])0x70000E00;
        lightDir = (float (*)[4])0x70001300;
        world = worldCtx;
        gCs = cs;
        g_dontTraverseObjects = 1;
        tracingShadow = 0;
        _fvector *t = viewGetTrans(0);
        void *ld = psGetLightDir();
        int pl = *(int *)(pointLights + 0x820);
        hierCacheForAsm(t, ld, pl, fogGetFarClipRange(), 1.0f);
        hierCacheWorldMatsAsm(world, plightGetParaLight());
        __asm__ volatile("sync
	sync.p");
        hierCsUpdate(cs, world, world->lightEnv, &eo);
        hierTraverseAsm(cs, &eo, 0);
        g_doFOVGroups = 1;
        g_dontTraverseObjects = 0;
    }
}
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0LoadEoAsm__FP6QwData);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0GetEoAsm__FP6QwData);

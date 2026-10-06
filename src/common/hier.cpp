#include "common.h"
#include "hieri_types.h"

struct Mat4 {
    float m[4][4];
} __attribute__ ((aligned (16)));

extern _hierSkelBone **skelStack;
extern float (*skelMatStack)[4][4];
extern int g_cameraLosPointsPerView[5];
extern unsigned *lastGsCtx;
typedef void (*TravCb)(_cs *, unsigned, unsigned, float (&)[4][4], _fvector *);
extern TravCb gTraversalCallback;
extern unsigned FxToKill[32];
void particleKillFx(int &);

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
INCLUDE_ASM("asm/nonmatchings/common/hier", hierTraceSky__Fi);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierTraceWorld__Fi);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierTraceCsList__Fi);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierTraceHPCsList__Fi);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierLoadVu1Ucode__Fv);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierLoadVu0Ucode__Fv);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierCsUpdate__FP3_csP9_worldctxP9_lightenvP8_fvector);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0MulMatrix3x3_1__FPA3_fN20);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierPush__FPP9_hierheadiUiUiPUifP17_animCharInstanceii);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierTranslateSkel);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierRotateSkel);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierAnimTransNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierStartRotMatPRH);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierGetRotMatPRH);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierAttachPtNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierScaleNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierAnimScaleNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierSkelBoneNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierLightNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierParticleNode);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierShadow__Fii);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierCheckVifComplete__Fv);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierWaitForVif1__Fv);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0HierRotateAsm__FPA3_A3_fi);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierGetRotatesAsm__Fi);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0FovTestResAsm__Fv);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0MulEoForObj__FPA3_f);
INCLUDE_ASM("asm/nonmatchings/common/hier", hierLod__FP8_hierlodfPP9_hierheadP10_hierstack);
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
INCLUDE_ASM("asm/nonmatchings/common/hier", hierInitCs__FP3_cs);
void hierSetSwitch(_hierswitch *sw, int which)
{
    if (sw && sw->head.opcode == 6 && which < sw->numKids)
        sw->whichChild = which;
}
INCLUDE_ASM("asm/nonmatchings/common/hier", hierSetCameraLosPoint__FP8_fvectorii);
void hierSetLosPointsPerView(int idx, int points)
{
    g_cameraLosPointsPerView[idx] = points;
}
INCLUDE_ASM("asm/nonmatchings/common/hier", hierSetCamera);
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
INCLUDE_ASM("asm/nonmatchings/common/hier", hierTraverseCSForLocators__FP3_csb);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0LoadEoAsm__FP6QwData);
INCLUDE_ASM("asm/nonmatchings/common/hier", vu0GetEoAsm__FP6QwData);

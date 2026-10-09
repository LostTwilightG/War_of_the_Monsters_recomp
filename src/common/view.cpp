#include "common.h"
#include "hieri_types.h"

struct _todInfo14;

/* One view (player camera): its coordinate system. */
struct _viewInfo {
    CS *cs;      /* 0x00 */
    int pad[4];
};

/* Per-view double-buffered draw state (0x360 bytes, not decoded yet). */
struct _viewDb {
    char data[0x360];
};

/* A 4x4 matrix copied as one 16-byte aligned block. */
struct Matrix16 {
    FVECTOR row[4];
};

extern _viewInfo viewInfo[5];
extern _worldctx worldCtx[5];
extern _viewDb viewDb[5];
extern int viewNumViews;
extern int viewCurView;
extern int gUseUnifiedView;
__asm__("#SNFIX_SMALL viewCurView");
__asm__("#SNFIX_SMALL gUseUnifiedView");

void todGetTOD(int *on, float *minutesPerSecond, _todInfo14 *out);
void todSetTOD(int on, float minutesPerSecond, void *data, float version);
void mathfRotMatrixRPH(float (*mat)[4], _fvector *rph);

INCLUDE_ASM("asm/nonmatchings/common/view", viewCreate__F10_viewportsi);
INCLUDE_ASM("asm/nonmatchings/common/view", viewUpdate__Fi);
INCLUDE_ASM("asm/nonmatchings/common/view", viewInFOV__FiP8_fvectorT1);
INCLUDE_ASM("asm/nonmatchings/common/view", viewInFOV__FiP8_fvectorT1f);
INCLUDE_ASM("asm/nonmatchings/common/view", viewShadowUpdate__Fi);
INCLUDE_ASM("asm/nonmatchings/common/view", viewStoreNorms1InVu0__Fv);
INCLUDE_ASM("asm/nonmatchings/common/view", viewStoreNorms2InVu0__Fv);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetWorld2ScreenMat__Fi);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetShadWorldScreenMat__Fv);
void viewGetTOD(int *on, float *minutesPerSecond, _todInfo14 *info)
{
    todGetTOD(on, minutesPerSecond, info);
}
void viewSetTOD(int on, float minutesPerSecond, void *data, float version)
{
    todSetTOD(on, minutesPerSecond, data, version);
}
FVECTOR *viewGetRot(int view)
{
    return &viewInfo[view].cs->rot;
}
FMATRIX_16 *viewGetMat(int view)
{
    return &viewInfo[view].cs->mat;
}
FVECTOR *viewGetTrans(int view)
{
    return &viewInfo[view].cs->trans;
}
void viewSetRot(_fvector *rot, int view)
{
    CS *cs = viewInfo[view].cs;

    cs->rot.x = rot->x;
    cs->rot.y = rot->y;
    cs->rot.z = rot->z;
    mathfRotMatrixRPH(cs->mat, &cs->rot);
}
void viewSetRot(float (*mat)[4][4], int view)
{
    CS *cs = viewInfo[view].cs;

    __asm__ volatile("lq $8, %4\n"
                     "lq $9, %5\n"
                     "lq $10, %6\n"
                     "lq $11, %7\n"
                     "sq $8, %0\n"
                     "sq $9, %1\n"
                     "sq $10, %2\n"
                     "sq $11, %3"
                     : "=m"(cs->mat[0][0]), "=m"(cs->mat[1][0]), "=m"(cs->mat[2][0]), "=m"(cs->mat[3][0])
                     : "m"((*mat)[0][0]), "m"((*mat)[1][0]), "m"((*mat)[2][0]), "m"((*mat)[3][0])
                     : "$8", "$9", "$10", "$11");
}
void viewSetTrans(_fvector *trans, int view)
{
    viewInfo[view].cs->trans.x = trans->x;
    viewInfo[view].cs->trans.y = trans->y;
    viewInfo[view].cs->trans.z = trans->z;
}
int viewGetCurView(void)
{
    return viewCurView;
}
CS *viewGetViewCs(int view)
{
    return viewInfo[view].cs;
}
_worldctx *viewGetWorldCtx(int view)
{
    return &worldCtx[view];
}
void viewSetWorldEpNode(HierHead *ep)
{
    int i;

    for (i = 4; i >= 0; i--)
        worldCtx[i].ep = ep;
}
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetAmbientVol__Fiffff);
FVECTOR *viewGetWeTrans(int view)
{
    return &worldCtx[view].eo;
}
FMATRIX *viewGetWeMat(int view)
{
    return &worldCtx[view].weMat;
}
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetSkyEntry__FP9_hierheadi);
CS *viewGetSky(int view)
{
    return worldCtx[view].skyCs;
}
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetSkyTrans__FP8_fvectori);
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetSkyNode__FP9_hierheadi);
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetVUPacketMat__FPA3_fN30i);
INCLUDE_ASM("asm/nonmatchings/common/view", viewApplySwap__FPA3_fT0);
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetBgColor__FUlUlUlUl);
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetTODBgColor__FUlUlUlUl);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetBgColor__FRiN30);
_viewDb *viewGetDb(int view)
{
    return &viewDb[view];
}
int viewGetNumViews(void)
{
    return gUseUnifiedView ? 1 : viewNumViews;
}
void viewSetNumViews(int n)
{
    viewNumViews = n;
}
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetCenter__FiPiT1);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetWH__FiPiT1);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetWorldToScreenMat__Fi);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetZBuffParams__FiPfT1);
INCLUDE_ASM("asm/nonmatchings/common/view", viewInit__Fv);
INCLUDE_ASM("asm/nonmatchings/common/view", viewTweakInit__Fv);
INCLUDE_ASM("asm/nonmatchings/common/view", viewTweakSetFov__Fv);
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetAmbient__Ffff);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetAmbient__FPfN20);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetAnimAmbient__FPfN20);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetFov__FiPfT1);
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetFov__Fiff);
INCLUDE_ASM("asm/nonmatchings/common/view", viewComputeNormal__FP8_fvectorN30);
INCLUDE_ASM("asm/nonmatchings/common/view", viewToggleSplitScreen__FbT0);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetScreenDisplay__Fv);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGrappleConfig__Fv);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetDef__Fi);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetDef__F10_viewports);
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetOddEven__Fi);
INCLUDE_ASM("asm/nonmatchings/common/view", viewIsOddScan__Fv);

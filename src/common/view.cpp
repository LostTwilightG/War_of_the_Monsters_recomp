#include "common.h"
#include "hieri_types.h"
#include "cs_pool.h"

struct _todInfo14;

/* Screen layouts (only the first three are known: full screen and the two split-screen halves). */
enum _viewports { VIEWPORT_0, VIEWPORT_1, VIEWPORT_2 };

/* Screen rectangle and field of view of a viewport (0xC0 bytes). */
struct _viewDef {
    float fovH;            /* 0x00 */
    unsigned short width;  /* 0x04 */
    unsigned short height; /* 0x06 */
    int pad8[4];
    int centerX;           /* 0x18 */
    int centerY;           /* 0x1C */
    float fovV;            /* 0x20 */
    char pad24[0xC0 - 0x24];
};

/* One view (player camera): its coordinate system and screen rectangle. */
struct _viewInfo {
    CS *cs;        /* 0x00 */
    _viewDef *def; /* 0x04 */
    int unk8;      /* 0x08 */
    int unkC;      /* 0x0C */
    int unk10;     /* 0x10 */
};

/* GS RGBAQ register value (color bytes, Q = 1.0 in the high word). */
union GsRgbaq {
    unsigned long rgbaq;
    unsigned char c[8];
};

/* Per-view double-buffered draw state (mostly not decoded yet). */
struct _viewDb {
    char pad0[0x1B0];
    GsRgbaq bgColor0; /* 0x1B0: clear color, buffer 0 */
    char pad1[0x320 - 0x1B8];
    GsRgbaq bgColor1; /* 0x320: clear color, buffer 1 */
    char pad2[0x360 - 0x328];
};
/* bgColor1 seen from bgColor0, as retail addresses it */
#define VIEW_BG_STRIDE ((0x320 - 0x1B0) / sizeof(GsRgbaq))
typedef char _size__viewDb[sizeof(_viewDb) == 0x360 ? 1 : -1];

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
extern float s_viewAmbientVolBlend[4];
extern float s_viewAmbientVolRed[4];
extern float s_viewAmbientVolGreen[4];
extern float s_viewAmbientVolBlue[4];
extern int viewBgR, viewBgG, viewBgB, viewBgA;
extern int viewTODBgR, viewTODBgG, viewTODBgB, viewTODBgA;
extern FMATRIX worldToScreenMat[5];
extern FMATRIX viewScreenMats[5];
extern _viewDef viewDef[];
extern FMATRIX viewShadWorldToScrMat;
extern int viewRearLargeActive;
extern int viewRearViewConfig;
extern int viewAmbientChanged;
extern float viewFovH;
extern float viewAmbientRed, viewAmbientGreen, viewAmbientBlue;
extern float viewAnimAmbientRed, viewAnimAmbientGreen, viewAnimAmbientBlue;
extern int viewOddScan __asm__("D_006F8DF0");
int todActive(void);
void todInit(void);
void viewSetFov(int view, float h, float v);
void viewCreate(_viewports vp, int view);
_viewDef *viewGetDef(int view);
void todSetSkyEntry(HierHead *node);
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
FMATRIX *viewGetWorld2ScreenMat(int view)
{
    return &worldToScreenMat[view];
}
FMATRIX *viewGetShadWorldScreenMat(void)
{
    return &viewShadWorldToScrMat;
}
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
void viewSetAmbientVol(int vol, float blend, float red, float green, float blue)
{
    s_viewAmbientVolBlend[vol] = blend;
    s_viewAmbientVolRed[vol] = red;
    s_viewAmbientVolGreen[vol] = green;
    s_viewAmbientVolBlue[vol] = blue;
}
FVECTOR *viewGetWeTrans(int view)
{
    return &worldCtx[view].eo;
}
FMATRIX *viewGetWeMat(int view)
{
    return &worldCtx[view].weMat;
}
/* Sky groups with id 0 go in the sky slot (and clear the second one), id 4 is the cloud layer. */
void viewSetSkyEntry(HierHead *node, int view)
{
    _worldctx *w = &worldCtx[view];

    if (!w->skyCs)
        w->skyCs = CsPool::csActivate();
    if (!w->skyCs2)
        w->skyCs2 = CsPool::csActivate();
    if (!w->skyClouds)
        w->skyClouds = CsPool::csActivate();
    if (node->id2 == 0) {
        w->skyCs->epNode = node;
        w->skyCs2->epNode = 0;
    }
    if (node->id2 == 4)
        w->skyClouds->epNode = node;
    if (view == 0)
        todSetSkyEntry(node);
}
CS *viewGetSky(int view)
{
    return worldCtx[view].skyCs;
}
void viewSetSkyTrans(_fvector *trans, int view)
{
    _worldctx *w = &worldCtx[view];

    if (w->skyCs) {
        w->skyCs->trans.x = trans->x;
        w->skyCs->trans.y = trans->y;
        w->skyCs->trans.z = trans->z;
    }
    if (w->skyCs2) {
        w->skyCs2->trans.x = trans->x;
        w->skyCs2->trans.y = trans->y;
        w->skyCs2->trans.z = trans->z;
    }
    if (w->skyClouds) {
        w->skyClouds->trans.x = trans->x;
        w->skyClouds->trans.y = trans->y;
        w->skyClouds->trans.z = trans->z;
    }
}
/* layer 0: sky, 1: second sky (TOD cross-fade), 2: clouds; set for every view. */
void viewSetSkyNode(HierHead *node, int layer)
{
    int i;

    if (layer == 0) {
        for (i = 0; i < 5; i++) {
            if (worldCtx[i].skyCs)
                worldCtx[i].skyCs->epNode = node;
        }
    } else if (layer == 1) {
        for (i = 0; i < 5; i++) {
            if (worldCtx[i].skyCs2)
                worldCtx[i].skyCs2->epNode = node;
        }
    } else if (layer == 2) {
        for (i = 0; i < 5; i++) {
            if (worldCtx[i].skyClouds)
                worldCtx[i].skyClouds->epNode = node;
        }
    }
}
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetVUPacketMat__FPA3_fN30i);
INCLUDE_ASM("asm/nonmatchings/common/view", viewApplySwap__FPA3_fT0);
/* Clear color; written into each view's GS RGBAQ (both buffers) unless time of day drives it. */
void viewSetBgColor(unsigned long r, unsigned long g, unsigned long b, unsigned long a)
{
    int i;

    if (!todActive()) {
        for (i = 0; i < viewNumViews; i++) {
            GsRgbaq *bg = &viewDb[i].bgColor0;

            bg[0].rgbaq = r | g << 8 | b << 16 | a << 24 | 0x3F80000000000000UL;
            bg[VIEW_BG_STRIDE].rgbaq = r | g << 8 | b << 16 | a << 24 | 0x3F80000000000000UL;
        }
    }
    viewBgR = r;
    viewBgG = g;
    viewBgB = b;
    viewBgA = a;
}
void viewSetTODBgColor(unsigned long r, unsigned long g, unsigned long b, unsigned long a)
{
    int i;

    if (todActive()) {
        for (i = 0; i < viewNumViews; i++) {
            GsRgbaq *bg = &viewDb[i].bgColor0;

            bg[0].rgbaq = r | g << 8 | b << 16 | a << 24 | 0x3F80000000000000UL;
            bg[VIEW_BG_STRIDE].rgbaq = r | g << 8 | b << 16 | a << 24 | 0x3F80000000000000UL;
        }
    }
    viewTODBgR = r;
    viewTODBgG = g;
    viewTODBgB = b;
    viewTODBgA = a;
}
void viewGetBgColor(int &r, int &g, int &b, int &a)
{
    r = viewDb[0].bgColor0.c[0];
    g = viewDb[0].bgColor0.c[1];
    b = viewDb[0].bgColor0.c[2];
    a = viewDb[0].bgColor0.c[3];
}
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
void viewGetCenter(int view, int *x, int *y)
{
    *x = viewInfo[view].def->centerX;
    *y = viewInfo[view].def->centerY;
}
void viewGetWH(int view, int *w, int *h)
{
    *w = viewInfo[view].def->width;
    *h = viewInfo[view].def->height;
}
FMATRIX *viewGetWorldToScreenMat(int view)
{
    return &worldToScreenMat[view];
}
void viewGetZBuffParams(int view, float *a, float *b)
{
    float (*m)[4] = viewScreenMats[view];

    *a = m[2][2];
    *b = m[2][3];
}
void viewInit(void)
{
    int i;

    for (i = 0; i < 5; i++) {
        viewInfo[i].cs = 0;
        viewInfo[i].def = 0;
        viewInfo[i].unk8 = 0;
        viewInfo[i].unkC = 0;
    }
    todInit();
    viewRearLargeActive = 0;
    viewRearViewConfig = 1;
    viewAmbientChanged = 0;
}
void viewTweakInit(void)
{
}
void viewTweakSetFov(void)
{
    int i;

    for (i = 0; i < viewNumViews; i++)
        viewSetFov(i, viewFovH, viewGetDef(i)->fovV);
}
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetAmbient__Ffff);
void viewGetAmbient(float *r, float *g, float *b)
{
    *r = viewAmbientRed;
    *g = viewAmbientGreen;
    *b = viewAmbientBlue;
}
int viewGetAnimAmbient(float *r, float *g, float *b)
{
    *r = viewAnimAmbientRed;
    *g = viewAnimAmbientGreen;
    *b = viewAnimAmbientBlue;
    return viewAmbientChanged;
}
void viewGetFov(int view, float *h, float *v)
{
    float (*m)[4] = viewScreenMats[view];

    *h = viewInfo[view].def->fovH;
    *v = m[1][1] / *h;
}
INCLUDE_ASM("asm/nonmatchings/common/view", viewSetFov__Fiff);
INCLUDE_ASM("asm/nonmatchings/common/view", viewComputeNormal__FP8_fvectorN30);
void viewToggleSplitScreen(bool, bool)
{
    if (viewNumViews < 3) {
        if (viewNumViews == 1) {
            viewNumViews = 2;
            viewCreate(VIEWPORT_1, 0);
            viewCreate(VIEWPORT_2, 1);
        } else {
            viewNumViews = 1;
            viewCreate(VIEWPORT_0, 0);
        }
    }
}
INCLUDE_ASM("asm/nonmatchings/common/view", viewGetScreenDisplay__Fv);
INCLUDE_ASM("asm/nonmatchings/common/view", viewGrappleConfig__Fv);
_viewDef *viewGetDef(int view)
{
    return viewInfo[view].def;
}
_viewDef *viewGetDef(_viewports vp)
{
    return &viewDef[vp];
}
void viewSetOddEven(int odd)
{
    viewOddScan = odd != 0;
}
int viewIsOddScan(void)
{
    return viewOddScan;
}

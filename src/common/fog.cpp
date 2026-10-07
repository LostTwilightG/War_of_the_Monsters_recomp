#include "common.h"
#include "hieri_types.h"

extern float fogMinRange, fogMaxRange, fogMaxVal, fogFarClip, fogFA, fogFB;
extern int fogColorRed, fogColorGreen, fogColorBlue, fogChanged, fogBackgroundColorIsSlaved;
extern float fogTODMinRange, fogTODMaxRange, fogTODMaxVal, fogTODFarClip, fogTODFA, fogTODFB;
extern int fogTODColorRed, fogTODColorGreen, fogTODColorBlue;

int todActive(void);
void fogCalcFogValues(float maxRange, float minRange, float maxVal, float *fa, float *fb);

INCLUDE_ASM("asm/nonmatchings/common/fog", fogInit__Fv);
void fogInitStats(void)
{
}
void fogSetChanged(void)
{
    fogCalcFogValues(fogMaxRange, fogMinRange, fogMaxVal, &fogFA, &fogFB);
    fogChanged = 1;
}
INCLUDE_ASM("asm/nonmatchings/common/fog", fogUpdate__Fi);
void fogCalcFogValues(float maxRange, float minRange, float maxVal, float *fa, float *fb)
{
    float v = (maxVal - 1.0f) * (minRange * maxRange) / (maxRange - minRange);

    *fb = v;
    *fa = 1.0f - v / maxRange;
}
void fogGetFogValues(QwData *q)
{
    q->fVec[0] = fogFA;
    q->fVec[1] = fogMaxVal;
    q->fVec[2] = 1.0f;
    q->fVec[3] = fogFB;
}
void fogGetFogParms(float *minR, float *maxR, float *maxV, float *farClip, int *r, int *g, int *b)
{
    *minR = fogMinRange;
    *maxR = fogMaxRange;
    *maxV = fogMaxVal;
    *farClip = fogFarClip;
    *r = fogColorRed;
    *g = fogColorGreen;
    *b = fogColorBlue;
}
float fogGetFogMinRange(void)
{
    return fogMinRange;
}
void fogSetFogParms(float minR, float maxR, float maxV, float farClip, int r, int g, int b)
{
    fogColorRed = r;
    fogColorGreen = g;
    fogFarClip = farClip;
    fogColorBlue = b;
    fogChanged = 1;
    fogMinRange = minR;
    fogMaxRange = maxR;
    fogMaxVal = maxV;
    fogCalcFogValues(maxR, minR, maxV, &fogFA, &fogFB);
}
INCLUDE_ASM("asm/nonmatchings/common/fog", fogSetVolumeParms__Fiffffiii);
float fogGetFarClipRange(void)
{
    if (todActive())
        return fogTODFarClip;
    return fogFarClip;
}
int fogGetIsSlavedToBg(void)
{
    return fogBackgroundColorIsSlaved;
}
void fogGetTODFogValues(QwData *q)
{
    q->fVec[0] = fogTODFA;
    q->fVec[1] = fogTODMaxVal;
    q->fVec[2] = 1.0f;
    q->fVec[3] = fogTODFB;
}
void fogGetTODFogParms(float *minR, float *maxR, float *maxV, float *farClip, int *r, int *g, int *b)
{
    *minR = fogTODMinRange;
    *maxR = fogTODMaxRange;
    *maxV = fogTODMaxVal;
    *farClip = fogTODFarClip;
    *r = fogTODColorRed;
    *g = fogTODColorGreen;
    *b = fogTODColorBlue;
}
void fogSetTODFogParms(float minR, float maxR, float maxV, float farClip, int r, int g, int b)
{
    fogTODColorRed = r;
    fogTODColorGreen = g;
    fogTODFarClip = farClip;
    fogTODColorBlue = b;
    fogChanged = 1;
    fogTODMinRange = minR;
    fogTODMaxRange = maxR;
    fogTODMaxVal = maxV;
    fogCalcFogValues(maxR, minR, maxV, &fogTODFA, &fogTODFB);
}
void fogAdjust(float k)
{
    fogChanged = 1;
    fogMinRange *= k;
    fogFarClip *= k;
    fogMaxRange *= k;
}

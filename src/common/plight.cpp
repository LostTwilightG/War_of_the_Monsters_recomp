#include "common.h"

extern int visLength, doParaLights, visualizeLights;
extern float paraL[4][4];
extern float paraLightR[3], paraLightG[3], paraLightB[3], paraLightH[3], paraLightP[3];
extern float paraTODLightR[3], paraTODLightG[3], paraTODLightB[3], paraTODLightH[3], paraTODLightP[3];
extern float specularColor;

void plightUpdateParaLights(void);

void plightInit(void)
{
    visLength = 10;
    doParaLights = 0;
    visualizeLights = 0;
    plightUpdateParaLights();
}
void plightParaActivate(void)
{
    doParaLights = 1;
}
int plightParaActive(void)
{
    return doParaLights;
}
void plightParaDeactivate(void)
{
    doParaLights = 0;
}
float (*plightGetParaLight(void))[4]
{
    return paraL;
}
INCLUDE_ASM("asm/nonmatchings/common/plight", plightUpdateParaLights__Fv);
INCLUDE_ASM("asm/nonmatchings/common/plight", pLightCompSpecular__Fi);
void plightGetParaLight(int i, float *r, float *g, float *b, float *h, float *p)
{
    *r = paraLightR[i];
    *g = paraLightG[i];
    *b = paraLightB[i];
    *h = paraLightH[i];
    *p = paraLightP[i];
}
void plightSetParaLight(int i, float r, float g, float b, float h, float p)
{
    paraLightR[i] = r;
    paraLightG[i] = g;
    paraLightB[i] = b;
    paraLightH[i] = h;
    paraLightP[i] = p;
}
void plightSetTODParaLight(int i, float r, float g, float b, float h, float p)
{
    if ((unsigned)i < 3) {
        paraTODLightR[i] = r;
        paraTODLightG[i] = g;
        paraTODLightB[i] = b;
        paraTODLightH[i] = h;
        paraTODLightP[i] = p;
    }
}
INCLUDE_ASM("asm/nonmatchings/common/plight", plightUpdateLights__Fv);
void pLightSetSpecularIntens(float f)
{
    specularColor = f;
}

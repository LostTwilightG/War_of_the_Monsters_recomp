#include "common.h"
#include "hieri_types.h"

class PointLight {
public:
    int active;
    char pad4[0xC];
    float x, y, z;
    float range;
    float r, g, b;
    float falloff;
    int f30;
    float intensity;
    int f38;
    int f3C;

    void Init(void);
    void SetPos(_fvector *p);
    void SetPos(float px, float py, float pz);
    void CompIntensity(float i);
    void SetRange(float rng);
    void CompIntensity(void);
    void SetColor(float cr, float cg, float cb);
};

class PointLights {
public:
    PointLight lights[32];
    char pad800[0x20];
    int count;
    int f824;
    char pad828[8];
    char pkt[0x10];

    void Init(void);
    int Create(void);
    char *GetPktPtr(void);
    int RenderObject(void);
};

#ifdef NON_MATCHING
/* 21/23 words: order of the seven constant stores differs */
void PointLight::Init(void)
{
    z = 150.0f;
    g = 255.0f;
    falloff = 64.0f;
    range = 400.0f;
    intensity = 80.0f;
    b = 0;
    y = 0;
    r = 0;
    x = 0;
    active = 0;
    f38 = 0;
    f30 = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/PointLight", Init__10PointLight);
#endif
void PointLight::SetPos(_fvector *p)
{
    x = p->x;
    y = p->y;
    z = p->z;
}
void PointLight::SetPos(float px, float py, float pz)
{
    x = px;
    y = py;
    z = pz;
}
void PointLight::CompIntensity(float i)
{
    intensity = i;
    falloff = range * range / (i * 16.0f);
}
void PointLight::SetRange(float rng)
{
    range = rng;
    falloff = rng * rng / (intensity * 16.0f);
}
void PointLight::CompIntensity(void)
{
    falloff = range * range / (intensity * 16.0f);
}
void PointLight::SetColor(float cr, float cg, float cb)
{
    r = cr;
    g = cg;
    b = cb;
}
void PointLights::Init(void)
{
    int i;

    count = 0;
    for (i = 0; i < 32; i++)
        lights[i].Init();
}
int PointLights::Create(void)
{
    int i;

    for (i = 0; i < 32; i++) {
        if (!lights[i].active) {
            lights[i].active = 1;
            return i;
        }
    }
    return -1;
}
char *PointLights::GetPktPtr(void)
{
    return pkt;
}
int PointLights::RenderObject(void)
{
    if (count) {
        if (f824)
            return 1;
    }
    return 0;
}
INCLUDE_ASM("asm/nonmatchings/common/PointLight", IntersectObject__11PointLightsPA3_fP8_fvectorT2fT2P3_cs);
INCLUDE_ASM("asm/nonmatchings/common/PointLight", GetClosestPts__11PointLightsi);

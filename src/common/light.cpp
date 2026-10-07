#include "common.h"
#include "hieri_types.h"

extern _lightenv gLightEnv[3];

void lightInit(void)
{
    int i;

    for (i = 0; i < 3; i++) {
        _lightenv *e = &gLightEnv[i];

        e->dir[0][0] = 0;
        e->dir[0][1] = 1.0f;
        e->dir[0][2] = 0;
        e->dir[0][3] = 0;
        e->dir[1][0] = -1.0f;
        e->dir[1][1] = 0;
        e->dir[1][2] = 0;
        e->dir[1][3] = 0;
        e->dir[2][0] = 0;
        e->dir[2][1] = 0;
        e->dir[2][2] = 0;
        e->dir[2][3] = 0;
        e->dir[3][0] = 0;
        e->dir[3][1] = 0;
        e->dir[3][2] = 0;
        e->dir[3][3] = 1.0f;
        e->color[3][0] = 0.2f;
        e->color[3][1] = 0.2f;
        e->color[3][2] = 0.2f;
        e->color[3][3] = 0.2f;
    }
    gLightEnv[2].color[3][1] = 0;
    gLightEnv[2].color[3][2] = 255.0f;
    gLightEnv[2].color[3][0] = 0;
}
void lightSetAmbient(int i, float r, float g, float b)
{
    _lightenv *e = &gLightEnv[i];

    e->color[3][0] = r;
    e->color[3][1] = g;
    e->color[3][2] = b;
}
_lightenv *lightGetEnv(int i)
{
    return &gLightEnv[i];
}

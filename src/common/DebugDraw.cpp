#include "common.h"
#include "hieri_types.h"

class DebugDraw {
public:
    static _hierhead *s_ep[33];

    static void init(void);
    static void registerVisual(_hierhead *h);
};

void DebugDraw::init(void)
{
    int i;

    for (i = 31; i >= 0; i--)
        s_ep[i] = 0;
}
void DebugDraw::registerVisual(_hierhead *h)
{
    unsigned idx = h->id1 - 0x3E80;

    if (idx < 0x21)
        s_ep[idx] = h;
}
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawSphere__9DebugDraw);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawSphere__9DebugDrawR8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawCS__9DebugDrawR8_fvectorRA3_A3_ff);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawCollisionLines__9DebugDrawR8_fvectorUiUi);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawCollisionLines__9DebugDrawR8_fvectorfiUiUi);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawNormal__9DebugDrawR8_fvectorT1fUi);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawBox__9DebugDrawR8_fvectorT1Ui);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawBox__9DebugDrawRA3_A3_fR8_fvectorN22Ui);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", drawCone__9DebugDrawR8_fvectorT1ffUi);
INCLUDE_ASM("asm/nonmatchings/common/DebugDraw", destroyCS__9DebugDrawPv);

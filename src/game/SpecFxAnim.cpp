#include "common.h"
#include "hieri_types.h"

struct _animHandle {
    int a, b, c, d;
};
void animationStart(_animHandle h, bool loop);

class SpecFxAnim {
public:
    _animHandle anims[3];
    int active;
    char pad34[4];
    _cs *cs;

    void kill(void);
    void launch(void);
    void end(void);
    void startAnim(int i);
    void updateOrient(void);
};

INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", addInstance__10SpecFxAnimP9_hierheadi);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", isInstanceAvailable__10SpecFxAnimP7Monsteri);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", getInstance__10SpecFxAnimP7Monsteri);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", __10SpecFxAnim);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", registerModel__10SpecFxAnimP9_hierheadi);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", initBeforeDbLoad__10SpecFxAnim);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", initAfterDbLoad__10SpecFxAnim);
void SpecFxAnim::launch(void)
{
    updateOrient();
    startAnim(0);
}
void SpecFxAnim::end(void)
{
    startAnim(2);
    active = 0;
    cs->drawMe = 0;
}
void SpecFxAnim::kill(void)
{
    active = 0;
    cs->drawMe = 0;
}
void SpecFxAnim::startAnim(int i)
{
    animationStart(anims[i], true);
}
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", updateOrient__10SpecFxAnim);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", __static_initialization_and_destruction_0_001CE9A8);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", _GLOBAL_$I$_10SpecFxAnim$s_numFxs);

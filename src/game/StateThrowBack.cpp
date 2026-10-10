#include "common.h"

INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __14StateThrowBackQ212MonsterState2IdUi);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", init__14StateThrowBackP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", transitionOK__14StateThrowBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", transitionInto__14StateThrowBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", takeStomp__14StateThrowBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", update__14StateThrowBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", enterSubState__14StateThrowBackQ214StateThrowBack8SubState);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", updateFlail__14StateThrowBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", rampInAdds__14StateThrowBackf);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", rampOutAdds__14StateThrowBackf);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", updateDynamics__14StateThrowBackf);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", updateUpperAdds__14StateThrowBackR8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", updateLowerAdds__14StateThrowBackR8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", rotAroundBaseSphere__14StateThrowBackR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", handleCollis__14StateThrowBackR9_hdResult);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", handleDamage__14StateThrowBackR9_hdResult);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", acceptHit__14StateThrowBackR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", handlePreemption__14StateThrowBackP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __17StateBeingSlammed);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", transitionInto__17StateBeingSlammed);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __16StateBeingThrown);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", transitionInto__16StateBeingThrown);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __14StateKnockBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", transitionInto__14StateKnockBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __17StateGrappleBreak);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", transitionInto__17StateGrappleBreak);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __tf14StateThrowBack);
void setThrowSource__14StateThrowBackP7Monster(void *self, int v) __asm__("setThrowSource__14StateThrowBackP7Monster");
void setThrowSource__14StateThrowBackP7Monster(void *self, int v)
{
    *(int *)((char *)self + 0x524) = v;
}
void setPauseDuration__14StateThrowBackf(void *self, float v) __asm__("setPauseDuration__14StateThrowBackf");
void setPauseDuration__14StateThrowBackf(void *self, float v)
{
    *(float *)((char *)self + 0x53C) = v;
}
void setCounterable__14StateThrowBackb(void *self, int v) __asm__("setCounterable__14StateThrowBackb");
void setCounterable__14StateThrowBackb(void *self, int v)
{
    *(int *)((char *)self + 0x38) = v;
}
int getSubState__14StateThrowBack(void *self) __asm__("getSubState__14StateThrowBack");
int getSubState__14StateThrowBack(void *self)
{
    return *(int *)((char *)self + 0x260);
}
int getThrowSource__14StateThrowBack(void *self) __asm__("getThrowSource__14StateThrowBack");
int getThrowSource__14StateThrowBack(void *self)
{
    return *(int *)((char *)self + 0x524);
}
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", isCounterAvailable__14StateThrowBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", _vt$14StateKnockBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", _vt$17StateGrappleBreak);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", _vt$16StateBeingThrown);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", _vt$17StateBeingSlammed);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", _vt$14StateThrowBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", D_006F1A08);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __tf17StateBeingSlammed);
void setDownTime__17StateBeingSlammedf(void *self, float v) __asm__("setDownTime__17StateBeingSlammedf");
void setDownTime__17StateBeingSlammedf(void *self, float v)
{
    *(float *)((char *)self + 0x48) = v;
}
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __tf16StateBeingThrown);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __tf17StateGrappleBreak);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", __tf14StateKnockBack);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", func_001D6570);
INCLUDE_ASM("asm/nonmatchings/game/StateThrowBack", func_001D6590);

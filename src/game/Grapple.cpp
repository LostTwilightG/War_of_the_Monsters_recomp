#include "common.h"

INCLUDE_ASM("asm/nonmatchings/game/Grapple", __12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", transitionOK__12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", transitionFeasible__12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", transitionInto__12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", update__12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", handlePreemption__12StateGrappleP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", enterSubState__12StateGrappleQ212StateGrapple8SubState);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", detach__12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", release__12StateGrappleb);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", __13StateGrappled);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", transitionOK__13StateGrappledi);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", transitionInto__13StateGrappled);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", update__13StateGrappled);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", healthDeltaAchieved__13StateGrappledf);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", enterSubState__13StateGrappledQ213StateGrappled8SubState);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", detach__13StateGrappled);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", breakFree__13StateGrappled);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", isOverhead__13StateGrappled);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", handlePreemption__13StateGrappledP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", _vt$13StateGrappled);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", _vt$12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", __tf12StateGrapple);
unsigned char getSubState__12StateGrapple(void *self) __asm__("getSubState__12StateGrapple");
unsigned char getSubState__12StateGrapple(void *self)
{
    return *(unsigned char *)((char *)self + 0x6C);
}
INCLUDE_ASM("asm/nonmatchings/game/Grapple", getRecoilFields__12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", getAttachWindowStart__12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", getAttachWindowEnd__12StateGrapple);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", __tf13StateGrappled);
INCLUDE_ASM("asm/nonmatchings/game/Grapple", setStruggleDisable__13StateGrappledi);
unsigned char getSubState__13StateGrappled(void *self) __asm__("getSubState__13StateGrappled");
unsigned char getSubState__13StateGrappled(void *self)
{
    return *(unsigned char *)((char *)self + 0x4C);
}

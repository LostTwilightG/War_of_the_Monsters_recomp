#include "common.h"

INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __14StateAirStrike);
void init__14StateAirStrikeP7Monster(void *self, int v) __asm__("init__14StateAirStrikeP7Monster");
void init__14StateAirStrikeP7Monster(void *self, int v)
{
    *(int *)((char *)self + 0xC) = v;
}
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__14StateAirStrike);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__14StateAirStrike);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__14StateAirStrike);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", flyBeacon__14StateAirStrike);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__14StateAirStrikeP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__14StateAirStrikeR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __14StateBugAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__14StateBugAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__14StateBugAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__14StateBugAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__14StateBugAttackP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__14StateBugAttackR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __17StateCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", init__17StateCrowdControlP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__17StateCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__17StateCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__17StateCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__17StateCrowdControlP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__17StateCrowdControlR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", init__15StateFireBreathP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__15StateFireBreathR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handleApplyMint__15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", init__16StateGrappleHookP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__16StateGrappleHookP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", testCollis__16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getChainEnd__16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", updateHoming__16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__16StateGrappleHookR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", init__14StateLavaBlastP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", enterSubState__14StateLavaBlastQ214StateLavaBlast8SubState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__14StateLavaBlastP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__14StateLavaBlastR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", init__17StateRobotSpecialP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", enterSubState__17StateRobotSpecialQ217StateRobotSpecial8SubState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__17StateRobotSpecialR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__17StateRobotSpecialP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __16StateRockSpecial);
void init__16StateRockSpecialP7Monster(void *self, int v) __asm__("init__16StateRockSpecialP7Monster");
void init__16StateRockSpecialP7Monster(void *self, int v)
{
    *(int *)((char *)self + 0xC) = v;
}
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__16StateRockSpecialP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__16StateRockSpecialR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", init__14StateSonicRoarP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", testConeCollis__14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__14StateSonicRoarP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__14StateSonicRoarR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __14StateZapAttack);
void init__14StateZapAttackP7Monster(void *self, int v) __asm__("init__14StateZapAttackP7Monster");
void init__14StateZapAttackP7Monster(void *self, int v)
{
    *(int *)((char *)self + 0xC) = v;
}
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__14StateZapAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__14StateZapAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__14StateZapAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__14StateZapAttackP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__14StateZapAttackR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __12StateTopSpin);
void init__12StateTopSpinP7Monster(void *self, int v) __asm__("init__12StateTopSpinP7Monster");
void init__12StateTopSpinP7Monster(void *self, int v)
{
    *(int *)((char *)self + 0xC) = v;
}
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__12StateTopSpin);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__12StateTopSpin);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__12StateTopSpin);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__12StateTopSpinP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__12StateTopSpinR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __16StateCannonHands);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", init__16StateCannonHandsP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__16StateCannonHands);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__16StateCannonHands);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__16StateCannonHands);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", enterSubState__16StateCannonHandsQ216StateCannonHands8SubState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__16StateCannonHandsR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", handlePreemption__16StateCannonHandsP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __15StateUltraTazer);
void init__15StateUltraTazerP7Monster(void *self, int v) __asm__("init__15StateUltraTazerP7Monster");
void init__15StateUltraTazerP7Monster(void *self, int v)
{
    *(int *)((char *)self + 0xC) = v;
}
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionOK__15StateUltraTazer);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", transitionInto__15StateUltraTazer);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__15StateUltraTazer);
void testCollis__15StateUltraTazer(void *self) __asm__("testCollis__15StateUltraTazer");
void testCollis__15StateUltraTazer(void *self)
{
}
void handlePreemption__15StateUltraTazerP12MonsterState(void *self) __asm__("handlePreemption__15StateUltraTazerP12MonsterState");
void handlePreemption__15StateUltraTazerP12MonsterState(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", acceptHit__15StateUltraTazerR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", func_001CDE40);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", func_001CDE48);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$15StateUltraTazer);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$16StateCannonHands);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$12StateTopSpin);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$14StateZapAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$17StateCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$14StateBugAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", _vt$14StateAirStrike);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf14StateAirStrike);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", flyBeacon__14StateAirStrikePv);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__14StateAirStrike);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__14StateAirStrike);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf14StateBugAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__14StateBugAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__14StateBugAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf17StateCrowdControl);
void setFxEp__17StateCrowdControlP9_hierhead(void *self, int v) __asm__("setFxEp__17StateCrowdControlP9_hierhead");
void setFxEp__17StateCrowdControlP9_hierhead(void *self, int v)
{
    *(int *)((char *)self + 0x2F8) = v;
}
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getMaxRange__17StateCrowdControl);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__15StateFireBreath);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__16StateGrappleHook);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getOnFireDamage__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getOnFireDuration__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getImpactDamage__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getSpeedMPH__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getHorizontalHoming__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getVerticalHoming__14StateLavaBlast);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getMissileDuration__17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getMissileMaxSpeedMPH__17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getMissileHomingFactor__17StateRobotSpecial);
void * getShockConfig__17StateRobotSpecial(void *self) __asm__("getShockConfig__17StateRobotSpecial");
void * getShockConfig__17StateRobotSpecial(void *self)
{
    return (char *)self + 0x58;
}
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getFxAnimSpeed__17StateRobotSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getHeadDuration__16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getKnockBackUp__16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", getKnockBackOut__16StateRockSpecial);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__14StateSonicRoar);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf14StateZapAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindUp__14StateZapAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", inWindDown__14StateZapAttack);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf12StateTopSpin);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf16StateCannonHands);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", __tf15StateUltraTazer);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", update__8OgreMacePv);
INCLUDE_ASM("asm/nonmatchings/game/SpecialStates", func_001CE450);

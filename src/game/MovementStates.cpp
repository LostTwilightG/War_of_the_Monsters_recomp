#include "common.h"
#include "game/game.h"
#include "game/monster_state.h"
#include "game/pad_flags.h"
#include "game/pickup.h"

/* Movement states: a state's tunables are blocks inside the object ("configs"); jumping and flying pick the heavy block while holding a monster or a two-handed pickup. */
class JumpFlyBase : public MonsterState {
public:
};
class StateFly : public JumpFlyBase {
public:
    char *getRelevantConfig(void);
    float getJumpHeightGain(void);
    float getFlapHeightGain(void);
    float getFlapStaminaDrain(void);
    int transitionFeasible(void);
};
class StateJump : public JumpFlyBase {
public:
    char *getRelevantConfig(void);
    float getJumpHeightGain(void);
    int transitionFeasible(void);
};
class StateRamAttack : public MonsterState {
public:
    void setMaxSpeedMPH(float mph);
    void handlePreemption(MonsterState *next);
};
class StateDash : public MonsterState {
public:
    void handlePreemption(MonsterState *next);
};
class StateClimb : public MonsterState {
public:
    void handlePreemption(MonsterState *next);
};
class MonsterSound {
public:
    void terminateDashSound(void);
};
void particleKillFx(int &handle);
#define SCFG(p, o) (*(float *)((char *)(p) + (o)))

INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __13StateButtSlam);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionOK__13StateButtSlam);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionFeasible__13StateButtSlam);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionInto__13StateButtSlam);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", update__13StateButtSlam);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handleCollis__13StateButtSlamR9_hdResult);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", acceptHit__13StateButtSlamR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handlePreemption__13StateButtSlamP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", init__10StateClimbP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionOK__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionFeasible__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionInto__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", update__10StateClimb);
#ifdef NON_MATCHING
void StateClimb::handlePreemption(MonsterState *next)
{
    float *q = (float *)((char *)this + 0x90);

    *(float *)((char *)owner + 0x1B8) = 1.0f;
    *(int *)((char *)owner + 0x280) = 1;
    q[0] = 0.0f;
    q[1] = 0.0f;
    q[2] = 0.0f;
    q[3] = 1.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handlePreemption__10StateClimbP12MonsterState);
#endif
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", enterSubState__10StateClimbQ210StateClimb8SubState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", checkWallContact__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getMinContact__10StateClimbP9_hdResultib);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __9StateDash);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionOK__9StateDash);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionInto__9StateDash);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", update__9StateDash);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", enterSubState__9StateDashQ29StateDash8SubState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handleCollis__9StateDashR9_hdResult);
void StateDash::handlePreemption(MonsterState *next)
{
    ((MonsterSound *)((char *)owner + 0x1A7C))->terminateDashSound();
    *(int *)((char *)owner + 0x484) = 1;
}
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionInto__11JumpFlyBase);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", update__11JumpFlyBase);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handleStateTransitions__11JumpFlyBase);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __8StateFly);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", init__8StateFlyP7Monster);
#ifdef NON_MATCHING
/* Height reached by the jump launch speed (config +0xC) against gravity: v^2 / (2g), g being negative. */
float StateFly::getJumpHeightGain(void)
{
    float v = SCFG(getRelevantConfig(), 0xC);

    return -(v * v) / (2.0f * game->m_gravity);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getJumpHeightGain__8StateFly);
#endif
#ifdef NON_MATCHING
float StateFly::getFlapHeightGain(void)
{
    float v = SCFG(getRelevantConfig(), 0x14);

    return -(v * v) / (2.0f * game->m_gravity);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getFlapHeightGain__8StateFly);
#endif
float StateFly::getFlapStaminaDrain(void)
{
    return SCFG(getRelevantConfig(), 0x10);
}
#ifdef NON_MATCHING
char *StateFly::getRelevantConfig(void)
{
    char *m = (char *)owner;

    if (*(int *)(m + 0x68B4) == 0) {
        int heavy = 0;

        if (*(int *)(m + 0x68A4) != 0)
            heavy = (int)((**(Pickup ***)(m + 0x68A4))->bits >> 1) & 1;
        if (heavy)
            return (char *)this + 0x7C;
        return (char *)this + 0x44;
    }
    return (char *)this + 0x7C;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getRelevantConfig__8StateFly);
#endif
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionOK__8StateFly);
#ifdef NON_MATCHING
/* Jumping/flying is only possible while the pad entry one frame back has its field 0x2A at 0. */
int StateFly::transitionFeasible(void)
{
    return *(short *)((char *)(*(PadFlags *)((char *)owner + 0x5040))[1] + 0x2A) == 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionFeasible__8StateFly);
#endif
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionInto__8StateFly);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", update__8StateFly);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", enterSubState__8StateFlyQ211JumpFlyBase8SubState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handleCollis__8StateFlyR9_hdResult);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handlePreemption__8StateFlyP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __9StateJump);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", init__9StateJumpP7Monster);
#ifdef NON_MATCHING
/* Height reached by the jump launch speed (config +0xC) against gravity: v^2 / (2g), g being negative. */
float StateJump::getJumpHeightGain(void)
{
    float v = SCFG(getRelevantConfig(), 0xC);

    return -(v * v) / (2.0f * game->m_gravity);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getJumpHeightGain__9StateJump);
#endif
#ifdef NON_MATCHING
char *StateJump::getRelevantConfig(void)
{
    char *m = (char *)owner;

    if (*(int *)(m + 0x68B4) == 0) {
        int heavy = 0;

        if (*(int *)(m + 0x68A4) != 0)
            heavy = (int)((**(Pickup ***)(m + 0x68A4))->bits >> 1) & 1;
        if (heavy)
            return (char *)this + 0x68;
        return (char *)this + 0x38;
    }
    return (char *)this + 0x68;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getRelevantConfig__9StateJump);
#endif
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionOK__9StateJump);
#ifdef NON_MATCHING
/* Jumping/flying is only possible while the pad entry one frame back has its field 0x2A at 0. */
int StateJump::transitionFeasible(void)
{
    return *(short *)((char *)(*(PadFlags *)((char *)owner + 0x5040))[1] + 0x2A) == 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionFeasible__9StateJump);
#endif
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionInto__9StateJump);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", update__9StateJump);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", enterSubState__9StateJumpQ211JumpFlyBase8SubState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handleCollis__9StateJumpR9_hdResult);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handlePreemption__9StateJumpP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __14StateRamAttack);
void StateRamAttack::setMaxSpeedMPH(float mph)
{
    SCFG(this, 0x30) = mph;
    owner->recomputeDynamics();
}
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionOK__14StateRamAttack);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionInto__14StateRamAttack);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", update__14StateRamAttack);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handleCollis__14StateRamAttackR9_hdResult);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", enterSubState__14StateRamAttackQ214StateRamAttack8SubState);
/* Another state takes over: the ram's three effects stop. */
void StateRamAttack::handlePreemption(MonsterState *next)
{
    *(int *)((char *)owner + 0x484) = 1;
    particleKillFx(*(int *)((char *)this + 0xB0));
    particleKillFx(*(int *)((char *)this + 0xB4));
    particleKillFx(*(int *)((char *)this + 0xB8));
}
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", acceptHit__14StateRamAttackR8HitEvent);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __8StateRun);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", init__8StateRunP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionOK__8StateRun);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", transitionInto__8StateRun);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", update__8StateRun);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", enterSubState__8StateRunQ28StateRun8SubState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", handlePreemption__8StateRunP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getAnimSpeed__8StateRunf);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", func_001769B8);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", func_001769C0);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", func_001769C8);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", D_006ED4B0);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", D_006ED4F0);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", _vt$8StateRun);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", _vt$14StateRamAttack);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", _vt$9StateJump);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", _vt$8StateFly);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", _vt$11JumpFlyBase);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", _vt$9StateDash);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", _vt$10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", _vt$13StateButtSlam);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __tf13StateButtSlam);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __tf10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getSubState__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getClimbMotion__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getHuckVelocity__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getContactNormal__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getContactInteractive__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getContactFlags__10StateClimb);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __tf9StateDash);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getSubState__9StateDash);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __tf11JumpFlyBase);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __11JumpFlyBaseQ212MonsterState2IdQ212MonsterState4Caps);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", setFootPref__11JumpFlyBaseQ211JumpFlyBase8FootPref);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getSubState__11JumpFlyBase);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getApexHeight__11JumpFlyBase);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getHeightGain__11JumpFlyBase);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", D_006ED800);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __tf8StateFly);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getSubState__8StateFly);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __tf9StateJump);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __tf14StateRamAttack);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", setRamDuration__14StateRamAttackf);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", setAirDeccel__14StateRamAttackf);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", setAcceleration__14StateRamAttackf);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", __tf8StateRun);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", setLandMomentum__8StateRunf);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", getAnimIndex__8StateRun);
INCLUDE_ASM("asm/nonmatchings/game/MovementStates", func_00176DA0);

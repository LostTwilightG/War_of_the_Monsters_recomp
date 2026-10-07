#include "common.h"
#include "game/game.h"
#include "game/fire_breath.h"

INCLUDE_ASM("asm/nonmatchings/game/Monster", setWaterLevel__Ff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getWaterLevel__Fv);
INCLUDE_ASM("asm/nonmatchings/game/Monster", isUnderwater__Ff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", recomputeDynamics__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", playerUpdateInputs__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", update__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", startCinema__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateCinema__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", endCinema__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateBoundingSphere__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateReticle__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", setReticles__7Monsteri);
INCLUDE_ASM("asm/nonmatchings/game/Monster", setTrans__7MonsterR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/Monster", setRot__7Monsterfff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", setMat__7MonsterRA3_A3_f);
INCLUDE_ASM("asm/nonmatchings/game/Monster", setEnvMapping__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", clearEnvMapping__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateShadow__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", calcGlowIntensity__Fiii);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updatePowerUpGlow__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateBoostAndRage__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateLock__7Monster19MonsterReticleState);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateLookAt__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateAirLegOverride__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateAnimContacts__7Monsterb);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getStaminaGain__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getTarget__7MonsterR8_fvectorb);
INCLUDE_ASM("asm/nonmatchings/game/Monster", throwPickup__7Monsterif);
INCLUDE_ASM("asm/nonmatchings/game/Monster", dropPickup__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", launchDefaultProjectile__7Monsteri);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateDefaultProjectile__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updatePosition__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", takeHit__7MonsterP8_fvectorfi);
INCLUDE_ASM("asm/nonmatchings/game/Monster", takeDamage__7MonsterfbP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", takeAdditiveRecoil__7MonsterR8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/Monster", knockBack__7MonsterR8_fvectorff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", blowUpMonster__7MonsterP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateDeathSequence__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", registerComboHit__7MonsterP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestMonster__7Monsterf);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestMonster2D__7Monsterf);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestMonster__7Monsteriif);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestMonsterWithLos__7Monsterf);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestMonsterToOrientation__7Monsterff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestMonsterToOrientation__7Monsteriiff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestMonsterToPunch__7Monsterfff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestMonsterInFOV__7MonsterR8_fvectorfT1fRf);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestTargetable__7MonsterUsbff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getClosestTargetable__7MonsterUsbfff);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getLookAtTarget__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getDestructibleFromReticle__7Monsterf);
INCLUDE_ASM("asm/nonmatchings/game/Monster", addAttachment__7MonsterP9_hierhead);
INCLUDE_ASM("asm/nonmatchings/game/Monster", handleAction__7MonsterP15ActAiNavigation);
INCLUDE_ASM("asm/nonmatchings/game/Monster", attachPickupImpaler__7MonsterGQ2t10LinkedList1ZP6Pickup8IteratorP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/Monster", detachPickupImpaler__7Monsterb);
INCLUDE_ASM("asm/nonmatchings/game/Monster", dropPickupImpaler__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", handleLocator__7MonsterUiRA3_A3_fP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/Monster", enterNewState__7MonsterP12MonsterState);
INCLUDE_ASM("asm/nonmatchings/game/Monster", landingShake__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getCollisionDamage__7Monsterf);
INCLUDE_ASM("asm/nonmatchings/game/Monster", creditStamina__7Monsterfb);
INCLUDE_ASM("asm/nonmatchings/game/Monster", creditHealth__7Monsterf);
INCLUDE_ASM("asm/nonmatchings/game/Monster", breathFire__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", lightOnFire__7Monsterffi);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateOnFire__7Monster);
void Monster::startShocking(float a, float b)
{
    m_beingShockedCount = a;
    m_beingShockedDamage = b;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateBeingShocked__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", startBeingImpaled__7Monsterff);
bool Monster::isHolding(void)
{
    return m_pickup != 0 || m_target != 0;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", isHoldingLarge__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", isBlocking__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", isIdle__7MonsterUi);
INCLUDE_ASM("asm/nonmatchings/game/Monster", inCameraFov__7MonsterR8_fvectorT1);
bool Monster::hasPinTarget(void)
{
    char *pin = (char *)m_pinTarget;

    return pin && (*(unsigned short *)(pin + 4) & 2);
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateClosestPath__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", okToDrawReticle__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", setCloakOn__7Monster);
void Monster::setCloakOff(void)
{
    m_cloaked = 0;
    m_cloakTime = 0;
    clearEnvMapping();
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", attachFxToHandle__FPiP8_fvectorUi);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateWaterWake__7Monsterfb);
INCLUDE_ASM("asm/nonmatchings/game/Monster", cleanUpForMovie__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", setShadowOnOff__7Monsterb);
INCLUDE_ASM("asm/nonmatchings/game/Monster", setSecondaryShadowBlocker__7MonsterP3_cs);
INCLUDE_ASM("asm/nonmatchings/game/Monster", drainSpecial__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", isSpecialAvailable__C7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", func_00163000);
INCLUDE_ASM("asm/nonmatchings/game/Monster", func_00163020);
INCLUDE_ASM("asm/nonmatchings/game/Monster", _vt$7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", __tf7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", __7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateTurn__7Monsterb);
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateMove__7Monsterb);
void Monster::stopFireBreath(void)
{
    ((FireBreath *)((char *)this + 0x68C0))->ApplyMint();
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", putOutFire__7Monster);
void *Monster::getMotionRot(void)
{
    return (char *)this + 0xA0;
}
void *Monster::getPrevTrans(void)
{
    return (char *)this + 0x90;
}
void *Monster::getPrevMat(void)
{
    return (char *)this + 0x50;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", getLocatorTrans__7Monsteri);
INCLUDE_ASM("asm/nonmatchings/game/Monster", getLocatorMat__7Monsteri);
void *Monster::getPinTrans(void)
{
    return (char *)this + 0x3E30;
}
void *Monster::getLookAtTrans(void)
{
    return (char *)this + 0x3E80;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", getAnim__7Monster11MonsterAnim);
void *Monster::getDynamics(void)
{
    return (char *)this + 0x100;
}
void *Monster::getVel(void)
{
    return (char *)this + 0x260;
}
int Monster::getPickup(void)
{
    return m_pickup;
}
int Monster::getImpaler(void)
{
    return m_impaler;
}
int Monster::getReverseImpaler(void)
{
    return m_reverseImpaler;
}
void * Monster::getGrapplee(void)
{
    return m_target;
}
Monster * Monster::getGrappler(void)
{
    return m_grappler;
}
Monster * Monster::getGrappleAttempt(void)
{
    return m_grappleAttempt;
}
Monster * Monster::getBeamVictim(void)
{
    return m_beamVictim;
}
#ifdef NON_MATCHING
/* 2/4 words, untuned: retail splits the offset as 0x8450 + 0x1C */
Monster *Monster::getKiller(void)
{
    return m_killer;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Monster", getKiller__7Monster);
#endif
int Monster::getPinTarget(void)
{
    return m_pinTarget;
}
float Monster::getPinTime(void)
{
    return m_pinTime;
}
PlayerDat * Monster::getPlayerInfo(void)
{
    return m_playerInfo;
}
void *Monster::getAi(void)
{
    return (char *)this + 0x4E0;
}
_cs * Monster::getReticleCS(void)
{
    return m_reticleCS;
}
_cs * Monster::getStickyReticleCS(void)
{
    return m_stickyReticleCS;
}
_cs *Monster::getShadow(void)
{
    return m_shadow;
}
void *Monster::getFireBreath(void)
{
    return (char *)this + 0x68C0;
}
void *Monster::getMonsterSound(void)
{
    return (char *)this + 0x1A7C;
}
void *Monster::getStaminaMeter(void)
{
    return (char *)this + 0x460;
}
void *Monster::getHealthMeter(void)
{
    return (char *)this + 0x448;
}
void *Monster::getLeadVec(void)
{
    return (char *)this + 0x6990;
}
int Monster::getAutoLeadMovesReticle(void)
{
    return m_autoLeadMovesReticle;
}
void *Monster::getReticleLosResult(void)
{
    return (char *)this + 0x6C50;
}
void *Monster::getShadowHDResult(void)
{
    return (char *)this + 0x1A40;
}
AiPath *Monster::getClosestPath(void)
{
    return m_closestPath;
}
int Monster::getReticleState(void) const
{
    return m_reticleState;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", getEnemyInfo__7MonsterR7Monster);
int *Monster::getState(void)
{
    return m_state;
}
int Monster::getStateId(void)
{
    return *m_state;
}
int Monster::getPrevStateId(void)
{
    return *m_prevState;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", getCameraData__7MonsteriQ26Camera9CameraPOV);
int Monster::getType(void) const
{
    return m_playerNum;
}
int Monster::getIndex(void) const
{
    return m_index;
}
int Monster::getName(void) const
{
    return m_typeBits;
}
int Monster::getDupId(void) const
{
    return m_dupId;
}
int Monster::getNumInits(void) const
{
    return m_numInits;
}
int Monster::getMonsterNum(void) const
{
    return m_monsterNum;
}
int Monster::getSkinNum(void) const
{
    return m_skinNum;
}
float Monster::getSpeed(void)
{
    return m_speed;
}
float Monster::getHealth(void)
{
    return m_health;
}
float Monster::getMaxHealth(void)
{
    return m_maxHealth;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", getStamina__7Monster);
float Monster::getHeight(void) const
{
    return m_bodyHeight + m_heightAboveCOG;
}
float Monster::getWidth(void) const
{
    return m_width;
}
int Monster::getCameraThatFollows(void) const
{
    return m_cameraView;
}
int Monster::getWinsThisGame(void) const
{
    return m_winsThisGame;
}
float Monster::getClimbSpeed(void) const
{
    return m_climbSpeed;
}
float Monster::getClimbStrafeSpeed(void) const
{
    return m_climbStrafeSpeed;
}
int Monster::getFallTime(void) const
{
    return m_fallTime;
}
float Monster::getGroundHeight(void) const
{
    return *(float *)((char *)m_shadow + 0x18);
}
float Monster::getHeightAboveCOG(void) const
{
    return m_heightAboveCOG;
}
float Monster::getRunTime(void) const
{
    return m_runTime;
}
int Monster::getPlayerAiOrFodderNum(void) const
{
    return m_index;
}
int Monster::getIsCameraFollowingThisMonster(void)
{
    return m_cameraFollows;
}
float Monster::puPunchDamageMod(ePickupType t) const
{
    return m_puPunchDamageMod[t];
}
float Monster::puLaunchDamageMod(ePickupType t) const
{
    return m_puLaunchDamageMod[t];
}
float Monster::puDurationMod(ePickupType t) const
{
    return m_puDurationMod[t];
}
float Monster::puSpeedMod(ePickupType t) const
{
    return m_puSpeedMod[t];
}
float Monster::getDpDamage(void) const
{
    return m_dpDamage;
}
float Monster::getDpDuration(void) const
{
    return m_dpDuration;
}
float Monster::getDpSpeed(void) const
{
    return m_dpSpeed;
}
float Monster::getDpHomingFactor(void) const
{
    return m_dpHomingFactor;
}
float Monster::getDpVertHomingFactor(void) const
{
    return m_dpVertHomingFactor;
}
float Monster::getDpHeadingBreak(void) const
{
    return m_dpHeadingBreak;
}
float Monster::getDpPitchBreak(void) const
{
    return m_dpPitchBreak;
}
int Monster::getCamIdleCircuitTime(void) const
{
    return m_camIdleCircuitTime;
}
float Monster::getCamIdleFactor(void) const
{
    return m_camIdleFactor;
}
float Monster::getLandingShakeAmp(void) const
{
    return m_landingShakeAmp;
}
float Monster::getLandingShakeFreq(void) const
{
    return m_landingShakeFreq;
}
float Monster::getLandingShakeDur(void) const
{
    return m_landingShakeDur;
}
float Monster::getLandingShakeFalloff(void) const
{
    return m_landingShakeFalloff;
}
float Monster::getLandingShakeMag(void) const
{
    return m_landingShakeMag;
}
float Monster::getPitchRate1(void) const
{
    return m_pitchRate1;
}
float Monster::getPitchRate2(void) const
{
    return m_pitchRate2;
}
float Monster::getMaxHeadingChange(void) const
{
    return m_maxHeadingChange;
}
float Monster::getAimPitch(void) const
{
    return m_aimPitch;
}
float Monster::getAimHeading(void) const
{
    return m_aimHeading;
}
float Monster::getRearOffset(void) const
{
    return m_rearOffset;
}
float Monster::getTargetingMod(void) const
{
    return m_targetingMod;
}
int Monster::getFallTimeBeforePitch(void) const
{
    return m_fallTimeBeforePitch;
}
float Monster::getPinMaxPitch(void) const
{
    return m_pinMaxPitch;
}
float Monster::getPinMuckingDist(void) const
{
    return m_pinMuckingDist;
}
float Monster::getOnFireCount(void) const
{
    return m_onFireCount;
}
float Monster::getOnFireDamage(void) const
{
    return m_onFireDamage;
}
float Monster::getBeingShockedCount(void) const
{
    return m_beingShockedCount;
}
float Monster::getBeingShockedDamage(void) const
{
    return m_beingShockedDamage;
}
int Monster::getLaunchCounter(void) const
{
    return m_launchCounter;
}
int Monster::getLaunchDelay(void) const
{
    return m_launchDelay;
}
bool Monster::isVulnerable(void) const
{
    return m_unk49 != 0;
}
bool Monster::isDead(void) const
{
    return m_dead != 0;
}
bool Monster::isCloaked(void) const
{
    return m_cloaked != 0;
}
bool Monster::isTurning(void) const
{
    return m_turning != 0;
}
bool Monster::isFalling(void) const
{
    return m_freeFalling != 0;
}
bool Monster::attacksEnabled(void) const
{
    return m_attacksEnabled != 0;
}
bool Monster::ranDeathSequence(void) const
{
    return m_unkF6 != 0;
}
bool Monster::ranVictorySequence(void) const
{
    return m_unkF7 != 0;
}
#ifdef NON_MATCHING
/* 6/9 words, untuned: branch shape */
bool Monster::isFullHealth(void)
{
    return m_maxHealth <= m_health;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Monster", isFullHealth__7Monster);
#endif
#ifdef NON_MATCHING
/* 7/10 words, untuned: branch shape */
bool Monster::isFullStamina(void)
{
    return m_stamina.maxLevel <= m_stamina.cur;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Monster", isFullStamina__7Monster);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Monster", isTargetPinning__7Monster);
bool Monster::isOnFire(void)
{
    return m_onFireCount > 0.0f;
}
bool Monster::isBeingShocked(void)
{
    return m_beingShockedCount > 0.0f;
}
bool Monster::inSpecialState(void)
{
    return m_state == m_specialState;
}
void Monster::setCameraFollowsMonster(int view, bool follows)
{
    m_cameraView = view;
    m_cameraFollows = follows;
}
void Monster::setCs(_cs * v)
{
    m_cs = v;
}
void Monster::setGodMode(bool v)
{
    m_godMode = v;
}
void Monster::setWinsThisGame(int v)
{
    m_winsThisGame = v;
}
void Monster::setDamageModifier(float v)
{
    m_damageModifier = v;
}
void Monster::setRanDeathSequence(bool v)
{
    m_unkF6 = v;
}
void Monster::setRanVictorySequence(bool v)
{
    m_unkF7 = v;
}
void Monster::setDead(bool v)
{
    m_dead = v;
}
void Monster::setTurning(bool v)
{
    m_turning = v;
}
void Monster::setPadEnabled(bool v)
{
    m_unkF9 = v;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", setPickup__7MonsterGQ2t10LinkedList1ZP6Pickup8Iterator);
void Monster::setGrapplee(Monster * v)
{
    m_target = v;
}
void Monster::setGrappler(Monster * v)
{
    m_grappler = v;
}
void Monster::setGrappleAttempt(Monster * v)
{
    m_grappleAttempt = v;
}
void Monster::setBeamVictim(Monster * v)
{
    m_beamVictim = v;
}
void Monster::setFreeFalling(bool v)
{
    m_freeFalling = v;
}
void Monster::setFallTime(int v)
{
    m_fallTime = v;
}
void Monster::setDupId(int v)
{
    m_dupId = v;
}
void Monster::setTypeOfMonster(int v)
{
    m_playerNum = v;
}
void Monster::setMonsterNum(int v)
{
    m_monsterNum = v;
}
void Monster::setSkinNum(int v)
{
    m_skinNum = v;
}
void Monster::setPlayerAiOrFodderNum(int v)
{
    m_index = v;
}
void Monster::setPlayerInfo(PlayerDat * v)
{
    m_playerInfo = v;
}
void Monster::setInteractiveIndex(int v)
{
    m_id = v;
}
int Monster::getInteractiveIndex(void)
{
    return m_id;
}
void Monster::setName(int v)
{
    m_typeBits = v;
}
void Monster::setPitchRate1(float v)
{
    m_pitchRate1 = v;
}
void Monster::setPitchRate2(float v)
{
    m_pitchRate2 = v;
}
void Monster::setCamIdleCircuitTime(int v)
{
    m_camIdleCircuitTime = v;
}
void Monster::setCamIdleFactor(float v)
{
    m_camIdleFactor = v;
}
void Monster::setClosestPath(AiPath * v)
{
    m_closestPath = v;
}
void Monster::setVulnerable(bool v)
{
    m_unk49 = v;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", setInvulnerabilityDuration__7Monsteri);
void Monster::setAttacksEnabled(bool v)
{
    m_attacksEnabled = v;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", setAttackDisableDuration__7Monsteri);
void Monster::setAimHeadingEnabled(bool v)
{
    m_aimHeadingEnabled = v;
}
void Monster::setLookAtOverride(_fvector * v)
{
    m_lookAtOverride = v;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", drainStamina__7Monsterfb);
INCLUDE_ASM("asm/nonmatchings/game/Monster", enableStaminaRegen__7Monsterb);
void Monster::enableSpecialWeapon(bool v)
{
    m_specialWeapon = v;
}
void Monster::startHealthPowerUpGlow(int a, int b)
{
    m_healthGlow[0] = a;
    m_healthGlow[2] = b;
    m_healthGlow[1] = b;
}
void Monster::startStaminaPowerUpGlow(int a, int b)
{
    m_staminaGlow[0] = a;
    m_staminaGlow[2] = b;
    m_staminaGlow[1] = b;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", startSpecialPowerUpGlow__7Monsteri);
INCLUDE_ASM("asm/nonmatchings/game/Monster", endSpecialPowerUpGlow__7Monster);
void Monster::incrementWinsThisGame(int n)
{
    m_winsThisGame += n;
}
int Monster::getHudTexture(void)
{
    return m_hudTexture;
}
void Monster::okToGlow(bool v)
{
    m_okToGlow = v;
}
void Monster::okToUnify(bool v)
{
    m_camUnify = v;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", updateClosestPath__7MonsterPv);
INCLUDE_ASM("asm/nonmatchings/game/Monster", restoreVulnerability__7MonsterPv);
int Monster::restoreVulnerability(void)
{
    m_unk49 = 1;
    return 0;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", restoreAttacksEnabled__7MonsterPv);
int Monster::restoreAttacksEnabled(void)
{
    m_attacksEnabled = 1;
    return 0;
}
void Monster::setShadow(_cs * v)
{
    m_shadow = v;
}
void Monster::setHudTexture(int v)
{
    m_hudTexture = v;
}
void Monster::setReticleCS(_cs * v)
{
    m_reticleCS = v;
}
void Monster::setStickyReticleCS(_cs * v)
{
    m_stickyReticleCS = v;
}
bool Monster::lyingOnGround(void)
{
    int *st = m_state;
    bool r = false;

    if (st[1] & 0x10)
        r = *(int *)((char *)st + 0x260) == 2;
    return r;
}
void *Monster::getFootHDResult(void)
{
    return (char *)this + 0x3B0;
}
INCLUDE_ASM("asm/nonmatchings/game/Monster", func_00163CB8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC070);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC0A0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC0D0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC0F8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC108);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC128);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC148);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC160);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC178);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC198);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC1C0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC1E0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC200);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC220);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC238);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC250);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC270);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC288);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC2A0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC2C0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC2E0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC2F0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC310);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC330);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC350);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC370);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC388);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC3A8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC3C0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC3D0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC3F8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC420);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC440);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC460);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC478);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC490);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC4A0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC4B8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC4D0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC4E8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC4F8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC508);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC518);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC528);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC538);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC550);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC570);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC590);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC5A8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC5D0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC600);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC618);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC628);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC638);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC648);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC658);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC678);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC698);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC6A8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC6C8);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC6F0);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC700);
INCLUDE_ASM("asm/nonmatchings/game/Monster", D_006EC710);

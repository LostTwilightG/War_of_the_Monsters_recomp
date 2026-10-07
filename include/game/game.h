#ifndef GAME_H
#define GAME_H

#include "engine.h"
#include "game/hud.h"
#include "game/weapons.h"
#include "game/stamina_meter.h"
#include "game/pad_flags.h"
#include "game/token_manager.h"
#include "hieri_types.h"

/* Partial class layouts recovered from usage. Unknown regions are padding until identified. */

class MonsterState;

class Monster {
public:
    void drainSpecial();
    void enterNewState(MonsterState *state);
    void takeDamage(float dmg, bool b, Monster *src);
    void initAfterDbLoad(void);
    void update(void);
    void updateCinema(void);
    void updatePosition(void);
    float getAimHeading(void) const;
    float getAimPitch(void) const;
    float getBeingShockedCount(void) const;
    float getBeingShockedDamage(void) const;
    float getCamIdleFactor(void) const;
    float getClimbSpeed(void) const;
    float getClimbStrafeSpeed(void) const;
    float getDpDamage(void) const;
    float getDpDuration(void) const;
    float getDpHeadingBreak(void) const;
    float getDpHomingFactor(void) const;
    float getDpPitchBreak(void) const;
    float getDpSpeed(void) const;
    float getDpVertHomingFactor(void) const;
    float getHealth(void);
    float getHeightAboveCOG(void) const;
    float getLandingShakeAmp(void) const;
    float getLandingShakeDur(void) const;
    float getLandingShakeFalloff(void) const;
    float getLandingShakeFreq(void) const;
    float getLandingShakeMag(void) const;
    float getMaxHeadingChange(void) const;
    float getMaxHealth(void);
    float getOnFireCount(void) const;
    float getOnFireDamage(void) const;
    float getPinMaxPitch(void) const;
    float getPinMuckingDist(void) const;
    float getPinTime(void);
    float getPitchRate1(void) const;
    float getPitchRate2(void) const;
    float getRearOffset(void) const;
    float getRunTime(void) const;
    float getSpeed(void);
    float getTargetingMod(void) const;
    float getWidth(void) const;
    int getAutoLeadMovesReticle(void);
    int getBeamVictim(void);
    int getCamIdleCircuitTime(void) const;
    int getCameraThatFollows(void) const;
    int getClosestPath(void);
    int getDupId(void) const;
    int getFallTime(void) const;
    int getFallTimeBeforePitch(void) const;
    int getGrappleAttempt(void);
    int getGrappler(void);
    int getHudTexture(void);
    int getImpaler(void);
    int getIndex(void) const;
    int getInteractiveIndex(void);
    int getIsCameraFollowingThisMonster(void);
    int getLaunchCounter(void) const;
    int getLaunchDelay(void) const;
    int getMonsterNum(void) const;
    int getName(void) const;
    int getNumInits(void) const;
    int getPickup(void);
    int getPinTarget(void);
    int getPlayerAiOrFodderNum(void) const;
    int getPlayerInfo(void);
    int getReticleCS(void);
    int getReticleState(void) const;
    int getReverseImpaler(void);
    int getShadow(void);
    int getSkinNum(void) const;
    int getState(void);
    int getStickyReticleCS(void);
    int getType(void) const;
    int getWinsThisGame(void) const;
    void * getGrapplee(void);
    void enableSpecialWeapon(bool v);
    void okToGlow(bool v);
    void okToUnify(bool v);
    void setAimHeadingEnabled(bool v);
    void setAttacksEnabled(bool v);
    void setCamIdleCircuitTime(int v);
    void setCamIdleFactor(float v);
    void setDamageModifier(float v);
    void setDead(bool v);
    void setDupId(int v);
    void setFallTime(int v);
    void setFreeFalling(bool v);
    void setGodMode(bool v);
    void setHudTexture(int v);
    void setInteractiveIndex(int v);
    void setMonsterNum(int v);
    void setName(int v);
    void setPadEnabled(bool v);
    void setPitchRate1(float v);
    void setPitchRate2(float v);
    void setPlayerAiOrFodderNum(int v);
    void setRanDeathSequence(bool v);
    void setRanVictorySequence(bool v);
    void setSkinNum(int v);
    void setTurning(bool v);
    void setTypeOfMonster(int v);
    void setVulnerable(bool v);
    void setWinsThisGame(int v);

    char pad0[0xC - 0x0];
    _cs * m_cs;   /* 0xC */
    char pad10[0x14 - 0x10];
    int m_typeBits;   /* 0x14 */
    int m_playerNum;   /* 0x18 */
    int m_dupId;   /* 0x1C */
    int m_id;   /* 0x20 */
    int m_index;   /* 0x24 */
    int m_monsterNum;   /* 0x28 */
    int m_numInits;   /* 0x2C */
    int m_skinNum;   /* 0x30 */
    int m_state;   /* 0x34 */
    char pad38[0x3C - 0x38];
    int m_unk3C;   /* 0x3C */
    float m_runTime;   /* 0x40 */
    char pad44[0x49 - 0x44];
    unsigned char m_unk49;   /* 0x49 */
    unsigned char m_attacksEnabled;   /* 0x4A */
    char pad4B[0xB0 - 0x4B];
    float m_maxHeadingChange;   /* 0xB0 */
    char padB4[0xB8 - 0xB4];
    float m_heightAboveCOG;   /* 0xB8 */
    char padBC[0xC0 - 0xBC];
    float m_rearOffset;   /* 0xC0 */
    float m_width;   /* 0xC4 */
    char padC8[0xDC - 0xC8];
    float m_pitchRate1;   /* 0xDC */
    float m_pitchRate2;   /* 0xE0 */
    char padE4[0xE8 - 0xE4];
    signed char m_dead;   /* 0xE8 */
    unsigned char m_godMode;   /* 0xE9 */
    char padEA[0xEC - 0xEA];
    unsigned char m_unkEC;   /* 0xEC */
    char padED[0xF1 - 0xED];
    unsigned char m_turning;   /* 0xF1 */
    char padF2[0xF3 - 0xF2];
    unsigned char m_specialWeapon;   /* 0xF3 */
    char padF4[0xF6 - 0xF4];
    unsigned char m_unkF6;   /* 0xF6 */
    unsigned char m_unkF7;   /* 0xF7 */
    char padF8[0xF9 - 0xF8];
    unsigned char m_unkF9;   /* 0xF9 */
    char padFA[0x250 - 0xFA];
    float m_speed;   /* 0x250 */
    char pad254[0x280 - 0x254];
    unsigned char m_freeFalling;   /* 0x280 */
    char pad281[0x284 - 0x281];
    int m_fallTimeBeforePitch;   /* 0x284 */
    int m_fallTime;   /* 0x288 */
    int m_camIdleCircuitTime;   /* 0x28C */
    float m_camIdleFactor;   /* 0x290 */
    float m_climbSpeed;   /* 0x294 */
    char pad298[0x29C - 0x298];
    float m_climbStrafeSpeed;   /* 0x29C */
    char pad2A0[0x448 - 0x2A0];
    float m_maxHealth;   /* 0x448 */
    float m_health;   /* 0x44C */
    char pad450[0x460 - 0x450];
    StaminaMeter m_stamina;   /* 0x460 */
    char pad48C[0x4AC - 0x48C];
    unsigned char m_okToGlow;   /* 0x4AC */
    char pad4AD[0x4D8 - 0x4AD];
    int m_playerInfo;   /* 0x4D8 */
    char pad4DC[0x1A10 - 0x4DC];
    int m_closestPath;   /* 0x1A10 */
    char pad1A14[0x1A3C - 0x1A14];
    int m_shadow;   /* 0x1A3C */
    char pad1A40[0x5040 - 0x1A40];
    PadFlags m_padFlags;   /* 0x5040 */
    char pad6854[0x6868 - 0x6854];
    int m_hudTexture;   /* 0x6868 */
    char pad686C[0x68A0 - 0x686C];
    int m_reticleState;   /* 0x68A0 */
    int m_pickup;   /* 0x68A4 */
    int m_impaler;   /* 0x68A8 */
    int m_reverseImpaler;   /* 0x68AC */
    int m_grappler;   /* 0x68B0 */
    void * m_target;   /* 0x68B4 */
    int m_grappleAttempt;   /* 0x68B8 */
    int m_beamVictim;   /* 0x68BC */
    char pad68C0[0x697C - 0x68C0];
    int m_launchDelay;   /* 0x697C */
    int m_launchCounter;   /* 0x6980 */
    char pad6984[0x69A8 - 0x6984];
    float m_damageModifier;   /* 0x69A8 */
    float m_dpDamage;   /* 0x69AC */
    float m_dpDuration;   /* 0x69B0 */
    float m_dpSpeed;   /* 0x69B4 */
    float m_dpHomingFactor;   /* 0x69B8 */
    float m_dpVertHomingFactor;   /* 0x69BC */
    float m_dpHeadingBreak;   /* 0x69C0 */
    float m_dpPitchBreak;   /* 0x69C4 */
    char pad69C8[0x6BF8 - 0x69C8];
    int m_reticleCS;   /* 0x6BF8 */
    int m_stickyReticleCS;   /* 0x6BFC */
    char pad6C00[0x6C04 - 0x6C00];
    int m_pinTarget;   /* 0x6C04 */
    float m_pinTime;   /* 0x6C08 */
    char pad6C0C[0x6C14 - 0x6C0C];
    float m_targetingMod;   /* 0x6C14 */
    char pad6C18[0x6C20 - 0x6C18];
    float m_pinMaxPitch;   /* 0x6C20 */
    float m_pinMuckingDist;   /* 0x6C24 */
    char pad6C28[0x6C30 - 0x6C28];
    int m_autoLeadMovesReticle;   /* 0x6C30 */
    char pad6C34[0x6C3C - 0x6C34];
    int m_aimHeadingEnabled;   /* 0x6C3C */
    float m_aimHeading;   /* 0x6C40 */
    float m_aimPitch;   /* 0x6C44 */
    char pad6C48[0x6CB8 - 0x6C48];
    float m_onFireCount;   /* 0x6CB8 */
    float m_onFireDamage;   /* 0x6CBC */
    char pad6CC0[0x6CC4 - 0x6CC0];
    float m_beingShockedCount;   /* 0x6CC4 */
    float m_beingShockedDamage;   /* 0x6CC8 */
    char pad6CCC[0x6CD4 - 0x6CCC];
    int m_isCameraFollowingThisMonster;   /* 0x6CD4 */
    int m_cameraThatFollows;   /* 0x6CD8 */
    float m_landingShakeAmp;   /* 0x6CDC */
    float m_landingShakeFreq;   /* 0x6CE0 */
    float m_landingShakeDur;   /* 0x6CE4 */
    float m_landingShakeFalloff;   /* 0x6CE8 */
    float m_landingShakeMag;   /* 0x6CEC */
    char pad6CF0[0x7970 - 0x6CF0];
    int m_camUnify;   /* 0x7970 */
    char pad7974[0x10E70 - 0x7974];
    char m_victoryState[1];   /* 0x10E70 */
    char pad10E71[0x11190 - 0x10E71];
};
typedef char _size_Monster[sizeof(Monster) == 0x11190 ? 1 : -1];

class TheGame {
public:
    Hud m_huds[4];                   /* 0x000: one per view, stride 0x2E0 */
    Monster m_slots[16];             /* 0xB80: monster array, stride 0x11190; m_numSlots are in use */
    char pad112480[0x11C370 - 0x112480]; /* includes the Weapons spawner at 0x112490 */
    TokenManager m_tokens[2];        /* 0x11C370 */
    char pad11E378[0x120380 - 0x11E378];
    Monster *m_monsters[8];          /* 0x120380: indexed up to m_numMonsters; vegas also reads [4] and [5] */
    char pad1203A0[0x1203C4 - 0x1203A0];
    float m_gravity;                 /* 0x1203C4 */
    int m_gameMode;                  /* 0x1203C8: 1 = the mode AiScript3Mile/central set up special levels for */
    int m_matchMode;                 /* 0x1203CC: 0 or 1 selects the two-player-style AI in AiGrappleAttack */
    int m_levelId;                   /* 0x1203D0: level id (1 central, 2 vegas, 3 canyon2, 5 airport, 6 threemile, 7 sanfran, 8/15 island, 9 tokyo, 10 ufo, 11 final boss, 26 bigshot, 27 crush) */
    int m_numSlots;                  /* 0x1203D4: loop bound over the per-player blocks (gameSlotBase) */
    int m_numMonsters;               /* 0x1203D8: loop bound over m_monsters */
    char pad1203DC[0x1203E0 - 0x1203DC];
    int m_numAIs;                    /* 0x1203E0: AI monsters, stored in m_monsters[4..] */
    int m_numAIsAlive;               /* 0x1203E4: cached by GetNumAIsAlive */
    int m_viewSlot[2];               /* 0x1203E8: monster slot each view follows (gameInitCamera); [0] also selects the level data block */
    char pad1203F0[0x12043C - 0x1203F0];
    int m_won[2];                    /* 0x12043C */
    int m_playerMask;                /* 0x120444: bit 0 = player 1 active, bit 1 = player 2 active (inferred) */
    char pad120448[0x120450 - 0x120448];
    int f120450, f120454, f120458, f12045C; /* zeroed by InitAfterDbLoad */
    int f120460;                     /* set to 1 by InitAfterDbLoad */
    char pad120464[4];
    int f120468;                     /* set to 1 by InitAfterDbLoad */
    struct PadTweaks {               /* 0x12046C: copied into every monster's PadFlags by UpdatePadTweaks */
        int t16C4;
        float t16D0;
        float t16D4;
        int t16C8;
        int t16F4;
        int t16F8;
        int t16FC;
        int t17AC;
        int t17B0;
        int t16DC;
        int t16E0;
    } m_padTweaks;
    int m_actuator[8];               /* 0x120498: per-pad rumble actuator enable flags */

    void Init(void);
    void InitBeforeDbLoad(void);
    void InitAfterDbLoad(void);
    void Update(void);
    void Update2(void);
    void ResetLevel(void);
    void UnpauseLevel(void);
    void gameResolveCollisions(void);
    void gameResolveLifeAndDeath(void);
    void gameCheckForCloseCombat(void);
    void SetGravity(float g);
    void SetOkToUnify(void);
    int GetNumAIsAlive(void);
    float GetCameraMaxHeight(_fvector *pos);
    void gameInitCamera(int view, int slot);
    void UpdatePadTweaks(void);
    static void traversalCallback(_cs *cs, unsigned a, unsigned b, float (&m)[4][4], _fvector *eo);
    static void genericEventHandler(unsigned event);

    Weapons *getWeapons(void) { return (Weapons *)((char *)this + 0x112490); }
};

extern TheGame *game;

/* The Hud array sits at the start of TheGame (stride 0x2E0); the weapon spawner at 0x112490. */
static inline Hud *gameHud(int i)
{
    return (Hud *)((char *)game + i * 0x2E0);
}
static inline Weapons *gameWeapons(void)
{
    return (Weapons *)((char *)game + 0x112490);
}

/* Per-level data block: game + idx * 0x11190 + 0xB80. Kept as one offset so the add order matches retail. */
static inline Monster *gameSlotBase(int idx)
{
    return (Monster *)((char *)game + (idx * 0x11190 + 0xB80));
}
extern int gHudEnable;
extern int gUseUnifiedView;

class Camera {
public:
    enum CameraPOV { POV_0, POV_1, POV_2, POV_3 };
};

class Cameras {
public:
    char pad0[0x2A58];
    int m_state;              /* 0x2A58: 7 = cinema */

    static Cameras m_cameras;
    static void InitCrushMonsters(Monster *a, Monster *b);
    static void Update(void);
    static void SetCameraToFollowMonster(int view, Monster *m);
    static void SetCameraPOV(int view, Camera::CameraPOV pov);
};

void fontSetColor(int font, int r, int g, int b, int a);
void fontSetCharSizesInSubPixels(int font, int w, int h, int spacing, int unk);
void fontSpritePrintXY(int font, int x, int y, char *str);
void fontSpritePrintCenteredXY(int font, int x, int y, char *str);
void fontSpritePrintRightXY(int font, int x, int y, char *str);
void fontSetDefaultColor(int font);
void fontSetDefaultSize(int font);
extern "C" int sprintf(char *, const char *, ...);

#endif

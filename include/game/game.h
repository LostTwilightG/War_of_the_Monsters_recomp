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

    char pad0[0xC];
    _cs *m_cs;            /* 0x0C: the monster's scene-graph node; its translation is the world position */
    char pad10[4];
    int m_typeBits;       /* 0x14: monster type << 5 */
    int m_playerNum;      /* 0x18: 0 = unused slot, 1/2 = controlling player (inferred from Update) */
    char pad1C[4];
    int m_id;             /* 0x20: player/monster id used by hit histories and pickups */
    char pad24[0x3C - 0x24];
    int m_unk3C;          /* 0x3C: zeroed by InitAfterDbLoad */
    char pad40[0x49 - 0x40];
    unsigned char m_unk49; /* 0x49: set to 1 by ResetLevel */
    char pad4A[0xE8 - 0x4A];
    signed char m_dead;   /* 0xE8: nonzero once dead (GetNumAIsAlive counts the zeros) */
    char padE9[3];
    unsigned char m_unkEC; /* 0xEC */
    char padED[0xF6 - 0xED];
    unsigned char m_unkF6; /* 0xF6 */
    unsigned char m_unkF7; /* 0xF7 */
    char padF8;
    unsigned char m_unkF9; /* 0xF9 */
    char padFA[0x44C - 0xFA];
    float m_health;       /* 0x44C */
    char pad450[0x460 - 0x450];
    StaminaMeter m_stamina; /* 0x460 */
    char pad48C[0x5040 - 0x48C];
    PadFlags m_padFlags;  /* 0x5040: input interpretation; the pad tweaks (0x6704..) are inside it */
    char pad6854[0x68B4 - 0x6854];
    void *m_target;       /* 0x68B4: current target (null when none) */
    char pad68B8[0x7970 - 0x68B8];
    int m_camUnify;       /* 0x7970: zeroed by the cam-unify trigger volume */
    char pad7974[0x10E70 - 0x7974];
    char m_victoryState[1]; /* 0x10E70: embedded MonsterState (type TBD) */
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

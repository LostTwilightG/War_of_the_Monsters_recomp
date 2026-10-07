#ifndef GAME_H
#define GAME_H

#include "engine.h"
#include "game/hud.h"
#include "game/weapons.h"
#include "game/stamina_meter.h"
#include "hieri_types.h"

/* Partial class layouts recovered from usage. Unknown regions are padding until identified. */

class MonsterState;

class Monster {
public:
    void drainSpecial();
    void enterNewState(MonsterState *state);
    void takeDamage(float dmg, bool b, Monster *src);

    char pad0[0xC];
    _cs *m_cs;            /* 0x0C: the monster's scene-graph node; its translation is the world position */
    char pad10[4];
    int m_typeBits;       /* 0x14: monster type << 5 */
    char pad18[0x20 - 0x18];
    int m_id;             /* 0x20: player/monster id used by hit histories and pickups */
    char pad24[0xEC - 0x24];
    unsigned char m_unkEC; /* 0xEC */
    char padED[0xF7 - 0xED];
    unsigned char m_unkF7; /* 0xF7 */
    char padF8;
    unsigned char m_unkF9; /* 0xF9 */
    char padFA[0x44C - 0xFA];
    float m_health;       /* 0x44C */
    char pad450[0x460 - 0x450];
    StaminaMeter m_stamina; /* 0x460 */
    char pad48C[0x68B4 - 0x48C];
    void *m_target;       /* 0x68B4: current target (null when none) */
    char pad68B8[0x7970 - 0x68B8];
    int m_camUnify;       /* 0x7970: zeroed by the cam-unify trigger volume */
    char pad7974[0x10E70 - 0x7974];
    char m_victoryState[1]; /* 0x10E70: embedded MonsterState (type TBD) */
    char pad10E71[0x11190 - 0x10E71];
};
typedef char _size_Monster[sizeof(Monster) == 0x11190 ? 1 : -1];

class TokenManager {
public:
    int grandTotal();

    char pad0[0x1004];
};

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
    int m_phase;                     /* 0x1203D0: 7 / 1 are checked by AiBrain and StaminaMeter */
    int m_numSlots;                  /* 0x1203D4: loop bound over the per-player blocks (gameSlotBase) */
    int m_numMonsters;               /* 0x1203D8: loop bound over m_monsters */
    char pad1203DC[0x1203E8 - 0x1203DC];
    int m_levelIdx;                  /* 0x1203E8: selects the level block (gameSlotBase) */
    char pad1203EC[0x12043C - 0x1203EC];
    int m_won[2];                    /* 0x12043C */
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

class Cameras {
public:
    static void InitCrushMonsters(Monster *a, Monster *b);
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

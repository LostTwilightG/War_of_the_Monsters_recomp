#ifndef GAME_H
#define GAME_H

#include "engine.h"

/* Partial class layouts recovered from usage. Unknown regions are padding until identified. */

class MonsterState;

class Monster {
public:
    void drainSpecial();
    void enterNewState(MonsterState *state);

    char pad0[0x14];
    int m_typeBits;       /* 0x14: monster type << 5 */
    char pad18[0xEC - 0x18];
    unsigned char m_unkEC; /* 0xEC */
    char padED[0xF7 - 0xED];
    unsigned char m_unkF7; /* 0xF7 */
    char padF8;
    unsigned char m_unkF9; /* 0xF9 */
    char padFA[0x10E70 - 0xFA];
    char m_victoryState[1]; /* 0x10E70: embedded MonsterState (type TBD) */
};

class TokenManager {
public:
    int grandTotal();

    char pad0[0x1004];
};

class TheGame {
public:
    char pad0[0x11C370];
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

/* Per-level data block: game + idx * 0x11190 + 0xB80. Kept as one offset so the add order matches retail. */
static inline void *gameSlotBase(int idx)
{
    return (char *)game + (idx * 0x11190 + 0xB80);
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

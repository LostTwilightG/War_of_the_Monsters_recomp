#ifndef GAME_H
#define GAME_H

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
    Monster *m_monsters[2];          /* 0x120380 */
    char pad120388[0x12043C - 0x120388];
    int m_won[2];                    /* 0x12043C */
};

extern TheGame *game;
extern int gHudEnable;

class Cameras {
public:
    static void InitCrushMonsters(Monster *a, Monster *b);
};

int timerGetFieldsLastFrame();
void fontSetColor(int font, int r, int g, int b, int a);
void fontSetCharSizesInSubPixels(int font, int w, int h, int spacing, int unk);
void fontSpritePrintXY(int font, int x, int y, char *str);
void fontSpritePrintCenteredXY(int font, int x, int y, char *str);
void fontSpritePrintRightXY(int font, int x, int y, char *str);
void fontSetDefaultColor(int font);
void fontSetDefaultSize(int font);
extern "C" int sprintf(char *, const char *, ...);

#endif

#ifndef HUD_H
#define HUD_H

#include "engine.h"

/* One per player view; the array starts at the beginning of TheGame (stride 0x2E0), see gameHud(). */
class Hud {
public:
    int view;                   /* 0x00: index of this hud's view */
    char pad4[4];
    int comboHits;              /* 0x08 */
    int staminaCredit;          /* 0x0C: energy gained since the message appeared */
    int healthCredit;           /* 0x10 */
    int msgId[3];               /* 0x14: the three message slots (0 = free) */
    char pad20[0x2C - 0x20];
    int msgTimer[3];            /* 0x2C */
    char pad38[0x50 - 0x38];
    int msgAlpha[3];            /* 0x50 */
    char pad5C[0xD0 - 0x5C];
    int f0D0;
    char pad0D4[0x1D0 - 0xD4];
    _animHandle h1D0;
    char pad1E0[0x2E0 - 0x1E0];

    void initForReplay(void);
    void addMessage(int a, int b);
    void addTextBoxMessage(int id);
    void registerHealthCredit(int amount);
    void registerComboHit(void);
    void registerStaminaCredit(int amount);
    void print(void);
};

typedef char _size_Hud[sizeof(Hud) == 0x2E0 ? 1 : -1];

#endif

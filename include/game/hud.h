#ifndef HUD_H
#define HUD_H

#include "engine.h"

/* One per player view; the array starts at the beginning of TheGame (stride 0x2E0), see gameHud(). */
class Hud {
public:
    char pad0[0xD0];
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
};

typedef char _size_Hud[sizeof(Hud) == 0x2E0 ? 1 : -1];

#endif

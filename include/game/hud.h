#ifndef HUD_H
#define HUD_H

/* One per player view; the array starts at the beginning of TheGame (stride 0x2E0), see gameHud(). */
class Hud {
public:
    char pad0[0x2E0];

    void initForReplay(void);
    void addMessage(int a, int b);
    void addTextBoxMessage(int id);
    void registerHealthCredit(int amount);
};

typedef char _size_Hud[sizeof(Hud) == 0x2E0 ? 1 : -1];

#endif

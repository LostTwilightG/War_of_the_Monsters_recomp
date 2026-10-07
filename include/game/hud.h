#ifndef HUD_H
#define HUD_H

/* One per player view; the array starts at the beginning of TheGame (stride 0x2E0), see gameHud(). */
class Hud {
public:
    void addMessage(int a, int b);
    void addTextBoxMessage(int id);
};

#endif

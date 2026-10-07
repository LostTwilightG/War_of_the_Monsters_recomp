#include "common.h"

class GamePad {
public:
    int f0, f4, f8, fC, f10, f14;

    void clearInputs(void);
};

void GamePad::clearInputs(void)
{
    f0 = 0;
    f4 = 0;
    f8 = 0;
    fC = 0;
    f10 = 0;
    f14 = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/GamePad", loadPadInputs__7GamePadi);

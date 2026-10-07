#include "common.h"

struct _animHandle {
    int a, b, c, d;
};
void animationPause(_animHandle h);
void animationSetToBeginning(_animHandle h, bool b);
void animationStart(_animHandle h, bool b);

class DodgeBallLevel {
public:
    char pad0[8];
    int countdown;
    char padC[4];
    _animHandle anims[3];
    int state;

    void ResetCountdown(void);
};

INCLUDE_ASM("asm/nonmatchings/game/DodgeBallLevel", initAfterDbLoad__14DodgeBallLevel);
INCLUDE_ASM("asm/nonmatchings/game/DodgeBallLevel", update__14DodgeBallLeveli);
void DodgeBallLevel::ResetCountdown(void)
{
    int i;

    countdown = 540;
    for (i = 2; i >= 0; i--) {
        animationPause(anims[2 - i]);
        animationSetToBeginning(anims[2 - i], true);
    }
    state = 2;
    animationStart(anims[2], true);
}
INCLUDE_ASM("asm/nonmatchings/game/DodgeBallLevel", __static_initialization_and_destruction_0_0012F148);
INCLUDE_ASM("asm/nonmatchings/game/DodgeBallLevel", _GLOBAL_$I$_14DodgeBallLevel$instance);

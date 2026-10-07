#include "common.h"
#include "game/game.h"
#include "engine.h"


class DodgeBallLevel {
public:
    int f0, f4;
    int countdown;
    char padC[4];
    _animHandle anims[3];
    int state;

    void ResetCountdown(void);
    void initAfterDbLoad(void);
};

#ifdef NON_MATCHING
/* 53/67 words, untuned: s1/s2 register swap */
void DodgeBallLevel::initAfterDbLoad(void)
{
    int i;

    countdown = -1;
    f0 = 0;
    f4 = 0;
    gHudEnable = 0;
    for (i = 0; i < 3; i++) {
        animationGetHandle(&anims[i], 0x27C9, 0, i);
        animationLoop(anims[i], true);
        animationSetSpeed(anims[i], 1.0f);
    }
    for (i = 0; i < 2; i++)
        game->m_monsters[i]->drainSpecial();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/DodgeBallLevel", initAfterDbLoad__14DodgeBallLevel);
#endif
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

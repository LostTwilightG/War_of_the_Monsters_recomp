#include "common.h"

struct GamePadClip {
    unsigned char data[0x20];
};

extern GamePadClip D_006F93C0;
extern GamePadClip D_006F9400;
extern GamePadClip D_006F9420;

class AiPadClips {
public:
    /* the retail symbols carry a trailing "v" (see common/CsColor.cpp) */
    static GamePadClip *getTaunt(void) __asm__("getTaunt__10AiPadClipsv");
    static GamePadClip *getDash(void) __asm__("getDash__10AiPadClipsv");
    static GamePadClip *getThrow(void) __asm__("getThrow__10AiPadClipsv");
};

GamePadClip *AiPadClips::getTaunt(void)
{
    return &D_006F93C0;
}
GamePadClip *AiPadClips::getDash(void)
{
    return &D_006F9400;
}
GamePadClip *AiPadClips::getThrow(void)
{
    return &D_006F9420;
}
INCLUDE_ASM("asm/nonmatchings/game/AiPadClips", __static_initialization_and_destruction_0_001062E0);
INCLUDE_ASM("asm/nonmatchings/game/AiPadClips", _GLOBAL_$I$getTaunt__10AiPadClipsv);

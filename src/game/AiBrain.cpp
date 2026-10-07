#include "common.h"
#include "game/game.h"

struct Ai;

class AiActionGroup {
public:
    void reset(void);
    void update(Ai &ai);
};

class AiBrain : public AiActionGroup {
public:
    char pad[0xAAC];
    float f_AAC;

    void reset(Ai &ai);
    void update(Ai &ai);
};

INCLUDE_ASM("asm/nonmatchings/game/AiBrain", _vt$21AiGrapplingActionList);
INCLUDE_ASM("asm/nonmatchings/game/AiBrain", _vt$20AiGrappledActionList);
INCLUDE_ASM("asm/nonmatchings/game/AiBrain", __7AiBrain);
INCLUDE_ASM("asm/nonmatchings/game/AiBrain", init__7AiBrainR2Ai);
#ifdef NON_MATCHING
/* 19/20 words, untuned */
void AiBrain::reset(Ai &ai)
{
    AiActionGroup::reset();
    if (game->m_phase == 7)
        f_AAC = 500.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiBrain", reset__7AiBrainR2Ai);
#endif
void AiBrain::update(Ai &ai)
{
    AiActionGroup::update(ai);
}

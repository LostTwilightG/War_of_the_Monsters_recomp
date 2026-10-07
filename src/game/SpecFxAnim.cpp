#include "common.h"
#include "hieri_types.h"
#include "engine.h"


struct Monster;

__asm__("#SNFIX_SMALL _10SpecFxAnim$s_numFxs");

class SpecFxAnim {
public:
    _animHandle anims[3];
    int active;
    char pad34[4];
    _cs *cs;
    int model;
    int owner;
    char pad44[0];

    static SpecFxAnim s_fxPool[16];
    static int s_numFxs;

    static void addInstance(_hierhead *h, int i);
    static int isInstanceAvailable(Monster *m, int i);
    static void initBeforeDbLoad(void);
    void registerModel(_hierhead *h, int i);

    void kill(void);
    void launch(void);
    void end(void);
    void startAnim(int i);
    void updateOrient(void);
};

void SpecFxAnim::addInstance(_hierhead *h, int i)
{
    s_fxPool[s_numFxs++].registerModel(h, i);
}
#ifdef NON_MATCHING
/* 17/20 words, untuned */
int SpecFxAnim::isInstanceAvailable(Monster *m, int i)
{
    int left = s_numFxs;
    SpecFxAnim *p = s_fxPool;

    for (; left != 0; left--, p++) {
        if (!p->active && p->model == i && p->owner == *(int *)((char *)m + 0x1C))
            return 1;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", isInstanceAvailable__10SpecFxAnimP7Monsteri);
#endif
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", getInstance__10SpecFxAnimP7Monsteri);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", __10SpecFxAnim);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", registerModel__10SpecFxAnimP9_hierheadi);
#ifdef NON_MATCHING
/* 5/17 words, untuned: retail zeroes the pool with one pointer loop */
void SpecFxAnim::initBeforeDbLoad(void)
{
    int i;

    s_numFxs = 0;
    for (i = 15; i >= 0; i--) {
        s_fxPool[i].cs = 0;
        s_fxPool[i].active = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", initBeforeDbLoad__10SpecFxAnim);
#endif
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", initAfterDbLoad__10SpecFxAnim);
void SpecFxAnim::launch(void)
{
    updateOrient();
    startAnim(0);
}
void SpecFxAnim::end(void)
{
    startAnim(2);
    active = 0;
    cs->drawMe = 0;
}
void SpecFxAnim::kill(void)
{
    active = 0;
    cs->drawMe = 0;
}
void SpecFxAnim::startAnim(int i)
{
    animationStart(anims[i], true);
}
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", updateOrient__10SpecFxAnim);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", __static_initialization_and_destruction_0_001CE9A8);
INCLUDE_ASM("asm/nonmatchings/game/SpecFxAnim", _GLOBAL_$I$_10SpecFxAnim$s_numFxs);

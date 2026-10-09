#include "common.h"

class Ai;
class resetcom {
public:
    void forceRead(void);
};
class AiReset : public resetcom {
public:
    void setAi(Ai *a);
};
extern char aiTweaksReset[] __asm__("D_006F9440");
extern Ai *s_ai;
__asm__("#SNFIX_SMALL s_ai");
class AiTweaks {
public:
    static void setActiveAi(Ai *a, bool load);
};

void AiTweaks::setActiveAi(Ai *a, bool load)
{
    s_ai = a;
    if (load) {
        ((AiReset *)aiTweaksReset)->setAi(a);
        ((resetcom *)aiTweaksReset)->forceRead();
    }
}
INCLUDE_ASM("asm/nonmatchings/game/AiTweaks", __static_initialization_and_destruction_0_00113CE8);
INCLUDE_ASM("asm/nonmatchings/game/AiTweaks", _GLOBAL_$I$setActiveAi__8AiTweaksP2Aib);

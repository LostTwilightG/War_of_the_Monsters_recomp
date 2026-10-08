#include "common.h"

class resetcom {
public:
    void init(void);
};
extern char monsterTweaksReset[] __asm__("D_00725240");
class Monster;
class MonsterTweaks {
public:
    static void init(void);
    static void setActiveMonster(Monster *m, bool b);
    void resetTweaks(void);
};

void MonsterTweaks::init(void)
{
    ((resetcom *)monsterTweaksReset)->init();
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterTweaks", setActiveMonster__13MonsterTweaksP7Monsterb);
void MonsterTweaks::resetTweaks(void)
{
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterTweaks", __static_initialization_and_destruction_0_0016E108);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTweaks", _GLOBAL_$I$init__13MonsterTweaks);

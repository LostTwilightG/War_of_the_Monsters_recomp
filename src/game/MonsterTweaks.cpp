#include "common.h"
#include "game/game.h"

class resetcom {
public:
    void init(void);
    void forceRead(void);
};
class charreset : public resetcom {
public:
    void setCharacter(Monster *m);
};
extern Monster *s_mon;
__asm__("#SNFIX_SMALL s_mon");
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
/* Makes `m` the monster whose parameters are being tweaked: loads its character file (re-reading when asked) and recomputes its dynamics. */
void MonsterTweaks::setActiveMonster(Monster *m, bool reread)
{
    s_mon = m;
    ((charreset *)monsterTweaksReset)->setCharacter(m);
    if (reread)
        ((resetcom *)monsterTweaksReset)->forceRead();
    s_mon->recomputeDynamics();
}
void MonsterTweaks::resetTweaks(void)
{
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterTweaks", __static_initialization_and_destruction_0_0016E108);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTweaks", _GLOBAL_$I$init__13MonsterTweaks);

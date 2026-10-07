#include "common.h"
#include "game/power_ups.h"
#include "point_tool_kit.h"
#include "game/shell.h"
#include "hieri_types.h"
#include "game/game.h"

struct PowerUpPoint {
    char pad0[0x2C];
    unsigned char type;
    unsigned char arg;
    char pad2E[0x40 - 0x2E];
};

extern char D_006F7E10[];
extern char D_006F7E18[];
__asm__("#SNFIX_SMALL shell");

class PowerUpTool : public PointToolKit {
public:
    void *levelData;

    void init(int i) __asm__("init__11PowerUpTooli");
    void exportPoints(void);
    void loadPoints(char *name, bool b);
};

INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", _vt$11PowerUpTool);
void PowerUpTool::loadPoints(char *name, bool b)
{
    char path[0x20];
    char *fmt = D_006F7E10;

    if (!name)
        name = shell->GetLevelName();
    Shell::formatFilename(path, fmt, name, D_006F7E18);
    PointToolKit::loadPoints(path, b);
}
void PowerUpTool::init(int i)
{
    PointToolKit::init(0);
    levelData = gameSlotBase(game->m_viewSlot[0]);
}
void PowerUpTool::exportPoints(void)
{
    PowerUps *pu = &PowerUps::instance;
    PowerUpPoint *p;
    int n;

    pu->KillPowerUps();
    p = (PowerUpPoint *)getPoint(0);
    for (n = numPoints; n != 0; n--, p++)
        pu->CreatePowerUp(p->type, p->arg, (_fvector *)p);
}
INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", __static_initialization_and_destruction_0_00188250);
INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", __tf11PowerUpTool);
INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", __11PowerUpTool);
INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", _GLOBAL_$I$_11PowerUpTool$instance);

#include "common.h"
#include "hieri_types.h"
#include "game/game.h"

struct PowerUpPoint {
    char pad0[0x2C];
    unsigned char type;
    unsigned char arg;
    char pad2E[0x40 - 0x2E];
};

class Shell {
public:
    char *GetLevelName(void);
    static void formatFilename(char *dst, const char *a, const char *b, const char *c);
};
extern Shell *shell;
extern char D_006F7E10[];
extern char D_006F7E18[];
__asm__("#SNFIX_SMALL shell");

class PowerUps {
public:
    static PowerUps instance;
    void KillPowerUps(void);
    void CreatePowerUp(int type, unsigned char arg, _fvector *pos);
};

/* PointToolKit is the base in retail (init/getPoint live in common); its layout is flattened here. */
class PowerUpTool {
public:
    PowerUpPoint points[256];
    int numPoints;
    char pad4004[0x4050 - 0x4004];
    void *levelData;

    void init(int i) __asm__("init__11PowerUpTooli");
    void exportPoints(void);
    void loadPoints(char *name, bool b);
};

INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", _vt$11PowerUpTool);
void PointToolKitLoad(PowerUpTool *t, char *name, bool b) __asm__("loadPoints__12PointToolKitPcb");
void PowerUpTool::loadPoints(char *name, bool b)
{
    char path[0x20];
    char *fmt = D_006F7E10;

    if (!name)
        name = shell->GetLevelName();
    Shell::formatFilename(path, fmt, name, D_006F7E18);
    PointToolKitLoad(this, path, b);
}
void PointToolKitInit(PowerUpTool *t, int i) __asm__("init__12PointToolKiti");
PowerUpPoint *PointToolKitGetPoint(PowerUpTool *t, int i) __asm__("getPoint__12PointToolKiti");
void PowerUpTool::init(int i)
{
    PointToolKitInit(this, 0);
    levelData = (char *)game + (*(int *)((char *)game + 0x1203E8) * 0x11190 + 0xB80);
}
void PowerUpTool::exportPoints(void)
{
    PowerUps *pu = &PowerUps::instance;
    PowerUpPoint *p;
    int n;

    pu->KillPowerUps();
    p = PointToolKitGetPoint(this, 0);
    for (n = numPoints; n != 0; n--, p++)
        pu->CreatePowerUp(p->type, p->arg, (_fvector *)p);
}
INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", __static_initialization_and_destruction_0_00188250);
INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", __tf11PowerUpTool);
INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", __11PowerUpTool);
INCLUDE_ASM("asm/nonmatchings/game/PowerUpTool", _GLOBAL_$I$_11PowerUpTool$instance);

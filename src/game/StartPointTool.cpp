#include "common.h"
#include "hieri_types.h"
#include "game/game.h"

class Shell {
public:
    char *GetLevelName(void);
    static void formatFilename(char *dst, const char *a, const char *b, const char *c);
};
extern Shell *shell;
extern char D_006F8320[];
extern char D_006F8328[];
extern float quantizer;
__asm__("#SNFIX_SMALL quantizer");

struct StartPointData {
    char pad0[0x2C];
    unsigned char type;
    unsigned char angle;
    unsigned char flag;
    char pad2F[0x40 - 0x2F];
};

class StartPoints {
public:
    static StartPoints m_instance;
    void clear(void);
    void addPoint(unsigned char type, _fvector *pos, float angle, bool b);
};

/* PointToolKit is the base in retail (init/getPoint/loadPoints live in common); its layout is flattened here. */
class StartPointTool {
public:
    StartPointData points[256];
    int numPoints;
    char pad4004[0x4050 - 0x4004];
    void *levelData;

    void init(int i) __asm__("init__14StartPointTooli");
    void loadPoints(void);
    void exportPoints(void);
    unsigned char degreesToData(float d);
    float dataToDegrees(unsigned char c);
};
void PointToolKitInit(StartPointTool *t, int i) __asm__("init__12PointToolKiti");
StartPointData *PointToolKitGetPoint(StartPointTool *t, int i) __asm__("getPoint__12PointToolKiti");
void PointToolKitLoad(StartPointTool *t, char *name, bool b) __asm__("loadPoints__12PointToolKitPcb");

INCLUDE_ASM("asm/nonmatchings/game/StartPointTool", _vt$14StartPointTool);
void StartPointTool::loadPoints(void)
{
    char path[0x20];
    char *fmt = D_006F8320;

    Shell::formatFilename(path, fmt, shell->GetLevelName(), D_006F8328);
    StartPoints::m_instance.clear();
    PointToolKitLoad(this, path, true);
}
void StartPointTool::init(int i)
{
    PointToolKitInit(this, 0);
    levelData = gameSlotBase(game->m_levelIdx);
}
void StartPointTool::exportPoints(void)
{
    StartPoints *sp = &StartPoints::m_instance;
    StartPointData *p;
    int n;

    sp->clear();
    p = PointToolKitGetPoint(this, 0);
    for (n = numPoints; n != 0; n--, p++)
    {
        float a = dataToDegrees(p->angle);

        sp->addPoint(p->type, (_fvector *)p, a, p->flag != 0);
    }
}
unsigned char StartPointTool::degreesToData(float d)
{
    if (d < 0.0f)
        d += 360.0f;
    d *= quantizer;
    return (int)d;
}
float StartPointTool::dataToDegrees(unsigned char c)
{
    float d = (float)c / quantizer;

    if (d > 180.0f)
        d -= 360.0f;
    return d;
}
INCLUDE_ASM("asm/nonmatchings/game/StartPointTool", __static_initialization_and_destruction_0_001CED60);
INCLUDE_ASM("asm/nonmatchings/game/StartPointTool", __tf14StartPointTool);
INCLUDE_ASM("asm/nonmatchings/game/StartPointTool", __14StartPointTool);
INCLUDE_ASM("asm/nonmatchings/game/StartPointTool", _GLOBAL_$I$_14StartPointTool$instance);

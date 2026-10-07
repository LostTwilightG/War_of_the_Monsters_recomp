#include "common.h"
#include "hieri_types.h"
#include "game/game.h"

class Shell {
public:
    char *GetLevelName(void);
    static void formatFilename(char *dst, const char *a, const char *b, const char *c);
};
extern Shell *shell;
extern char D_006F7998[];
__asm__("#SNFIX_SMALL shell");

struct AiPathNet;

/* PointToolKit is the base in retail; its layout is flattened here. */
class AiPathTool {
public:
    char points[256 * 0x40];
    int numPoints;
    char pad4004[0x4050 - 0x4004];
    AiPathNet *net;
    char *name;
    char pad4058[4];
    void *levelData;

    void init(int i) __asm__("init__10AiPathTooli");
    void loadPoints(char *n, bool b);
};
void PointToolKitInit(AiPathTool *t, int i) __asm__("init__12PointToolKiti");
void PointToolKitLoad(AiPathTool *t, char *name, bool b) __asm__("loadPoints__12PointToolKitPcb");

void AiPathTool::init(int i)
{
    PointToolKitInit(this, 0);
    levelData = (char *)game + (*(int *)((char *)game + 0x1203E8) * 0x11190 + 0xB80);
}
void AiPathTool::loadPoints(char *n, bool b)
{
    char path[0x20];
    char *fmt = D_006F7998;

    PointToolKitLoad(this, (Shell::formatFilename(path, fmt, n ? n : shell->GetLevelName(), name), path), b);
}
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", exportPoints__10AiPathTool);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", _vt$10AiPathTool);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", __static_initialization_and_destruction_0_00107960);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", __tf10AiPathTool);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", __10AiPathToolP9AiPathNetPcT2);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", _GLOBAL_$I$_10AiPathTool$instance);

#include "common.h"
#include "point_tool_kit.h"
#include "game/shell.h"
#include "hieri_types.h"
#include "game/game.h"

extern char D_006F7998[];
__asm__("#SNFIX_SMALL shell");

struct AiPathNet;

class AiPathTool : public PointToolKit {
public:
    AiPathNet *net;
    char *name;
    char pad4058[4];
    void *levelData;

    void init(int i) __asm__("init__10AiPathTooli");
    void loadPoints(char *n, bool b);
};

void AiPathTool::init(int i)
{
    PointToolKit::init(0);
    levelData = gameSlotBase(game->m_viewSlot[0]);
}
void AiPathTool::loadPoints(char *n, bool b)
{
    char path[0x20];
    char *fmt = D_006F7998;

    PointToolKit::loadPoints((Shell::formatFilename(path, fmt, n ? n : shell->GetLevelName(), name), path), b);
}
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", exportPoints__10AiPathTool);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", _vt$10AiPathTool);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", __static_initialization_and_destruction_0_00107960);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", __tf10AiPathTool);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", __10AiPathToolP9AiPathNetPcT2);
INCLUDE_ASM("asm/nonmatchings/game/AiPathTool", _GLOBAL_$I$_10AiPathTool$instance);

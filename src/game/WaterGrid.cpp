#include "common.h"

class WaterGrid {
public:
    unsigned update(void);
    static unsigned update(void *p);
};

INCLUDE_ASM("asm/nonmatchings/game/WaterGrid", __9WaterGridP9_hierhead);
INCLUDE_ASM("asm/nonmatchings/game/WaterGrid", reset__9WaterGrid);
INCLUDE_ASM("asm/nonmatchings/game/WaterGrid", update__9WaterGrid);
INCLUDE_ASM("asm/nonmatchings/game/WaterGrid", disturb__9WaterGridR8_fvectorff);
unsigned WaterGrid::update(void *p)
{
    return ((WaterGrid *)p)->update();
}

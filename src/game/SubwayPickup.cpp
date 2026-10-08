#include "common.h"

INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", _vt$12SubwayPickup);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", __12SubwayPickup);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", initAfterDbLoad__12SubwayPickup);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", takeHit__12SubwayPickupP8_fvectorfi);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", kill__12SubwayPickup);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", drop__12SubwayPickup);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", update__12SubwayPickup);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", func_001CF870);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", func_001CF880);
INCLUDE_ASM("asm/nonmatchings/game/SubwayPickup", __tf12SubwayPickup);
struct _fvector;
class SubwayPickup {
public:
    _fvector *getVel(void);
};
_fvector *SubwayPickup::getVel(void)
{
    return (_fvector *)((char *)this + 0x230);
}

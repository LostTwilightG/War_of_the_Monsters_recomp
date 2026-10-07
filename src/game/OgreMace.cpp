#include "common.h"
#include "hieri_types.h"

class OgreMace {
public:
    char pad0[0x214];
    _fvector *endPos;

    void setEndPos(_fvector *p);
};

INCLUDE_ASM("asm/nonmatchings/game/OgreMace", __8OgreMaceP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/OgreMace", setMode__8OgreMaceQ28OgreMace4Mode);
INCLUDE_ASM("asm/nonmatchings/game/OgreMace", update__8OgreMace);
void OgreMace::setEndPos(_fvector *p)
{
    endPos = p;
}

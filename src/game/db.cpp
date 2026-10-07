#include "common.h"
#include "hieri_types.h"
#include "engine.h"

class CsPool {
public:
    static _cs *csActivate(void);
    static _cs *csHPActivate(void);
};

INCLUDE_ASM("asm/nonmatchings/game/db", dbInitDb__FP9_dbheader10_vramAddrs);
INCLUDE_ASM("asm/nonmatchings/game/db", dbProcInteractive__FP9_hierheadP8_fvectorPA3_f);
_cs *dbGetCSForModel(_hierhead *h)
{
    _cs *cs = CsPool::csActivate();

    cs->drawMe = 1;
    hierSetCsEpNode(cs, h);
    return cs;
}
_cs *dbGetHPCSForModel(_hierhead *h)
{
    _cs *cs = CsPool::csHPActivate();

    cs->drawMe = 1;
    hierSetCsEpNode(cs, h);
    return cs;
}

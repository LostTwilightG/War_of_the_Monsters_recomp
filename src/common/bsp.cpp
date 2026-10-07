#include "common.h"
#include "hieri_types.h"

struct _bspresult;

extern _bspresult *gRet;
int bspVecTestIn(_hierbsp *b, _fvector *a, _fvector *c);
float mathfPlaneTest(_plane *pl, _fvector *pt);
int bspVecTest(_hierbsp *b, _fvector *a, _fvector *c, _bspresult *r);

int bspVecTest(_hierbsp *b, _fvector *a, _fvector *c, _bspresult *r)
{
    gRet = r;
    return bspVecTestIn(b, a, c);
}
INCLUDE_ASM("asm/nonmatchings/common/bsp", bspVecTestIn__FP8_hierbspP8_fvectorT1);
int bspPointTest(_hierbsp *b, _fvector *pt)
{
    int ret = 0x200;

    while (b) {
        int neg = mathfPlaneTest(&b->plane, pt) < 0.0f;

        ret = neg ? b->matIdBack : b->matIdFront;
        b = neg ? b->backKid : b->frontKid;
    }
    return ret;
}
int bspVecTest(_hierbsp *b, _linesegment *ls, _bspresult *r)
{
    return bspVecTest(b, (_fvector *)ls, (_fvector *)((char *)ls + 0x10), r);
}
INCLUDE_ASM("asm/nonmatchings/common/bsp", bspPlaneTest2Vec__FP6_planeP8_fvectorN21);
INCLUDE_ASM("asm/nonmatchings/common/bsp", bspInterp__FP8_fvector);
INCLUDE_ASM("asm/nonmatchings/common/bsp", bspTestChild__FP8_hierbspsP8_fvectorT2P10_bspresult);

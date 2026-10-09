#include "common.h"
#include "point_tool_kit.h"

INCLUDE_ASM("asm/nonmatchings/common/PointToolKit", init__12PointToolKiti);
INCLUDE_ASM("asm/nonmatchings/common/PointToolKit", loadPoints__12PointToolKitPcb);
INCLUDE_ASM("asm/nonmatchings/common/PointToolKit", _vt$12PointToolKit);
#ifdef NON_MATCHING
/* 0/7 words: retail writes reticle through a derived pointer (inline vector setter) */
void PointToolKit::resetReticle(void)
{
    reticleX = 0;
    reticleY = 10.0f;
    reticleZ = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/PointToolKit", resetReticle__12PointToolKit);
#endif
PointToolKit::Point *PointToolKit::getPoint(int i)
{
    return &points[i];
}
int PointToolKit::getPathType(Point *a, Point *b)
{
    int r = hasPath(a, b);

    return r + hasPath(b, a) * 2;
}
int PointToolKit::hasPath(Point *a, Point *b)
{
    int idx = getPointIndex(b);
    int i;

    for (i = 0; i < a->numPaths; i++) {
        if (a->paths[i] == idx)
            return 1;
    }
    return 0;
}
int PointToolKit::getPointIndex(Point *p)
{
    return p - points;
}
INCLUDE_ASM("asm/nonmatchings/common/PointToolKit", __tf12PointToolKit);
INCLUDE_ASM("asm/nonmatchings/common/PointToolKit", __12PointToolKit);
void PointToolKit::setPointHeight(float h)
{
    pointHeight = h * 0.5f;
}
void PointToolKit::setPointWidth(float w)
{
    pointWidth = w * 0.5f;
}
void PointToolKit::setPointMoveStep(float s)
{
    pointMoveStep = s;
}
void PointToolKit::setReticleMoveStep(float s)
{
    reticleMoveStep = s;
}
void exportPoints__12PointToolKit(void *self) __asm__("exportPoints__12PointToolKit");
void exportPoints__12PointToolKit(void *self)
{
}
int PointToolKit::getNumPoints(void)
{
    return numPoints;
}

#include "common.h"

/* Virtual members (init, loadPoints, exportPoints), the ctor, __tf and the vtable stay as asm: the vtable is
   emitted with the first virtual function, so defining those here would duplicate it. */
class PointToolKit {
public:
    struct Point {
        char data[0x10];
        int numPaths;
        int paths[11];
    };

    Point points[256];
    int numPoints;
    int view;
    float *mat;
    float *trans;
    float f4010;
    char pad4014[0xC];
    int reticleX;
    float reticleY;
    int reticleZ;
    char pad402C[4];
    int f4030;
    float pointHeight;
    float pointWidth;
    float pointMoveStep;
    float reticleMoveStep;
    void *vptr;

    void init(int v);
    void loadPoints(char *name, bool b);
    void resetReticle(void);
    Point *getPoint(int i);
    int getPathType(Point *a, Point *b);
    int hasPath(Point *a, Point *b);
    int getPointIndex(Point *p);
    void setPointHeight(float h);
    void setPointWidth(float w);
    void setPointMoveStep(float s);
    void setReticleMoveStep(float s);
    void exportPoints(void);
    int getNumPoints(void);
};

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
INCLUDE_ASM("asm/nonmatchings/common/PointToolKit", exportPoints__12PointToolKit);
int PointToolKit::getNumPoints(void)
{
    return numPoints;
}

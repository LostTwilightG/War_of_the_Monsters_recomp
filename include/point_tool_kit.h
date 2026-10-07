#ifndef POINT_TOOL_KIT_H
#define POINT_TOOL_KIT_H

/* Editor-style point container the game's data tools (PathTool, PowerUpTool, StartPointTool, AiPathTool) build on.
   Virtual members (init, loadPoints, exportPoints), the ctor, __tf and the vtable stay as asm: the vtable is emitted with
   the first virtual function, so declaring them virtual here would duplicate it. Derived tools therefore declare their own
   non-virtual versions and reach the base through PointToolKit::. */
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
    char pad4048[8]; /* derived tools start at 0x4050 */

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

#endif

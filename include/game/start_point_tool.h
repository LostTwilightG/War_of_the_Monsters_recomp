#ifndef START_POINT_TOOL_H
#define START_POINT_TOOL_H

#include "point_tool_kit.h"

/* Editor tool that places monster start points. */
class StartPointTool : public PointToolKit {
public:
    void *levelData;

    static StartPointTool instance;

    void init(int i) __asm__("init__14StartPointTooli");
    void loadPoints(void);
    void exportPoints(void);
    unsigned char degreesToData(float d);
    float dataToDegrees(unsigned char c);
};

#endif

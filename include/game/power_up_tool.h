#ifndef POWER_UP_TOOL_H
#define POWER_UP_TOOL_H

#include "point_tool_kit.h"

/* Editor tool that places power-ups; the single instance is exported to PowerUps at level load. */
class PowerUpTool : public PointToolKit {
public:
    void *levelData;

    static PowerUpTool instance;

    void init(int i) __asm__("init__11PowerUpTooli");
    void exportPoints(void);
    void loadPoints(char *name, bool b);
};

#endif

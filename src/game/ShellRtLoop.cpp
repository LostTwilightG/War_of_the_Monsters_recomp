#include "common.h"

void animationRunGlobal(void);

class ShellRtLoop {
public:
    char pad[0x20];
    int started;

    void startField(bool b);
    void onStart(void);
    void onEnd(void);
};

void ShellRtLoop::startField(bool b)
{
    if (!started)
        onStart();
    if (b)
        animationRunGlobal();
}
INCLUDE_ASM("asm/nonmatchings/game/ShellRtLoop", endField__11ShellRtLoopb);
void ShellRtLoop::onStart(void)
{
    started = 1;
}
void ShellRtLoop::onEnd(void)
{
    started = 0;
}

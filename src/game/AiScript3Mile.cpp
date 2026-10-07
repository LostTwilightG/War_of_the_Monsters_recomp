#include "common.h"
#include "game/game.h"
#include "engine.h"

struct _animHandle;

class TaskManager {
public:
    static TaskManager global;
    int add(unsigned (*func)(void *), void *arg, int n);
};

class AiScript3Mile {
public:
    enum State { S0 };
    char pad0[0x20];

    AiScript3Mile();
    unsigned update(void);
    void enterState(State s);
    static unsigned update(void *p);
};

class AiScriptTokyo {
public:
    char pad0[8];
    int f8;

    AiScriptTokyo();
    unsigned update(void);
    static unsigned update(void *p);
};


AiScript3Mile::AiScript3Mile()
{
    TaskManager::global.add(update, this, 1);
    if (game->m_gameMode == 1)
        enterState(S0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiScript3Mile", update__13AiScript3Mile);
INCLUDE_ASM("asm/nonmatchings/game/AiScript3Mile", enterState__13AiScript3MileQ213AiScript3Mile5State);
AiScriptTokyo::AiScriptTokyo()
{
    f8 = 0;
    TaskManager::global.add(update, this, 1);
    animationGetHandle((_animHandle *)this, 0x2744, 0, 0);
}
INCLUDE_ASM("asm/nonmatchings/game/AiScript3Mile", update__13AiScriptTokyo);
unsigned AiScript3Mile::update(void *p)
{
    return ((AiScript3Mile *)p)->update();
}
unsigned AiScriptTokyo::update(void *p)
{
    return ((AiScriptTokyo *)p)->update();
}
INCLUDE_ASM("asm/nonmatchings/game/AiScript3Mile", func_0010F6E8);

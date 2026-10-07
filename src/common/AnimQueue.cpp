#include "common.h"

struct _animHandle {
    int a;
    int b;
    int c;
    int d;
};

void animationSetToBeginning(_animHandle h, bool b);
void animationPause(_animHandle h);

class AnimQueuable {
public:
    _animHandle *handle;
    int f4;
    int f8;
    int fC;
    int f10;
    int f14;

    AnimQueuable();
};

class AnimQueue {
public:
    AnimQueuable q[12];
    int count;

    AnimQueue();
    void Reset(void);
    void Push(_animHandle *h, int a, int b, int c);
    void Pop(void);
    int taskUpdate(void);
    void RequestInterruption(void);
    static unsigned taskUpdate(void *p);
};

AnimQueuable::AnimQueuable()
{
    handle = 0;
    f4 = 0;
    f8 = 0;
    fC = 0;
}
AnimQueue::AnimQueue()
{
    count = 0;
}
void AnimQueue::Reset(void)
{
    int i;

    for (i = 0; i < 12; i++) {
        AnimQueuable *p = &q[i];


        if (p->handle) {
            animationSetToBeginning(*p->handle, true);
            animationPause(*p->handle);
        }
        p->handle = 0;
        p->f4 = 0;
        p->f8 = 0;
        p->fC = 0;
        p->f10 = 0;
        p->f14 = 0;
    }
    count = 0;
}
INCLUDE_ASM("asm/nonmatchings/common/AnimQueue", Push__9AnimQueueP11_animHandleiii);
void AnimQueue::Pop(void)
{
    int i;

    for (i = 0; i < count; i++)
        q[i] = q[i + 1];
    count--;
}
INCLUDE_ASM("asm/nonmatchings/common/AnimQueue", taskUpdate__9AnimQueue);
INCLUDE_ASM("asm/nonmatchings/common/AnimQueue", RequestInterruption__9AnimQueue);
INCLUDE_ASM("asm/nonmatchings/common/AnimQueue", taskUpdate__9AnimQueuePv);

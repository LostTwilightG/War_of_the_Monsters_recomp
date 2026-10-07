#include "common.h"

unsigned timerGetFieldCount(void);

struct BidirLink {
    BidirLink *prev;
    BidirLink *next;
};

static inline void bidirUnlink(BidirLink *l)
{
    BidirLink *p = l->prev;
    BidirLink *n;

    if (p)
        p->next = l->next;
    n = l->next;
    if (n)
        n->prev = l->prev;
    l->next = 0;
}
static inline void bidirInsertAfter(BidirLink *head, BidirLink *l)
{
    l->prev = head;
    l->next = head->next;
    head->next = l;
    if (l->next)
        l->next->prev = l;
}

class TaskManager {
public:
    struct TaskConfig {
        TaskConfig *prev;
        TaskConfig *next;
        unsigned time;
        unsigned (*func)(void *);
        void *arg;
    };

    int unk0;
    TaskConfig *head;
    BidirLink freeList;

    TaskManager();
    void init(unsigned n);
    TaskConfig *add(unsigned (*f)(void *), void *arg, int delay);
    TaskConfig *add(unsigned (*f)(), int delay);
    void remove(void *&handle);
    int update(void);
    void executeAndDeleteAll(void);
    TaskConfig *createTask(void);
    void deleteTask(TaskConfig *t);
};

INCLUDE_ASM("asm/nonmatchings/common/TaskManager", __11TaskManager);
INCLUDE_ASM("asm/nonmatchings/common/TaskManager", init__11TaskManagerUi);
TaskManager::TaskConfig *TaskManager::add(unsigned (*f)(void *), void *arg, int delay)
{
    TaskConfig *t = createTask();

    if (t) {
        unsigned time = timerGetFieldCount() + delay;

        t->func = f;
        t->time = time;
        t->arg = arg;
    }
    return t;
}
TaskManager::TaskConfig *TaskManager::add(unsigned (*f)(), int delay)
{
    TaskConfig *t = createTask();

    if (t) {
        unsigned time = timerGetFieldCount() + delay;

        t->func = (unsigned (*)(void *))f;
        t->time = time;
        t->arg = 0;
    }
    return t;
}
void TaskManager::remove(void *&handle)
{
    if (handle) {
        deleteTask((TaskConfig *)handle);
        handle = 0;
    }
}
#ifdef NON_MATCHING
/* 24/47 words: retail block layout of the run/skip branches differs */
int TaskManager::update(void)
{
    unsigned fc = timerGetFieldCount();
    TaskConfig *t = head;

    while (t) {
        if (fc < t->time) {
            t = t->next;
        } else {
            if (t->arg)
                t->time = t->func(t->arg);
            else
                t->time = ((unsigned (*)())t->func)();
            if (t->time) {
                t->time += fc;
                t = t->next;
            } else {
                TaskConfig *n = t->next;

                deleteTask(t);
                t = n;
            }
        }
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/TaskManager", update__11TaskManager);
#endif
#ifdef NON_MATCHING
/* 19/32 words: task/next pointers get swapped callee-saved registers */
void TaskManager::executeAndDeleteAll(void)
{
    TaskConfig *n;
    TaskConfig *t = head;

    while (t) {
        if (t->arg) {
            t->func(t->arg);
            n = t->next;
        } else {
            t->time = ((unsigned (*)())t->func)();
            n = t->next;
        }
        deleteTask(t);
        t = n;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/TaskManager", executeAndDeleteAll__11TaskManager);
#endif
TaskManager::TaskConfig *TaskManager::createTask(void)
{
    TaskConfig *t = (TaskConfig *)freeList.next;

    if (!t)
        return 0;
    bidirUnlink((BidirLink *)t);
    bidirInsertAfter((BidirLink *)this, (BidirLink *)t);
    return t;
}
void TaskManager::deleteTask(TaskConfig *t)
{
    bidirUnlink((BidirLink *)t);
    bidirInsertAfter(&freeList, (BidirLink *)t);
}
INCLUDE_ASM("asm/nonmatchings/common/TaskManager", __static_initialization_and_destruction_0_00222338);
INCLUDE_ASM("asm/nonmatchings/common/TaskManager", func_00222380);
INCLUDE_ASM("asm/nonmatchings/common/TaskManager", _GLOBAL_$I$_11TaskManager$global);

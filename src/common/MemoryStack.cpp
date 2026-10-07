#include "common.h"

class MemoryStack {
public:
    void *start;
    char *low;
    int mark;
    char *end;
    char *hi1;
    char *hi2;

    MemoryStack(void *mem, unsigned size);
    void clear(void);
};

MemoryStack::MemoryStack(void *mem, unsigned size)
{
    start = mem;
    end = (char *)mem + size;
    clear();
}
void MemoryStack::clear(void)
{
    char *p = (char *)(((int)end - 4) & ~3);
    int *q;
    int old;

    mark = 0;
    low = (char *)start;
    hi1 = p;
    hi2 = p;
    *(int *)p = 0;
    q = (int *)(((int)low + 3) & ~3);
    old = mark;
    mark = (int)q;
    low = (char *)(q + 1);
    *q = old;
}
INCLUDE_ASM("asm/nonmatchings/common/MemoryStack", __static_initialization_and_destruction_0_0020F3C8);
INCLUDE_ASM("asm/nonmatchings/common/MemoryStack", _GLOBAL_$I$_11MemoryStack$global);
INCLUDE_ASM("asm/nonmatchings/common/MemoryStack", _GLOBAL_$D$_11MemoryStack$global);

#include "common.h"
#include "memory_stack.h"

MemoryStack::MemoryStack(void *mem, unsigned size)
{
    start = mem;
    end = (char *)mem + size;
    clear();
}
void MemoryStack::clear(void)
{
    char *p = (char *)(((int)end - 4) & ~3);

    mark = 0;
    low = (char *)start;
    hi1 = p;
    hi2 = p;
    *(int *)p = 0;
    pushMark();
}
INCLUDE_ASM("asm/nonmatchings/common/MemoryStack", __static_initialization_and_destruction_0_0020F3C8);
INCLUDE_ASM("asm/nonmatchings/common/MemoryStack", _GLOBAL_$I$_11MemoryStack$global);
INCLUDE_ASM("asm/nonmatchings/common/MemoryStack", _GLOBAL_$D$_11MemoryStack$global);

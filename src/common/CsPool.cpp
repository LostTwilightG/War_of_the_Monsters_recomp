#include "common.h"
#include "hieri_types.h"
#include "cs_pool.h"

extern "C" int printf(const char *, ...);

struct StackLayout {
    void *start;
    char *low;
    int mark;
    char *end;
};

extern StackLayout gMemStack __asm__("_11MemoryStack$global");
void *operator new(unsigned, void *);
void InitColGridCsListArray(void);
void hierInitCs(_cs *);
void hdRemoveCsFromGrid(_cs *);

#ifdef NON_MATCHING
/* 55/57 words: order of the six list-head stores differs */
void CsPool::init(void)
{
    CsNode *node;
    int i;

    m_activeList.next = &m_activeList;
    m_activeList.prev = &m_activeList;
    m_inactiveList.prev = &m_inactiveList;
    m_inactiveList.next = &m_inactiveList;
    m_HPActiveList.prev = &m_HPActiveList;
    m_HPActiveList.next = &m_HPActiveList;
    for (i = 0; i < 1024; i++) {
        char *mem = (char *)(((int)gMemStack.low + 0xF) & -16);

        gMemStack.low = mem + 0xC;
        node = new (mem) CsNode;
        node->cs = &m_csPool[i];
        node->prev = &m_inactiveList;
        node->next = m_inactiveList.next;
        m_inactiveList.next = node;
        if (node->next)
            node->next->prev = node;
    }
    InitColGridCsListArray();
}
#else
INCLUDE_ASM("asm/nonmatchings/common/CsPool", init__6CsPool);
#endif
INCLUDE_ASM("asm/nonmatchings/common/CsPool", csActivate__6CsPool);
INCLUDE_ASM("asm/nonmatchings/common/CsPool", csDeactivate__6CsPoolP3_cs);
INCLUDE_ASM("asm/nonmatchings/common/CsPool", csHPActivate__6CsPool);
INCLUDE_ASM("asm/nonmatchings/common/CsPool", csHPDeactivate__6CsPoolP3_cs);
INCLUDE_ASM("asm/nonmatchings/common/CsPool", __static_initialization_and_destruction_0_001F7858);
INCLUDE_ASM("asm/nonmatchings/common/CsPool", _GLOBAL_$I$_6CsPool$m_activeList);

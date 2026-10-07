#include "common.h"
#include "hieri_types.h"

extern "C" int printf(const char *, ...);

struct ColGridCsListNode {
    ColGridCsListNode *next;
    _cs *cs;
};

class ColGridCsList {
public:
    ColGridCsListNode *head;

    void pushCsFront(_cs *cs);
    void removeCs(_cs *cs);
};

extern ColGridCsListNode D_007B3050[1024];
extern ColGridCsListNode *D_006F8DF4;

ColGridCsListNode *allocateColGridCsListNode(void);
void deallocColGridCsListNode(ColGridCsListNode *node);

#ifdef NON_MATCHING
/* 2/16 words: loop strength reduction / store order differ */
void InitColGridCsListArray(void)
{
    int i;
    ColGridCsListNode *n = D_007B3050;

    for (i = 0x3FE; i >= 0; i--, n++) {
        n->next = n + 1;
        n->cs = 0;
    }
    D_006F8DF4 = D_007B3050;
    D_007B3050[1023].next = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/ColGrid", InitColGridCsListArray__Fv);
#endif
#ifdef NON_MATCHING
/* 7/18 words: retail lays the success path after the printf path */
ColGridCsListNode *allocateColGridCsListNode(void)
{
    ColGridCsListNode *n = D_006F8DF4;

    if (!n) {
        printf("Too many CS's in CS lists\n");
        return 0;
    }
    D_006F8DF4 = n->next;
    n->next = 0;
    return n;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/ColGrid", allocateColGridCsListNode__Fv);
#endif
void deallocColGridCsListNode(ColGridCsListNode *node)
{
    node->next = D_006F8DF4;
    D_006F8DF4 = node;
}
void ColGridCsList::pushCsFront(_cs *cs)
{
    ColGridCsListNode *n = allocateColGridCsListNode();

    if (n) {
        n->cs = cs;
        n->next = head;
        head = n;
    }
}
#ifdef NON_MATCHING
/* 1/29 words: retail peels the first loop iteration */
void ColGridCsList::removeCs(_cs *cs)
{
    ColGridCsListNode *prev = (ColGridCsListNode *)this;

    while (prev->next) {
        if (prev->next->cs == cs) {
            ColGridCsListNode *n = prev->next;
            ColGridCsListNode *nx = n->next;

            deallocColGridCsListNode(n);
            prev->next = nx;
            return;
        }
        prev = prev->next;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/ColGrid", removeCs__13ColGridCsListP3_cs);
#endif

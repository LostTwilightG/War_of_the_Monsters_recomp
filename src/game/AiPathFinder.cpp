#include "common.h"
#include "bidir_link.h"
#include "memory_stack.h"

extern "C" void *memset(void *, int, unsigned);
struct AiPath {
    char data[0x14];
};

/* A* path search over the AI path net. A node (0x20 bytes) is a BidirLink plus costs (g at 8, estimate at 0xC, f at 0x10), the path it stands for,
   its parent in the search and an "open" flag; the open list is kept sorted by f, the global list indexes nodes by path number. */
class AiPathFinder {
public:
    struct Node : BidirLink {
        float cost;
        float estimate;
        float total;
        AiPath *path;
        Node *parent;
        int open;
    };
    struct OpenList : BidirLink {
        void add(Node *n);
        void adjust(Node *n);
        Node *popBest(void);
        Node *find(AiPath *p);
    };
    struct GlobalList {
        Node **nodes;
        void init(int n);
    };

    void *net;          /* 0x00 AiPathNet */
    void *monster;      /* 0x04 */
    void *goal;         /* 0x08 */
    void *targetPos;    /* 0x0C */
    OpenList open;      /* 0x10 */
    GlobalList global;  /* 0x18 */
    float f1C;
    float zWeight;
    float zWeightHolding;

    int compileResultForReturn(AiPath **out, Node *last);
};
extern MemoryStack *g_mem;
__asm__("#SNFIX_SMALL g_mem");


INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", __12AiPathFinderR9AiPathNetR7Monster);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computePath__12AiPathFinderPP6AiPathR8_fvectorR6AiPath);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", traceSuccessor__12AiPathFinderR6AiPathR10AiPathNodePQ212AiPathFinder4NodeT1);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computeCostEstimate__12AiPathFinderR6AiPathR10AiPathNodeT1);
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computeCostActual__12AiPathFinderR6AiPathR10AiPathNodeT1);
/* Walks the parent chain from the goal node, then writes the paths start-to-goal into `out` (zero terminated); returns how many. */
int AiPathFinder::compileResultForReturn(AiPath **out, Node *last)
{
    AiPath **tmp = (AiPath **)global.nodes;
    int n = 0;

    while (last != 0) {
        n++;
        *tmp = last->path;
        last = last->parent;
        tmp++;
    }
    tmp--;
    {
        int i;

        for (i = n; i != 0; i--) {
            *out = *tmp;
            tmp--;
            out++;
        }
    }
    *out = 0;
    return n;
}
#ifdef NON_MATCHING
/* Sorted insert by f (total): before the first node with a larger f, else at the end. */
void AiPathFinder::OpenList::add(Node *n)
{
    BidirLink *at = this;
    BidirLink *cur = at->next;

    if (cur != 0) {
        do {
            if (n->total < ((Node *)cur)->total) {
                n->next = cur;
                n->prev = cur->prev;
                cur->prev = n;
                if (n->prev != 0)
                    n->prev->next = n;
                return;
            }
            at = cur;
            cur = at->next;
        } while (cur != 0);
    }
    n->prev = at;
    n->next = at->next;
    at->next = n;
    if (n->next != 0)
        n->next->prev = n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", add__Q212AiPathFinder8OpenListPQ212AiPathFinder4Node);
#endif
/* A node whose f changed: unlink it and insert it again at its sorted place. */
void AiPathFinder::OpenList::adjust(Node *n)
{
    if (n->prev != 0)
        n->prev->next = n->next;
    if (n->next != 0)
        n->next->prev = n->prev;
    n->next = 0;
    n->prev = 0;
    add(n);
}
/* Removes and returns the head of the list (lowest f), or 0 when empty. */
AiPathFinder::Node *AiPathFinder::OpenList::popBest(void)
{
    Node *n = (Node *)next;

    if (n != 0) {
        if (n->prev != 0)
            n->prev->next = n->next;
        if (n->next != 0)
            n->next->prev = n->prev;
        n->next = 0;
        n->prev = 0;
    }
    return n;
}
AiPathFinder::Node *AiPathFinder::OpenList::find(AiPath *p)
{
    Node *n = (Node *)next;

    while (n != 0) {
        if (n->path == p)
            return n;
        n = (Node *)n->next;
    }
    return 0;
}
/* One Node pointer per path, taken from the global memory stack and cleared. */
void AiPathFinder::GlobalList::init(int n)
{
    int size = n * 4;
    char *base = (char *)(((int)g_mem->low + 0xF) & ~0xF);

    g_mem->low = base + (size & 0xFFFF);
    nodes = (Node **)base;
    memset(base, 0, size);
}
void BidirLink::init(void)
{
    next = 0;
    prev = 0;
}

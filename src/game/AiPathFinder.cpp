#include "common.h"
#include "vecmath.h"
#include "bidir_link.h"
#include "memory_stack.h"

extern "C" void *memset(void *, int, unsigned);
struct AiPath {
    char data[0x14];
};
struct AiPathNode {
    char data[0x30];
};
struct _fvector;
extern "C" float sqrtf(float);
class Monster {
public:
    bool isHolding(void);
};
#define U16(p, o) (*(unsigned short *)((char *)(p) + (o)))
#define FL(p, o) (*(float *)((char *)(p) + (o)))
#define PT(p, o) ((float *)((char *)(p) + (o)))
extern float s_zWeight;
extern float s_impossibleCost;
__asm__("#SNFIX_SMALL s_zWeight");
__asm__("#SNFIX_SMALL s_impossibleCost");

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
    float computeCostEstimate(AiPath &path, AiPathNode &node, AiPath &goal);
    float computeCostActual(AiPath &path, AiPathNode &node, AiPath &other);
    void traceSuccessor(AiPath &path, AiPathNode &node, Node *parent, AiPath &goal);
    void computePath(AiPath **out, _fvector &target, AiPath &goal);
};
extern MemoryStack *g_mem;
__asm__("#SNFIX_SMALL g_mem");


INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", __12AiPathFinderR9AiPathNetR7Monster);
#ifdef NON_MATCHING
/* A* from the path the monster stands on to `goal`, targeting `target`; the result (paths start to goal, zero terminated) goes to `out`. Working memory
   comes from the global stack and is released at the end. */
void AiPathFinder::computePath(AiPath **out, _fvector &target, AiPath &goal)
{
    Node *cur;
    AiPath *start = *(AiPath **)((char *)monster + 0x1A10);

    g_mem->pushMark();
    s_zWeight = ((Monster *)monster)->isHolding() ? zWeightHolding : zWeight;
    open.next = 0;
    open.prev = 0;
    global.init(U16(net, 2));
    targetPos = &target;
    this->goal = &goal;
    cur = (Node *)(((int)g_mem->low + 0xF) & ~0xF);
    g_mem->low = (char *)cur + 0x20;
    cur->init();
    cur->cost = 0.0f;
    cur->estimate = computeCostEstimate(*start, *(AiPathNode *)PT(net, U16(start, 0) * 0x30 + 0x10), goal);
    cur->total = cur->estimate;
    cur->path = start;
    cur->parent = 0;
    if (cur != 0 && cur->path != &goal) {
        do {
            AiPath *p = cur->path;
            char *a = (char *)net + U16(p, 0) * 0x30 + 0x10;
            char *b;
            int k, n;

            n = *(unsigned char *)(a + 0x28);
            for (k = 0; k < n; k++)
                traceSuccessor(*(AiPath *)((char *)net + U16(a, 0x10 + k * 2) * 0x14 + 0x3850), *(AiPathNode *)a, cur, goal);
            b = (char *)net + U16(cur->path, 2) * 0x30 + 0x10;
            n = *(unsigned char *)(b + 0x28);
            for (k = 0; k < n; k++)
                traceSuccessor(*(AiPath *)((char *)net + U16(b, 0x10 + k * 2) * 0x14 + 0x3850), *(AiPathNode *)b, cur, goal);
            cur->open = 0;
            cur = open.popBest();
        } while (cur != 0 && cur->path != &goal);
    }
    compileResultForReturn(out, cur);
    g_mem->low = (char *)g_mem->mark;
    g_mem->mark = *(int *)g_mem->low;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computePath__12AiPathFinderPP6AiPathR8_fvectorR6AiPath);
#endif
#ifdef NON_MATCHING
/* One A* expansion: cost of reaching `path` through `parent` (never straight back to the parent's parent); keep it if it beats what the global list has
   for that path (new node, or re-sort an open one), as long as it stays under s_impossibleCost. */
void AiPathFinder::traceSuccessor(AiPath &path, AiPathNode &node, Node *parent, AiPath &goal)
{
    float g;
    Node **slot;
    Node *n;
    int wasOpen = 0;

    if (parent->parent != 0 && parent->parent->path == &path)
        return;
    g = parent->cost + computeCostActual(*parent->path, node, path);
    if (s_impossibleCost <= g)
        return;
    slot = &global.nodes[((char *)&path - ((char *)net + 0x3850)) / 0x14];
    n = *slot;
    if (n != 0) {
        if (n->cost <= g)
            return;
        wasOpen = n->open != 0;
    } else {
        n = (Node *)(((int)g_mem->low + 0xF) & ~0xF);
        g_mem->low = (char *)n + 0x20;
        n->init();
        *slot = n;
    }
    n->cost = g;
    n->estimate = computeCostEstimate(path, node, goal);
    n->path = &path;
    n->parent = parent;
    n->open = 1;
    n->total = g + n->estimate;
    if (wasOpen)
        open.adjust(n);
    else
        open.add(n);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", traceSuccessor__12AiPathFinderR6AiPathR10AiPathNodePQ212AiPathFinder4NodeT1);
#endif
#ifdef NON_MATCHING
/* Straight-line distance from `node` to the goal path's first node, scaled by f1C (3.0). */
float AiPathFinder::computeCostEstimate(AiPath &path, AiPathNode &node, AiPath &goal)
{
    float *a = (float *)&node;
    float *b = PT(net, U16(&goal, 0) * 0x30 + 0x10);
    float dx = a[0] - b[0];
    float dy = a[1] - b[1];
    float dz = a[2] - b[2];

    return eeSqrtf(dx * dx + dy * dy + dz * dz) * f1C;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computeCostEstimate__12AiPathFinderR6AiPathR10AiPathNodeT1);
#endif
#ifdef NON_MATCHING
/* Cost of walking `path` into `node`: from the monster's own position when `path` is the one it stands on, else the path's length/4 with the
   height difference between its two end nodes (sign flipped when it leads into `other`). Uphill (dz > 0) adds dz/cost * s_zWeight. When `other`
   is the goal path the straight leg to the target position is added the same way. */
float AiPathFinder::computeCostActual(AiPath &path, AiPathNode &node, AiPath &other)
{
    float cost;
    float dx, dy, dz;
    float *n = (float *)&node;

    if (&path == *(AiPath **)((char *)monster + 0x1A10)) {
        float *m = PT(*(char **)((char *)monster + 0xC), 0x10);

        dx = n[0] - m[0];
        dy = n[1] - m[1];
        dz = n[2] - m[2];
        cost = eeSqrtf(dx * dx + dy * dy + dz * dz);
    } else {
        float *a = PT(net, U16(&path, 0) * 0x30 + 0x10);
        float *b = PT(net, U16(&path, 2) * 0x30 + 0x10);

        dx = a[0] - b[0];
        dy = a[1] - b[1];
        dz = a[2] - b[2];
        cost = (float)U16(&path, 0x10) * 0.25f;
        if (U16(&path, 2) == U16(&other, 0) || U16(&path, 2) == U16(&other, 2))
            dz = -dz;
    }
    if (dz > 0.0f) {
        dz = dz / cost;
        cost += dz * s_zWeight;
    }
    if (&other == (AiPath *)goal) {
        float *t = (float *)targetPos;
        float ex = t[0] - n[0];
        float ey = t[1] - n[1];
        float ez = t[2] - n[2];

        cost += eeSqrtf(ex * ex + ey * ey + ez * ez);
        if (ez > 0.0f) {
            ez = ez / cost;
            cost += ez * s_zWeight;
        }
    }
    return cost;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiPathFinder", computeCostActual__12AiPathFinderR6AiPathR10AiPathNodeT1);
#endif
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

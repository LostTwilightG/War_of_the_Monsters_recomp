#include "common.h"
#include "hieri_types.h"

/* One time-of-day key (12 of them, 0x80 bytes each); the fields are filled by todSetTOD. */
struct _todInfo {
    char data[0x80];
};

/* Node of the time-of-day key list (indices into CTODLinkList::nodes, -1 = none). */
struct _LinkNode {
    int next;       /* 0x00: -1 while the node is free */
    int prev;       /* 0x04 */
    int index;      /* 0x08 */
    int time;       /* 0x0C */
    _todInfo *info; /* 0x10 */
};

class CTODLinkList {
public:
    int numEntries;      /* 0x00 */
    int head;            /* 0x04 */
    int tail;            /* 0x08 */
    _LinkNode nodes[12]; /* 0x0C */

    CTODLinkList(void);
    ~CTODLinkList(void);
    int AddEntry(void);
    void DeleteEntry(int);
    int GetNumEntries(void);
    _LinkNode *FindNodeByTime(float);
    int FindFreeNode(void);
    void SortList(void);
    void ClearNode(_LinkNode *node);
};

extern _todInfo todInfo[12];

/* One sky group (todSetSkyEntry): its first three objects' colors and UV scroll / fog burn-through. */
struct _todSky {
    HierHead *hier; /* 0x00 */
    int pad[3];
    struct {
        float red, green, blue, alpha;
    } layer[3]; /* 0x10 */
    struct {
        float u, v, fogBurnThru, zBufferFudge;
    } mod[3]; /* 0x40 */
};

extern "C" float cosf(float);

extern int todTimeOfDayOn;
extern int todSetTimeOfDay;
extern float todTimeOfDayMinutesPerSecond;
extern float todTimeOfDay;
extern int todFrame;
extern int todAddFrame;
extern int todDeleteFrame;
__asm__("#SNFIX_SMALL todTimeOfDayOn");
__asm__("#SNFIX_SMALL todSetTimeOfDay");
__asm__("#SNFIX_SMALL todTimeOfDayMinutesPerSecond");
__asm__("#SNFIX_SMALL todTimeOfDay");
__asm__("#SNFIX_SMALL todFrame");
__asm__("#SNFIX_SMALL todAddFrame");
__asm__("#SNFIX_SMALL todDeleteFrame");
extern float todAmbientRed;
extern float todAmbientGreen;
extern float todAmbientBlue;
extern int todNumSkys;
extern int todSkySetFound;
extern _todSky todSky[10];
extern int s_todPlightsEnabled[];

int todActive(void);

void todBoundAngle(float *angle)
{
    while (*angle >= 180.0f)
        *angle -= 360.0f;
    while (*angle <= -180.0f)
        *angle += 360.0f;
}
/* Default key i of 12 (every two hours): cosine ramp from lo (midday) to hi (midnight). */
float todCalculateValue(int i, float lo, float hi)
{
    return lo + cosf(((float)i / 5.5f - 1.0f) * 1.5707964f) * (hi - lo);
}
INCLUDE_ASM("asm/nonmatchings/common/tod", todInit__Fv);
void todInitStats(void)
{
}
INCLUDE_ASM("asm/nonmatchings/common/tod", todUpdate__Fi);
int todActive(void)
{
    return todTimeOfDayOn;
}
INCLUDE_ASM("asm/nonmatchings/common/tod", todGetTOD__FPiPfP10_todInfo14);
INCLUDE_ASM("asm/nonmatchings/common/tod", todSetTOD__FifPvf);
float todGetCurrentTOD(void)
{
    if (todActive())
        return todTimeOfDay;
    return -1.0f;
}
void todGetAmbient(float *r, float *g, float *b)
{
    *r = todAmbientRed;
    *g = todAmbientGreen;
    *b = todAmbientBlue;
}
/* Registers a sky group (its id selects the slot); its first three objects give the layer colors and UV/fog. */
void todSetSkyEntry(HierHead *node)
{
    if (todNumSkys < 10 && node->opcode == GROUP_NODE) {
        _hiergroup *group = (_hiergroup *)node;
        _todSky *sky = &todSky[node->id2];
        int i;

        sky->hier = node;
        for (i = 0; i < group->numKids; i++) {
            _hierobject *obj = (_hierobject *)group->child[i];

            if (group->child[i]->opcode == OBJECT_NODE && i < 3) {
                /* retail indexes the entry as rows of four floats: row i + 1 is layer[i], row i + 4 is mod[i] */
                float (*row)[4] = (float (*)[4])sky;

                row[i + 1][0] = obj->redAnim;
                row[i + 1][1] = obj->greenAnim;
                row[i + 1][2] = obj->blueAnim;
                row[i + 1][3] = obj->alphaAnim;
                row[i + 4][0] = obj->uOffset;
                row[i + 4][1] = obj->vOffset;
                row[i + 4][2] = obj->fogBurnThru;
                row[i + 4][3] = obj->zBufferFudge;
            }
        }
        todNumSkys++;
    }
    if (todSky[0].hier && todSky[1].hier && todSky[2].hier && todSky[3].hier)
        todSkySetFound = 1;
}
INCLUDE_ASM("asm/nonmatchings/common/tod", todSetSkyObjects__Fiffffff);
void todSetPlightsActive(int which, int on)
{
    s_todPlightsEnabled[which] = on;
}
CTODLinkList::CTODLinkList(void)
{
    int i;

    numEntries = 0;
    head = -1;
    tail = -1;
    for (i = 0; i < 12; i++) {
        ClearNode(&nodes[i]);
        nodes[i].info = &todInfo[i];
    }
}
CTODLinkList::~CTODLinkList(void)
{
}
int CTODLinkList::AddEntry(void)
{
    if (numEntries < 12) {
        int idx = FindFreeNode();

        if (idx != -1 && head == -1) {
            tail = head = idx;
            numEntries++;
        }
    }
    return 0;
}
void CTODLinkList::DeleteEntry(int)
{
}
int CTODLinkList::GetNumEntries(void)
{
    return numEntries;
}
_LinkNode *CTODLinkList::FindNodeByTime(float)
{
    return 0;
}
int CTODLinkList::FindFreeNode(void)
{
    int i;

    for (i = 0; i < 12; i++) {
        if (nodes[i].next == -1)
            return i;
    }
    return -1;
}
void CTODLinkList::SortList(void)
{
}
void CTODLinkList::ClearNode(_LinkNode *node)
{
    node->next = -1;
    node->prev = -1;
    node->index = -1;
    node->time = -1;
    node->info = 0;
}
INCLUDE_ASM("asm/nonmatchings/common/tod", __static_initialization_and_destruction_0_00225688);
INCLUDE_ASM("asm/nonmatchings/common/tod", _GLOBAL_$I$todLink);
INCLUDE_ASM("asm/nonmatchings/common/tod", _GLOBAL_$D$todLink);

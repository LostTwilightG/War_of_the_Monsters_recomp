#include "common.h"
#include "hieri_types.h"

/* One time-of-day key, file format 1.4 (12 of them, one every two hours); todUpdate blends the keys around the current time. */
struct _todInfo14 {
    float time;                                    /* 0x00: hour */
    float ambientRed, ambientGreen, ambientBlue;   /* 0x04 */
    float fogMinRange, fogMaxRange, fogMaxVal;     /* 0x10 */
    float fogFarClip;                              /* 0x1C */
    float fogRed, fogGreen, fogBlue;               /* 0x20 */
    float unk2C[3];                                /* 0x2C */
    float unk38;                                   /* 0x38 */
    float dirHeading;                              /* 0x3C */
    float dirRed, dirGreen, dirBlue;               /* 0x40 */
    float unk4C;                                   /* 0x4C */
    float backRed, backGreen, backBlue;            /* 0x50 */
    int sky;                                       /* 0x5C: sky set */
    float unk60;                                   /* 0x60 */
    float unk64[7];                                /* 0x64 */
};

/* Older key layouts accepted by todSetTOD. */
struct _todInfo10 {
    float time;
    float ambientRed, ambientGreen, ambientBlue;
    float fogRed, fogGreen, fogBlue;
    float fogMinRange;
    int sky;
};
struct _todInfo11 {
    float time;
    float ambientRed, ambientGreen, ambientBlue;
    float fogRed, fogGreen, fogBlue;
    float fogMinRange;
    int sky;
    float unk24, unk28; /* not loaded */
    float unk60;
    float unk64[3];
};
struct _todInfo12 {
    float time;
    float ambientRed, ambientGreen, ambientBlue;
    float fogMinRange, fogMaxRange, fogMaxVal, fogFarClip;
    float fogRed, fogGreen, fogBlue;
    float unk38;
    float dirHeading;
    float dirRed, dirGreen, dirBlue;
    int sky;
    float unk44, unk48; /* not loaded */
    float unk60;
    float unk64[3];
};
struct _todInfo13 {
    float time;
    float ambientRed, ambientGreen, ambientBlue;
    float fogMinRange, fogMaxRange, fogMaxVal, fogFarClip;
    float fogRed, fogGreen, fogBlue;
    float unk38;
    float dirHeading;
    float dirRed, dirGreen, dirBlue;
    float unk4C;
    int sky;
    float unk60;
    float unk64[7];
};
typedef char _size__todInfo10[sizeof(_todInfo10) == 0x24 ? 1 : -1];
typedef char _size__todInfo11[sizeof(_todInfo11) == 0x3C ? 1 : -1];
typedef char _size__todInfo12[sizeof(_todInfo12) == 0x5C ? 1 : -1];
typedef char _size__todInfo13[sizeof(_todInfo13) == 0x68 ? 1 : -1];
typedef char _size__todInfo14[sizeof(_todInfo14) == 0x80 ? 1 : -1];

/* Node of the time-of-day key list (indices into CTODLinkList::nodes, -1 = none). */
struct _LinkNode {
    int next;       /* 0x00: -1 while the node is free */
    int prev;       /* 0x04 */
    int index;      /* 0x08 */
    int time;       /* 0x0C */
    _todInfo14 *info; /* 0x10 */
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

extern _todInfo14 todInfo[12];

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
float todCalculateValue(int i, float lo, float hi);

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
void todInit(void)
{
    int i;

    todNumSkys = 0;
    todTimeOfDayMinutesPerSecond = 30.0f;
    todSkySetFound = 0;
    todTimeOfDayOn = 0;
    todSetTimeOfDay = 0;
    todTimeOfDay = 0.0f;
    todFrame = 0;
    todAddFrame = 0;
    todDeleteFrame = 0;
    for (i = 9; i >= 0; i--)
        todSky[i].hier = 0;
    for (i = 0; i < 12; i++) {
        todInfo[i].ambientRed = todCalculateValue(i, 10.0f, 30.0f);
        todInfo[i].ambientGreen = todCalculateValue(i, 10.0f, 30.0f);
        todInfo[i].ambientBlue = todCalculateValue(i, 10.0f, 30.0f);
        todInfo[i].fogMinRange = 3000.0f;
        todInfo[i].fogMaxRange = 8000.0f;
        todInfo[i].fogMaxVal = 255.0f;
        todInfo[i].fogFarClip = 8000.0f;
        todInfo[i].fogRed = todCalculateValue(i, 20.0f, 50.0f);
        todInfo[i].fogGreen = todCalculateValue(i, 20.0f, 50.0f);
        todInfo[i].fogBlue = todCalculateValue(i, 20.0f, 50.0f);
        todInfo[i].unk2C[0] = todCalculateValue(i, 20.0f, 90.0f);
        todInfo[i].unk2C[1] = todCalculateValue(i, 20.0f, 90.0f);
        todInfo[i].unk2C[2] = todCalculateValue(i, 20.0f, 90.0f);
        todInfo[i].unk38 = 0.0f;
        todInfo[i].dirRed = todCalculateValue(i, 70.0f, 70.0f);
        todInfo[i].dirGreen = todCalculateValue(i, 70.0f, 200.0f);
        todInfo[i].dirBlue = todCalculateValue(i, 70.0f, 200.0f);
        todInfo[i].unk4C = 20.0f;
        todInfo[i].backRed = 128.0f;
        todInfo[i].backGreen = 128.0f;
        todInfo[i].backBlue = 128.0f;
        todInfo[i].unk60 = 0.0f;
        todInfo[i].unk64[0] = 1.0f;
        todInfo[i].unk64[1] = 1.0f;
        todInfo[i].unk64[2] = 1.0f;
        todInfo[i].unk64[3] = 1.0f;
        todInfo[i].unk64[4] = 1.0f;
        todInfo[i].unk64[5] = 1.0f;
        todInfo[i].unk64[6] = 1.0f;
    }
    todInfo[0].time = 0.0f;
    todInfo[1].time = 2.0f;
    todInfo[2].time = 4.0f;
    todInfo[3].time = 6.0f;
    todInfo[4].time = 8.0f;
    todInfo[5].time = 10.0f;
    todInfo[6].time = 12.0f;
    todInfo[7].time = 14.0f;
    todInfo[8].time = 16.0f;
    todInfo[9].time = 18.0f;
    todInfo[10].time = 20.0f;
    todInfo[11].time = 22.0f;
    todInfo[0].sky = 2;
    todInfo[1].sky = 2;
    todInfo[2].sky = 2;
    todInfo[3].sky = 3;
    todInfo[4].sky = 3;
    todInfo[5].sky = 0;
    todInfo[6].sky = 0;
    todInfo[7].sky = 0;
    todInfo[8].sky = 0;
    todInfo[9].sky = 1;
    todInfo[10].sky = 1;
    todInfo[11].sky = 2;
    todInfo[0].dirHeading = 270.0f;
    todInfo[1].dirHeading = 300.0f;
    todInfo[2].dirHeading = 330.0f;
    todInfo[3].dirHeading = 0.0f;
    todInfo[4].dirHeading = 30.0f;
    todInfo[5].dirHeading = 60.0f;
    todInfo[6].dirHeading = 90.0f;
    todInfo[7].dirHeading = 120.0f;
    todInfo[8].dirHeading = 150.0f;
    todInfo[9].dirHeading = 180.0f;
    todInfo[10].dirHeading = 210.0f;
    todInfo[11].dirHeading = 240.0f;
}
void todInitStats(void)
{
}
INCLUDE_ASM("asm/nonmatchings/common/tod", todUpdate__Fi);
int todActive(void)
{
    return todTimeOfDayOn;
}
#ifdef NON_MATCHING
/* 98% (16 differing words incl. relocs): the spilled loop counter is reloaded before the compare in retail */
void todGetTOD(int *on, float *minutesPerSecond, _todInfo14 *out)
{
    int i;

    *on = todTimeOfDayOn;
    *minutesPerSecond = todTimeOfDayMinutesPerSecond;
    for (i = 0; i < 12; i++, out++) {
        out->time = todInfo[i].time;
        out->ambientRed = todInfo[i].ambientRed;
        out->ambientGreen = todInfo[i].ambientGreen;
        out->ambientBlue = todInfo[i].ambientBlue;
        out->fogMinRange = todInfo[i].fogMinRange;
        out->fogMaxRange = todInfo[i].fogMaxRange;
        out->fogMaxVal = todInfo[i].fogMaxVal;
        out->fogFarClip = todInfo[i].fogFarClip;
        out->fogRed = todInfo[i].fogRed;
        out->fogGreen = todInfo[i].fogGreen;
        out->fogBlue = todInfo[i].fogBlue;
        out->unk2C[0] = todInfo[i].unk2C[0];
        out->unk2C[1] = todInfo[i].unk2C[1];
        out->unk2C[2] = todInfo[i].unk2C[2];
        out->unk38 = todInfo[i].unk38;
        out->dirHeading = todInfo[i].dirHeading;
        out->dirRed = todInfo[i].dirRed;
        out->dirGreen = todInfo[i].dirGreen;
        out->dirBlue = todInfo[i].dirBlue;
        out->unk4C = todInfo[i].unk4C;
        out->backRed = todInfo[i].backRed;
        out->backGreen = todInfo[i].backGreen;
        out->backBlue = todInfo[i].backBlue;
        out->sky = todInfo[i].sky;
        out->unk60 = todInfo[i].unk60;
        out->unk64[0] = todInfo[i].unk64[0];
        out->unk64[1] = todInfo[i].unk64[1];
        out->unk64[2] = todInfo[i].unk64[2];
        out->unk64[3] = todInfo[i].unk64[3];
        out->unk64[4] = todInfo[i].unk64[4];
        out->unk64[5] = todInfo[i].unk64[5];
        out->unk64[6] = todInfo[i].unk64[6];
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/tod", todGetTOD__FPiPfP10_todInfo14);
#endif
/* Loads the time-of-day settings of a level: 12 keys in file format 1.0 to 1.4 (key times keep their defaults). */
#ifdef NON_MATCHING
/* 175/394: same per-version copies; the 1.3 and 1.4 loops strength-reduce different todInfo field addresses (register pressure) */
void todSetTOD(int on, float minutesPerSecond, void *data, float version)
{
    int i;

    if ((unsigned)on < 2)
        todSetTimeOfDay = on;
    else
        todSetTimeOfDay = 0;
    if (minutesPerSecond <= 60.0f && 0.0f <= minutesPerSecond)
        todTimeOfDayMinutesPerSecond = minutesPerSecond;
    else
        todTimeOfDayMinutesPerSecond = 30.0f;
    if (version == 1.0f) {
        _todInfo10 *src = (_todInfo10 *)data;

        for (i = 0; i < 12; i++, src++) {
            todInfo[i].ambientRed = src->ambientRed;
            todInfo[i].ambientGreen = src->ambientGreen;
            todInfo[i].ambientBlue = src->ambientBlue;
            todInfo[i].fogMinRange = src->fogMinRange;
            todInfo[i].fogRed = src->fogRed;
            todInfo[i].fogGreen = src->fogGreen;
            todInfo[i].fogBlue = src->fogBlue;
            todInfo[i].sky = src->sky;
        }
    }
    if (version == 1.1f) {
        _todInfo11 *src = (_todInfo11 *)data;

        for (i = 0; i < 12; i++, src++) {
            todInfo[i].ambientRed = src->ambientRed;
            todInfo[i].ambientGreen = src->ambientGreen;
            todInfo[i].ambientBlue = src->ambientBlue;
            todInfo[i].fogRed = src->fogRed;
            todInfo[i].fogGreen = src->fogGreen;
            todInfo[i].fogBlue = src->fogBlue;
            todInfo[i].fogMinRange = src->fogMinRange;
            todInfo[i].sky = src->sky;
            todInfo[i].unk60 = src->unk60;
            todInfo[i].unk64[0] = src->unk64[0];
            todInfo[i].unk64[1] = src->unk64[1];
            todInfo[i].unk64[2] = src->unk64[2];
        }
    }
    if (version == 1.2f) {
        _todInfo12 *src = (_todInfo12 *)data;

        for (i = 0; i < 12; i++, src++) {
            todInfo[i].ambientRed = src->ambientRed;
            todInfo[i].ambientGreen = src->ambientGreen;
            todInfo[i].ambientBlue = src->ambientBlue;
            todInfo[i].fogMinRange = src->fogMinRange;
            todInfo[i].fogMaxRange = src->fogMaxRange;
            todInfo[i].fogMaxVal = src->fogMaxVal;
            todInfo[i].fogFarClip = src->fogFarClip;
            todInfo[i].fogRed = src->fogRed;
            todInfo[i].fogGreen = src->fogGreen;
            todInfo[i].fogBlue = src->fogBlue;
            todInfo[i].unk38 = src->unk38;
            todInfo[i].dirHeading = src->dirHeading;
            todInfo[i].dirRed = src->dirRed;
            todInfo[i].dirGreen = src->dirGreen;
            todInfo[i].dirBlue = src->dirBlue;
            todInfo[i].sky = src->sky;
            todInfo[i].unk60 = src->unk60;
            todInfo[i].unk64[0] = src->unk64[0];
            todInfo[i].unk64[1] = src->unk64[1];
            todInfo[i].unk64[2] = src->unk64[2];
        }
    }
    if (version == 1.3f) {
        _todInfo13 *src = (_todInfo13 *)data;

        for (i = 0; i < 12; i++, src++) {
            todInfo[i].ambientRed = src->ambientRed;
            todInfo[i].ambientGreen = src->ambientGreen;
            todInfo[i].ambientBlue = src->ambientBlue;
            todInfo[i].fogMinRange = src->fogMinRange;
            todInfo[i].fogMaxRange = src->fogMaxRange;
            todInfo[i].fogMaxVal = src->fogMaxVal;
            todInfo[i].fogFarClip = src->fogFarClip;
            todInfo[i].fogRed = src->fogRed;
            todInfo[i].fogGreen = src->fogGreen;
            todInfo[i].fogBlue = src->fogBlue;
            todInfo[i].unk38 = src->unk38;
            todInfo[i].dirHeading = src->dirHeading;
            todInfo[i].dirRed = src->dirRed;
            todInfo[i].dirGreen = src->dirGreen;
            todInfo[i].dirBlue = src->dirBlue;
            todInfo[i].unk4C = src->unk4C;
            todInfo[i].sky = src->sky;
            todInfo[i].unk60 = src->unk60;
            todInfo[i].unk64[0] = src->unk64[0];
            todInfo[i].unk64[1] = src->unk64[1];
            todInfo[i].unk64[2] = src->unk64[2];
            todInfo[i].unk64[3] = src->unk64[3];
            todInfo[i].unk64[4] = src->unk64[4];
            todInfo[i].unk64[5] = src->unk64[5];
            todInfo[i].unk64[6] = src->unk64[6];
        }
    }
    if (version == 1.4f) {
        _todInfo14 *src = (_todInfo14 *)data;

        for (i = 0; i < 12; i++, src++) {
            todInfo[i].ambientRed = src->ambientRed;
            todInfo[i].ambientGreen = src->ambientGreen;
            todInfo[i].ambientBlue = src->ambientBlue;
            todInfo[i].fogMinRange = src->fogMinRange;
            todInfo[i].fogMaxRange = src->fogMaxRange;
            todInfo[i].fogMaxVal = src->fogMaxVal;
            todInfo[i].fogFarClip = src->fogFarClip;
            todInfo[i].fogRed = src->fogRed;
            todInfo[i].fogGreen = src->fogGreen;
            todInfo[i].fogBlue = src->fogBlue;
            todInfo[i].unk2C[0] = src->unk2C[0];
            todInfo[i].unk2C[1] = src->unk2C[1];
            todInfo[i].unk2C[2] = src->unk2C[2];
            todInfo[i].unk38 = src->unk38;
            todInfo[i].dirHeading = src->dirHeading;
            todInfo[i].dirRed = src->dirRed;
            todInfo[i].dirGreen = src->dirGreen;
            todInfo[i].dirBlue = src->dirBlue;
            todInfo[i].unk4C = src->unk4C;
            todInfo[i].backRed = src->backRed;
            todInfo[i].backGreen = src->backGreen;
            todInfo[i].backBlue = src->backBlue;
            todInfo[i].sky = src->sky;
            todInfo[i].unk60 = src->unk60;
            todInfo[i].unk64[0] = src->unk64[0];
            todInfo[i].unk64[1] = src->unk64[1];
            todInfo[i].unk64[2] = src->unk64[2];
            todInfo[i].unk64[3] = src->unk64[3];
            todInfo[i].unk64[4] = src->unk64[4];
            todInfo[i].unk64[5] = src->unk64[5];
            todInfo[i].unk64[6] = src->unk64[6];
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/tod", todSetTOD__FifPvf);
#endif
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
/* Applies a sky's stored layer values to its objects: colors scaled, z fudge / fog burn-through offset. */
void todSetSkyObjects(int which, float alpha, float zBufferFudge, float fogBurnThru, float red, float green, float blue)
{
    _todSky *sky = &todSky[which];
    _hiergroup *group = (_hiergroup *)sky->hier;
    float (*row)[4] = (float (*)[4])sky;
    int i;

    if (group == 0 || group->head.opcode != GROUP_NODE)
        return;
    for (i = 0; i < group->numKids; i++) {
        _hierobject *obj = (_hierobject *)group->child[i];

        if (group->child[i]->opcode == OBJECT_NODE) {
            obj->fogBurnThru = row[i + 4][2] + fogBurnThru;
            obj->zBufferFudge = row[i + 4][3] + zBufferFudge;
            obj->redAnim = row[i + 1][0] * red;
            obj->greenAnim = row[i + 1][1] * green;
            obj->blueAnim = row[i + 1][2] * blue;
            obj->alphaAnim = row[i + 1][3] * alpha;
        }
    }
}
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

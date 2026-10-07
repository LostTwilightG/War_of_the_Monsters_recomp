#include "common.h"
#include "hieri_types.h"

class StartPoints {
public:
    struct Point {
        _fvector pos;
        float f10;
        int f14;
        int pad[2];
    };
    struct PointType {
        Point *points;
        int count;
        int max;
        int next;
    };

    static PointType s_pointTypes[4];

    static void addPoint(unsigned char type, _fvector *pos, float f, bool b);
    static void clear(void);
    static int getNumPoints(unsigned char type);
    static Point *getPoint(unsigned char type, int i);
    static bool isThisTypeFull(unsigned char type);
    static int getNextPoint(unsigned char type);
};

#ifdef NON_MATCHING
/* 1/22 words, untuned: scheduling of the point copy */
void StartPoints::addPoint(unsigned char type, _fvector *pos, float f, bool b)
{
    PointType *t = &s_pointTypes[type];
    int idx;
    Point *p;

    if (t->count < t->max)
        idx = t->count++;
    else
        idx = t->max - 1;
    p = &t->points[idx];
    p->pos = *pos;
    p->f14 = b;
    p->f10 = f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/StartPoints", addPoint__11StartPointsUcP8_fvectorfb);
#endif
void StartPoints::clear(void)
{
    int i;

    for (i = 0; i < 4; i++) {
        s_pointTypes[i].count = 0;
        s_pointTypes[i].next = 0;
    }
}
#ifdef NON_MATCHING
/* 4/7 words, untuned: index computation order */
int StartPoints::getNumPoints(unsigned char type)
{
    return s_pointTypes[type].count;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/StartPoints", getNumPoints__11StartPointsUc);
#endif
#ifdef NON_MATCHING
/* 3/9 words, untuned: index computation order */
StartPoints::Point *StartPoints::getPoint(unsigned char type, int i)
{
    return &s_pointTypes[type].points[i];
}
#else
INCLUDE_ASM("asm/nonmatchings/game/StartPoints", getPoint__11StartPointsUci);
#endif
bool StartPoints::isThisTypeFull(unsigned char type)
{
    return !(s_pointTypes[type].count < s_pointTypes[type].max);
}
#ifdef NON_MATCHING
/* 2/15 words, untuned: modulo expansion order */
int StartPoints::getNextPoint(unsigned char type)
{
    PointType *t = &s_pointTypes[type];
    int r = t->next;

    t->next = (r + 1) % t->count;
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/StartPoints", getNextPoint__11StartPointsUc);
#endif

#ifndef START_POINTS_H
#define START_POINTS_H

#include "hieri_types.h"

/* Spawn points per monster slot (4 types). The methods take `this` in retail even though all state is static;
   callers go through the single instance m_instance. */
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
    static StartPoints m_instance;

    void addPoint(unsigned char type, _fvector *pos, float f, bool b);
    void clear(void);
    int getNumPoints(unsigned char type);
    Point *getPoint(unsigned char type, int i);
    static bool isThisTypeFull(unsigned char type);
    int getNextPoint(unsigned char type);
};

#endif

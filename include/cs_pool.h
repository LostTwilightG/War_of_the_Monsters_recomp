#ifndef CS_POOL_H
#define CS_POOL_H

#include "hieri_types.h"

/* Node of the intrusive lists CsPool keeps (active, high-priority active, inactive). */
struct CsNode {
    _cs *cs;
    CsNode *next;
    CsNode *prev;
};

/* Pool of 1024 scene-graph control structures. Everything is static: retail never touches `this`. */
class CsPool {
public:
    static CsNode m_activeList;
    static CsNode m_HPActiveList;
    static CsNode m_inactiveList;
    static _cs m_csPool[1024];

    static void init(void);
    static _cs *csActivate(void);
    static void csDeactivate(_cs *cs);
    static _cs *csHPActivate(void);
    static void csHPDeactivate(_cs *cs);
};

#endif

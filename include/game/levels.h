#ifndef LEVELS_H
#define LEVELS_H

/* Per-level hooks called from TheGame (one set per level; the level id is TheGame::m_levelId). */
void centralUpdate(void);
void vegasUpdate(void);
void canyon2Update(void);
void airportUpdate(void);
void islandUpdate(void);
void threeMileUpdate(void);
void tokyoUpdate(void);
void sanFranUpdate(void);
void ufoUpdate(void);

void centralInitAfter(void);
void vegasInitAfter(void);
void canyon2InitAfter(void);
void airportInitAfter(void);
void islandInitAfter(void);
void threeMileInitAfter(void);
void tokyoInitAfter(void);
void sanFranInitAfter(void);
void ufoInitAfter(void);

class FinalBoss {
public:
    void update(void);
    void initAfter(void);
};
extern FinalBoss finalBoss;

/* "Big shot" mode keeps its per-state update functions in a table of member pointers, like CrushLevel. */
class BigShotLevel {
public:
    char pad0[0xE8];
    int m_state; /* 0xE8: index into UPDATE_FUNK */

    void initForReplay(void);

    static BigShotLevel instance;
    static void (BigShotLevel::*UPDATE_FUNK[])();
};

#endif

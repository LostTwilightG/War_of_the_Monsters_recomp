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

class FinalBoss {
public:
    void update(void);
};
extern FinalBoss finalBoss;

/* "Big shot" mode keeps its per-state update functions in a table of member pointers, like CrushLevel. */
class BigShotLevel {
public:
    char pad0[0xE8];
    int m_state; /* 0xE8: index into UPDATE_FUNK */

    static BigShotLevel instance;
    static void (BigShotLevel::*UPDATE_FUNK[])();
};

#endif

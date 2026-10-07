#ifndef LEVEL_PICKUPS_H
#define LEVEL_PICKUPS_H

/* Per-level pickup spawning; everything is static. */
class Monster;
class LevelPickups {
public:
    static void update(void);
    static void computeHighlight(Monster &m);
    static void initAfterDbLoad(void);
    static void turnOffMilitary(void);
    static void initMilitaryForCentral(void);
};

#endif

#ifndef LEVEL_PICKUPS_H
#define LEVEL_PICKUPS_H

/* Per-level pickup spawning; everything is static. */
class Monster;
class LevelPickups {
public:
    struct Info {
        char pad0[0x30];
        float staminaGain;   /* 0x30 */
        char pad34[0x20];
    };
    static Info s_info[];

    static void update(void);
    static void computeHighlight(Monster &m);
    static void initAfterDbLoad(void);
    static void turnOffMilitary(void);
    static void initMilitaryForCentral(void);
};

#endif

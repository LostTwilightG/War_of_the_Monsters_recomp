#ifndef LEVEL_PICKUPS_H
#define LEVEL_PICKUPS_H

/* Per-level pickup spawning; everything is static. */
class Monster;
class Pickup;
class DbInteractive;
struct _fvector;
template <class T>
class LinkedList {
public:
    class Iterator {
    public:
        T cur;
    };
};
typedef LinkedList<Pickup *>::Iterator PickupIter;
class LevelPickups {
public:
    static void dropPickup(PickupIter it);
    static void throwPickup(PickupIter it, _fvector &dir, DbInteractive *by, DbInteractive *target);
    static void killPickup(PickupIter it, int how);
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

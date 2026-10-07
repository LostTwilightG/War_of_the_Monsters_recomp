#ifndef WEAPONS_H
#define WEAPONS_H

#include "hieri_types.h"

/* Projectile spawner embedded in TheGame at 0x112490, see gameWeapons(). */
class Weapons {
public:
    void UpdateWeapons(void);
    void InitWeaponsAfter(void);
    void CreateLavaBall(_fvector *pos, _fvector *dir, _fvector *target, int a, int b);
};

#endif

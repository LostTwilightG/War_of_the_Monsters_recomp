#ifndef PICKUP_FX_H
#define PICKUP_FX_H

#include "hieri_types.h"

/* Declarations shared by the pickups' explosion code (kill): a particle burst and the destructible sound. */
class LevelObjectSoundManager {
public:
    void playDestructibleSound(unsigned id, _fvector *pos);
};
int particleCreateFx(_fvector *pos, int id, float scale, float (*m)[4], float strength);
class TheGame;
extern TheGame *game;

#define PICKUP_SOUNDS() ((LevelObjectSoundManager *)((char *)game + 0x121570))

#endif

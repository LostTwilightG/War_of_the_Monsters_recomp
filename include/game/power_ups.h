#ifndef POWER_UPS_H
#define POWER_UPS_H

#include "hieri_types.h"

class PowerUps {
public:
    static PowerUps instance;

    void update(void);
    void KillPowerUps(void);
    void CreatePowerUp(int type, unsigned char arg, _fvector *pos);
};

#endif

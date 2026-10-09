#ifndef PICKUP_SOUND_H
#define PICKUP_SOUND_H

/* Looping engine/turret sound owned by a pickup; embedded at a per-pickup offset. */
class PickupSound {
public:
    void terminateTurretSound(void);
    void terminatePickupSound(void);
    void updatePickupSound(void);
};

#endif

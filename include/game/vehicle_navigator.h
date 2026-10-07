#ifndef VEHICLE_NAVIGATOR_H
#define VEHICLE_NAVIGATOR_H

/* Steering helper embedded in the AI-driven pickups (tank, missile truck). Only the parts used so far are named. */
class VehicleNavigator {
public:
    char pad0[0x10];
    float f10;
    float f14;
    char pad18[0x90 - 0x18];

    void init(void);
};

#endif

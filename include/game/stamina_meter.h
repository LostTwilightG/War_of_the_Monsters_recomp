#ifndef STAMINA_METER_H
#define STAMINA_METER_H

/* Stamina bar. `exhausted` is set when it hits zero; while exhausted the bar refills through a separate pool
   (recoverPool) and a countdown (exhaustTimer) until it passes gLowStaminaThreshold * max. */
class StaminaMeter {
public:
    float max;
    float regenRate;
    float maxLevel; /* credit target of creditFull */
    float fC;
    float cur;
    float recoverPool;
    int exhaustTimer;
    int exhaustTime;
    int exhausted;
    int enabled;
    char *owner;

    StaminaMeter();
    void init(void);
    void reset(void);
    void update(int dt);
    void creditFull(void);
    void creditBaseOnly(float amount);
    void credit(float amount);
    int hasEnough(float amount);
    float getMaxLevel(void);
};

#endif

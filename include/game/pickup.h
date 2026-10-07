#ifndef PICKUP_H
#define PICKUP_H

/* Base of the pickups (cars, military, subway ...). Virtual in retail: derived pickups declare their own
   non-virtual overrides and keep vtable/ctor/__tf as asm. */
class Pickup {
public:
    void kill(void);
    void grab(int i);
    void setVisualState(int s);
    void hatCheck(void);
};

#endif

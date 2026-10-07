#ifndef PICKUP_H
#define PICKUP_H

#include "hieri_types.h"

/* Base of the pickups (cars, military, subway, tanks ...). Virtual in retail (vptr at 0): derived pickups declare
   their own non-virtual overrides and keep vtable/ctor/__tf as asm. Members start at 0xE0. */
class Pickup {
public:
    void *vptr;               /* 0x00 */
    unsigned short flags;     /* 0x04: bit 1 cleared on grab/kill, bit 3 cleared when health runs out */
    char pad6[6];
    _cs *cs;                  /* 0x0C */
    char pad10[0x48 - 0x10];
    float health;             /* 0x48 */
    char pad4C[4];
    unsigned long long bits;  /* 0x50: bit 0 set when the cs is shared and must not be deactivated */
    char pad58[0xD8 - 0x58];
    int heldState;            /* 0xD8: zeroed on drop */
    char padDC[4];

    void kill(void);
    void grab(int i);
    void setVisualState(int s);
    void hatCheck(void);
};
typedef char _size_Pickup[sizeof(Pickup) == 0xE0 ? 1 : -1];

#endif

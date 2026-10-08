#include "common.h"

/* Foot/hand contact weights of an animation, double buffered: contacts[cur] is the current frame.
   Limb order: 0,1 = hands, 2..5 = feet. A limb is "in contact" when its weight is above 0.5. */
class AnimContact {
public:
    void *anim;
    int channel[6];
    char pad1C[0];
    float contacts[2][6];
    char pad4C[0];
    int cur;

    int getLimb(int id);
    int numContacts(void);
    int numHandContacts(void);
    int numFootContacts(void);
    int numNewContacts(void);
    int numNewFootContacts(void);
    int numNewHandContacts(void);
};

INCLUDE_ASM("asm/nonmatchings/game/AnimContact", init__11AnimContactR11_animHandle);
INCLUDE_ASM("asm/nonmatchings/game/AnimContact", update__11AnimContact);
int AnimContact::getLimb(int id)
{
    switch (id) {
    case 0x802:
        return 0;
    case 0x803:
        return 1;
    case 0x804:
        return 2;
    case 0x805:
        return 3;
    case 0x816:
        return 4;
    case 0x817:
        return 5;
    }
    return -1;
}
/* retail pads the jump table to 24 words */
__asm__(".section .rodata
	.word 0
	.word 0
	.text");
INCLUDE_ASM("asm/nonmatchings/game/AnimContact", _12BigShotLevel$UPDATE_FUNK);
#ifdef NON_MATCHING
/* 8/20 words, untuned */
int AnimContact::numContacts(void)
{
    int n = 0;
    int i;

    for (i = 0; i < 6; i++) {
        if (contacts[cur][i] > 0.5f)
            n++;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AnimContact", numContacts__11AnimContact);
#endif
#ifdef NON_MATCHING
/* 8/20 words, untuned */
int AnimContact::numHandContacts(void)
{
    int n = 0;
    int i;

    for (i = 0; i < 2; i++) {
        if (contacts[cur][i] > 0.5f)
            n++;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AnimContact", numHandContacts__11AnimContact);
#endif
#ifdef NON_MATCHING
/* 15/20 words, untuned */
int AnimContact::numFootContacts(void)
{
    int n = 0;
    int i;

    for (i = 2; i < 6; i++) {
        if (contacts[cur][i] > 0.5f)
            n++;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AnimContact", numFootContacts__11AnimContact);
#endif
#ifdef NON_MATCHING
/* 0/32 words, untuned: loop/branch layout */
int AnimContact::numNewContacts(void)
{
    int n = 0;
    int i;

    for (i = 0; i < 6; i++) {
        int isNew = 0;

        if (contacts[cur][i] > 0.5f) {
            if (contacts[cur ? 0 : 1][i] < 0.5f)
                isNew = 1;
        }
        n += isNew;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AnimContact", numNewContacts__11AnimContact);
#endif
#ifdef NON_MATCHING
/* 0/32 words, untuned: loop/branch layout */
int AnimContact::numNewHandContacts(void)
{
    int n = 0;
    int i;

    for (i = 0; i < 2; i++) {
        int isNew = 0;

        if (contacts[cur][i] > 0.5f) {
            if (contacts[cur ? 0 : 1][i] < 0.5f)
                isNew = 1;
        }
        n += isNew;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AnimContact", numNewHandContacts__11AnimContact);
#endif
#ifdef NON_MATCHING
/* 0/33 words, untuned: loop/branch layout */
int AnimContact::numNewFootContacts(void)
{
    int n = 0;
    int i;

    for (i = 2; i < 6; i++) {
        int isNew = 0;

        if (contacts[cur][i] > 0.5f) {
            if (contacts[cur ? 0 : 1][i] < 0.5f)
                isNew = 1;
        }
        n += isNew;
    }
    return n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AnimContact", numNewFootContacts__11AnimContact);
#endif

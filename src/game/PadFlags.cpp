#include "common.h"
#include "engine.h"
#include "game/pad_flags.h"

extern "C" void *memset(void *, int, unsigned);
unsigned timerGetFieldCount(void);

INCLUDE_ASM("asm/nonmatchings/game/PadFlags", __8PadFlags);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", init__8PadFlagsP7Monster);
void PadFlags::setCurrentAction(ButtonActions button, MappedActions mapped)
{
    curButtonAction = button;
    curMappedAction = mapped;
    curActionTime = timerGetFieldCount();
}
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", pushAction__8PadFlags13ButtonActions13MappedActions);
#ifdef NON_MATCHING
/* Oldest actions are dropped once they are older than tweak17B0 fields (checked the first time after a reset); returns the pending button action or NONE. */
ButtonActions PadFlags::nextButtonAction(void)
{
    if (actionsStarted == 0) {
        int now = timerGetFieldCount();

        for (;;) {
            int expired = 0;

            if (usedList.next != &usedList)
                expired = (now - usedList.next->action->time) >= tweak17B0;
            if (!expired)
                break;
            popAction();
        }
        actionsStarted = 1;
    }
    if (usedList.next != &usedList)
        return (ButtonActions)usedList.next->action->button;
    return BUTTON_ACTION_NONE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", nextButtonAction__8PadFlags);
#endif
#ifdef NON_MATCHING
MappedActions PadFlags::nextMappedAction(void)
{
    if (actionsStarted == 0) {
        int now = timerGetFieldCount();

        for (;;) {
            int expired = 0;

            if (usedList.next != &usedList)
                expired = (now - usedList.next->action->time) >= tweak17B0;
            if (!expired)
                break;
            popAction();
        }
        actionsStarted = 1;
    }
    if (usedList.next != &usedList)
        return (MappedActions)usedList.next->action->mapped;
    return (MappedActions)0xF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", nextMappedAction__8PadFlags);
#endif
#ifdef NON_MATCHING
/* 3/25 words, untuned: register choice for the list nodes */
void PadFlags::popAction(void)
{
    ActionNode *n = usedList.next;
    ActionNode *p;

    if (n == &usedList)
        return;
    p = n->prev;
    if (p) {
        p->next = n->next;
        p = n->next;
    } else {
        p = n->next;
    }
    if (p)
        p->prev = n->prev;
    n->next = 0;
    n->prev = &freeList;
    n->next = freeList.next;
    freeList.next = n;
    if (n->next)
        n->next->prev = n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", popAction__8PadFlags);
#endif
void PadFlags::clear(int idx, int value)
{
    memset(ring + idx * ENTRY_SIZE, value, ENTRY_SIZE);
}
#ifdef NON_MATCHING
/* 22/26 words, untuned: store order */
void PadFlags::saveAndClear(int value)
{
    ringIndex++;
    if (ringIndex == RING_SIZE)
        ringIndex = 0;
    clear(ringIndex, value);
    actionsStarted = 0;
    if (f16E8 == f16E4) {
        int *p;

        f17E0 = 0;
        p = &f17E0;
        p[2] = 0;
        p[1] = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", saveAndClear__8PadFlagsi);
#endif
void PadFlags::clearModifiers(void)
{
    modifier1708 = 0;
    modifier1700 = 0;
    modifier1704 = 0;
    modifier170C = 0;
}
PadEntry *PadFlags::operator[](int back)
{
    int n = back;
    int i;

    if (n >= RING_SIZE)
        n = RING_SIZE;
    i = ringIndex - n;

    if (i < 0)
        i += RING_SIZE;
    return (PadEntry *)(ring + i * ENTRY_SIZE);
}
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", interpretInputs__8PadFlagsR7GamePad);
#ifdef NON_MATCHING
/* Left stick as a vector: x = right - left, y = up - down (each 0..255 scaled to 0..1), z = 0. */
void PadFlags::computeMotionVec(_fvector &v)
{
    PadEntry *e = (*this)[0];

    v.x = (float)e->f1C * 0.003921569f - (float)e->f1A * 0.003921569f;
    v.z = 0.0f;
    v.y = (float)*(unsigned short *)(e->data + 0x12) * 0.003921569f - (float)*(unsigned short *)(e->data + 0x14) * 0.003921569f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", computeMotionVec__8PadFlagsR8_fvector);
#endif
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", computeMotionRot__8PadFlags);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", updateViewChanges__8PadFlagsR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", okToChangeMap__8PadFlags);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", dashDoubleTap__8PadFlagsR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", checkCombos__8PadFlagsR7GamePad);
void PadFlags::checkTaunt(void)
{
}
void PadFlags::clearSecretCode(void)
{
    *(int *)((char *)this + 0x17F0) = 0;
    *(int *)((char *)this + 0x17F8) = 0;
    *(int *)((char *)this + 0x180C) = 0;
    *(int *)((char *)this + 0x17FC) = 0;
}
void PadFlags::clearCombo(void)
{
    int i;

    f17F0 = 0;
    f17F8 = 0;
    f180C = 0;
    f17FC = 0;
    f17F4 = 0;
    for (i = 0; i < RING_SIZE; i++)
        clear(i, 0);
}

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
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", nextButtonAction__8PadFlags);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", nextMappedAction__8PadFlags);
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
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", computeMotionVec__8PadFlagsR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", computeMotionRot__8PadFlags);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", updateViewChanges__8PadFlagsR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", okToChangeMap__8PadFlags);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", dashDoubleTap__8PadFlagsR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", checkCombos__8PadFlagsR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", checkTaunt__8PadFlags);
INCLUDE_ASM("asm/nonmatchings/game/PadFlags", clearSecretCode__8PadFlags);
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

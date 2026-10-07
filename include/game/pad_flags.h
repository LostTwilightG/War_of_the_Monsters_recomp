#ifndef PAD_FLAGS_H
#define PAD_FLAGS_H

#include "bidir_link.h"

class GamePad;
class Monster;
struct _fvector;

/* Input interpretation state of one monster (embedded in Monster at 0x5040): a ring of 60 per-frame snapshots,
   a queue of pending button actions, combo/secret-code tracking and the pad tweak values copied from TheGame
   by UpdatePadTweaks. Offsets are relative to the PadFlags object. */
class PadFlags {
public:
    enum { RING_SIZE = 60, ENTRY_SIZE = 0x5E };

    /* Action queue node (12 bytes): linked into the used list at 0x1794 or the free list at 0x17A0. */
    struct ActionNode {
        void *action;     /* 0x0: points at the {button, mapped, time} record of the action */
        ActionNode *prev; /* 0x4 */
        ActionNode *next; /* 0x8 */
    };

    char ring[RING_SIZE * ENTRY_SIZE];  /* 0x0000 */
    char pad15F8[0x16C4 - RING_SIZE * ENTRY_SIZE];
    int tweak16C4;                      /* 0x16C4: tweaks copied from TheGame (see UpdatePadTweaks) */
    int tweak16C8;
    char pad16CC[4];
    float tweak16D0;
    float tweak16D4;
    char pad16D8[4];
    int tweak16DC;
    int tweak16E0;                      /* 0x16E0: movement threshold used by okToChangeMap */
    int f16E4;
    int f16E8;
    int f16EC;
    int ringIndex;                      /* 0x16F0: current slot in `ring`, wraps at 60 */
    int tweak16F4;
    int tweak16F8;
    int tweak16FC;
    int modifier1700;                   /* 0x1700..0x170C cleared by clearModifiers */
    int modifier1704;
    int modifier1708;
    int modifier170C;
    int curButtonAction;                /* 0x1710 */
    int curMappedAction;                /* 0x1714 */
    int curActionTime;                  /* 0x1718 */
    ActionNode nodes[10];               /* 0x171C */
    char pad1794[0x1794 - 0x171C - 10 * 12];
    BidirLink usedList;                 /* 0x1794: sentinel of pending actions (newest first) */
    int tweak17AC;                      /* 0x17AC: delay before an action is superseded */
    int tweak17B0;                      /* 0x17B0: delay before an action expires */
    int actionsStarted;                 /* 0x17B4 */

    void init(Monster *m);
    void clear(int idx, int value);
    void saveAndClear(int unused);
    void clearModifiers(void);
    char *operator[](int back);         /* ring entry `back` frames before the current one */
    void setCurrentAction(int button, int mapped);
    void pushAction(int button, int mapped);
    int nextButtonAction(void);
    int nextMappedAction(void);
    void popAction(void);
    void interpretInputs(GamePad &pad);
    void computeMotionVec(_fvector &v);
    void computeMotionRot(void);
    void updateViewChanges(GamePad &pad);
    int okToChangeMap(void);
    int dashDoubleTap(GamePad &pad);
    void checkCombos(GamePad &pad);
    void checkTaunt(void);
    void clearSecretCode(void);
    void clearCombo(void);
};

#endif

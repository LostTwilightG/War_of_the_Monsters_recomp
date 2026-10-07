#include "common.h"

/* Plays back a recorded sequence of pad button states. Layout inferred from the assembly. */
struct GamePad {
    unsigned char buttons[1];
};
struct PadEvent {
    unsigned char button;
    unsigned char value;
    unsigned short duration;
};
struct PadTrack {
    PadEvent *events;
    unsigned count;
};
struct PadClip {
    PadTrack *tracks;
    unsigned numTracks;
};
struct TrackState {
    unsigned pos;
    unsigned timer;
};

class GamePadClipPlayer {
public:
    PadClip *clip;
    int playing;
    TrackState state[1];

    int rewind(void);
    int update(GamePad &pad);
};

#ifdef NON_MATCHING
/* 0/24 words, untuned: retail walks the tracks with incremented pointers */
int GamePadClipPlayer::rewind(void)
{
    unsigned i;

    for (i = 0; i < clip->numTracks; i++) {
        state[i].pos = 0;
        state[i].timer = clip->tracks[i].events[0].duration;
    }
    playing = 1;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/GamePadClipPlayer", rewind__17GamePadClipPlayer);
#endif
#ifdef NON_MATCHING
/* 2/46 words, untuned: retail walks the tracks with incremented pointers */
int GamePadClipPlayer::update(GamePad &pad)
{
    int active = 0;
    unsigned i;

    for (i = 0; i < clip->numTracks; i++) {
        PadTrack *t = &clip->tracks[i];

        if (state[i].pos < t->count) {
            PadEvent *ev = &t->events[state[i].pos];

            active = 1;
            if (ev->value)
                pad.buttons[ev->button] = ev->value;
            if (--state[i].timer == 0) {
                state[i].pos++;
                if (state[i].pos < t->count)
                    state[i].timer = ev[1].duration;
            }
        }
    }
    playing = active;
    return active;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/GamePadClipPlayer", update__17GamePadClipPlayerR7GamePad);
#endif

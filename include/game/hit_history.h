#ifndef HIT_HISTORY_H
#define HIT_HISTORY_H

/* Remembers which targets a swing/effect already hit so one source cannot hit the same target every frame. */
class HitHistory {
public:
    int f0;
    int f4;

    HitHistory();
    void reset(void);
    int newHit(int id, bool b);
};

#endif

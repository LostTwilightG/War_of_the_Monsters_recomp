#ifndef FIRE_BREATH_H
#define FIRE_BREATH_H

class Monster;

/* Per-monster fire breath effect (embedded in Monster at 0x68C0). */
class MonsterSound {
public:
    void updateFireBreath(void);
    void playCloakingSound(void);
};

class StateFireBreath {
public:
    void handleApplyMint(void);
};

class FireBreath {
public:
    int state;
    Monster *owner;
    char pad8[0x8C - 8];
    int fx0;
    int fx1;
    int fx2;
    float timeLeft;

    FireBreath(Monster &m);
    void Init(void);
    unsigned Update(void);
    void ApplyMint(void);
    int TestCollis(void);
    static unsigned Update(void *p);
};

#endif

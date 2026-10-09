#ifndef AI_H
#define AI_H

class Monster;
class DbInteractive;
class PowerUp {
public:
    enum Type { TYPE_0, TYPE_1, TYPE_2, TYPE_3, TYPE_4 };
};
class Pickup;
class _fvector;

/* The AI controller of one computer-driven monster. It "presses buttons" by writing 0xFF into the monster's GamePad byte array (pad):
   0 target-pin, 3 target-pin (secondary), 4 block, 6 jump, 7 grab/toss/thrash, 8 heavy punch, 9 light punch/fire, 0xB taunt, 0xE special.
   The object is large (the relevance tables sit past 0x500); only fields seen in use are named. */
class Ai {
public:
    Monster *monster;               /* 0x000 */
    Monster *m_opponents[16];       /* 0x004: the other monsters, null-terminated */
    unsigned char *pad;             /* 0x044: the virtual pad the AI drives */
    char pad48[0x2E0 - 0x48];
    int *m_gate;                    /* 0x2E0: pointer to a word that, when 0, lets every action start (mask 3) */
    char pad2E4[4];
    DbInteractive *m_focus;         /* 0x2E8: what the AI is looking at / aiming for (see setFocus) */
    char pad2EC[0x2F8 - 0x2EC];
    float m_puMax;                  /* 0x2F8: cap of the power-up relevances below */
    char pad2FC[0x304 - 0x2FC];
    float m_puRelevance[1];         /* 0x304: per power-up type, grows when a power-up of that type is useful (creditRelevance) */
    char pad308[0x49C - 0x308];
    float m_pickupMax;              /* 0x49C: cap of the pickup relevances below */
    char pad4A0[0x518 - 0x4A0];
    float m_pickupRelevance[1];     /* 0x518: per pickup kind */
    char pad51C[0x1518 - 0x51C];
    int m_reflexMin;                /* 0x1518: reflex delay range (fields) */
    int m_reflexMax;                /* 0x151C */
    int m_mashMin;                  /* 0x1520: button-mash delay range (fields) */
    int m_mashMax;                  /* 0x1524 */

    int isMinion(void);
    int okayToCC(Monster *m, float f);
    float getClosestStillApproach(DbInteractive &d, float t);
    float getClosestApproach(DbInteractive &d, float t);

    void setFocus(DbInteractive *d);
    float getGroundHeight(void);
    float getCommonGroundProbability(Monster &m);
    float getFovRelevance(float a, float b);
    float getPowerUpGrabRange(void);
    int overPit(Monster &m);
    int pinningAi(void);
    int getReflexDelay(void);
    int getButtonMashDelay(void);
    void *getBestPickup(float &best, float range);
    void *getBestHealthPowerup(float &best, float range);
    void *getBestStaminaPowerup(float &best, float range);
    void *getBestSpecialPowerup(float &best, float range);
    void *getBestPowerupByType(PowerUp::Type t, float &best, float range);
    void creditRelevance(PowerUp *p, float amount);
    void creditRelevance(Pickup *p, float amount);

    void targetPin(void);
    void lightPunch(void);
    void heavyPunch(void);
    void block(void);
    void counter(void);
    void fireProjectile(void);
    void grab(void);
    void toss(void);
    void thrash(void);
    void buttStomp(void);
    void jump(void);
    void crowdControl(void);
    void specialAttack(void);
    void taunt(void);
};

#endif

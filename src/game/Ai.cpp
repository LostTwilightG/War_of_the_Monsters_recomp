#include "common.h"
#include "engine.h"
#include "game/game.h"
#include "game/ai.h"

#define MI(m, o) (*(int *)((char *)(m) + (o)))
#define MF(m, o) (*(float *)((char *)(m) + (o)))
#define MB(m, o) (*(signed char *)((char *)(m) + (o)))


INCLUDE_ASM("asm/nonmatchings/game/Ai", globalInit__2Ai);
INCLUDE_ASM("asm/nonmatchings/game/Ai", globalUpdate__2Ai);
void toggleMode__2Ai(void *self) __asm__("toggleMode__2Ai");
void toggleMode__2Ai(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/Ai", __2AiR7MonsterR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/Ai", init__2Ai);
INCLUDE_ASM("asm/nonmatchings/game/Ai", updateInputs__2Ai);
float Ai::getGroundHeight(void)
{
    return MF(MI(monster, 0x1A3C), 0x18);
}
INCLUDE_ASM("asm/nonmatchings/game/Ai", getCommonGroundProbability__2AiR7Monster);
#ifdef NON_MATCHING
/* untuned: min.s/max.s FPR allocation */
float Ai::getFovRelevance(float a, float b)
{
    float r = (a - b) / (1.0f - b);

    float z = 0.0f;

    return z > r ? z : r;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Ai", getFovRelevance__2Aiff);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Ai", getClosestApproach__2AiR13DbInteractivef);
INCLUDE_ASM("asm/nonmatchings/game/Ai", getClosestStillApproach__2AiR13DbInteractivef);
INCLUDE_ASM("asm/nonmatchings/game/Ai", inFlight__2AiR13DbInteractive);
int Ai::pinningAi(void)
{
    int *p = (int *)MI(monster, 0x6C04);

    if (p) {
        if (p[0] == 1) {
            if (p[6] == 2)
                return 1;
        }
    }
    return 0;
}
void monitorProjectiles__2Ai(void *self) __asm__("monitorProjectiles__2Ai");
void monitorProjectiles__2Ai(void *self)
{
}
INCLUDE_ASM("asm/nonmatchings/game/Ai", updatePathNet__2Ai);
void Ai::targetPin(void)
{
    pad[0] = 0xFF;
    pad[3] = 0xFF;
}
void Ai::lightPunch(void)
{
    pad[9] = 0xFF;
}
void Ai::heavyPunch(void)
{
    pad[8] = 0xFF;
}
void Ai::block(void)
{
    pad[4] = 0xFF;
}
void Ai::counter(void)
{
    pad[4] = 0xFF;
    pad[8] = 0xFF;
}
void Ai::fireProjectile(void)
{
    pad[9] = 0xFF;
}
void Ai::grab(void)
{
    pad[7] = 0xFF;
}
void Ai::toss(void)
{
    pad[7] = 0xFF;
}
void Ai::thrash(void)
{
    pad[7] = 0xFF;
}
void Ai::buttStomp(void)
{
    pad[14] = 0xFF;
    pad[8] = 0xFF;
}
void Ai::jump(void)
{
    pad[6] = 0xFF;
}
void Ai::crowdControl(void)
{
    pad[6] = 0xFF;
    pad[9] = 0xFF;
}
void Ai::specialAttack(void)
{
    pad[8] = 0xFF;
    pad[7] = 0xFF;
}
void Ai::taunt(void)
{
    pad[11] = 0xFF;
}
int Ai::getReflexDelay(void)
{
    return mathfRand(m_reflexMin, m_reflexMax);
}
int Ai::getButtonMashDelay(void)
{
    return mathfRand(m_mashMin, m_mashMax);
}
float Ai::getPowerUpGrabRange(void)
{
    return MF(monster, 0x3A4) - 5.0f;
}
int Ai::isMinion(void)
{
    if (game->m_gameMode == 1) {
        int l = game->m_levelId;

        if (l == 6 || l == 3)
            return 1;
    }
    return 0;
}
int Ai::overPit(Monster &m)
{
    return MB(&m, 0x1A40) == 0;
}
INCLUDE_ASM("asm/nonmatchings/game/Ai", inTub__2AiR8_fvector);
extern Ai *s_ccAi __asm__("_2Ai$s_ccAi");
__asm__("#SNFIX_SMALL _2Ai$s_ccAi");
void Ai::setFocus(DbInteractive *d)
{
    if (!d && s_ccAi == this)
        s_ccAi = 0;
    m_focus = d;
}
#ifdef NON_MATCHING
/* untuned: min.s/max.s FPR allocation */
void Ai::creditRelevance(PowerUp *p, float amount)
{
    float mx = m_puMax;
    float *r = &m_puRelevance[*((unsigned char *)p + 3)];
    float v = *r + amount;

    *r = v < mx ? v : mx;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Ai", creditRelevance__2AiP7PowerUpf);
#endif
#ifdef NON_MATCHING
/* untuned: min.s/max.s FPR allocation */
void Ai::creditRelevance(Pickup *p, float amount)
{
    int *owner = *(int **)((char *)p + 0xC);
    float mx = m_pickupMax;
    float *r = (float *)((char *)this + 0x518 + ((*(unsigned *)*owner >> 5) & 0x1FFC));
    float v = *r + amount;

    *r = v < mx ? v : mx;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Ai", creditRelevance__2AiP6Pickupf);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Ai", okayToCC__2AiP7Monsterf);
INCLUDE_ASM("asm/nonmatchings/game/Ai", updateRelevances__2Ai);
INCLUDE_ASM("asm/nonmatchings/game/Ai", updateOpponentList__2Ai);
INCLUDE_ASM("asm/nonmatchings/game/Ai", getBestPowerupByType__2AiQ27PowerUp4TypeRff);
INCLUDE_ASM("asm/nonmatchings/game/Ai", getBestHealthPowerup__2AiRff);
INCLUDE_ASM("asm/nonmatchings/game/Ai", getBestStaminaPowerup__2AiRff);
INCLUDE_ASM("asm/nonmatchings/game/Ai", getBestSpecialPowerup__2AiRff);
INCLUDE_ASM("asm/nonmatchings/game/Ai", getBestPickup__2AiRff);

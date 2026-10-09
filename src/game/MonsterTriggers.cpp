#include "common.h"
#include "game/game.h"
#include "game/shell.h"
#include "engine.h"

struct TriggerHead;
#define TF(t, o) (*(float *)((char *)(t) + (o)))
class SoundManager {
public:
    void setSoundVolumeInfo(Monster *m, int id, _fvector *pos);
};
void threeMileDecStamina(float amount);
Monster *threeMileGetTubInitiator(void);
Monster *tokyoGetTidalWaveSource(void);
extern int tidalWaveActive;
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", checkTriggerTree__Fv);
void doDeathVolume(Monster *m, TriggerHead *t)
{
    m->takeDamage(m->m_health, false, 0);
}
/* Sound volume trigger: tells the sound manager the volume zone (id in the trigger's top bits) and where it is. */
void doSoundVolume(Monster *m, TriggerHead *t)
{
    _fvector pos;

    pos.x = TF(t, 0xC);
    pos.y = TF(t, 0x10);
    pos.z = TF(t, 0x14);
    ((SoundManager *)((char *)shell + 0x2C30))->setSoundVolumeInfo(m, *(unsigned *)t >> 18, &pos);
}
#ifdef NON_MATCHING
/* Damage per second from the trigger; in the Tokyo tidal wave and the three-mile tub the damage is attributed to their source monster. */
void doDamageVolume(Monster *m, TriggerHead *t)
{
    Monster *src = 0;
    float amount;

    if (game->m_levelId == 9) {
        if (tidalWaveActive != 0) {
            *(float *)((char *)m + 0x1E8) = 1000.0f;
            src = tokyoGetTidalWaveSource();
        }
    }
    if (game->m_levelId == 6) {
        *(int *)((char *)m + 0x1A80) = 1;
        src = threeMileGetTubInitiator();
    }
    amount = TF(t, 4) / (float)(timerGetFieldsLastFrame() * 60);
    m->takeDamage(amount, false, src);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doDamageVolume__FP7MonsterP11TriggerHead);
#endif
/* The trigger's field 4 is health per second; it is credited per frame. */
void doHealthVolume(Monster *m, TriggerHead *t)
{
    float perSecond = TF(t, 4);

    m->creditHealth(perSecond / (float)(timerGetFieldsLastFrame() * 60));
}
#ifdef NON_MATCHING
void doStaminaVolume(Monster *m, TriggerHead *t)
{
    float before = m->m_stamina.cur;
    float amount = TF(t, 4) / (float)(timerGetFieldsLastFrame() * 60);

    m->creditStamina(amount, false);
    if (game->m_levelId == 6 && before != m->m_stamina.cur)
        threeMileDecStamina(amount);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doStaminaVolume__FP7MonsterP11TriggerHead);
#endif
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doFireVolume__FP7MonsterP11TriggerHead);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doShockVolume__FP7MonsterP11TriggerHead);
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doImpaleVolume__FP7MonsterP11TriggerHead);
void doCamUnifyVolume(Monster *m, TriggerHead *t)
{
    m->m_camUnify = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/MonsterTriggers", doCamVolume__FP7MonsterP11TriggerHead);

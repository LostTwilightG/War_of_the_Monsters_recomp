#include "common.h"
#include "game/game.h"
#include "game/shell.h"
#include "game/pad_flags.h"
#include "game/stamina_meter.h"
#include "task_manager.h"
#include "engine.h"

extern "C" void *memset(void *, int, unsigned);
struct _animCharInstance;
struct DbInteractive;
class HealthMeter {
public:
    void init(void);
    void reset(void);
};
class Ai {
public:
    void init(void);
};
class AnimPappy {
public:
    void init(_animHandle &h);
};
class AnimBlend {
public:
    void init(_animHandle &h);
    void rampIn(float t);
    void setSpeed(float s);
    void setPercent(float p);
};
class Interactives {
public:
    static void setInteractive(int i, DbInteractive *p);
};
class MonsterTweaks {
public:
    static void setActiveMonster(Monster *m, bool b);
};
int inputGetPlayerPad(int i);
void animationInitModifierBlends(_animCharInstance *c);

#define MI(p, o) (*(int *)((char *)(p) + (o)))
#define MF(p, o) (*(float *)((char *)(p) + (o)))
#define MP(p, o) ((char *)(p) + (o))

#ifdef NON_MATCHING
/* Switches off the invulnerability marker of every collision list (their last 0x10-byte entry, field 0x24) for a fresh monster. */
static void clearListFlags(Monster *m)
{
    int i;
    int n = MI(m, 0x1A38);

    for (i = 0; i < n; i++) {
        char *list = ((char **)MP(m, 0x1A14))[i];

        MI(list, (MI(list, 4) - 1) * 0x10 + 0x24) = 0;
    }
}
#endif

#ifdef NON_MATCHING
/* 62/71 words: store order */
void Monster::initBeforeDbLoad(void)
{
    int i;

    m_playerNum = 0;
    m_numInits = 0;
    m_state = 0;
    m_prevState = 0;
    MI(this, 0x7978) = 0;
    MI(this, 0x797C) = 0;
    MI(this, 0x7980) = 0;
    memset(MP(this, 0xB0), 0, 0x38);
    m_playerInfo = 0;
    MI(this, 0x1A3C) = 0;
    MI(this, 0x6BF8) = 0;
    MI(this, 0x6BFC) = 0;
    MI(this, 0x3120) = 0;
    MI(this, 0x6C00) = 0;
    MI(this, 0x6C04) = 0;
    MI(this, 0x68A0) = 0;
    MI(this, 0x6860) = 0;
    MI(this, 0x6864) = 0;
    MI(this, 0x6874) = 0;
    memset(MP(this, 0x6C50), 0, 0x30);
    memset(MP(this, 0x1A40), 0, 0x30);
    MI(this, 0x1A38) = 0;
    for (i = 8; i >= 0; i--)
        MI(this, 0x1A14 + i * 4) = 0;
    MI(this, 0x68A4) = 0;
    MI(this, 0x68A8) = 0;
    MI(this, 0x68AC) = 0;
    MI(this, 0x68B0) = 0;
    MI(this, 0x68B4) = 0;
    MI(this, 0x68B8) = 0;
    MI(this, 0x68BC) = 0;
    MI(this, 0x48) = 0;
    MI(this, 0x30B0) = 0;
    MI(this, 0x6974) = -1;
    MI(this, 0x6C84) = -1;
    MI(this, 0x6C80) = -1;
    MI(this, 0x4A8) = -1;
    MI(this, 0x4A4) = -1;
    MI(this, 0x434) = -1;
    MI(this, 0x430) = -1;
    MI(this, 0x6CB4) = -1;
    ((HealthMeter *)MP(this, 0x448))->init();
    m_stamina.init();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", initBeforeDbLoad__7Monster);
#endif
#ifdef NON_MATCHING
class AnimContact {
public:
    void init(_animHandle &h);
};
class MonsterDynamics {
public:
    void init(void);
};
class FireBreath {
public:
    void Init(void);
};
class MonsterSound {
public:
    void initMonsterSound(Monster *m);
};
class AiPathNet {
public:
    int getClosestPath(_fvector &pos, unsigned char flags);
};
void animationResetCharacter(_animHandle &h);
void hierSetCsDrawMe(_cs *cs, unsigned char v);
void mathfUnitMatrix(float (*m)[4]);
void particleKillFx(int &handle);
extern char pointLights[];
extern char camerasObj[] __asm__("_7Cameras$m_cameras");
extern char aiPathNetMonster[] __asm__("_9AiPathNet$monster");

/* Brings a monster to life at the start of a round and after every death: clears the per-life flags (0xE8..0xFD), drops what it carries,
   kills its effects, resets its cs to an identity matrix, puts it on a start point, re-initialises pad flags, dynamics, contacts, fire
   breath, meters, sound and the two base animation blends (200-tick ramp, speed 0, half weight), and enters the idle state (0x7984). */
void Monster::init(void)
{
    char *cs;
    float *mat;

    memset(MP(this, 0xE8), 0, 0x16);
    MI(this, 0x6870) = 0xFF;
    *(char *)MP(this, 0xF9) = 1;
    MI(this, 0x438) = 0;
    MI(this, 0x68B0) = 0;
    MI(this, 0x68B4) = 0;
    MI(this, 0x68B8) = 0;
    MI(this, 0x6C00) = 0;
    MI(this, 0x6C04) = 0;
    MI(this, 0x5020) = 0;
    *(char *)MP(this, 0x49) = 1;
    *(char *)MP(this, 0x4A) = 1;
    MI(this, 0x6C3C) = 0;
    MI(this, 0x6C30) = 0;
    *(char *)MP(this, 0x4AC) = 1;
    MI(this, 0x1A70) = 0;
    MI(this, 0x4B0) = 0;
    MI(this, 0x4B4) = 0;
    MI(this, 0x4B8) = 0;
    MI(this, 0x4BC) = 0;
    MI(this, 0x4C0) = 0;
    MI(this, 0x4C4) = 0;
    MI(this, 0x4C8) = 0;
    MI(this, 0x4CC) = 0;
    MI(this, 0x4D0) = 0;
    MI(this, 0x686C) = 0;
    MI(this, 0x6980) = 0;
    MI(this, 0x6CB8) = 0;
    MI(this, 0x6CC4) = 0;
    MI(this, 0x311C) = 0;
    MI(this, 0x3120) = 0;
    *(char *)MP(this, 0xF3) = 1;
    *(char *)MP(this, 0xE8) = 0;
    *(char *)MP(this, 0xF6) = 0;
    *(char *)MP(this, 0xFA) = 0;
    MI(this, 0xD8) = 0;
    if (MI(this, 0x68A4) != 0)
        dropPickup();
    if (MI(this, 0x68A8) != 0 || MI(this, 0x68AC) != 0)
        dropPickupImpaler();
    if (MI(this, 0x6974) >= 0)
        *(int *)(pointLights + (MI(this, 0x6974) << 6) + 0x38) = 1;
    particleKillFx(*(int *)MP(this, 0x6CB4));
    particleKillFx(*(int *)MP(this, 0x4A4));
    particleKillFx(*(int *)MP(this, 0x4A8));
    particleKillFx(*(int *)MP(this, 0x6C80));
    particleKillFx(*(int *)MP(this, 0x6C84));
    particleKillFx(*(int *)MP(this, 0x430));
    particleKillFx(*(int *)MP(this, 0x434));
    hierSetCsDrawMe(*(_cs **)MP(this, 0x1A3C), 1);
    hierSetCsDrawMe(m_cs, 1);
    cs = (char *)m_cs;
    cs[0xD] = 1;
    MI(this, 0xA0) = 0;
    MI(this, 0xA8) = 0;
    MI(this, 0xA4) = 0;
    mathfUnitMatrix((float (*)[4])(cs + 0x20));
    mat = (float *)(cs + 0x20);
    memcpy(MP(this, 0x50), mat, 0x40);
    {
        float *rot = (float *)((char *)m_cs + 0x60);

        rot[0] = 0.0f;
        rot[1] = 0.0f;
        rot[2] = 0.0f;
        rot[3] = 1.0f;
    }
    game->gameGetStartPoint(this);
    MI(this, 0x1A10) = ((AiPathNet *)aiPathNetMonster)->getClosestPath(*(_fvector *)((char *)m_cs + 0x10), 0xFF);
    ((PadFlags *)MP(this, 0x5040))->init(this);
    ((MonsterDynamics *)MP(this, 0x100))->init();
    ((AnimContact *)MP(this, 0x3124))->init(*(_animHandle *)MP(this, 0x2290));
    ((FireBreath *)MP(this, 0x68C0))->Init();
    ((HealthMeter *)MP(this, 0x448))->reset();
    m_stamina.reset();
    MI(this, 0x488) = (int)this;
    ((MonsterSound *)MP(this, 0x1A7C))->initMonsterSound(this);
    animationResetCharacter(*(_animHandle *)MP(this, 0x2290));
    ((AnimBlend *)MP(this, 0x2FE0))->rampIn(200.0f);
    ((AnimBlend *)MP(this, 0x2FE0))->setSpeed(0.0f);
    ((AnimBlend *)MP(this, 0x2FE0))->setPercent(0.5f);
    ((AnimBlend *)MP(this, 0x3048))->rampIn(200.0f);
    ((AnimBlend *)MP(this, 0x3048))->setSpeed(0.0f);
    ((AnimBlend *)MP(this, 0x3048))->setPercent(0.5f);
    if (MI(this, 0x30B0) != 0)
        ((AnimBlend *)MI(this, 0x30B0))->rampIn(0.0f);
    enterNewState((MonsterState *)MP(this, 0x7984));
    setCloakOff();
    MI(camerasObj + MI(this, 0x6CD8) * 0xEB0, 0xD98) = 1;
    MI(this, 0x2C) = MI(this, 0x2C) + 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", init__7Monster);
#endif

#ifdef NON_MATCHING
/* 10/73 words: retail inlines the list loop */
void Monster::playerInit(void)
{
    int cond;

    m_playerNum = 1;
    init();
    MI(this, 0x672C) = *(int *)((char *)shell + m_index * 4 + 0x2B8C) - 1;
    *(int *)m_playerInfo = inputGetPlayerPad(m_index);
    if (m_numInits >= 2)
        setInvulnerabilityDuration(0x78);
    if (game->m_gameMode >= 6 && game->m_gameMode < 9)
        cond = 0;
    else
        cond = shell->m_numPlayers >= 2 && shell->m_numAIs >= 2;
    if (cond)
        clearListFlags(this);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", playerInit__7Monster);
#endif

#ifdef NON_MATCHING
/* 19/55 words: retail inlines the list loop */
void Monster::aiInit(void)
{
    m_playerNum = 2;
    init();
    ((Ai *)MP(this, 0x4E0))->init();
    if (game->m_matchMode < 2)
        m_specialWeapon = 0;
    if (m_numInits >= 2 && game->m_gameMode != 1)
        setInvulnerabilityDuration(0x78);
    clearListFlags(this);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", aiInit__7Monster);
#endif

#ifdef NON_MATCHING
/* 6/80 words: untuned */
void Monster::initAfterDbLoad(void)
{
    int i;

    *(int *)this = 1;
    m_flags |= 0xE;
    Interactives::setInteractive(m_id, (DbInteractive *)this);
    for (i = 0; i < 0x12C; i++)
        animationGetHandle((_animHandle *)MP(this, 0x1CF0 + i * 0x10), m_typeBits, m_dupId, i);
    animationGetHandle((_animHandle *)MP(this, 0x28E0), m_typeBits + 5, m_dupId, 0);
    animationInitModifierBlends(*(_animCharInstance **)MP(this, 0x2290));
    ((AnimPappy *)MP(this, 0x2FB0))->init(*(_animHandle *)MP(this, 0x2290));
    ((AnimBlend *)MP(this, 0x2FE0))->init(*(_animHandle *)MP(this, 0x2690));
    *(unsigned short *)MP(this, 0x2FEE) = 0x3E7;
    *(unsigned char *)MP(this, 0x2FED) = 4;
    ((AnimBlend *)MP(this, 0x3048))->init(*(_animHandle *)MP(this, 0x26A0));
    *(unsigned short *)MP(this, 0x3056) = 0x3E7;
    *(unsigned char *)MP(this, 0x3055) = 4;
    ((AnimBlend *)MP(this, 0x30B4))->init(*(_animHandle *)MP(this, 0x26B0));
    MI(this, 0x697C) = 4;
    MF(this, 0x6978) = 8.0f;
    initDynamics();
    collisInitPoints();
    ((PadFlags *)MP(this, 0x5040))->init(this);
    MonsterTweaks::setActiveMonster(this, true);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", initAfterDbLoad__7Monster);
#endif
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", initDynamics__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", initCameraData__7Monster);
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", update__10BoneSpringPv);

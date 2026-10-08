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
INCLUDE_ASM("asm/nonmatchings/game/MonsterInit", init__7Monster);

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

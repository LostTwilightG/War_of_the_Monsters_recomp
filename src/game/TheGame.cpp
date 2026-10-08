#include "common.h"
#include "game/game.h"
#include "game/shell.h"
#include "game/power_ups.h"
#include "game/crush_level.h"
#include "game/levels.h"
#include "game/level_pickups.h"
#include "game/streaming_sound.h"
#include "task_manager.h"
#include "memory_stack.h"
#include "game/power_up_tool.h"
#include "game/start_point_tool.h"
#include "game/token_manager.h"

class Debris {
public:
    static void InitAfter(void);
};
class SpecFxAnim {
public:
    static void initAfterDbLoad(void);
};
class HomingBug {
public:
    static void initAfterDbLoad(void);
};
class Ai {
public:
    static void globalInit(void);
};
class ActionDispatch {
public:
    static void initGenericEvent(void (*handler)(unsigned));
};
class LevelObjectSoundManager {
public:
    void initLevelObjectSoundManager(void);
};

class Destructibles {
public:
    void Update(void);
};
extern Destructibles *destructibles;
extern "C" int printf(const char *, ...);
class DbInteractive;
class Interactives {
public:
    static int addInteractive(DbInteractive *p);
};
_cs *dbGetCSForModel(_hierhead *h);
void hierSetCsDrawMe(_cs *cs, unsigned char v);

void checkTriggerTree(void);


INCLUDE_ASM("asm/nonmatchings/game/TheGame", Init__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", InitBeforeDbLoad__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", traversalCallback__7TheGameP3_csUiUiRA3_A3_fP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", genericEventHandler__7TheGameUi);
#ifdef NON_MATCHING
/* 128/216 words, untuned: written from the m2c draft */
void TheGame::InitAfterDbLoad(void)
{
    int i;

    f120468 = 1;
    if (m_gameMode != 8) {
        PowerUps::instance.initPowerUpsAfter();
        PowerUpTool::instance.loadPoints(0, true);
        StartPointTool::instance.loadPoints();
    }
    particleInitAfter();
    hdInit();
    getWeapons()->InitWeaponsAfter();
    Debris::InitAfter();
    LevelPickups::initAfterDbLoad();
    SpecFxAnim::initAfterDbLoad();
    HomingBug::initAfterDbLoad();
    Ai::globalInit();
    m_minUpdateRate = timerGetMinUpdateRate();
    f120460 = 1;
    f120454 = 0;
    f120458 = 0;
    f120450 = 0;
    f12045C = 0;
    for (i = m_numSlots - 1; i >= 0; i--) {
        m_slots[i].initAfterDbLoad();
        m_slots[i].m_winsThisGame = 0;
    }
    MemoryStack::global.pushMark();
    hierSetTraversalCallback(traversalCallback);
    ActionDispatch::initGenericEvent(genericEventHandler);
    ((LevelObjectSoundManager *)((char *)this + 0x121570))->initLevelObjectSoundManager();
    gUseUnifiedView = 0;
    Cameras::SetCameraPOV(2, Camera::POV_3);
    switch (m_levelId) {
    case 1:
        if (shell->m_mode == 1)
            centralInitAfter();
        break;
    case 2:
        if (shell->m_mode == 1)
            vegasInitAfter();
        break;
    case 3:
        if (shell->m_mode == 1)
            canyon2InitAfter();
        break;
    case 5:
        airportInitAfter();
        break;
    case 8:
    case 15:
        islandInitAfter();
        for (i = 0; i < m_numMonsters; i++) {
            m_tokens[i].setMax("Destructibles", 0);
            m_tokens[i].setMilestone("Destructibles", 1, 1, TokenManager::MILESTONE_1);
        }
        break;
    case 6:
        threeMileInitAfter();
        break;
    case 9:
        tokyoInitAfter();
        break;
    case 7:
        sanFranInitAfter();
        break;
    case 10:
        if (shell->m_mode == 1)
            ufoInitAfter();
        break;
    case 11:
        if (shell->m_mode == 1)
            finalBoss.initAfter();
        break;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", InitAfterDbLoad__7TheGame);
#endif
#ifdef NON_MATCHING
/* 23/252 words, untuned: written from the m2c draft */
void TheGame::Update(void)
{
    int i;
    Monster *m;

    m = m_slots;
    for (i = m_numSlots; i != 0; i--, m++) {
        if (m->m_playerNum != 0) {
            int active = ((m_playerMask & 1) && m->m_playerNum == 1) || ((m_playerMask & 2) && m->m_playerNum == 2);

            if (active)
                m->update();
            else
                m->updateCinema();
        }
    }
    m = m_slots;
    for (i = m_numSlots; i != 0; i--, m++) {
        if (m->m_playerNum != 0) {
            int active = ((m_playerMask & 1) && m->m_playerNum == 1) || ((m_playerMask & 2) && m->m_playerNum == 2);

            if (active)
                m->updatePosition();
        }
    }
    gameResolveLifeAndDeath();
    checkTriggerTree();
    if (m_gameMode != 7 && m_gameMode != 9)
        gameCheckForCloseCombat();
    destructibles->Update();
    Cameras::Update();
    PowerUps::instance.update();
    gTaskManager0.update();
    switch (m_levelId) {
    case 1:
        if (shell->m_mode == 1)
            centralUpdate();
        break;
    case 2:
        if (shell->m_mode == 1)
            vegasUpdate();
        break;
    case 3:
        if (shell->m_mode == 1)
            canyon2Update();
        break;
    case 5:
        airportUpdate();
        break;
    case 8:
    case 15:
        islandUpdate();
        break;
    case 6:
        threeMileUpdate();
        break;
    case 9:
        tokyoUpdate();
        break;
    case 7:
        sanFranUpdate();
        break;
    case 10:
        if (shell->m_mode == 1)
            ufoUpdate();
        break;
    case 11:
        if (shell->m_mode == 1)
            finalBoss.update();
        break;
    case 26:
        if (shell->m_mode == 8)
            (BigShotLevel::instance.*BigShotLevel::UPDATE_FUNK[BigShotLevel::instance.m_state])();
        break;
    case 27:
        if (shell->m_mode == 9)
            (CrushLevel::instance.*CrushLevel::UPDATE_FUNK[CrushLevel::instance.m_suddenDeath])();
        break;
    }
    ((StreamingSoundManager *)((char *)this + 0x1204C0))->updateStreamingSoundManager(Cameras::m_cameras.m_state == 7);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", Update__7TheGame);
#endif
void TheGame::Update2(void)
{
    gameResolveCollisions();
    TaskManager::global.update();
    getWeapons()->UpdateWeapons();
    if (m_playerMask & 4)
        LevelPickups::update();
}
/* The first monster model parsed for a (type, dup) pair keeps the highest `dup` seen so far: models of the same monster type
   (the object id with its low 5 bits cleared) are numbered 1.. in the order the level lists them. */
#ifdef NON_MATCHING
/* 1/65 words: untuned, from the m2c draft */
void TheGame::MonsterParse(_hierhead *h, _fvector *pos)
{
    int found = -1;
    int dup;
    unsigned id = *(unsigned *)h >> 18;
    int rem = id & 0x1F;
    int type = id - rem;

    for (dup = 0; dup < 0x32; dup++) {
        if (GetMonsterFromName(type, dup) != 0)
            found = dup;
    }
    if (rem == 0) {
        _cs *cs = dbGetCSForModel(h);

        m_curDupId = found + 1;
        AddMonster(cs, type, found + 1);
        return;
    }
    if ((int)id < 0x400) {
        Monster *m = GetMonsterFromName(type, found);

        if (m != 0)
            m->addAttachment(h);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", MonsterParse__7TheGameP9_hierheadP8_fvector);
#endif

#ifdef NON_MATCHING
/* 38/73 words: untuned */
void TheGame::AddMonster(_cs *cs, int type, int dup)
{
    Monster *m = &m_slots[m_numSlots];
    unsigned *head;

    m->m_dupId = dup;
    m_slotInteractive[m_numSlots] = Interactives::addInteractive((DbInteractive *)&m_slots[m_numSlots]);
    m->m_typeBits = type;
    m->m_id = m_slotInteractive[m_numSlots];
    m->m_cs = cs;
    m->m_playerNum = 0;
    cs->drawMe = 0;
    head = (unsigned *)cs->epNode;
    *head = (*head & 0x3FFFF) | (type << 18);
    head = (unsigned *)cs->epNode;
    *head = (*head & 0xFFFC007F) | ((m_slotInteractive[m_numSlots] & 0x7FF) << 7);
    m_numSlots++;
    printf("Got monster %d with cs %p, interactiveIndex %d\n", type, cs, m->m_id);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", AddMonster__7TheGameP3_csii);
#endif

#ifdef NON_MATCHING
/* 0/33 words: retail peels the first slot */
Monster *TheGame::GetMonsterFromName(int type, int dup)
{
    int i;

    for (i = 0; i < m_numSlots; i++) {
        if (m_slots[i].m_typeBits == type && m_slots[i].m_dupId == dup)
            return &m_slots[i];
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", GetMonsterFromName__7TheGameii);
#endif

#ifdef NON_MATCHING
/* 4/84 words: untuned */
void TheGame::SetPlayerMonster(int pIdx, int type, int dup, int view, int skin)
{
    int i;

    if (m_numSlots == 0)
        printf(">>>>>>>>>>  No Monsters In Database!!!  <<<<<<<<<<\n");
    for (i = 0; i < m_numSlots; i++) {
        Monster *m = &m_slots[i];

        if (m->m_typeBits == type && m->m_dupId == dup) {
            m->m_index = pIdx;
            m->m_monsterNum = i;
            m->m_playerInfo = (PlayerDat *)((char *)this + 0x112480 + pIdx * 4);
            m_monsters[pIdx] = m;
            m->playerInit();
            m->m_skinNum = skin;
            if (i < 2)
                *(int *)((char *)m + 0x672C) = *(int *)((char *)shell + 0x2B8C + i * 4) - 1;
            if (view >= 0)
                gameInitCamera(view, i);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetPlayerMonster__7TheGameiiiii);
#endif

#ifdef NON_MATCHING
/* A monster that is not driven by a player or an AI (the boss halves, the inactive crowd) is switched off: no owner, nothing drawn. */
static void disableMonster(Monster *m)
{
    m->m_playerNum = 0;
    m->m_cs->drawMe = 0;
    m->m_shadow->drawMe = 0;
    (*(_cs **)((char *)m + 0x71F8))->drawMe = 0;
    (*(_cs **)((char *)m + 0x71FC))->drawMe = 0;
}

/* 3/205 words: retail inlines the five disable blocks */
void TheGame::SetAIMonster(int aiIdx, int type, int dup, int skin)
{
    int i;

    m_numAIs = 0;
    for (i = 0; i < m_numSlots; i++) {
        Monster *m = &m_slots[i];

        if (m->m_typeBits == type && m->m_dupId == dup) {
            m->m_monsterNum = i;
            m->m_index = aiIdx;
            m->m_playerInfo = 0;
            m_monsters[4 + aiIdx] = m;
            m->m_skinNum = skin;
            m->aiInit();
            if (m->m_shadow != 0)
                hierSetCsDrawMe(m->m_shadow, 1);
            if (m_numMonsters < 2)
                gameInitCamera(1, i);
            else
                gameInitCamera(3, i);
        }
        if (m_slots[i].m_playerNum == 2 || m_slots[i].m_typeBits == 0x1C0 || m_slots[i].m_typeBits == 0x1E0)
            m_numAIs++;
        if (m_slots[i].m_typeBits == 0x1A0)
            disableMonster(&m_slots[i]);
        if (m_levelId == 2 && m_gameMode == 1 && m_slots[i].m_typeBits == 0x40 && m_slots[i].m_playerNum == 2)
            disableMonster(&m_slots[i]);
        if (m_slots[i].m_typeBits == 0x1C0) {
            disableMonster(&m_slots[i]);
            *(Monster **)((char *)&finalBoss + 0x74) = &m_slots[i];
        }
        if (m_slots[i].m_typeBits == 0x1E0) {
            disableMonster(&m_slots[i]);
            *(Monster **)((char *)&finalBoss + 0x78) = &m_slots[i];
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetAIMonster__7TheGameiiii);
#endif
void TheGame::gameInitCamera(int view, int slot)
{
    m_viewSlot[view] = slot;
    Cameras::SetCameraToFollowMonster(view, &m_slots[slot]);
}
float TheGame::GetCameraMaxHeight(_fvector *pos)
{
    return 10000.0f;
}
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameResolveCollisions__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameResolveLifeAndDeath__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameCheckForCloseCombat__7TheGame);
void TheGame::SetGravity(float g)
{
    m_gravity = g;
}
void TheGame::SetOkToUnify(void)
{
    int i;

    for (i = 0; i < m_numSlots; i++)
        m_slots[i].m_camUnify = 1;
}
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameGetStartPoint__7TheGameP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", getClosestMonster__7TheGameR8_fvectorfRf);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", getClosestPlayer__7TheGameR8_fvectorfRf);
int TheGame::GetNumAIsAlive(void)
{
    int i;

    m_numAIsAlive = 0;
    for (i = 0; i < m_numAIs; i++) {
        if (m_monsters[4 + i]->m_dead == 0)
            m_numAIsAlive++;
    }
    return m_numAIsAlive;
}
#ifdef NON_MATCHING
/* 55/91 words, untuned: loop shape */
void TheGame::ResetLevel(void)
{
    int i;
    int unified;

    for (i = 0; i < 4; i++)
        m_huds[i].initForReplay();
    for (i = 0; i < m_numMonsters; i++) {
        PadFlags &pf = m_monsters[i]->m_padFlags;

        pf.saveAndClear(1);
        pf[0]->f3E = 0;
        pf.clearModifiers();
        if (gUseUnifiedView == 0)
            pf.f16E8 = 0;
    }
    for (i = 0; i < m_numSlots; i++) {
        Monster *m = &m_slots[i];

        if (m->m_playerNum != 0) {
            m->m_unkF6 = 0;
            m->m_unkF7 = 0;
            m->m_unk49 = 1;
        }
    }
    unified = gUseUnifiedView;
    if (m_gameMode == 8)
        BigShotLevel::instance.initForReplay();
    if (unified == 0)
        Cameras::SetCameraPOV(2, Camera::POV_3);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", ResetLevel__7TheGame);
#endif
void TheGame::UnpauseLevel(void)
{
    int i;

    for (i = 0; i < m_numMonsters; i++) {
        PadFlags &pf = m_monsters[i]->m_padFlags;

        pf.saveAndClear(1);
        pf[0]->f3E = 0;
        pf.clearModifiers();
        if (gUseUnifiedView == 0 && m_gameMode != 9)
            pf.f16E8 = 0;
    }
}
#ifdef NON_MATCHING
/* 13/62 words, untuned: loop shape */
void TheGame::UpdatePadTweaks(void)
{
    int i;

    for (i = 0; i < m_numMonsters; i++) {
        PadFlags &pf = m_monsters[i]->m_padFlags;

        pf.tweak16C8 = m_padTweaks.t16C8;
        pf.tweak16C4 = m_padTweaks.t16C4;
        pf.tweak16DC = m_padTweaks.t16DC;
        pf.tweak16D0 = m_padTweaks.t16D0;
        pf.tweak16D4 = m_padTweaks.t16D4;
        pf.tweak16F4 = m_padTweaks.t16F4;
        pf.tweak16F8 = m_padTweaks.t16F8;
        pf.tweak16FC = m_padTweaks.t16FC;
        pf.tweak17AC = m_padTweaks.t17AC;
        pf.tweak17B0 = m_padTweaks.t17B0;
        pf.tweak16E0 = m_padTweaks.t16E0;
    }
    for (i = 0; i < 8; i++)
        inputUseActuator(i, m_actuator[i]);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/TheGame", UpdatePadTweaks__7TheGame);
#endif
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameReestablishViews__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", monsterPush__7TheGamePP9_hierheadUi);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", monsterPop__7TheGameP13_monsterstackPP9_hierhead);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", fadeOutAndIn__7TheGamei);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", updateFadeOutAndIn__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", fadeOut__7TheGamei);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", updateFadeOut__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetVibration__7TheGameib);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", AddCrushMessage__7TheGamei);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", __static_initialization_and_destruction_0_00139F00);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", func_00139F78);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", func_00139F98);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", updateFadeOutAndIn__7TheGamePv);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", updateFadeOut__7TheGamePv);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", _GLOBAL_$I$resetObj);

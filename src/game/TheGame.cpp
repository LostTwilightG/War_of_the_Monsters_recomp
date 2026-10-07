#include "common.h"
#include "game/game.h"
#include "game/shell.h"
#include "game/power_ups.h"
#include "game/crush_level.h"
#include "game/levels.h"
#include "game/level_pickups.h"
#include "game/streaming_sound.h"
#include "task_manager.h"

class Destructibles {
public:
    void Update(void);
};
extern Destructibles *destructibles;
void checkTriggerTree(void);


INCLUDE_ASM("asm/nonmatchings/game/TheGame", Init__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", InitBeforeDbLoad__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", traversalCallback__7TheGameP3_csUiUiRA3_A3_fP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", genericEventHandler__7TheGameUi);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", InitAfterDbLoad__7TheGame);
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
    TaskManager::global.update();
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
INCLUDE_ASM("asm/nonmatchings/game/TheGame", MonsterParse__7TheGameP9_hierheadP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", AddMonster__7TheGameP3_csii);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", GetMonsterFromName__7TheGameii);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetPlayerMonster__7TheGameiiiii);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetAIMonster__7TheGameiiii);
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

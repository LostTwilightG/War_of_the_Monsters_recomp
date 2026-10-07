#include "common.h"
#include "game/game.h"
#include "game/shell.h"
#include "game/power_ups.h"
#include "game/crush_level.h"
#include "game/levels.h"
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
INCLUDE_ASM("asm/nonmatchings/game/TheGame", Update2__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", MonsterParse__7TheGameP9_hierheadP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", AddMonster__7TheGameP3_csii);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", GetMonsterFromName__7TheGameii);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetPlayerMonster__7TheGameiiiii);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetAIMonster__7TheGameiiii);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameInitCamera__7TheGameii);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", GetCameraMaxHeight__7TheGameP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameResolveCollisions__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameResolveLifeAndDeath__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameCheckForCloseCombat__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetGravity__7TheGamef);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", SetOkToUnify__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", gameGetStartPoint__7TheGameP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", getClosestMonster__7TheGameR8_fvectorfRf);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", getClosestPlayer__7TheGameR8_fvectorfRf);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", GetNumAIsAlive__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", ResetLevel__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", UnpauseLevel__7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/TheGame", UpdatePadTweaks__7TheGame);
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

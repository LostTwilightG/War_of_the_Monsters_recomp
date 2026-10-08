#include "common.h"
#include "game/shell.h"

/* One monster-select slot (0x2C bytes); the table holds groups of four per player, then the unlock flags of the levels. */
struct SelectSlot {
    int taken;      /* 0x00: the slot holds a model */
    int state;      /* 0x04 */
    char pad8[0x24];
};
extern SelectSlot monsterSelectMode[];
extern int numModsLeft;
extern int unlocked_3092 __asm__("unlocked.3092");

int monsterIdToMonsterEnum(int id)
{
    return id / 32;
}
INCLUDE_ASM("asm/nonmatchings/game/screen", screenPrepare__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", setReelSwitches__FiiP13_SaveGameData);
INCLUDE_ASM("asm/nonmatchings/game/screen", changeReel__Fiii);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenMain__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", updateSelectSwitches__Fiiii);
INCLUDE_ASM("asm/nonmatchings/game/screen", updateSelectSwitches3P__Fiiiiii);
INCLUDE_ASM("asm/nonmatchings/game/screen", updateSelectSwitches4P__Fiiiiiiii);
INCLUDE_ASM("asm/nonmatchings/game/screen", updateSelectSwitchesMG__Fiiii);
INCLUDE_ASM("asm/nonmatchings/game/screen", updateSelectSwitchesAI__Fii);
INCLUDE_ASM("asm/nonmatchings/game/screen", initSelectSwitches__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", initSelectSwitchesMG__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenGameModes1P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE2F0);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenCharSelect1P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenSelect1AI__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenCtlrCfg__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenGameModes2P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE460);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE488);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenCharSelect3P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE4D8);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE508);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE530);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE558);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE590);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE5B0);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenCharSelect2P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenMinigames2P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE698);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE6D0);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE720);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE740);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE770);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE7A0);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE7D0);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE7F0);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE820);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE850);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE878);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE890);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE8F8);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE960);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EE978);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE9D0);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EE9F0);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEA48);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEA58);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEA88);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEA98);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEAA8);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEAB8);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEAD8);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEAF0);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEB00);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEB30);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEB48);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEB60);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EEB80);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEB98);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEBC0);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEBE8);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEC10);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEC40);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EEC70);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EECA0);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EECD0);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EECF0);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EED08);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EED30);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EED40);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EED50);
INCLUDE_ASM("asm/nonmatchings/game/screen", D_006EED68);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EED80);
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EEDD0);
#ifndef NON_MATCHING /* the C++ levelUnlocked builds its own table */
INCLUDE_ASM("asm/nonmatchings/game/screen", jtbl_006EEE30);
#endif
INCLUDE_ASM("asm/nonmatchings/game/screen", on_bit_006EEE78);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenLevelSelect__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenLoadSave__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", setSlider__Fif);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenOptions__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenFreeForAllOptions1P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenFreeForAllOptions2P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenElimOptions2P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenMGSelect__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenBestOf__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenMemoryCardInit__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenTransition__Fb);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenUnlocker__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenCharSelect4P__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenWaitForStart__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", changeScreen__Fiiiib);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenGetInput__Fi);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenPrepareDefault__Fii);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenWriteInstructions__Fif);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenGetFontAlpha__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", setButtonAlpha__Ff);
INCLUDE_ASM("asm/nonmatchings/game/screen", screenSetButtonAlpha__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", showButtons__Fi);
INCLUDE_ASM("asm/nonmatchings/game/screen", killButtons__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", saveUnlocks__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", saveSettings__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", loadSettings__Fv);
#ifdef NON_MATCHING
/* 7/19 words: address math scheduling */
int isSlotOpen(int group, int slot)
{
    SelectSlot *s = (SelectSlot *)((char *)monsterSelectMode + slot * 0x2C + group * 0xB0);

    if (s->state == 1 && s->taken == s->state)
        return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/screen", isSlotOpen__Fii);
#endif
INCLUDE_ASM("asm/nonmatchings/game/screen", resetSelectables__Fv);
void resetGameMode(void)
{
    int mode = *(int *)((char *)shell + 0x2B54);

    if (mode != 0)
        shell->m_mode = mode;
}

#ifdef NON_MATCHING
/* 6/26 words: numModsLeft not gp-relative on first store */
int modelsLeft(int group)
{
    int i = 3;
    SelectSlot *s = (SelectSlot *)((char *)monsterSelectMode + (group - 1) * 0xB0);

    numModsLeft = 4;
    do {
        if (s->taken == 0 || s->state == 0)
            numModsLeft--;
        s = (SelectSlot *)((char *)s + 0x2C);
    } while (--i >= 0);
    return numModsLeft;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/screen", modelsLeft__Fi);
#endif

/* Levels 10, 11, 14, 15, 25, 26 and 27 are locked until their flag in the select table is set; every other level is open. */
#ifdef NON_MATCHING
/* 9/56 words: retail keeps a jump table */
int levelUnlocked(int level)
{
    switch (level) {
    case 10:
        unlocked_3092 = *(int *)((char *)monsterSelectMode + 0x768) != 0;
        break;
    case 11:
        unlocked_3092 = *(int *)((char *)monsterSelectMode + 0x73C) != 0;
        break;
    case 14:
        unlocked_3092 = *(int *)((char *)monsterSelectMode + 0x710) != 0;
        break;
    case 15:
        unlocked_3092 = *(int *)((char *)monsterSelectMode + 0x6E4) != 0;
        break;
    case 25:
        unlocked_3092 = *(int *)((char *)monsterSelectMode + 0x794) != 0;
        break;
    case 26:
        unlocked_3092 = *(int *)((char *)monsterSelectMode + 0x7C0) != 0;
        break;
    case 27:
        unlocked_3092 = *(int *)((char *)monsterSelectMode + 0x7EC) != 0;
        break;
    default:
        unlocked_3092 = 1;
        break;
    }
    return unlocked_3092;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/screen", levelUnlocked__Fi);
#endif
INCLUDE_ASM("asm/nonmatchings/game/screen", findAvailableMonster__Fiii);
INCLUDE_ASM("asm/nonmatchings/game/screen", resetModels__Fv);
INCLUDE_ASM("asm/nonmatchings/game/screen", __static_initialization_and_destruction_0_001A6668);
INCLUDE_ASM("asm/nonmatchings/game/screen", _GLOBAL_$I$NamecardDistance);

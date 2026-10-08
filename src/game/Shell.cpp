#include "common.h"
#include "game/shell.h"

extern "C" int printf(const char *, ...);
extern "C" int sprintf(char *, const char *, ...);
extern "C" char *strcpy(char *, const char *);

struct sceCdlFILE {
    unsigned lsn;
    unsigned size;
    char name[16];
    char date[8];
};
struct _dbheader;

int zipInflateAll(char *name, void *dest);
void fileOnlyNgpFile(char *dest, int size);
char *fileTrimPath(char *path);
void fileAddName(char *name);
void fileCdSearchFile(sceCdlFILE *f, char *path);
char *fileGetTimeString(void);
void informProgressBar(float f);
void initProgressBar(int a, int b, int c);
void viewSetNumViews(int n);
enum _viewports { VIEWPORT_7 = 7 };
void viewCreate(_viewports vp, int i);
void dbsRelocateFileZero(_vramAddrs v, bool b);
void dbInitDb(_dbheader *db, _vramAddrs v);

extern char D_00731500[];                 /* path of the file being loaded */
extern char whichLevel[];                 /* name of the command-line level */
extern char D_006F81D0[];                 /* "ngp" */
extern char D_006F81C0[];                 /* "rtx" */
extern char D_006F81C8[];                 /* "tex" */
extern char D_006F81E0[];                 /* disc root prefix */
extern char D_006F81E8[];                 /* name suffix */
extern char GameLevelNames_006EF748[][10];
extern char MonsterLongNames_006EF860[][8];
extern char D_006F81D8[];                 /* "%s%d" */
extern char D_006F81A8[];                 /* "%s" */
extern char D_006EF458[];                 /* "Unknown monster (%d) for player %d..." */
extern char D_006EF498[];                 /* "Unknown monster (%d) for ai %d..." */
int fileReadf(char *name, void *dest);
void fileAddNgpFile(char *addr, int size);
char *getNextNgpLoadAddr(void);
char *getNextResLoadAddr(void);
char *getNextTexLoadAddr(void);
void fileAddResFile(char *addr, int size);
void fileAddTexFile(char *addr, int size);
void fileOnlyResFile(char *dest, int size);
void fileOnlyTexFile(char *dest, int size);
struct QwData;
void texmResInit(_vramAddrs v, QwData *q);
void texmInit(_vramAddrs v);
extern int UseCommandLineLevel;
extern int gWhichMicroSection;
extern int gWhichMacroSection;


#define SH(o) (*(int *)((char *)shell + (o)))

/* Boot and session flow of the whole game:
   boot -> (optional intro/outro movie) -> front-end menus (userintMain) -> a play session: load the level, the monsters and their
   textures, build the world, create the players, then run rtMain frame by frame until the session ends, restarting the level on request. */
extern "C" void __main(void);
int mathfRand(int lo, int hi);
class Destructibles;
class TheGame;
extern Destructibles *destructibles;
extern TheGame *game;
extern char shellDestructibles[];
extern char shellInfo[];
extern char shellGame[];
extern char resetObj[];
extern int exitFromAdvStory;
extern int currScreen;
extern int needIntro;
extern int needOutro;
extern int nextMovie;
extern int g_frame;
extern int g_onStartupFrame;
extern char whichLevel[];
extern char _10CrushLevel$instance[] __asm__("_10CrushLevel$instance");
extern char _12BigShotLevel$instance[] __asm__("_12BigShotLevel$instance");
extern char _14DodgeBallLevel$instance[] __asm__("_14DodgeBallLevel$instance");

class SoundManager {
public:
    void doOneTimeInit(void);
    void loadShellSoundBanks(void);
    void initSoundManager(void);
    void manageReverb(void);
    void loadRTSoundBanks(void);
    void initVagStreaming(void);
    void disableSoundForCinema(void);
};
class ShellSound {
public:
    void terminateShellSound(void);
    void resetShellSoundFlags(void);
};
class StreamingSoundManager {
public:
    void initStreamingSoundManager(void);
};
class Hud {
public:
    void initAfter(int i);
};
class resetcom {
public:
    void setFileName(char *name, bool b);
};
class BigShotLevel {
public:
    void initAfterDbLoad(void);
};
class CrushLevel {
public:
    void initAfterDbLoad(void);
};
class DodgeBallLevel {
public:
    void initAfterDbLoad(void);
};
class TheGame {
public:
    void Init(void);
    void InitBeforeDbLoad(void);
    void InitAfterDbLoad(void);
    void ResetLevel(void);
    void UnpauseLevel(void);
    void UpdatePadTweaks(void);
    void gameReestablishViews(void);
    void SetPlayerMonster(int pIdx, int type, int dup, int view, int skin);
    void SetAIMonster(int aiIdx, int type, int dup, int skin);
};
class CsPool {
public:
    static void init(void);
};
void ResolveCommandLineArguments(int argc, char **argv);
void fileInitializeCd(void);
void inputInit(void);
void hierSetDmaIntHandler(void);
void inputSetInputMode(int m);
void uiInit(void);
void uiMain(int i);
int userintMain(void);
int rtMain(bool first);
void fontInit(_vramAddrs v, int i);
void threeMileInitAi(void);

#ifdef NON_MATCHING
/* 33/356 words: untuned, from the m2c draft + asm */
extern "C" int main(int argc, char **argv)
{
    int i;

    __main();
    destructibles = (Destructibles *)shellDestructibles;
    SH(0x2BB0) = 0;
    shell = (Shell *)shellInfo;
    game = (TheGame *)shellGame;
    ResolveCommandLineArguments(argc, argv);
    fileInitializeCd();
    inputInit();
    CsPool::init();
    ((TheGame *)game)->Init();
    *(int *)(shellGame + 0x1203CC) = 0;
    hierSetDmaIntHandler();
    ((SoundManager *)((char *)shell + 0x2C30))->doOneTimeInit();
    needIntro = 1;
    needOutro = 0;
    g_frame = 0;
    g_onStartupFrame = 0;
    shell->InitialMemCardScreen();
    for (;;) {
        ((SoundManager *)((char *)shell + 0x2C30))->loadShellSoundBanks();
        uiInit();
        do {
            printf("Entering movie selection\n");
            SH(0x2BD0) = 1;
            if (needIntro != 0 || needOutro != 0) {
                shell->m_mode = 0x3F;
                printf("Intro or Outro needed\n");
                *(int *)((char *)game + 0x1203C8) = 0x3F;
                shell->BootInitUi();
                inputSetInputMode(0);
                shell->InitBeforeUiDbLoad();
                if (needIntro != 0) {
                    nextMovie = 3;
                    printf("Playing Intro movie\n");
                    uiMain(0);
                }
                if (needOutro != 0) {
                    printf("Playing Outro movie\n");
                    uiMain(1);
                }
                needIntro = 0;
                needOutro = 0;
            } else {
                ((TheGame *)game)->Init();
            }
            shell->m_mode = 0x40;
            *(int *)((char *)game + 0x1203C8) = 0x40;
            shell->BootInitUserint();
            inputSetInputMode(0);
            shell->InitBeforeUserintDbLoad();
            shell->LoadUserintDB();
            shell->LoadUserintTexture();
            dbsRelocateFileZero(shell->getVramAddr(), UseCommandLineLevel);
            dbInitDb((_dbheader *)0xA00000, shell->getVramAddr());
            if (exitFromAdvStory == 1) {
                printf("Just came out of adventure mode Story\n");
                currScreen = exitFromAdvStory;
            }
        } while (userintMain() != 0);
        ((ShellSound *)((char *)shell + 0x2918))->terminateShellSound();
        ((SoundManager *)((char *)shell + 0x2C30))->initSoundManager();
        if (SH(0x2BD0) == 0)
            continue;
        do {
            SH(0x2BCC) = 1;
            ResolveCommandLineArguments(argc, argv);
            ((resetcom *)resetObj)->setFileName(whichLevel, UseCommandLineLevel);
            shell->InitRTState();
            ((SoundManager *)((char *)shell + 0x2C30))->disableSoundForCinema();
            shell->BootInitGame();
            inputSetInputMode(1);
            fontInit((_vramAddrs)0x69840, 1);
            ((TheGame *)game)->InitBeforeDbLoad();
            shell->LoadLevelFiles();
            shell->FinishLoadBar();
            shell->FadeScreen(0, false, 0, 0, 0, 0, 0x80, 2);
            ((TheGame *)game)->InitAfterDbLoad();
            shell->InitPlayers();
            ((SoundManager *)((char *)shell + 0x2C30))->manageReverb();
            for (i = 0; i < 4; i++)
                ((Hud *)((char *)game + i * 0x2E0))->initAfter(i);
            switch (shell->m_mode) {
            case 8:
                ((BigShotLevel *)_12BigShotLevel$instance)->initAfterDbLoad();
                break;
            case 9:
                ((CrushLevel *)_10CrushLevel$instance)->initAfterDbLoad();
                break;
            case 7:
                ((DodgeBallLevel *)_14DodgeBallLevel$instance)->initAfterDbLoad();
                break;
            }
            if (shell->m_levelNum == 6 && shell->m_mode == 1)
                threeMileInitAi();
            ((TheGame *)game)->UpdatePadTweaks();
            ((SoundManager *)((char *)shell + 0x2C30))->initVagStreaming();
            ((SoundManager *)((char *)shell + 0x2C30))->loadRTSoundBanks();
            ((TheGame *)game)->gameReestablishViews();
            ((StreamingSoundManager *)((char *)game + 0x1204C0))->initStreamingSoundManager();
            SH(0x2B60) = 0;
            SH(0x2B64) = 0;
            if (SH(0x2BCC) != 0) {
                bool first = true;

                do {
                    int r;

                    SH(0x2BA4) = 0;
                    r = rtMain(first);
                    first = false;
                    shell->EvaluateGameStatus(r);
                    if (SH(0x2BCC) != 0) {
                        if (SH(0x2BA8) != 0)
                            ((TheGame *)game)->ResetLevel();
                        else
                            ((TheGame *)game)->UnpauseLevel();
                    }
                } while (SH(0x2BCC) != 0);
            }
            ((ShellSound *)((char *)shell + 0x2918))->resetShellSoundFlags();
        } while (SH(0x2BD0) != 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Shell", main);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Shell", SelectAI__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", D_006EF180);
INCLUDE_ASM("asm/nonmatchings/game/Shell", RandomlySelectAI__5Shell);
#ifdef NON_MATCHING
struct SeenType {
    int type;
    int count;
};

class Destructibles {
public:
    void rebuildCanyon2Pillars(void);
    void rebuildCapitolPillars(void);
};
class HealthMeter {
public:
    void creditFull(void);
};
struct _cs;
void hierSetCsDrawMe(_cs *cs, unsigned char v);
extern int canyon2LevelProgression;
extern int craterSwitched;
extern int numDeadHeads;
extern char plantBoss[];
extern char *assBoss;
extern float assBossTier1Health;
extern float assBossTier2Health;
extern char finalBoss[];
extern char D_006F81A0[]; /* "AI %d\n" */

/* Instances the AI monsters of the level. `seen` is the table of monster types already placed (players first): every further monster
   of the same type gets the next duplicate number, which selects the model copy (see TheGame::GetMonsterFromName). */
static void spawnAIs(Shell *sh, SeenType *seen, int *nSeen, bool log)
{
    int j;
    int sel;

    for (j = 0; j < *(int *)((char *)sh + 0x2BB8); j++) {
        int *monsterSel = (int *)((char *)sh + 0x2948);
        int k;
        int found = 0;
        int at = 0;
        int dup;

        sel = monsterSel[j];
        for (k = 0; k < *nSeen; k++) {
            if (seen[k].type == sel) {
                at = k;
                found = 1;
            }
        }
        if (found) {
            dup = ++seen[at].count;
        } else {
            dup = 0;
            seen[*nSeen].type = sel;
            (*nSeen)++;
        }
        game->SetAIMonster(j, sel, dup, *(int *)((char *)sh + 0x2BE8 + j * 4));
        if (log)
            printf("set ai monster %d %d\n", sel, dup);
    }
}

void Shell::InitPlayers(void)
{
    SeenType seen[16];
    int n = 0;
    int i;
    int k;

    printf("num play: %d num AI: %d\n", m_numPlayers, m_numAIs);
    for (i = 0; i < m_numPlayers; i++)
        printf("PLAYER %d\n", m_monsterSel[i]);
    for (i = 0; i < m_numAIs; i++)
        printf(D_006F81A0, m_monsterSel[4 + i]);
    for (i = 0; i < 16; i++)
        seen[i].count = 0;
    *(int *)((char *)game + 0x1203D8) = m_numPlayers;
    for (i = 0; i < m_numPlayers; i++) {
        int sel = m_monsterSel[i];
        int found = 0;
        int at = 0;
        int dup;

        for (k = 0; k < n; k++) {
            if (seen[k].type == sel) {
                at = k;
                found = 1;
            }
        }
        if (found) {
            dup = ++seen[at].count;
        } else {
            dup = 0;
            seen[n].type = sel;
            n++;
        }
        game->SetPlayerMonster(i, sel, dup, i, m_costume[i != 0]);
        printf("set player monster %d %d\n", sel, dup);
        *(int *)((char *)(*(char **)((char *)game + 0x120380 + i * 4)) + 0x3C) = 0;
    }
    if (m_levelNum == 3 && m_mode == 1) {
        InitPlayerLives();
        if (canyon2LevelProgression == 0) {
            *(int *)((char *)game + 0x1203E0) = m_numAIs;
            spawnAIs(this, seen, &n, false);
        } else if (canyon2LevelProgression == 3) {
            if (assBoss != 0) {
                *(float *)(assBoss + 0x44C) = *(float *)(assBoss + 0x448);
                ((Destructibles *)destructibles)->rebuildCanyon2Pillars();
            }
        } else if (canyon2LevelProgression == 6) {
            if (assBoss != 0) {
                float max = *(float *)(assBoss + 0x448);
                float tier;

                if (assBossTier2Health < *(float *)(assBoss + 0x44C) / max)
                    tier = assBossTier1Health - 0.01f;
                else
                    tier = assBossTier2Health - 0.01f;
                *(float *)(assBoss + 0x44C) = max * tier;
                ((Destructibles *)destructibles)->rebuildCanyon2Pillars();
            }
        }
    } else if (m_levelNum == 6 && m_mode == 1) {
        InitPlayerLives();
        if (craterSwitched != 0) {
            char *p;

            numDeadHeads = 0;
            for (p = plantBoss; p < plantBoss + 0x6F0; p += 0x250) {
                if (*(float *)(p + 0x3C) <= 0.0f)
                    *(int *)(p + 0x38) = 0xE;
                *(int *)(p + 0x34) = 1;
                *(float *)(p + 0x3C) = 20.0f;
            }
        } else {
            *(int *)((char *)game + 0x1203E0) = m_numAIs;
            spawnAIs(this, seen, &n, false);
        }
    } else if (m_levelNum == 11 && m_mode == 1) {
        InitPlayerLives();
        if (*(int *)(finalBoss + 0x1C) == 0) {
            ((Destructibles *)destructibles)->rebuildCapitolPillars();
            *(float *)(finalBoss + 0x14) = (float)*(int *)((char *)game + 0x1203CC) * 75.0f + 100.0f;
            *(int *)((char *)game + 0x1203E0) = m_numAIs;
            spawnAIs(this, seen, &n, false);
        } else if (*(int *)(finalBoss + 0x1C) == 1) {
            ((HealthMeter *)(*(char **)(finalBoss + 0x74) + 0x448))->creditFull();
        } else if (*(int *)(finalBoss + 0x1C) == 2) {
            ((HealthMeter *)(*(char **)(finalBoss + 0x78) + 0x448))->creditFull();
        }
    } else {
        *(int *)((char *)game + 0x1203E0) = m_numAIs;
        spawnAIs(this, seen, &n, true);
        if (m_mode == 4 || m_mode == 6) {
            for (i = 0; i < m_numAIs; i++) {
                if (m_mode == 4 || m_mode == 6) {
                    char *m = *(char **)((char *)game + 0x120390 + i * 4);

                    if (i != 0) {
                        *(int *)(m + 0x18) = 0;
                        hierSetCsDrawMe(*(_cs **)(m + 0xC), 0);
                    } else {
                        *(int *)(m + 0x18) = 2;
                        hierSetCsDrawMe(*(_cs **)(m + 0xC), 1);
                    }
                }
            }
            *(int *)((char *)this + 0x2A70) = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitPlayers__5Shell);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitBeforeUiDbLoad__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitBeforeUserintDbLoad__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", __5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", _$_5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitRTState__5Shell);
#ifdef NON_MATCHING
extern int levelMonsters[][12];
extern int levelMonsterModels[][4];
void displayDialog(int which);
extern "C" void snd_StopAllSounds(void);

/* Decides what happens after rtMain returned `r` (2 = dialog dismissed, 5 = quit the session, otherwise the level ended) by asking the game mode's
   own evaluator. Modes: 0 demo (random level and monster), 1 story, 2 challenge, 3 free for all (with or without AIs), 4 endurance, 5 co-op,
   6 elimination, 7 dodgeball, 8 bigshot, 9 crush, 11 online battle. */
void Shell::EvaluateGameStatus(int r)
{
    if (r == 2) {
        *(char *)(*(char **)((char *)game + 0x98) + 0xC) = 0;
        displayDialog(0);
    } else if (r == 5) {
        SH(0x2BCC) = 0;
        SH(0x2BD0) = 0;
        *(int *)((char *)game + 0x1204C0 + 0x109C) = 0;
        snd_StopAllSounds();
        return;
    } else {
        switch (m_mode) {
        case 0: {
            int i, n;

            SH(0x2BCC) = 0;
            SH(0x2BD0) = 1;
            m_levelNum = mathfRand(1, 0xB);
            m_monsterSel[0] = mathfRand(1, 0xC) << 5;
            if (m_monsterSel[0] == 0xC0 || m_monsterSel[0] == 0x180)
                m_monsterSel[0] = 0x20;
            ((TheGame *)game)->SetPlayerMonster(0, m_monsterSel[0], 0, 0, m_costume[0]);
            n = *(int *)((char *)this + 0x29E0 + m_levelNum * 4);
            m_numAIs = n;
            for (i = 0; i < n; i++) {
                m_monsterSel[4 + i] = levelMonsters[m_levelNum][i];
                m_costume[2 + i] = levelMonsterModels[m_levelNum][i];
            }
            DisplayLoadBackground(false);
            break;
        }
        case 1:
            EvaluateOnePlayerStoryStatus(r);
            break;
        case 2:
            EvaluateOnePlayerChallengeStatus(r);
            break;
        case 4:
            EvaluateOnePlayerEnduranceStatus(r);
            break;
        case 5:
            EvaluateTwoPlayerCoopStatus(r);
            break;
        case 7:
            EvaluateDodgeBallStatus(r);
            break;
        case 3:
            if (*(int *)((char *)game + 0x1203E0) != 0) {
                EvaluateMultiPlayerBattleStatusAI(r);
                break;
            }
            /* fall through */
        case 6:
            EvaluateMultiPlayerBattleStatusNoAI(r);
            break;
        case 8:
            EvaluateBigShotStatus(r);
            break;
        case 9:
            EvaluateCrushStatus(r);
            break;
        case 11:
            EvaluateOnlineBattleStatus(r);
            break;
        default:
            printf("ERROR - Shell::EvaluateGameStatus does not recognize %i as a Game Mode!!!!
", m_mode);
            break;
        }
    }
    if (SH(0x2BA8) == 1) {
        DisplayLoadBackground(false);
        SH(0x2BA8) = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateGameStatus__5Shelli);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateOnePlayerStoryStatus__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateOnePlayerChallengeStatus__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateTwoPlayerCoopStatus__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateMultiPlayerBattleStatusNoAI__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateMultiPlayerBattleStatusAI__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateOnePlayerEnduranceStatus__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateBigShotStatus__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateCrushStatus__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateDodgeBallStatus__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateOnlineBattleStatus__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitPlayerLives__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", BootInitUi__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", BootInitUserint__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", BootInitUserint1__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", BootInitGame__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", ResolveCommandLineArguments__FiPPc);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitGS__5Shells);
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadUserintTexture__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadUserintTexture1__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadUserintDB__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadUserintDB1__5Shell);
#ifdef NON_MATCHING
/* 97/279 words: untuned, from the m2c draft + asm */
void Shell::LoadResTexture(void)
{
    char path[0x80];
    int i;
    int j;

    if (m_levelNum == 0 || UseCommandLineLevel != 0)
        formatFilename(path, whichLevel, D_006F81C0, SH_FILE_0);
    else if (m_levelNum < 0x1D)
        formatFilename(path, GameLevelNames_006EF748[m_levelNum], D_006F81C0, SH_FILE_0);
    else
        sprintf(path, "host0:monster.rtx");
    informProgressBar(0.0f);
    char *levelAddr = getNextNgpLoadAddr();
    fileOnlyResFile(levelAddr, zipInflateAll(path, levelAddr));
    gWhichMicroSection++;
    informProgressBar(0.0f);
    if (m_mode == 6) {
        for (i = 1; i < 6; i++) {
            informProgressBar(0.0f);
            formatFilename1(path, MonsterLongNames_006EF860[i], 0, D_006F81C0, SH_FILE_PLAYER);
            int sz = fileReadf(path, getNextResLoadAddr());
            fileAddResFile(getNextResLoadAddr(), sz);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        }
        for (i = 7; i < 12; i++) {
            informProgressBar(0.0f);
            formatFilename1(path, MonsterLongNames_006EF860[i], 0, D_006F81C0, SH_FILE_PLAYER);
            int sz = fileReadf(path, getNextResLoadAddr());
            fileAddResFile(getNextResLoadAddr(), sz);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        }
    } else {
        for (i = 0; i < m_numPlayers; i++) {
            if ((m_monsterSel[i] >> 5) < 0x10) {
                informProgressBar(0.0f);
                if (i == 0)
                    formatFilename1(path, MonsterLongNames_006EF860[m_monsterSel[0] >> 5], m_costume[0], D_006F81C0, SH_FILE_PLAYER);
                else if (i == 1)
                    formatFilename1(path, MonsterLongNames_006EF860[m_monsterSel[1] >> 5], m_costume[1], D_006F81C0, SH_FILE_PLAYER);
                else
                    formatFilename(path, MonsterLongNames_006EF860[m_monsterSel[i] >> 5], D_006F81C0, SH_FILE_PLAYER);
                int sz = fileReadf(path, getNextResLoadAddr());
                fileAddResFile(getNextResLoadAddr(), sz);
                gWhichMicroSection++;
                informProgressBar(0.0f);
            } else {
                printf(D_006EF458, m_monsterSel[i], i);
            }
        }
    }
    for (j = 0; j < m_numAIs; j++) {
        int sel = m_monsterSel[4 + j];

        if ((sel >> 5) < 0x10) {
            informProgressBar(0.0f);
            formatFilename1(path, MonsterLongNames_006EF860[sel >> 5], m_costume[2 + j], D_006F81C0, SH_FILE_AI);
            int sz = fileReadf(path, getNextResLoadAddr());
            fileAddResFile(getNextResLoadAddr(), sz);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        } else {
            printf(D_006EF498, sel, j);
        }
    }
    texmResInit(getVramAddr(), (QwData *)levelAddr);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadResTexture__5Shell);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Shell", D_006EF458);
INCLUDE_ASM("asm/nonmatchings/game/Shell", D_006EF498);
#ifdef NON_MATCHING
/* 11/267 words: untuned, from the m2c draft + asm */
void Shell::LoadTexture(void)
{
    char path[0x80];
    int i;
    int j;

    if (m_levelNum == 0 || UseCommandLineLevel != 0)
        formatFilename(path, whichLevel, D_006F81C8, SH_FILE_0);
    else if (m_levelNum < 0x1D)
        formatFilename(path, GameLevelNames_006EF748[m_levelNum], D_006F81C8, SH_FILE_0);
    else
        sprintf(path, "host0:monster.tex");
    informProgressBar(0.0f);
    int size = zipInflateAll(path, getNextNgpLoadAddr());
    fileOnlyTexFile(getNextNgpLoadAddr(), size);
    gWhichMicroSection++;
    informProgressBar(0.0f);
    if (m_mode == 6) {
        for (i = 1; i < 6; i++) {
            informProgressBar(0.0f);
            formatFilename1(path, MonsterLongNames_006EF860[i], 0, D_006F81C8, SH_FILE_PLAYER);
            int sz = fileReadf(path, getNextTexLoadAddr());
            fileAddTexFile(getNextTexLoadAddr(), sz);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        }
        for (i = 7; i < 12; i++) {
            informProgressBar(0.0f);
            formatFilename1(path, MonsterLongNames_006EF860[i], 0, D_006F81C8, SH_FILE_PLAYER);
            int sz = fileReadf(path, getNextTexLoadAddr());
            fileAddTexFile(getNextTexLoadAddr(), sz);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        }
    } else {
        for (i = 0; i < m_numPlayers; i++) {
            if ((m_monsterSel[i] >> 5) < 0x10) {
                if (i == 0)
                    formatFilename1(path, MonsterLongNames_006EF860[m_monsterSel[0] >> 5], m_costume[0], D_006F81C8, SH_FILE_PLAYER);
                else if (i == 1)
                    formatFilename1(path, MonsterLongNames_006EF860[m_monsterSel[1] >> 5], m_costume[1], D_006F81C8, SH_FILE_PLAYER);
                else
                    formatFilename(path, MonsterLongNames_006EF860[m_monsterSel[i] >> 5], D_006F81C8, SH_FILE_PLAYER);
                int sz = fileReadf(path, getNextTexLoadAddr());
                fileAddTexFile(getNextTexLoadAddr(), sz);
                gWhichMicroSection++;
                informProgressBar(0.0f);
            }
        }
    }
    for (j = 0; j < m_numAIs; j++) {
        int sel = m_monsterSel[4 + j];

        if ((sel >> 5) < 0x10) {
            informProgressBar(0.0f);
            formatFilename1(path, MonsterLongNames_006EF860[sel >> 5], m_costume[2 + j], D_006F81C8, SH_FILE_AI);
            int sz = fileReadf(path, getNextTexLoadAddr());
            fileAddTexFile(getNextTexLoadAddr(), sz);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        } else {
            printf(D_006EF498, sel, j);
        }
    }
    texmInit(getVramAddr());
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadTexture__5Shell);
#endif
#ifdef NON_MATCHING
/* 14/80 words: untuned, written from the m2c draft + asm */
void Shell::LoadLevelDB(void)
{
    char *path = D_00731500;

    if (m_levelNum == 0 || UseCommandLineLevel != 0) {
        formatFilename(D_00731500, whichLevel, D_006F81D0, SH_FILE_0);
    } else if (m_levelNum < 0x1D) {
        formatFilename(D_00731500, GameLevelNames_006EF748[m_levelNum], D_006F81D0, SH_FILE_0);
        strcpy(whichLevel, GameLevelNames_006EF748[m_levelNum]);
    } else {
        sprintf(D_00731500, "host0:monster.ngp");
    }
    informProgressBar(0.0f);
    fileOnlyNgpFile((char *)0xA00000, zipInflateAll(path, (void *)0xA00000));
    fileAddName(fileTrimPath(path));
    gWhichMicroSection++;
    informProgressBar(0.0f);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadLevelDB__5Shell);
#endif
#ifdef NON_MATCHING
/* 17/295 words: untuned, from the m2c draft + asm */
void Shell::LoadMonstersDB(void)
{
    char path[0x40];
    char name[0x80];
    int i;
    int j;

    if (m_mode == 6) {
        for (i = 1; i < 6; i++) {
            informProgressBar(0.0f);
            formatFilename1(path, MonsterLongNames_006EF860[i], 0, D_006F81D0, SH_FILE_PLAYER);
            int size = fileReadf(path, getNextNgpLoadAddr());
            fileAddNgpFile(getNextNgpLoadAddr(), size);
            sprintf(name, D_006F81D8, MonsterLongNames_006EF860[i], 0);
            fileAddName(name);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        }
        for (i = 7; i < 12; i++) {
            informProgressBar(0.0f);
            formatFilename1(path, MonsterLongNames_006EF860[i], 0, D_006F81D0, SH_FILE_PLAYER);
            int size = fileReadf(path, getNextNgpLoadAddr());
            fileAddNgpFile(getNextNgpLoadAddr(), size);
            sprintf(name, D_006F81D8, MonsterLongNames_006EF860[i], 0);
            fileAddName(name);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        }
    } else {
        for (i = 0; i < m_numPlayers; i++) {
            if ((m_monsterSel[i] >> 5) < 0x10) {
                informProgressBar(0.0f);
                if (i == 0) {
                    formatFilename1(path, MonsterLongNames_006EF860[m_monsterSel[0] >> 5], m_costume[0], D_006F81D0, SH_FILE_PLAYER);
                    sprintf(name, D_006F81D8, MonsterLongNames_006EF860[m_monsterSel[0] >> 5], m_costume[0]);
                } else if (i == 1) {
                    formatFilename1(path, MonsterLongNames_006EF860[m_monsterSel[1] >> 5], m_costume[1], D_006F81D0, SH_FILE_PLAYER);
                    sprintf(name, D_006F81D8, MonsterLongNames_006EF860[m_monsterSel[1] >> 5], m_costume[1]);
                } else {
                    /* retail quirk: the path of player 3/4 is built from player 2's monster, the registered name from their own */
                    formatFilename(path, MonsterLongNames_006EF860[m_monsterSel[1] >> 5], D_006F81D0, SH_FILE_PLAYER);
                    sprintf(name, D_006F81A8, MonsterLongNames_006EF860[m_monsterSel[i] >> 5]);
                }
                int size = fileReadf(path, getNextNgpLoadAddr());
                fileAddNgpFile(getNextNgpLoadAddr(), size);
                fileAddName(name);
                gWhichMicroSection++;
                informProgressBar(0.0f);
            } else {
                printf(D_006EF458, m_monsterSel[i], i);
            }
        }
    }
    for (j = 0; j < m_numAIs; j++) {
        int sel = m_monsterSel[4 + j];

        informProgressBar(0.0f);
        if ((sel >> 5) < 0x10) {
            formatFilename1(path, MonsterLongNames_006EF860[sel >> 5], m_costume[2 + j], D_006F81D0, SH_FILE_AI);
            int size = fileReadf(path, getNextNgpLoadAddr());
            fileAddNgpFile(getNextNgpLoadAddr(), size);
            informProgressBar(0.0f);
            sprintf(name, D_006F81D8, MonsterLongNames_006EF860[m_monsterSel[4 + j] >> 5], m_costume[2 + j]);
            fileAddName(name);
            gWhichMicroSection++;
            informProgressBar(0.0f);
        } else {
            printf(D_006EF498, sel, j);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadMonstersDB__5Shell);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Shell", AddEpNode__5ShelliP9_hierhead);
INCLUDE_ASM("asm/nonmatchings/game/Shell", formatFilename__5ShellPcPCcT2Q25Shell11_shFileType);
INCLUDE_ASM("asm/nonmatchings/game/Shell", D_006EF598);
INCLUDE_ASM("asm/nonmatchings/game/Shell", formatFilename1__5ShellPcPCciT2Q25Shell11_shFileType);
INCLUDE_ASM("asm/nonmatchings/game/Shell", formatFilename__5ShellPcPCcN22);
INCLUDE_ASM("asm/nonmatchings/game/Shell", getVramAddr__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", Use30HzMode__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", GetLevelName__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", SetMenuItemFlag__5Shelliii);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EnableMonsterSelection__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", MonsterIsChosen__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", MonsterExists__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", DisplayLoadBackground__5Shellb);
INCLUDE_ASM("asm/nonmatchings/game/Shell", MonsterIsLocked__5Shelli);
INCLUDE_ASM("asm/nonmatchings/game/Shell", init__7TagList);
#ifdef NON_MATCHING
/* 118/148 words: untuned */
void Shell::LoadLevelFiles(void)
{
    sceCdlFILE f;
    char name[0x80];
    int ngpSize;

    viewSetNumViews(1);
    viewCreate(VIEWPORT_7, 0);
    sprintf(name, "%s\\LVL\\%s.NGP;1%s", D_006F81E0, GetLevelName(), D_006F81E8);
    fileCdSearchFile(&f, name);
    ngpSize = f.size;
    sprintf(name, "%s\\LVL\\%s.TEX;1%s", D_006F81E0, GetLevelName(), D_006F81E8);
    fileCdSearchFile(&f, name);
    initProgressBar(m_numAIs + m_numPlayers, ngpSize, f.size);
    informProgressBar(0.0f);
    printf(" =+= Starting     time = %s
", fileGetTimeString());
    LoadLevelDB();
    gWhichMicroSection = 0;
    gWhichMacroSection++;
    informProgressBar(0.0f);
    printf(" =+= Levels Done  time = %s
", fileGetTimeString());
    LoadMonstersDB();
    gWhichMicroSection = 0;
    gWhichMacroSection++;
    informProgressBar(0.0f);
    printf(" =+= Monsters Done    time = %s
", fileGetTimeString());
    LoadResTexture();
    gWhichMicroSection = 0;
    gWhichMacroSection++;
    informProgressBar(0.0f);
    printf(" =+= ResTex Done  time = %s
", fileGetTimeString());
    LoadTexture();
    gWhichMicroSection = 0;
    gWhichMacroSection++;
    informProgressBar(0.0f);
    printf(" =+= Texture Done time = %s
", fileGetTimeString());
    dbsRelocateFileZero(getVramAddr(), UseCommandLineLevel);
    dbInitDb((_dbheader *)0xA00000, getVramAddr());
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Shell", LoadLevelFiles__5Shell);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Shell", FinishLoadBar__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", ResetLevel__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", GenesisMovie__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", FadeScreen__5ShellibUcUcUcUcUcUc);
INCLUDE_ASM("asm/nonmatchings/game/Shell", FadeScreen__5ShellibRUiT3UcUcUcUcUcUc);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitialMemCardScreen__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", __3Hud);
INCLUDE_ASM("asm/nonmatchings/game/Shell", __7TheGame);
INCLUDE_ASM("asm/nonmatchings/game/Shell", __static_initialization_and_destruction_0_001AC240);
INCLUDE_ASM("asm/nonmatchings/game/Shell", __7TagList);
INCLUDE_ASM("asm/nonmatchings/game/Shell", _GLOBAL_$I$g_noScripts);
INCLUDE_ASM("asm/nonmatchings/game/Shell", _GLOBAL_$D$g_noScripts);
INCLUDE_ASM("asm/nonmatchings/game/Shell", GameLevelNames_006EF748);
INCLUDE_ASM("asm/nonmatchings/game/Shell", MonsterLongNames_006EF860);
INCLUDE_ASM("asm/nonmatchings/game/Shell", on_bit_006EF8E8);
INCLUDE_ASM("asm/nonmatchings/game/Shell", off_bit);

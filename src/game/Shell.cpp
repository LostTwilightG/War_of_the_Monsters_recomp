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


INCLUDE_ASM("asm/nonmatchings/game/Shell", main);
INCLUDE_ASM("asm/nonmatchings/game/Shell", SelectAI__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", D_006EF180);
INCLUDE_ASM("asm/nonmatchings/game/Shell", RandomlySelectAI__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitPlayers__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitBeforeUiDbLoad__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitBeforeUserintDbLoad__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", __5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", _$_5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", InitRTState__5Shell);
INCLUDE_ASM("asm/nonmatchings/game/Shell", EvaluateGameStatus__5Shelli);
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

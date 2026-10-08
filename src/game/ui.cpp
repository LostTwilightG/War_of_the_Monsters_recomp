#include "common.h"
#include "game/shell.h"

extern "C" int printf(const char *, ...);
extern "C" void snd_StopAllSounds(void);

struct UserintReturn {
    int pending;  /* set by userintReturnToShell */
    int frames;   /* frames left before the menu really returns */
    int result;   /* what the shell is asked to do next */
};
extern UserintReturn gUserintReturn;
extern int pMode;
extern int continueDecoding;
extern int oldScreens[16];
extern int clutterDelay;
extern int clutterFrame;
extern int testPattern;
extern int whichPlayerIsPicking;
extern int incogTimer;
extern int currentMonster;
extern int lastMonster;
extern int currentLevel;
extern int uiWhichPad;
extern int clutterDelayCnt;
extern int chosenAI[10];
extern int AItoChoose;
extern int movieEnable;
extern int select;
extern int goLoadLevel;
extern int exitUi;
extern int gManualMonsterChangeHappened;
extern int gRealWorldEpNode;
extern int lastTicCount;
extern int thisTicCount;
extern int startTime;
extern int frameTime;
extern int movieTime;
extern int innerFrameTime;
extern int movieRequested;
extern int movieIsCompletelyFinished;
extern int nextMovie;
extern int nextScreen;
extern int gIntroMoviesHavePlayed;
extern int D_006F8D08;
extern int movieHasStarted_2745 __asm__("movieHasStarted.2745");
extern int movieHasStarted_2749 __asm__("movieHasStarted.2749");

INCLUDE_ASM("asm/nonmatchings/game/ui", uiMain__Fi);
INCLUDE_ASM("asm/nonmatchings/game/ui", D_006F1C90);
INCLUDE_ASM("asm/nonmatchings/game/ui", userintMain__Fv);
#ifdef NON_MATCHING
/* 2/12 words: store order */
void userintReturnToShell(int result, int frames, int extra)
{
    if (gUserintReturn.pending == 0) {
        gUserintReturn.frames = frames + (extra & 1);
        gUserintReturn.pending = 1;
        gUserintReturn.result = result;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/ui", userintReturnToShell__Fiii);
#endif
#ifdef NON_MATCHING
/* 5/12 words: branch layout */
int userintTimeToReturnToShell(void)
{
    if (gUserintReturn.pending == 0 || --gUserintReturn.frames >= 0)
        return 0;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/ui", userintTimeToReturnToShell__Fv);
#endif
INCLUDE_ASM("asm/nonmatchings/game/ui", uiFadeScreen__FUi);
INCLUDE_ASM("asm/nonmatchings/game/ui", uiInitScreens__FP3_cs);
#ifdef NON_MATCHING
/* 7/36 words: branch layout */
void uiIntro(void)
{
    if (movieRequested != 0)
        movieHasStarted_2745 = 1;
    if (movieHasStarted_2745 != 0 && movieIsCompletelyFinished != 0) {
        if (nextMovie == 0) {
            nextScreen = 0;
            select = 1;
            snd_StopAllSounds();
            exitUi = 1;
            movieHasStarted_2745 = 0;
            gIntroMoviesHavePlayed = 1;
            return;
        }
        movieHasStarted_2745 = 0;
        movieEnable = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/ui", uiIntro__Fv);
#endif
#ifdef NON_MATCHING
/* 7/36 words: branch layout */
void uiOutro(void)
{
    if (movieRequested != 0)
        movieHasStarted_2749 = 1;
    if (movieHasStarted_2749 != 0 && movieIsCompletelyFinished != 0) {
        if (nextMovie == 0) {
            nextScreen = 0;
            select = 1;
            snd_StopAllSounds();
            exitUi = 1;
            movieHasStarted_2749 = 0;
            gIntroMoviesHavePlayed = 1;
            return;
        }
        movieHasStarted_2749 = 0;
        movieEnable = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/ui", uiOutro__Fv);
#endif
INCLUDE_ASM("asm/nonmatchings/game/ui", playMovie__Fv);
/* Moves currentMonster (wrapping 0..15) by `step` until it names a monster that exists and nobody has chosen yet. */
#ifdef NON_MATCHING
/* 14/56 words: loop shape */
void uiValidateMonster(int step)
{
    int ok = 0;

    do {
        if (currentMonster < 0)
            currentMonster = 0xF;
        if (currentMonster >= 0x10)
            currentMonster = 0;
        if (shell->MonsterExists(currentMonster) != 0 && shell->MonsterIsChosen(currentMonster) == 0) {
            ok = 1;
        } else {
            if (shell->MonsterIsChosen(currentMonster) != 0)
                gManualMonsterChangeHappened = 1;
            currentMonster += step;
            if (step == 0)
                printf("Big fuckup!! we need to go somewhere, not sit here going nowhere.\n");
        }
    } while (ok == 0);
    D_006F8D08 = currentMonster;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/ui", uiValidateMonster__Fi);
#endif
INCLUDE_ASM("asm/nonmatchings/game/ui", uiAlignItemWithCamera__FP3_csfff);
INCLUDE_ASM("asm/nonmatchings/game/ui", letterboxDmaData__Ff);
INCLUDE_ASM("asm/nonmatchings/game/ui", blackenScreen__Fv);
#ifdef NON_MATCHING
/* 4/78 words: retail clears the arrays with pointer loops */
void uiInit(void)
{
    int i;

    pMode = 1;
    continueDecoding = 1;
    for (i = 15; i >= 0; i--)
        oldScreens[i] = 0;
    clutterDelay = 2;
    clutterFrame = -1;
    testPattern = 1;
    whichPlayerIsPicking = 0;
    incogTimer = 0;
    currentMonster = 0;
    lastMonster = 0;
    currentLevel = 1;
    uiWhichPad = -1;
    clutterDelayCnt = 0;
    for (i = 9; i >= 0; i--)
        chosenAI[i] = 0;
    AItoChoose = 0;
    movieEnable = 1;
    select = 0;
    goLoadLevel = 0;
    exitUi = 0;
    gManualMonsterChangeHappened = 0;
    gRealWorldEpNode = 0;
    lastTicCount = 0;
    thisTicCount = 0;
    startTime = 0;
    frameTime = 0;
    movieTime = 0;
    innerFrameTime = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/ui", uiInit__Fv);
#endif
INCLUDE_ASM("asm/nonmatchings/game/ui", __static_initialization_and_destruction_0_001DAE40);
INCLUDE_ASM("asm/nonmatchings/game/ui", _GLOBAL_$I$myMovie);
INCLUDE_ASM("asm/nonmatchings/game/ui", GameLevelNames_006F2208);

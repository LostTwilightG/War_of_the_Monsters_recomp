#include "common.h"

/* Request to leave the realtime loop: `active` is set by rtReturnToShell, `delay` frames later rtTimeToReturnToShell reports it. */
struct RtReturn {
    int active;
    int delay;
    int code;
};
extern RtReturn gRtReturn;
extern volatile int objsInPacket;
extern volatile int objsInAlphaPacket;
class RtLoopView {
public:
    static void startFrame(void) __asm__("startFrame__10RtLoopViewv");
    static void start(int sync);
    static void end(unsigned color, int sync);
};
extern "C" int sceGsSyncPath(int mode, int timeout);
void inputStopActuator(int pad, unsigned char which);
class TheGame;
extern TheGame *game;
int rtTimeToReturnToShell(void);
void rtReturnToShell(int code, int delay);

void RtLoopView::startFrame(void)
{
}
void RtLoopView::start(int sync)
{
}
void RtLoopView::end(unsigned color, int sync)
{
}
#ifdef NON_MATCHING
/* Callees are bound to their retail symbols by asm label, so each can be declared with the C-style signature this code needs. */
#define SYM(n) __asm__(n)
extern "C" int FlushCache(int mode);
extern "C" int sceGsSyncV(int mode);
extern "C" void hierSetCamera(int a, int b, int c);
extern "C" void snd_FlushSoundCommands(void);
extern "C" int printf(const char *, ...);

void rtLoopStartFrame(void) SYM("startFrame__10RtLoopViewv");
void rtPauseRT(unsigned frame);
void rtFadeScreen(int view, unsigned amount);

void debrisCullView(int view) SYM("CullView__6Debrisi");
void gsPktDma(void *gsPkt) SYM("DmaGsPktQueue__5GsPkt");
void gsPktFrameInit(void *gsPkt, int view, int parity) SYM("frameInit__5GsPktii");
void gameSetOkToUnify(void *g) SYM("SetOkToUnify__7TheGame");
void gameUpdate(void *g) SYM("Update__7TheGame");
void gameUpdate2(void *g) SYM("Update2__7TheGame");
void hudAddHealthIndicators(void *hud) SYM("addHealthIndicators__3Hud");
void hudBuildSeparator(void *hud) SYM("buildSeparator__3Hud");
void hudPrint(void *hud) SYM("print__3Hud");
void hudUpdate(void *hud) SYM("update__3Hud");
void animationRunGlobal(void);
void screenPolysDmaData(void) SYM("dmaData__11ScreenPolysv");
void motionBlurDrawOverlay(int parity, int field, int view) SYM("drawOverlay__12MotionBlurAAiii");
int motionBlurGetEnable(void) SYM("getEnable__12MotionBlurAA");
void motionBlurInit(int views, float a, float b) SYM("init__12MotionBlurAAiff");
void levelPickupsEnableHighlights(int view) SYM("enableHighlights__12LevelPickupsi");
void monsterSetReticles(int view) SYM("setReticles__7Monsteri");
void fontDmaFontData(void);
void hier(int view, int pass);
int ieGs2CircuitOnOff(void) SYM("ieGs2CircuitOnOff__24IncognitoEntertainmentGSv");
int ieGsHalfTexelOffsetOnOff(void) SYM("ieGsHalfTexelOffsetOnOff__24IncognitoEntertainmentGSv");
void ieGsPutDrawEnv(void *db, int parity, int zero) SYM("ieGsPutDrawEnv__24IncognitoEntertainmentGSPQ224IncognitoEntertainmentGS11ieGsDBuffDcii");
void ieGsSetHalfOffsetDc(int view, void *db, unsigned frame, int field, int numViews) SYM("ieGsSetHalfOffsetDc__24IncognitoEntertainmentGSiPQ224IncognitoEntertainmentGS11ieGsDBuffDcUiii");
void ieGsSwapDBuffDc(void *db, int parity, int circuit, int one) SYM("ieGsSwapDBuffDc__24IncognitoEntertainmentGSPQ224IncognitoEntertainmentGS11ieGsDBuffDciii");
int inputAnyKey(int pad);
void inputUpdate(void);
void particleClearBuffers(void);
void particleDraw(int view);
void particlePrepDraw(void);
void streamingResetMasterSoundVolume(void *m) SYM("resetMasterSoundVolume__21StreamingSoundManager");
void texmPrepTexInfoZero(void *fb) SYM("texmPrepTexInfoZero__FP11tGS_DISPFB2");
void timer1Handler(unsigned *p);
void timerEndOfRealTimeProcessing(void);
void timerGetCurTics(void);
void timerSetFrameStart(void);
void timerWaitForMinUpdateRate(unsigned *p);
void levelObjectSoundUpdate(void *m) SYM("updateLevelObjectSoundManager__23LevelObjectSoundManager");
void soundManagerUpdate(void *m) SYM("updateSoundManager__12SoundManager");
void taskManagerUpdate(void *m) SYM("update__11TaskManager");
void dodgeBallUpdate(void *m, int view) SYM("update__14DodgeBallLeveli");
void *viewGetDb(int view);
int viewGetNumViews(void);
void viewGrappleConfig(void);
void viewSetOddEven(int odd);
void viewUpdate(int view);

extern int tweakLastFrame;
extern int grappleLastFrame;
extern unsigned g_frame;
extern int gUseUnifiedView;
extern int moviePlaying;
extern int doTweaks;
extern int craterTransitionInProgress;
extern int g_noScripts;
extern char gsPkt[];
class Shell;
extern Shell *shell;
extern char bigShotInstance[] SYM("_12BigShotLevel$instance");
extern char dodgeBallInstance[] SYM("_14DodgeBallLevel$instance");
extern char debrisTaskManager[] SYM("_11TaskManager$debris");

#define GM(o) (*(int *)((char *)game + (o)))
#define HUD(i) ((char *)game + (i) * 0x2E0)

/* The realtime loop of a play session, one iteration per frame: poll input, then for every view (one, or two for split screen) run the
   game update (view 0 only), draw the world (hier), particles and HUD into the double buffer, and DMA the frame; at the end of the frame the sound
   managers run and the frame is paced. Returns the code given to rtReturnToShell. `first` fades the first frames in from black. */
int rtMain(bool first)
{
    unsigned oddEven;
    int numViews, view, next;
    void *db;
    int parity;

    tweakLastFrame = -1;
    if (grappleLastFrame == 0)
        motionBlurInit(viewGetNumViews(), 40.0f, 0.0f);
    gRtReturn.active = 0;
    GM(0x120464) = 0;
    g_frame = 0;
    oddEven = sceGsSyncV(0) == 0;
    *(volatile int *)0x10000800 = 0;
    timerSetFrameStart();
    while (rtTimeToReturnToShell() == 0) {
        rtLoopStartFrame();
        viewSetOddEven(oddEven);
        numViews = viewGetNumViews();
        RtLoopView::start(0);
        inputUpdate();
        if (inputAnyKey(0) == 0)
            inputAnyKey(1);
        snd_FlushSoundCommands();
        RtLoopView::end(0x2020FF80, 0);
        if (g_frame & 1)
            texmPrepTexInfoZero((char *)viewGetDb(0) + 0x10);
        else
            texmPrepTexInfoZero((char *)viewGetDb(0) + 0x50);
        gameSetOkToUnify(game);
        view = 0;
        if (gUseUnifiedView != 0) {
            if (grappleLastFrame == 0) {
                viewGrappleConfig();
                motionBlurInit(numViews, 40.0f, 0.0f);
                grappleLastFrame = 1;
            }
        } else if (grappleLastFrame != 0) {
            do {
            } while ((int)oddEven != (sceGsSyncV(0) == 0));
            viewGrappleConfig();
            motionBlurInit(numViews, 40.0f, 0.0f);
            grappleLastFrame = 0;
        }
        if (numViews > 0) {
            do {
                next = view + 1;
                gsPktFrameInit(gsPkt, view, (g_frame ^ 1) & 1);
                debrisCullView(view);
                if (moviePlaying == 0) {
                    int both = 0;

                    if (gUseUnifiedView != 0 && GM(0x1203D8) == 2) {
                        both = 1;
                    } else if (GM(0x1203C8) == 9) {
                        both = 1;
                    } else if (GM(0x1203C8) == 8) {
                        monsterSetReticles(*(int *)(bigShotInstance + 0x94));
                    } else {
                        monsterSetReticles(view);
                        levelPickupsEnableHighlights(view);
                    }
                    if (both) {
                        monsterSetReticles(view);
                        next = view + 1;
                        monsterSetReticles(view + 1);
                        levelPickupsEnableHighlights(0);
                        levelPickupsEnableHighlights(1);
                    }
                }
                RtLoopView::start(1);
                FlushCache(0);
                sceGsSyncPath(0, 0);
                db = viewGetDb(view);
                parity = g_frame & 1;
                ieGsSwapDBuffDc(db, parity, ieGs2CircuitOnOff(), 1);
                sceGsSyncPath(0, 0);
                RtLoopView::end(0xFF202080, 1);
                if (ieGsHalfTexelOffsetOnOff() != 0)
                    ieGsSetHalfOffsetDc(view, db, g_frame, oddEven, GM(0x1203D8));
                else
                    ieGsSetHalfOffsetDc(view, db, g_frame, 0, GM(0x1203D8));
                viewUpdate(view);
                if (gUseUnifiedView != 0 && GM(0x1203D8) == 2) {
                    hudUpdate(HUD(view));
                    hudUpdate(HUD(next));
                    hudAddHealthIndicators(HUD(view));
                    hudAddHealthIndicators(HUD(next));
                } else {
                    hudUpdate(HUD(view));
                    hudAddHealthIndicators(HUD(view));
                    if (GM(0x1203C8) == 7)
                        dodgeBallUpdate(dodgeBallInstance, view);
                }
                RtLoopView::start(0);
                hier(view, 0);
                RtLoopView::end(0xFFFF2080, 0);
                if (doTweaks == 0 && motionBlurGetEnable() == 0 && g_frame >= 0x1F && craterTransitionInProgress == 0)
                    rtPauseRT(g_frame);
                if (view == 0) {
                    RtLoopView::start(0);
                    gameUpdate(game);
                    RtLoopView::end(0x20FF2080, 0);
                    timerGetCurTics();
                    RtLoopView::start(0);
                    animationRunGlobal();
                    RtLoopView::end(0xFFFF2080, 0);
                    if (g_noScripts == 0)
                        hierSetCamera(0, 0, 0);
                }
                if (view == 1 || numViews == 1) {
                    RtLoopView::start(0);
                    gameUpdate2(game);
                    RtLoopView::end(0x20FF2080, 0);
                    RtLoopView::start(0);
                    particlePrepDraw();
                    particleClearBuffers();
                    RtLoopView::end(0x2020FF80, 0);
                }
                timerGetCurTics();
                RtLoopView::start(0);
                particleDraw(view);
                RtLoopView::end(0x20FF2080, 0);
                if (view == 0) {
                    RtLoopView::start(0);
                    taskManagerUpdate(debrisTaskManager);
                    RtLoopView::end(0x2020FF80, 0);
                }
                RtLoopView::start(1);
                sceGsSyncPath(0, 0);
                RtLoopView::end(0x20FF2080, 1);
                RtLoopView::start(0);
                ieGsPutDrawEnv(db, (g_frame ^ 1) & 1, 0);
                if (view == 0)
                    hudBuildSeparator(game);
                hier(view, 1);
                RtLoopView::end(0xFFFF2080, 0);
                sceGsSyncPath(0, 0);
                screenPolysDmaData();
                if (gUseUnifiedView != 0 && GM(0x1203D8) == 2) {
                    hudPrint(HUD(view));
                    hudPrint(HUD(next));
                } else {
                    hudPrint(HUD(view));
                }
                fontDmaFontData();
                timerGetCurTics();
                RtLoopView::start(1);
                sceGsSyncPath(0, 0);
                RtLoopView::end(0xFFFF2080, 1);
                RtLoopView::start(0);
                motionBlurDrawOverlay(g_frame & 1, oddEven, view);
                RtLoopView::end(0x2020FF80, 0);
                if (g_frame < 0x41 && first)
                    rtFadeScreen(view, g_frame * 2);
                sceGsSyncPath(0, 0);
                view = next;
                gsPktDma(gsPkt);
                snd_FlushSoundCommands();
            } while (view < numViews);
        }
        RtLoopView::start(1);
        sceGsSyncPath(0, 0);
        RtLoopView::end(0xFF202080, 1);
        levelObjectSoundUpdate((char *)game + 0x121570);
        soundManagerUpdate((char *)shell + 0x2C30);
        g_frame++;
        RtLoopView::start(0);
        timerEndOfRealTimeProcessing();
        timer1Handler(&oddEven);
        timerWaitForMinUpdateRate(&oddEven);
        RtLoopView::end(0x08080880, 0);
    }
    viewGrappleConfig();
    gRtReturn.active = 0;
    streamingResetMasterSoundVolume((char *)game + 0x1204C0);
    printf("Leaving rtMain(%d)\n", gRtReturn.code);
    return gRtReturn.code;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/rt", rtMain__Fb);
#endif
#ifdef NON_MATCHING
void fontSetSize(int a, int b);
void fontSetColor(int a, int r, int g, int b, int c);
void fontSpritePrintCenteredXY(int a, int x, int y, char *text);
int inputGetInput(int mask, int pad);
int inputIsCtlAvailable(int pad);
extern int gPlayerThatPaused;

/* Pause handling, called every frame once the game is past its first 30 frames: START (mask 8) on any living player's pad, or an unplugged
   controller, asks the shell to leave the loop with code 2 (the dialog). The delay handed over is the GS field bit (CSR bit 13). In bigshot
   only the player whose turn it is (bigShotInstance+0x94) can pause. While a movie plays the unplugged warning is drawn instead. */
void rtPauseRT(unsigned frame)
{
    int pressed = 0;
    int unplugged = 0;
    int i;

    if (GM(0x1203C8) == 8 && *(int *)(bigShotInstance + 0xE8) == 0) {
        int turn = *(int *)(bigShotInstance + 0x94);

        if (*(char *)(*(char **)((char *)game + 0x120380 + turn * 4) + 0xE8) == 0 && inputGetInput(8, turn) != 0) {
            gPlayerThatPaused = turn;
            pressed = 1;
        }
        for (i = 0; i < 2; i++) {
            if (inputIsCtlAvailable(i) == 0) {
                gPlayerThatPaused = i;
                unplugged = 1;
            }
        }
    } else {
        for (i = 0; i < GM(0x1203D8); i++) {
            if (*(char *)(*(char **)((char *)game + 0x120380 + i * 4) + 0xE8) == 0 && inputGetInput(8, i) != 0) {
                gPlayerThatPaused = i;
                pressed = 1;
            }
            if (inputIsCtlAvailable(i) == 0) {
                gPlayerThatPaused = i;
                unplugged = 1;
            }
        }
    }
    if (pressed != 0 && *(int *)((char *)shell + 0x2BA4) == 0 && moviePlaying == 0 && GM(0x120458) == 0 && GM(0x120454) == 0) {
        *(int *)((char *)shell + 0x2BA4) = 1;
        rtReturnToShell(2, (int)((*(volatile unsigned long long *)0x12001000 >> 13) & 1));
    }
    if (unplugged != 0) {
        if (moviePlaying == 0) {
            rtReturnToShell(2, (int)((*(volatile unsigned long long *)0x12001000 >> 13) & 1));
            return;
        }
        if ((frame & 0x7F) < 0x60) {
            fontSetSize(0, 0x12);
            fontSetColor(0, 0xFF, 0x3C, 0x3C, 0);
            fontSpritePrintCenteredXY(0, 0x140, 0xC8, "CONTROLLER UNPLUGGED");
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/rt", rtPauseRT__FUi);
#endif
void rtReturnToShell(int code, int delay)
{
    if (gRtReturn.active == 0) {
        gRtReturn.code = code;
        gRtReturn.active = 1;
        gRtReturn.delay = delay;
    }
}
/* Counts the delay down; when it runs out, stops the rumble of every pad and reports that it is time. */
int rtTimeToReturnToShell(void)
{
    if (gRtReturn.active != 0) {
        gRtReturn.delay--;
        if (gRtReturn.delay < 0) {
            int i;

            for (i = 0; i < *(int *)((char *)game + 0x1203D8); i++)
                inputStopActuator(i, 2);
            return 1;
        }
    }
    return 0;
}
INCLUDE_ASM("asm/nonmatchings/game/rt", rtFadeScreen__FiUi);
void rtWaitForVu1(void)
{
    if ((objsInPacket | objsInAlphaPacket) != 0) {
        do {
        } while ((objsInPacket | objsInAlphaPacket) != 0);
    }
    sceGsSyncPath(0, 0);
}

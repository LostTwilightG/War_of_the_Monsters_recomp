#include "common.h"
#include "task_manager.h"

struct _cs;
class Monster;
struct Camera {
    enum CameraPOV { POV_0, POV_1, POV_2, POV_3 };
};
/* Task callbacks (Pv) forward the task argument as `this` of the real member. */
class Cameras {
public:
    void CCFadeIn(void);
    void CCFadeOut(void);
    void LeaveUnifiedTask(void);
    static void CCFadeIn(void *p);
    static void CCFadeOut(void *p);
    static void LeaveUnifiedTask(void *p);
    static _cs *GetCsCameraFollows(int view);
    static void SetCameraPOV(int view, Camera::CameraPOV pov);
    static void ReattachCamera(int view);
    static void ResetPOVChange(int view, float t);
    static void SetCameraMonster(int view, Monster *m);
    static void StartShake(float f);
    static void ShakeFalloff(float f);
    static void TogglePOV(int view);
    static void InitCrushMonsters(Monster *a, Monster *b);
};
extern char camerasObj[] __asm__("_7Cameras$m_cameras");
extern int camerasNum __asm__("_7Cameras$m_numCameras");
extern int leaveTriggered __asm__("_7Cameras$m_leaveTriggered");
#define CAM(v) (camerasObj + (v) * 0xEB0)
#define CI(c, o) (*(int *)((c) + (o)))
#define CF(c, o) (*(float *)((c) + (o)))

INCLUDE_ASM("asm/nonmatchings/game/Cameras", _6Camera$UPDATE_FUNK);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", __as__11PitchChangeR11PitchChange);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", __as__9ShakeDataR9ShakeData);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", __as__17MonsterCameraDataR17MonsterCameraData);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", GetUnifiedTime__7Cameras);
#ifdef NON_MATCHING
/* 7/8 words: address arithmetic order */
_cs *Cameras::GetCsCameraFollows(int view)
{
    return *(_cs **)(*(char **)(camerasObj + view * 0xEB0 + 0xCF4) + 0xC);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Cameras", GetCsCameraFollows__7Camerasi);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Cameras", Init__7Cameras);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", SetCameraToFollowMonster__7CamerasiP7Monster);
#ifdef NON_MATCHING
void Cameras::SetCameraMonster(int view, Monster *m)
{
    leaveTriggered = 0;
    *(Monster **)(CAM(view) + 0xCF4) = m;
    TaskManager::global.remove(*(void **)(CAM(view) + 0xE68));
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Cameras", SetCameraMonster__7CamerasiP7Monster);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Cameras", SetCameraPOV__7CamerasiQ26Camera9CameraPOV);
/* Starts a POV transition lasting `t` ticks: weight 1, per-tick steps for the two interpolated values. */
void Cameras::ResetPOVChange(int view, float t)
{
    char *c = CAM(view);

    CF(c, 0xE5C) = 1.0f;
    CF(c, 0xE44) = t;
    CF(c, 0xE40) = 1.0f / t;
    CF(c, 0xE38) = CF(c, 0xDC4) / t;
    CF(c, 0xE3C) = CF(c, 0xDCC) / t;
}
INCLUDE_ASM("asm/nonmatchings/game/Cameras", AlignCsWithCamera__7CamerasiP3_csfff);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", AlignCsWithView__7CamerasiP3_csfff);
/* Flips the camera of `view` between POV 0 and POV 3. */
void Cameras::TogglePOV(int view)
{
    char *c = CAM(view);

    CI(c, 0xD98) = 1;
    if (CI(c, 0xCF8) == 0)
        CI(c, 0xCF8) = 3;
    else
        CI(c, 0xCF8) = 0;
    SetCameraPOV(view, (Camera::CameraPOV)CI(c, 0xCF8));
}
INCLUDE_ASM("asm/nonmatchings/game/Cameras", DetachCamera__7CamerasiP8_fvector);
void Cameras::ReattachCamera(int view)
{
    char *c = CAM(view);

    SetCameraPOV(view, (Camera::CameraPOV)CI(c, 0xCFC));
    CI(c, 0xD98) = 1;
}
INCLUDE_ASM("asm/nonmatchings/game/Cameras", StartShake__7CamerasR8_fvectorf);
#ifdef NON_MATCHING
void Cameras::StartShake(float f)
{
    int i;

    for (i = 0; i < camerasNum; i++)
        CF(CAM(i), 0xD88) = f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Cameras", StartShake__7Camerasf);
#endif
#ifdef NON_MATCHING
void Cameras::ShakeFalloff(float f)
{
    int i;

    for (i = 0; i < camerasNum; i++)
        CF(CAM(i), 0xD8C) = f;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Cameras", ShakeFalloff__7Camerasf);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Cameras", InitCCCamera__7CamerasP7MonsterT1);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", InitCCCameraFromThrow__7CamerasP7MonsterT1);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", InitCCMonsters__7CamerasP7MonsterT1);
#ifdef NON_MATCHING
/* 2/9 words: store order */
void Cameras::InitCrushMonsters(Monster *a, Monster *b)
{
    *(Monster **)(camerasObj + 0xE10) = b;
    *(Monster **)(camerasObj + 0xE0C) = a;
    *(Monster **)(camerasObj + 0xCF4) = a;
    *(int *)((char *)a + 0x6728) = 1;
    *(int *)((char *)b + 0x6728) = 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Cameras", InitCrushMonsters__7CamerasP7MonsterT1);
#endif
INCLUDE_ASM("asm/nonmatchings/game/Cameras", CCFadeIn__7Cameras);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", CCFadeOut__7Cameras);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", Update__7Cameras);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", LeaveUnifiedView__7CamerasUi);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", LeaveUnifiedTask__7Cameras);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", ccMonsNoLongerInRange__7Camerasf);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", ccMonsMovingTogether__7Cameras);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", InitPlantBossCamera__7Cameras);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", closeCombatUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", roamUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", staticUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", getupUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", knockbackUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", topdownUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", victoryUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", crushUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", normalUpdate__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", checkCollision__6Camerai);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", checkNewPosition__6Camera);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", posInFOV__6CameraR8_fvectorib);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", translateCamera__6CameraR8_fvectorfb);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", shakeCamera__6Camera);
void Cameras::CCFadeIn(void *p)
{
    ((Cameras *)p)->CCFadeIn();
}
void Cameras::CCFadeOut(void *p)
{
    ((Cameras *)p)->CCFadeOut();
}
void Cameras::LeaveUnifiedTask(void *p)
{
    ((Cameras *)p)->LeaveUnifiedTask();
}

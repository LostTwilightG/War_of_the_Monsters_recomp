#include "common.h"

struct _cs;
class Monster;
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
    static void InitCrushMonsters(Monster *a, Monster *b);
};
extern char camerasObj[] __asm__("_7Cameras$m_cameras");

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
INCLUDE_ASM("asm/nonmatchings/game/Cameras", SetCameraMonster__7CamerasiP7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", SetCameraPOV__7CamerasiQ26Camera9CameraPOV);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", ResetPOVChange__7Camerasif);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", AlignCsWithCamera__7CamerasiP3_csfff);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", AlignCsWithView__7CamerasiP3_csfff);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", TogglePOV__7Camerasi);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", DetachCamera__7CamerasiP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", ReattachCamera__7Camerasi);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", StartShake__7CamerasR8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", StartShake__7Camerasf);
INCLUDE_ASM("asm/nonmatchings/game/Cameras", ShakeFalloff__7Camerasf);
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

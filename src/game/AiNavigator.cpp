#include "common.h"
#include "hieri_types.h"

class Monster;
class AiPath;
class AiPathNet;
struct _hdResult;

extern AiPathNet *s_net;
__asm__("#SNFIX_SMALL s_net");

class AiPathFinder {
public:
    char pad[0x28];
    AiPathFinder(AiPathNet &net, Monster &m);
};

/* Steering/pathing helper of an AI monster. Members are reached by retail offset; sizeof is not known yet (PathInfo is the last member). */
class AiNavigator {
public:
    enum Status { STATUS_0, STATUS_1, STATUS_2, STATUS_3 };
    enum FailureHint { HINT_0 };

    /* Four probes in front of the monster: obstacle (0x10), second obstacle (0x50), floor (0x80), ceiling (0xC0). */
    class ObstacleSensor {
    public:
        AiNavigator *nav;       /* 0x00 */
        char pad4[0x10 - 4];
        char obstacleHit;       /* 0x10 */
        char pad11[0x28 - 0x11];
        float obstacleHeight;   /* 0x28 */
        char pad2C[0x40 - 0x2C];
        float obstacleRange;    /* 0x40 */
        char pad44[0x50 - 0x44];
        char obstacle2Hit;      /* 0x50 */
        char pad51[0x68 - 0x51];
        float obstacle2Height;  /* 0x68 */
        char pad6C[0x80 - 0x6C];
        char floorHit;          /* 0x80 */
        char pad81[0x98 - 0x81];
        float floorHeight;      /* 0x98 */
        char pad9C[0xC0 - 0x9C];
        char ceilHit;           /* 0xC0 */
        char padC1[0xD8 - 0xC1];
        float ceilHeight;       /* 0xD8 */
        char padDC[0x120 - 0xDC];
        float param120;         /* 0x120: 35.0 */
        float param124;         /* 0x124: 0.32 */
        float param128;         /* 0x128: 45.0 */
        float param12C;         /* 0x12C: 100.0 */

        ObstacleSensor(AiNavigator &n);
        void init(void);
        float getObstacleRange(void);
        float getObstacleHeight(void);
        _hdResult *getObstacle(void);
        float getCeilHeight(void);
        float getFloorHeight(void);
    };

    Monster *monster;           /* 0x000 */
    float f4;                   /* 0x004 */
    float f8;                   /* 0x008 */
    char padC[0x18 - 0xC];
    int mode;                   /* 0x018: 0 off, 1 wander, 3 target, 6 flee ... */
    int status;                 /* 0x01C */
    int failureHint;            /* 0x020 */
    char pad24[0x30 - 0x24];
    ObstacleSensor sensor;      /* 0x030 */
    AiPathFinder pathFinder;    /* 0x160 */
    _fvector *goal;             /* 0x188: where wander/target/flee steer */
    int f18C;                   /* 0x18C */
    int f190;                   /* 0x190 */
    float f194;                 /* 0x194 */
    float f198;                 /* 0x198 */
    float f19C;                 /* 0x19C */
    char pad1A0[0x218 - 0x1A0];
    int f218;                   /* 0x218 */
    int f21C;                   /* 0x21C */

    void wander(_fvector &p);
    void target(_fvector &p, float r);
    void flee(_fvector &p, float r);
    void disable(Status s, FailureHint h);
    void updateArrive(void);
    void updateSeek(void);
    AiPath *getFleePath(_fvector &p);
};

INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", __11AiNavigatorR7Monster);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", init__11AiNavigator);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", update__11AiNavigatorR7GamePad);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", updateSteering__11AiNavigatorR7GamePad);
void AiNavigator::disable(Status s, FailureHint h)
{
    f218 = 0;
    mode = 0;
    f21C = 0;
    failureHint = h;
    status = s;
}
void AiNavigator::wander(_fvector &p)
{
    goal = &p;
    mode = 1;
    status = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", seek__11AiNavigatorR8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", seek__11AiNavigatorR13DbInteractivef);
void AiNavigator::target(_fvector &p, float r)
{
    status = 0;
    f18C = 0;
    f19C = r;
    mode = 3;
    goal = &p;
}
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", arrive__11AiNavigatorR8_fvectorfff);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", arrive__11AiNavigatorR13DbInteractivefff);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", tag__11AiNavigatorR8_fvectorf);
#ifdef NON_MATCHING
/* 10/12 words: store order of the eight fields differs */
void AiNavigator::flee(_fvector &p, float r)
{
    f194 = r;
    goal = &p;
    f198 = r;
    f190 = 0;
    f19C = 1.0f;
    f18C = 0;
    mode = 6;
    status = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", flee__11AiNavigatorR8_fvectorf);
#endif
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", updateSeek__11AiNavigator);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", updateTarget__11AiNavigator);
void AiNavigator::updateArrive(void)
{
    updateSeek();
}
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", updateTag__11AiNavigator);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", updateFlee__11AiNavigator);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", targetPin__11AiNavigatorR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", pathSeekIntercept__11AiNavigator);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", pathSeek__11AiNavigator);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", avoid__11AiNavigatorR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", avoidClimbing__11AiNavigatorR8_fvectorT1);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", avoidJumping__11AiNavigatorR8_fvectorT1);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", avoidFlying__11AiNavigatorR8_fvectorT1);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", avoidGeneral__11AiNavigatorR8_fvectorT1);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", attemptClimb__11AiNavigatorR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", handleNoJumpOrClimb__11AiNavigatorP9_hdResultR8_fvectorT2);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", orientTo__11AiNavigatorR8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", strafeTo__11AiNavigatorR8_fvectorff);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", followPath__11AiNavigatorRQ211AiNavigator8PathInfoT1R8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", nextPathPoint__11AiNavigator);
AiPath *AiNavigator::getFleePath(_fvector &p)
{
    return (AiPath *)((char *)s_net + 0x3850);
}
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", inPathTransition__11AiNavigatorRQ211AiNavigator8PathInfoR8_fvectorb);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", getMaxJumpHeight__11AiNavigatorb);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", getMaxStandingJumpHeight__11AiNavigatorb);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", __Q211AiNavigator14ObstacleSensorR11AiNavigator);
void AiNavigator::ObstacleSensor::init(void)
{
    obstacleHit = 0;
    obstacle2Hit = 0;
    floorHit = 0;
    ceilHit = 0;
}
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", test__Q211AiNavigator14ObstacleSensor);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", getNextCollis__Q211AiNavigator14ObstacleSensorP9_hdResultiT1);
_hdResult *AiNavigator::ObstacleSensor::getObstacle(void)
{
    if (obstacleHit)
        return (_hdResult *)((char *)this + 0x10);
    return 0;
}
float AiNavigator::ObstacleSensor::getObstacleRange(void)
{
    return obstacleRange;
}
float AiNavigator::ObstacleSensor::getObstacleHeight(void)
{
    register float b __asm__("$f1") = obstacle2Height;
    register float a __asm__("$f2") = obstacleHeight;
    register float r __asm__("$f0");

    __asm__("max.s %0, %1, %2" : "=f"(r) : "f"(b), "f"(a));
    return r;
}
float AiNavigator::ObstacleSensor::getCeilHeight(void)
{
    if (ceilHit)
        return ceilHeight;
    return 99999.0f;
}
float AiNavigator::ObstacleSensor::getFloorHeight(void)
{
    if (floorHit)
        return floorHeight;
    return -99999.0f;
}
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", shouldAvoid__Q211AiNavigator14ObstacleSensorR8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", isObstacleClimbable__Q211AiNavigator14ObstacleSensor);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", isObstacleClimbable__Q211AiNavigator14ObstacleSensorR9_hdResult);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", init__Q211AiNavigator8PathInfoP6AiPathi);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", init__Q211AiNavigator8PathInfoP6AiPathT1b);
INCLUDE_ASM("asm/nonmatchings/game/AiNavigator", init__Q211AiNavigator8PathInfoP6AiPathP8_fvector);

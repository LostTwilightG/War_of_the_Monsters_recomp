#include "common.h"
#include "game/pickup.h"
#include "game/game.h"
#include "vecmath.h"

__asm__("#SNFIX_SMALL game");

void hdReparentCsGrid(_cs *cs, _fvector *pos, float f);
void mathfRotMatrixRPH(float (*m)[4], _fvector *rph);
class Weapon {
public:
    void GetVel(_fvector &v);
};
extern _fvector D_00731340;
struct DbInteractive;
class Interactives {
public:
    static int addInteractive(DbInteractive *p);
};


INCLUDE_ASM("asm/nonmatchings/game/Pickup", __6Pickup);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", init__6PickupP9_hierheadR8_fvectorPA3_fb11ePickupType);
void Pickup::initAfterDbLoad(void)
{
    int id = Interactives::addInteractive((DbInteractive *)this);
    unsigned *head = (unsigned *)cs->epNode;

    *head = (*head & 0xFFFC007F) | ((id & 0x7FF) << 7);
    hdReparentCsGrid(cs, (_fvector *)((char *)cs + 0x10), 0.0f);
}
bool Pickup::update(void)
{
    ((PickupSound *)((char *)this + 0x100))->updatePickupSound();
    unsigned long long b = bits & 0x40;

    if (b)
        hatCheck();
    return health > 0.0f;
}
bool Pickup::regenUpdate(void)
{
    if (--regenTimer <= 0) {
        unsigned long long b = bits & 1;

        if (b)
            return true;
    }
    return false;
}
#ifdef NON_MATCHING
/* 21/25 words: register choice for the home pointer and the lq/sq asm */
void Pickup::regen(void)
{
    register int t __asm__("$2");

    regenTimer = 0xA8C;
    cs->drawMe = 1;
    health = maxHealth;
    __asm__ volatile("lq %1, 0(%2)
	sq %1, %0" : "=m"(cs->trans), "=r"(t) : "r"((char *)this + 0x20));
    mathfRotMatrixRPH((float (*)[4])((char *)cs + 0x20), (_fvector *)((char *)this + 0x30));
    hatCheck();
}
#else
INCLUDE_ASM("asm/nonmatchings/game/Pickup", regen__6Pickup);
#endif
void Pickup::takeHit(_fvector *pos, float dmg, int x)
{
    health -= dmg;
}
void Pickup::takeUseDamage(float dmg)
{
    health -= dmg;
}
INCLUDE_ASM("asm/nonmatchings/game/Pickup", isTargetClear__6PickupP13DbInteractive);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", grab__6Pickupi);
void Pickup::drop(void)
{
    hatCheck();
    heldState = 0;
    setVisualState(0);
    hdReparentCsGrid(cs, (_fvector *)((char *)cs + 0x10), 0.0f);
}
INCLUDE_ASM("asm/nonmatchings/game/Pickup", kill__6Pickup);
void Pickup::initConfig(void)
{
    health = maxHealth = LevelPickups::s_info[pickupType].health;
}
INCLUDE_ASM("asm/nonmatchings/game/Pickup", updateCS__6PickupP8_fvectorRA3_A3_fb);
void Pickup::getVel(_fvector &v)
{
    if (weapIdx >= 0)
        ((Weapon *)((char *)gameWeapons() + (weapIdx * 0x170 + 0xC00)))->GetVel(v);
    else
        vecScale(&v, (_fvector *)((char *)cs + 0x30), speed);
}
_fvector *Pickup::getVel(void)
{
    char *vt = *(char **)((char *)this + 0x10);

    (*(void (**)(void *, _fvector *))(vt + 0x2C))((char *)this + *(short *)(vt + 0x28), &D_00731340);
    return &D_00731340;
}
void Pickup::setVisualState(int s)
{
    HierSwitch *sw = *(HierSwitch **)((char *)cs->epNode + 0x20);

    if (sw->head.opcode == 6)
        sw->whichChild = (s < sw->numKids) ? s : sw->numKids - 1;
}
INCLUDE_ASM("asm/nonmatchings/game/Pickup", hatCheck__6Pickup);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", initLocators__6PickupP9_hierhead);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", initHandle__6PickupP14_hiertranslate);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", initHandle__6PickupP11_hierrotate);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", initCollisSpheres__6PickupP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", swipeTrailEffect__6Pickup);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", collisTestBatAttack__6PickupfUi11MonsterAnimR10HitHistoryfff);
_fvector *Pickup::getCollisSphereTrans(int i)
{
    return (_fvector *)((char *)this + 0x110) + i;
}
float Pickup::getCollisSphereRadius(int i)
{
    return *(float *)((char *)this + (i << 4) + 0x11C);
}
INCLUDE_ASM("asm/nonmatchings/game/Pickup", updateGenerator__6Pickup);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", init__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", turnOffAllVehicles__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", turnOffMilitary__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", turnOnMilitary__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", turnOff2Handeds__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", initMilitaryForCentral__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", initMilitaryForVegas__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", createPickup__12LevelPickupsP9_hierheadR8_fvectorPA3_fb11ePickupType);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", initAfterDbLoad__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", createPickupHighlight__12LevelPickupsP9_hierhead);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", startCinema__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", endCinema__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", update__12LevelPickups);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", hatCheck__12LevelPickupsP8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", turnOffForDelayedHat__12LevelPickupsP8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", computeHighlight__12LevelPickupsR7Monster);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", enableHighlights__12LevelPickupsi);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", impalePickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8Iterator);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", grabPickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8Iteratori);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", dropPickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8Iterator);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", throwPickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8IteratorR8_fvectorP13DbInteractiveT3);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", throwRBPickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8IteratorR8_fvectorP13DbInteractive);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", deflectThrownPickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8IteratorR8_fvectorP13DbInteractiveT3);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", grabThrownPickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8Iteratori);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", prunePickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8Iterator);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", killPickup__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8Iteratori);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", inFlight__12LevelPickupsGQ2t10LinkedList1ZP6Pickup8Iterator);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", getClosestPickup__12LevelPickupsR8_fvectorT1);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", getClosestThrownPickup__12LevelPickupsR8_fvectorf);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", __static_initialization_and_destruction_0_00183128);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", func_001831D0);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", func_001831E0);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", __tf6Pickup);
PickupIter Pickup::getRef(void)
{
    return ref;
}
float Pickup::getHealth(void)
{
    return health;
}
float Pickup::getSpeed(void)
{
    return speed;
}
int Pickup::getDbId(void)
{
    return *(unsigned *)*(char **)cs >> 18;
}
int Pickup::getPickupType(void)
{
    return pickupType;
}
int Pickup::getWeapIdx(void)
{
    return weapIdx;
}
int Pickup::getGrabber(void)
{
    return heldState;
}
int Pickup::isRegenable(void)
{
    return (int)bits & 1;
}
int Pickup::isTwoHanded(void)
{
    return (int)(bits >> 1) & 1;
}
int Pickup::isRigidBody(void)
{
    return (int)(bits >> 2) & 1;
}
int Pickup::isADragonHead(void)
{
    return (int)(bits >> 5) & 1;
}
float Pickup::getSwipeRange(void)
{
    return swipeFar - swipeNear;
}
int Pickup::getNumCollisSpheres(void)
{
    return *(int *)((char *)this + 0x160);
}
void Pickup::setRef(PickupIter it)
{
    ref = it;
    *(Pickup **)it.cur = this;
}
void Pickup::setGrabber(int i)
{
    heldState = i;
}
void Pickup::setWeapIdx(int i)
{
    weapIdx = i;
}
void Pickup::updateGenerator(void *p)
{
    updateGenerator();
}
void Pickup::resetHealth(void)
{
    health = maxHealth;
}
float Pickup::getHandleRange(void)
{
    return handleRange;
}
INCLUDE_ASM("asm/nonmatchings/game/Pickup", _vt$6Pickup);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", func_00183368);
INCLUDE_ASM("asm/nonmatchings/game/Pickup", _GLOBAL_$I$__6Pickup);

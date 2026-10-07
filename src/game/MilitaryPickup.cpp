#include "common.h"
#include "game/military_pickup.h"
#include "hieri_types.h"
#include "cs_pool.h"

INCLUDE_ASM("asm/nonmatchings/game/MilitaryPickup", _vt$14MilitaryPickup);
void MilitaryPickup::grab(int i)
{
    Pickup::grab(i);
    flags &= 0xFFFD;
    breakFormation();
    state = 7;
}
void MilitaryPickup::kill(void)
{
    breakFormation();
    flags &= 0xFFFD;
    if ((bits & 1) == 0) {
        CsPool::csDeactivate(cs);
        cs = 0;
    }
}
INCLUDE_ASM("asm/nonmatchings/game/MilitaryPickup", breakFormation__14MilitaryPickup);
INCLUDE_ASM("asm/nonmatchings/game/MilitaryPickup", func_0014D9A8);
INCLUDE_ASM("asm/nonmatchings/game/MilitaryPickup", func_0014D9B8);
INCLUDE_ASM("asm/nonmatchings/game/MilitaryPickup", __tf14MilitaryPickup);
INCLUDE_ASM("asm/nonmatchings/game/MilitaryPickup", __14MilitaryPickup);
int MilitaryPickup::getState(void)
{
    return state;
}
int MilitaryPickup::getFocus(void)
{
    return focus;
}
void MilitaryPickup::leadFormation(void)
{
}
void MilitaryPickup::followFormation(void)
{
}
void MilitaryPickup::resignFormation(void)
{
}
void MilitaryPickup::setFormation(MilitaryFormation *f)
{
    formation = f;
}
void MilitaryPickup::setFormationPos(_fvector &p)
{
    register int t __asm__("$2");

    __asm__ volatile("lq %1, 0(%2)
	sq %1, %0" : "=m"(formationPos), "=r"(t) : "r"(&p));
}
_fvector *MilitaryPickup::getFormationPos(void)
{
    return &formationPos;
}

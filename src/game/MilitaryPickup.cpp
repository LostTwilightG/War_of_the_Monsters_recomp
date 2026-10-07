#include "common.h"
#include "hieri_types.h"

struct MilitaryFormation;

class CsPool {
public:
    static void csDeactivate(_cs *cs);
};

/* Pickup is the (virtual) base in retail; only the fields used here are laid out. */
class Pickup {
public:
    void grab(int i);
};

class MilitaryPickup {
public:
    char pad0[4];
    unsigned short flags;
    char pad6[6];
    _cs *cs;
    char pad10[0x50 - 0x10];
    unsigned long long bits;
    char pad58[0x170 - 0x58];
    int focus;
    int state;
    MilitaryFormation *formation;
    char pad17C[4];
    _fvector formationPos;

    void grab(int i);
    void kill(void);
    void breakFormation(void);
    int getState(void);
    int getFocus(void);
    void leadFormation(void);
    void followFormation(void);
    void resignFormation(void);
    void setFormation(MilitaryFormation *f);
    void setFormationPos(_fvector &p);
    _fvector *getFormationPos(void);
};

INCLUDE_ASM("asm/nonmatchings/game/MilitaryPickup", _vt$14MilitaryPickup);
void MilitaryPickup::grab(int i)
{
    ((Pickup *)this)->grab(i);
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

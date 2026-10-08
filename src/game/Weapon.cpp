#include "common.h"

INCLUDE_ASM("asm/nonmatchings/game/Weapon", __Q26Weapon8Dynamics);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", InitWeaponsBefore__7Weapons);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", InitWeaponsAfter__7Weapons);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", UpdateWeapons__7Weapons);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", WeaponInRadius__7WeaponsP8_fvectorfPP6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", UpdateRotation__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", UpdateLockOnTarget__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", UpdateLockAheadOfMonster__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", UpdateLockVertical__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", UpdateErraticity__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", UpdateTranslation__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", WeaponHD__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", D_006F28F8);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", setCsTransform__6WeaponP8_fvectorRA3_A3_f);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", ComputeReflection__6WeaponP8_fvectorT1b);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", CreateStaticWeapon__7WeaponsQ26Weapon10WeaponTypeiiP8_fvectorT4P9_hierhead);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", CreateStaticWeapon__7WeaponsGQ2t10LinkedList1ZP6Pickup8IteratoriiP8_fvector);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", CreateLavaBall__7WeaponsP8_fvectorN21ii);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", CalculateBallisticArc__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", InitWeapon__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", KillWeapon__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", ricochetParticlesOffSurface__FiP8_fvectorN31);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", DetonateWeapon__6WeaponR9_hdResult);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", DispatchRangeBasedDamage__6Weapon);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", GetVel__6WeaponR8_fvector);
class Monster;
class Weapons {
public:
    void DecrementWeaponCount(Monster *m, int type);
};
void Weapons::DecrementWeaponCount(Monster *m, int type)
{
}
INCLUDE_ASM("asm/nonmatchings/game/Weapon", AddWeaponEpNode__7WeaponsiP9_hierhead);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", CreateFire__7WeaponsP8_fvectorfi);
INCLUDE_ASM("asm/nonmatchings/game/Weapon", UpdateFlames__7Weapons);

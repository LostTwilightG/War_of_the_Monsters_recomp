#include "common.h"

extern "C" int printf(const char *, ...);

class CsColor {
public:
    struct Data {
        int enabled;
        float contrast;
        float brightness;
        Data();
    };

    /* retail names carry a trailing "v" that this compiler does not emit for static members */
    static int isEnabled(void) __asm__("isEnabled__7CsColorv");
    static float contrast(void) __asm__("contrast__7CsColorv");
    static float brightness(void) __asm__("brightness__7CsColorv");
    static void load(const Data *d);
    static void save(Data *d);
};

extern CsColor::Data D_007B3040;

CsColor::Data::Data()
{
    enabled = 1;
    contrast = 1.0f;
    brightness = 0;
}
int CsColor::isEnabled(void)
{
    return D_007B3040.enabled;
}
float CsColor::contrast(void)
{
    return D_007B3040.contrast;
}
float CsColor::brightness(void)
{
    return D_007B3040.brightness;
}
void CsColor::load(const Data *d)
{
    printf("LOADING CSCOLOR %d\n", 12);
    D_007B3040 = *d;
}
void CsColor::save(Data *d)
{
    printf("SAVING CSCOLOR\n");
    *d = D_007B3040;
}
INCLUDE_ASM("asm/nonmatchings/common/CsColor", __static_initialization_and_destruction_0_0022C438);
INCLUDE_ASM("asm/nonmatchings/common/CsColor", _GLOBAL_$I$__Q27CsColor4Data);

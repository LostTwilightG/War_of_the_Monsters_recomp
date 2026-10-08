#include "common.h"

#define INPUT_MAX_PADS 8
#define INPUT_CONFIG_BUTTONS 9

/* libpad read buffer of one controller, 32 bytes */
struct InputPadData {
    unsigned char status;
    unsigned char type;
    short buttons; /* active low */
    unsigned char analog[28];
};

extern int inputMode __asm__("D_006F8D8C");
__asm__("#SNFIX_SMALL D_006F8D8C");
extern InputPadData inputPadData[INPUT_MAX_PADS] __asm__("D_00779D00");
extern int inputCtlPadState[INPUT_MAX_PADS] __asm__("D_00779E60");
extern int inputPadType[INPUT_MAX_PADS] __asm__("D_00779EA0");
extern int D_00779EC0[INPUT_MAX_PADS];
extern int D_00779EE0[INPUT_MAX_PADS];
extern int D_00779F00[INPUT_MAX_PADS];
extern int inputConfigButtons[INPUT_MAX_PADS][INPUT_CONFIG_BUTTONS] __asm__("D_00779F20");
extern int inputButtonsMapped[INPUT_MAX_PADS] __asm__("D_0077A040");
extern int inputPlayerPad[INPUT_MAX_PADS] __asm__("D_0077A060");
extern int inputActuatorOn[INPUT_MAX_PADS] __asm__("D_0077A0C0");
extern int inputCtlAvailable[INPUT_MAX_PADS] __asm__("D_0077A0E0");

INCLUDE_ASM("asm/nonmatchings/common/input", inputInit__Fv);
INCLUDE_ASM("asm/nonmatchings/common/input", inputClearInputs__Fi);
INCLUDE_ASM("asm/nonmatchings/common/input", inputUpdate__Fv);
INCLUDE_ASM("asm/nonmatchings/common/input", inputUpdateState__Fii);
INCLUDE_ASM("asm/nonmatchings/common/input", inputSetActuator__FiP12ActuatorData);
INCLUDE_ASM("asm/nonmatchings/common/input", inputStopActuator__FiUc);
int inputUsingActuator(int pad)
{
    return inputActuatorOn[pad];
}
void inputUseActuator(int pad, bool use)
{
    inputActuatorOn[pad] = use;
}
INCLUDE_ASM("asm/nonmatchings/common/input", inputAnyKey__Fi);
int inputIsCtlAvailable(int pad)
{
    return inputCtlAvailable[pad];
}
int inputGetPadButtons(int pad)
{
    int buttons = 0;

    if (inputCtlAvailable[pad])
        buttons = ~inputPadData[pad].buttons;
    return buttons;
}
int inputGetPadType(int pad)
{
    return inputPadType[pad];
}
int inputGetCtlPadState(int pad)
{
    return inputCtlPadState[pad];
}
INCLUDE_ASM("asm/nonmatchings/common/input", inputGetPadAnalog__Fi);
void inputSetInputMode(int mode)
{
    int i;

    inputMode = mode;
    if (mode == 0) {
        for (i = 0; i < INPUT_MAX_PADS; i++) {
            D_00779EE0[i] = 0;
            D_00779EC0[i] = 300;
            D_00779F00[i] = 0;
        }
    }
}
int inputGetInputMode(void)
{
    return inputMode;
}
void inputSetButtonMap(int mapped, int pad)
{
    inputButtonsMapped[pad] = mapped;
}
void inputSetCButtonMap(int pad, int button, int value)
{
    inputConfigButtons[pad][button] = value;
}
int *inputGetConfigButtons(int pad)
{
    return inputConfigButtons[pad];
}
int inputGetPlayerPad(int player)
{
    return inputPlayerPad[player];
}
int inputAreButtonsMapped(int pad)
{
    return inputButtonsMapped[pad];
}
int inputGetCButtonMap(int pad, int button)
{
    return inputConfigButtons[pad][button];
}
INCLUDE_ASM("asm/nonmatchings/common/input", inputGetAnyInput__Fi);
INCLUDE_ASM("asm/nonmatchings/common/input", inputGetInput__Fii);
INCLUDE_ASM("asm/nonmatchings/common/input", inputGetAnalogButton__Fii);
INCLUDE_ASM("asm/nonmatchings/common/input", inputGetCtlPadAnalogAxis__Fii);
INCLUDE_ASM("asm/nonmatchings/common/input", inputGetShellAnalogInput__Fii);
INCLUDE_ASM("asm/nonmatchings/common/input", inputFixAnalogValue__Fii);
INCLUDE_ASM("asm/nonmatchings/common/input", inputScaleAnalogButton__Fiii);

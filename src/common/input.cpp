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
extern int inputAnyLatched[INPUT_MAX_PADS] __asm__("D_00779F00"); /* inputGetAnyInput fired; cleared by inputSetInputMode(0) */
extern int inputPadPort[INPUT_MAX_PADS] __asm__("D_00779E20");
extern int inputPadSlot[INPUT_MAX_PADS] __asm__("D_00779E40");
extern unsigned char inputActData[INPUT_MAX_PADS][6] __asm__("D_0077A130"); /* scePadSetActDirect data: [0] small motor, [1] big */

extern "C" {
void *memset(void *dst, int c, unsigned int n);
int scePadSetActDirect(int port, int slot, const unsigned char *data);
}
int inputGetPadButtons(int pad);
int inputGetPadAnalog(int pad);
int inputFixAnalogValue(int axis, int pad);
extern int inputConfigButtons[INPUT_MAX_PADS][INPUT_CONFIG_BUTTONS] __asm__("D_00779F20");
extern int inputButtonsMapped[INPUT_MAX_PADS] __asm__("D_0077A040");
extern int inputPlayerPad[INPUT_MAX_PADS] __asm__("D_0077A060");
extern int inputActuatorOn[INPUT_MAX_PADS] __asm__("D_0077A0C0");
extern int inputCtlAvailable[INPUT_MAX_PADS] __asm__("D_0077A0E0");

INCLUDE_ASM("asm/nonmatchings/common/input", inputInit__Fv);
void inputClearInputs(int pad)
{
    InputPadData *d = &inputPadData[pad];
    unsigned char status = d->status;
    unsigned char type = d->type;

    memset(d, 0, sizeof(InputPadData));
    d->status = status;
    d->type = type;
}
INCLUDE_ASM("asm/nonmatchings/common/input", inputUpdate__Fv);
INCLUDE_ASM("asm/nonmatchings/common/input", inputUpdateState__Fii);
INCLUDE_ASM("asm/nonmatchings/common/input", inputSetActuator__FiP12ActuatorData);
void inputStopActuator(int pad, unsigned char motor)
{
    if (motor == 2) {
        inputActData[pad][0] = 0;
        inputActData[pad][1] = 0;
    } else
        inputActData[pad][motor] = 0;
    scePadSetActDirect(inputPadPort[pad], inputPadSlot[pad], inputActData[pad]);
}
int inputUsingActuator(int pad)
{
    return inputActuatorOn[pad];
}
void inputUseActuator(int pad, bool use)
{
    inputActuatorOn[pad] = use;
}
int inputAnyKey(int pad)
{
    int any = 0;

    if (inputCtlAvailable[pad]) {
        if (inputGetPadButtons(pad) || inputGetPadAnalog(pad))
            any = 1;
    }
    return any;
}
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
int inputGetPadAnalog(int pad)
{
    union {
        int all;
        struct {
            unsigned int a : 8;
            unsigned int b : 8;
            unsigned int c : 8;
            unsigned int d : 8;
        } f;
    } axes;

    axes.all = 0;
    if (inputCtlAvailable[pad]) {
        axes.f.a = inputFixAnalogValue(2, pad);
        axes.f.b = inputFixAnalogValue(3, pad);
        axes.f.c = inputFixAnalogValue(0, pad);
        axes.f.d = inputFixAnalogValue(1, pad);
    }
    return axes.all;
}
void inputSetInputMode(int mode)
{
    int i;

    inputMode = mode;
    if (mode == 0) {
        for (i = 0; i < INPUT_MAX_PADS; i++) {
            D_00779EE0[i] = 0;
            D_00779EC0[i] = 300;
            inputAnyLatched[i] = 0;
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
int inputGetAnyInput(int pad)
{
    if (inputAnyLatched[pad])
        return 0;
    if (!inputAnyKey(pad))
        return 0;
    inputAnyLatched[pad] = 1;
    return 1;
}
INCLUDE_ASM("asm/nonmatchings/common/input", inputGetInput__Fii);
INCLUDE_ASM("asm/nonmatchings/common/input", inputGetAnalogButton__Fii);
int inputGetCtlPadAnalogAxis(int axis, int pad)
{
    int v = 0;

    if (inputCtlAvailable[pad]) {
        switch (axis) {
        case 0:
            v = inputPadData[pad].analog[0];
            break;
        case 1:
            v = inputPadData[pad].analog[1];
            break;
        case 2:
            v = inputPadData[pad].analog[2];
            break;
        case 3:
            v = inputPadData[pad].analog[3];
            break;
        }
    }
    return v;
}
int inputGetShellAnalogInput(int dir, int pad)
{
    int hit = 0;
    int x, y;

    if (inputCtlAvailable[pad] && inputPadType[pad] != 4) {
        x = 0x80 - inputGetCtlPadAnalogAxis(2, pad);
        y = 0x80 - inputGetCtlPadAnalogAxis(3, pad);
        inputGetCtlPadAnalogAxis(0, pad);
        inputGetCtlPadAnalogAxis(1, pad);
        switch (dir) {
        case 0x80: /* left */
            hit = x > 120;
            break;
        case 0x20: /* right */
            hit = x < -120;
            break;
        case 0x10: /* up */
            hit = y > 120;
            break;
        case 0x40: /* down */
            hit = y < -120;
            break;
        }
    }
    return hit;
}
int inputFixAnalogValue(int axis, int pad)
{
    int v = 0;
    int mag;
    float f;

    if (inputCtlAvailable[pad] && inputPadType[pad] != 4) {
        v = inputGetCtlPadAnalogAxis(axis, pad);
        if (v > 225) {
            f = (float)v - 225.0f;
            v = (int)(f * 64.0f / 30.0f + 64.0f);
            if (v > 127)
                v = 127;
        } else if (v > 162) {
            f = (float)v - 162.0f;
            v = (int)(f * 64.0f / 64.0f);
            if (v > 64)
                v = 64;
        } else if (v < 30) {
            f = (float)v;
            v = (int)(f * 64.0f / 30.0f + -127.0f);
            if (v <= -128)
                v = -127;
        } else if (v < 92) {
            f = 92.0f - (float)v;
            v = (int)(f * -64.0f / 62.0f);
            if (v <= -65)
                v = -64;
        } else
            v = 0;
        if (v > 127)
            v = 127;
        else if (v <= -128)
            v = -127;
        mag = v < 0 ? -v : v;
        if (mag <= 1)
            v = 0;
    }
    return v;
}
int inputScaleAnalogButton(int value, int min, int max)
{
    int v = min + (int)((float)value / 255.0f * ((float)max - (float)min));
    int mag = v < 0 ? -v : v;

    if (mag < 10)
        v = 0;
    else if (max < mag)
        v = max;
    return v;
}

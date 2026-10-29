#include <string.h>
#include <math.h>
#include <dolphin/pad.h>
#include <dolphin/si.h>
#include <dolphin/os/OSInterrupt.h>
#include <GSinput.h>
#include <GSgfx.h>

#define INPUT_DEVICE_MAX 4

#define INPUT_SAMPLING_RATE 11

#define INPUT_STICK_RADIUS 56
#define INPUT_SUBSTICK_RADIUS 44
#define INPUT_STICK_DEADZONE 10

#define INPUT_MOTOR_POWER_MAX 3600
#define INPUT_MOTOR_POWER_MIN 100
#define INPUT_MOTOR_SCALE 15

#define INPUT_SMOOTHING_LINEAR 0
#define INPUT_SMOOTHING_SNAP 1

#define INPUT_CLAMP_TRUNCATE 0
#define INPUT_CLAMP_NONE 1
#define INPUT_CLAMP_CIRCLE 2

#define INPUT_CONNECT_DONE 0
#define INPUT_CONNECT_WAIT 3

#define INPUT_REPEAT_IDLE 0
#define INPUT_REPEAT_PRESSED 1
#define INPUT_REPEAT_HELD 2

#define INPUT_REPEAT_DELAY 0.6f
#define INPUT_REPEAT_INTERVAL 0.1f

#define INPUT_MOTOR_IDLE 0
#define INPUT_MOTOR_RUMBLE 1
#define INPUT_MOTOR_STOP 2
#define INPUT_MOTOR_STOP_HARD 3

typedef struct GSinputPad {
    /* 0x00 */ s32 port;
    /* 0x04 */ s32 subType;
    /* 0x08 */ s32 clamping;
    /* 0x0C */ s32 connectState;
    /* 0x10 */ u8 smoothingRate;
    /* 0x11 */ u8 smoothingCount;
    /* 0x14 */ s32 smoothingFunc;
    /* 0x18 */ s32 repeatState;
    /* 0x1C */ u32 repeatButtons;
    /* 0x20 */ u32 repeatResult;
    /* 0x24 */ f32 repeatTimer;
    /* 0x28 */ PADStatus padStatus;
    /* 0x34 */ PADStatus status;
    /* 0x40 */ u32 lastButtons;
    /* 0x44 */ s8 lastLeftX;
    /* 0x45 */ s8 lastLeftY;
    /* 0x46 */ s8 lastRightX;
    /* 0x47 */ s8 lastRightY;
    /* 0x48 */ f32 leftStepX;
    /* 0x4C */ f32 leftStepY;
    /* 0x50 */ f32 rightStepX;
    /* 0x54 */ f32 rightStepY;
    /* 0x58 */ f32 leftPosX;
    /* 0x5C */ f32 leftPosY;
    /* 0x60 */ f32 rightPosX;
    /* 0x64 */ f32 rightPosY;
    /* 0x68 */ s8 clampLeftX;
    /* 0x69 */ s8 clampLeftY;
    /* 0x6A */ s8 clampRightX;
    /* 0x6B */ s8 clampRightY;
    /* 0x6C */ s32 motorState;
    /* 0x70 */ volatile u32 motorPower;
    /* 0x74 */ u32 motorDuration;
    /* 0x78 */ u8 motorDecay;
} GSinputPad;

GSinputPad _inputData[INPUT_DEVICE_MAX];
volatile s32 _padMotorLevel[INPUT_DEVICE_MAX];
volatile u32 _padRumble[INPUT_DEVICE_MAX];

u32 _padCallbackCount;
u32 _padResetBits;
u32 _padLastVsync;

static u8 _padReadPending;

static u8 _padMotorEnabled = 1;

extern "C" {
void _inputCheckConnect(void);
void _inputPadCallback(void);
void _inputUpdateMotors(void);
void _inputDoRepeat(GSinputPad* pad);
void _inputInitRepeat(GSinputPad* pad);
void _inputUpdateSmoothing(GSinputPad* pad);
void _inputDoSmoothing(GSinputPad* pad, s8 x, s8 y, s8* lastX, s8* lastY, f32* stepX,
                       f32* stepY, f32* posX, f32* posY);
void _inputDoClamping(GSinputPad* pad);
GSinputPad* _inputFindDevice(s32 port);
void ClampCircle(s8* x, s8* y, s8 radius, s8 deadzone);
}

extern "C" void GSinputClearButtonData(GSinputDevice device, u32 buttons)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad != NULL) {
        pad->status.button &= ~buttons;
    }
}

extern "C" void GSinputMotorStart(u32 chan, u8 mode, u8 strength, u32 duration, u8 decay)
{
    GSinputPad* pad;

    if (_padMotorEnabled == 0) {
        return;
    }
    pad = _inputFindDevice(chan);
    if (pad == NULL) {
        return;
    }
    if (mode != 0) {
        return;
    }
    pad->motorState = INPUT_MOTOR_RUMBLE;
    pad->motorPower = strength * INPUT_MOTOR_SCALE;
    pad->motorDuration = duration;
    pad->motorDecay = (u8)decay;
}

extern "C" s8 GSinputGetRightStickYData(int pad, int filtered)
{
    GSinputPad* dev = _inputFindDevice(pad);

    if (dev == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return dev->clampRightY;
    }
    return dev->status.substickY;
}

extern "C" s8 GSinputGetRightStickXData(int pad, int filtered)
{
    GSinputPad* dev = _inputFindDevice(pad);

    if (dev == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return dev->clampRightX;
    }
    return dev->status.substickX;
}

extern "C" s8 GSinputGetLeftStickYData(int pad, int filtered)
{
    GSinputPad* dev = _inputFindDevice(pad);

    if (dev == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return dev->clampLeftY;
    }
    return dev->status.stickY;
}

extern "C" s8 GSinputGetLeftStickXData(int pad, int filtered)
{
    GSinputPad* dev = _inputFindDevice(pad);

    if (dev == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return dev->clampLeftX;
    }
    return dev->status.stickX;
}

extern "C" u32 GSinputButtonsRepeat(GSinputDevice device)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad == NULL) {
        return 0;
    }
    return pad->repeatResult;
}

extern "C" u32 GSinputButtonsChanged(GSinputDevice device)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad == NULL) {
        return 0;
    }
    return pad->status.button ^ pad->lastButtons;
}

extern "C" u32 GSinputButtonsPressed(GSinputDevice device)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad == NULL) {
        return 0;
    }
    return pad->status.button;
}

extern "C" s32 GSinputGetSubType(s32 device)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad == NULL) {
        return 2;
    }
    return pad->subType;
}

extern "C" void GSinputResetRightSmoothing(GSinputDevice device, s8 x, s8 y)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad != NULL) {
        pad->lastRightX = x;
        pad->lastRightY = y;
        pad->rightPosX = (f32)x;
        pad->rightPosY = (f32)y;
    }
}

extern "C" void GSinputResetLeftSmoothing(GSinputDevice device, s8 x, s8 y)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad != NULL) {
        pad->lastLeftX = x;
        pad->lastLeftY = y;
        pad->leftPosX = (f32)x;
        pad->leftPosY = (f32)y;
    }
}

extern "C" void GSinputSetSmoothingFunc(GSinputDevice device, s32 func)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad != NULL) {
        pad->smoothingFunc = func;
    }
}

extern "C" void GSinputSetSmoothingRate(GSinputDevice device, u8 rate)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad != NULL) {
        pad->smoothingRate = rate;
    }
}

extern "C" void GSinputSetClamping(GSinputDevice device, s32 clamping)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad != NULL) {
        pad->clamping = clamping;
    }
}

extern "C" u8 GSinputIsConnected(s32 device)
{
    GSinputPad* pad = _inputFindDevice(device);

    if (pad == NULL) {
        return 0;
    }
    if (pad->connectState == INPUT_CONNECT_DONE) {
        return 1;
    }
    return 0;
}

extern "C" s32 GSinputRead(GSinputDevice device)
{
    GSinputPad* pad = _inputFindDevice(device);
    BOOL enabled;

    if (pad == NULL) {
        return 2;
    }
    pad->lastButtons = pad->status.button;
    enabled = OSDisableInterrupts();
    memcpy(&pad->status, &pad->padStatus, sizeof(PADStatus));
    OSRestoreInterrupts(enabled);
    if (_padReadPending != 0) {
        _inputCheckConnect();
        if (_padResetBits != 0) {
            PADReset(_padResetBits);
        }
        _padReadPending = 0;
    }
    _inputUpdateSmoothing(pad);
    _inputDoClamping(pad);
    _inputDoRepeat(pad);
    return pad->connectState;
}

extern "C" void GSinputReadEnd(void)
{
    _padReadPending = 0;
}

extern "C" void GSinputReadStart(void)
{
    _padReadPending = 1;
}

extern "C" void _inputCheckConnect(void)
{
    s32 i;

    _padResetBits = 0;
    for (i = 0; i < INPUT_DEVICE_MAX; i++) {
        GSinputPad* pad = _inputFindDevice(i + 1);
        u32 chanBit;
        u32 type;
        BOOL enabled;

        if (pad == NULL) {
            continue;
        }
        enabled = OSDisableInterrupts();
        chanBit = PAD_CHAN0_BIT >> i;
        type = SIProbe(i);
        OSRestoreInterrupts(enabled);
        switch (type) {
        case SI_GC_CONTROLLER:
        case SI_GC_WAVEBIRD:
            pad->subType = PAD_SPEC_0;
            if (pad->status.err == PAD_ERR_NO_CONTROLLER) {
                _padResetBits |= chanBit;
                pad->padStatus.err = PAD_ERR_NOT_READY;
                pad->connectState = INPUT_CONNECT_WAIT;
            } else {
                pad->connectState = INPUT_CONNECT_DONE;
            }
            break;
        default:
            pad->connectState = INPUT_CONNECT_WAIT;
            pad->subType = PAD_SPEC_2;
            pad->padStatus.err = PAD_ERR_NO_CONTROLLER;
            break;
        }
    }
}

extern "C" s32 GSinputOpen(s32 port)
{
    GSinputPad* pad = _inputFindDevice(GS_INPUT_NONE);

    if (pad == NULL) {
        return 4;
    }
    switch (port) {
    case GS_INPUT_PORT_1:
    case GS_INPUT_PORT_2:
    case GS_INPUT_PORT_3:
    case GS_INPUT_PORT_4:
        pad->port = port;
        break;
    default:
        return 1;
    }
    return 0;
}

extern "C" void GSinputInit(void)
{
    s32 i;

    _padCallbackCount = 0;
    _padResetBits = 0xF0000000;
    _padLastVsync = 0;
    for (i = 0; i < INPUT_DEVICE_MAX; i++) {
        _inputData[i].port = GS_INPUT_NONE;
        _inputData[i].clamping = INPUT_CLAMP_TRUNCATE;
        _inputData[i].connectState = INPUT_CONNECT_WAIT;
        _inputData[i].smoothingRate = 0;
        _inputData[i].smoothingFunc = INPUT_SMOOTHING_LINEAR;
        _inputData[i].lastButtons = 0;
        memset(&_inputData[i].padStatus, 0, sizeof(PADStatus));
        memset(&_inputData[i].status, 0, sizeof(PADStatus));
        _inputData[i].lastLeftX = 0;
        _inputData[i].lastLeftY = 0;
        _inputData[i].lastRightX = 0;
        _inputData[i].lastRightY = 0;
        _inputData[i].leftPosX = 0.0f;
        _inputData[i].leftPosY = 0.0f;
        _inputData[i].rightPosX = 0.0f;
        _inputData[i].rightPosY = 0.0f;
        _inputData[i].motorState = INPUT_MOTOR_STOP_HARD;
        _inputData[i].motorPower = 0;
        _inputData[i].motorDuration = 0;
        _inputData[i].motorDecay = 0;
        _inputInitRepeat(&_inputData[i]);
        _padRumble[i] = PAD_MOTOR_STOP_HARD;
        _padMotorLevel[i] = 0;
    }
    PADSetAnalogMode(0);
    PADInit();
    PADControlAllMotors((u32*)_padRumble);
    PADRecalibrate(_padResetBits);
    SISetSamplingRate(INPUT_SAMPLING_RATE);
    PADSetSamplingCallback(_inputPadCallback);
}

extern "C" void _inputPadCallback(void)
{
    PADStatus statuses[INPUT_DEVICE_MAX];
    s32 i;

    PADRead(statuses);
    for (i = 0; i < INPUT_DEVICE_MAX; i++) {
        PADStatus* status = &statuses[i];
        GSinputPad* pad = _inputFindDevice(i + 1);

        if (pad == NULL) {
            continue;
        }
        if (status->err == PAD_ERR_TRANSFER) {
            continue;
        }
        status->stickY = -status->stickY;
        status->substickY = -status->substickY;
        memcpy(&pad->padStatus, status, sizeof(PADStatus));
    }
    _inputUpdateMotors();
    _padCallbackCount++;
}

extern "C" void _inputUpdateMotors(void)
{
    s32 i;
    u8 update = 0;

    for (i = 0; i < INPUT_DEVICE_MAX; i++) {
        GSinputPad* pad = _inputFindDevice(i + 1);

        if (pad == NULL) {
            continue;
        }
        if (pad->connectState != INPUT_CONNECT_DONE) {
            continue;
        }
        switch (pad->motorState) {
        case INPUT_MOTOR_RUMBLE:
            if (pad->motorPower > INPUT_MOTOR_POWER_MAX) {
                if (_padRumble[i] != PAD_MOTOR_RUMBLE) {
                    _padRumble[i] = PAD_MOTOR_RUMBLE;
                    update = 1;
                    break;
                }
            }
            if (pad->motorPower < INPUT_MOTOR_POWER_MIN) {
                if (_padRumble[i] != PAD_MOTOR_STOP) {
                    _padRumble[i] = PAD_MOTOR_STOP;
                    update = 1;
                    break;
                }
            }
            _padMotorLevel[i] += pad->motorPower;
            if (_padMotorLevel[i] > INPUT_MOTOR_POWER_MAX) {
                _padMotorLevel[i] = 0;
                if (_padRumble[i] != PAD_MOTOR_RUMBLE) {
                    _padRumble[i] = PAD_MOTOR_RUMBLE;
                    update = 1;
                    break;
                }
            } else {
                if (_padRumble[i] != PAD_MOTOR_STOP) {
                    _padRumble[i] = PAD_MOTOR_STOP;
                    update = 1;
                    break;
                }
            }
            break;
        case INPUT_MOTOR_STOP:
            if (_padRumble[i] != PAD_MOTOR_STOP) {
                _padRumble[i] = PAD_MOTOR_STOP;
                update = 1;
            }
            break;
        case INPUT_MOTOR_STOP_HARD:
            if (_padRumble[i] != PAD_MOTOR_STOP_HARD) {
                _padRumble[i] = PAD_MOTOR_STOP_HARD;
                update = 1;
            }
            break;
        }
        if (pad->motorDuration != 0) {
            if (--pad->motorDuration == 0) {
                pad->motorState = INPUT_MOTOR_STOP;
            }
        }
        if (pad->motorDecay != 0) {
            if (pad->motorPower < pad->motorDecay) {
                pad->motorState = INPUT_MOTOR_STOP;
                pad->motorDecay = 0;
            } else {
                pad->motorPower -= pad->motorDecay;
            }
        }
    }
    if (update != 0) {
        PADControlAllMotors((u32*)_padRumble);
    }
}

extern "C" void _inputDoRepeat(GSinputPad* pad)
{
    u32 held;
    u32 pressed;
    f32 rate;

    held = pad->status.button;
    pressed = pad->status.button & (pad->status.button ^ pad->lastButtons);
    held &= ~(PAD_TRIGGER_L | PAD_TRIGGER_R);
    pressed &= ~(PAD_TRIGGER_L | PAD_TRIGGER_R);
    switch (pad->repeatState) {
    case INPUT_REPEAT_IDLE:
        pad->repeatResult = 0;
        pad->repeatState = INPUT_REPEAT_PRESSED;
        /* fall through */
    case INPUT_REPEAT_PRESSED:
        if (pressed == 0) {
            pad->repeatResult = 0;
            break;
        }
        pad->repeatButtons = pressed;
        pad->repeatResult = pressed;
        pad->repeatTimer = INPUT_REPEAT_DELAY;
        pad->repeatState = INPUT_REPEAT_HELD;
        break;
    case INPUT_REPEAT_HELD:
        pad->repeatResult = 0;
        rate = (f32)GSgfxVideoGetVsyncRate();
        pad->repeatTimer -= GSgfxVideoGetLastRenderTime() / rate;
        if (held != pad->repeatButtons) {
            pad->repeatState = INPUT_REPEAT_IDLE;
            break;
        }
        if (pad->repeatTimer < 0.0f) {
            pad->repeatResult = pad->repeatButtons;
            pad->repeatTimer = INPUT_REPEAT_INTERVAL;
        }
        break;
    }
}

extern "C" void _inputInitRepeat(GSinputPad* pad)
{
    pad->repeatState = INPUT_REPEAT_IDLE;
    pad->repeatResult = 0;
    pad->repeatButtons = 0;
}

extern "C" void _inputUpdateSmoothing(GSinputPad* pad)
{
    _inputDoSmoothing(pad, pad->status.stickX, pad->status.stickY, &pad->lastLeftX,
                      &pad->lastLeftY, &pad->leftStepX, &pad->leftStepY, &pad->leftPosX,
                      &pad->leftPosY);
    _inputDoSmoothing(pad, pad->status.substickX, pad->status.substickY, &pad->lastRightX,
                      &pad->lastRightY, &pad->rightStepX, &pad->rightStepY, &pad->rightPosX,
                      &pad->rightPosY);
}

extern "C" void _inputDoSmoothing(GSinputPad* pad, s8 x, s8 y, s8* lastX, s8* lastY,
                                  f32* stepX, f32* stepY, f32* posX, f32* posY)
{
    if (*lastX < x - 2 || *lastX > x + 2 || *lastY < y - 2 || *lastY > y + 2) {
        if (pad->smoothingFunc == INPUT_SMOOTHING_LINEAR) {
            *stepX = ((f32)x - *posX) / (f32)pad->smoothingRate;
            *stepY = ((f32)y - *posY) / (f32)pad->smoothingRate;
        } else if (pad->smoothingFunc == INPUT_SMOOTHING_SNAP) {
            *stepX = ((f32)x - *posX) / ((f32)pad->smoothingRate / 4.0f);
            *stepY = ((f32)y - *posY) / ((f32)pad->smoothingRate / 4.0f);
        }
        pad->smoothingCount = 0;
    }
    *lastX = x;
    *lastY = y;
    *posX += *stepX;
    *posY += *stepY;
    if (pad->smoothingFunc == INPUT_SMOOTHING_SNAP) {
        pad->smoothingCount++;
        if (pad->smoothingCount < pad->smoothingRate) {
            *stepX *= 0.5f;
            *stepY *= 0.5f;
            if ((*stepX > 0.0f ? *stepX : -*stepX) < 1.0f) {
                *stepX = (*stepX > 0.0f) ? 1 : -1;
            }
            if ((*stepY > 0.0f ? *stepY : -*stepY) < 1.0f) {
                *stepY = (*stepX > 0.0f) ? 1 : -1;
            }
        }
    }
    if (*stepX < 0.0f) {
        if (*posX < (f32)*lastX) {
            *posX = (f32)*lastX;
        }
    } else {
        if (*posX > (f32)*lastX) {
            *posX = (f32)*lastX;
        }
    }
    if (*stepY < 0.0f) {
        if (*posY < (f32)*lastY) {
            *posY = (f32)*lastY;
        }
    } else {
        if (*posY > (f32)*lastY) {
            *posY = (f32)*lastY;
        }
    }
}

extern "C" void _inputDoClamping(GSinputPad* pad)
{
    s8 x;
    s8 y;

    switch (pad->clamping) {
    case INPUT_CLAMP_TRUNCATE:
        pad->clampLeftX = (s8)pad->leftPosX;
        pad->clampLeftY = (s8)pad->leftPosY;
        pad->clampRightX = (s8)pad->rightPosX;
        pad->clampRightY = (s8)pad->rightPosY;
        break;
    case INPUT_CLAMP_CIRCLE:
        ClampCircle(&pad->status.stickX, &pad->status.stickY, INPUT_STICK_RADIUS,
                    INPUT_STICK_DEADZONE);
        ClampCircle(&pad->status.substickX, &pad->status.substickY, INPUT_SUBSTICK_RADIUS,
                    INPUT_STICK_DEADZONE);
        x = (s8)pad->leftPosX;
        y = (s8)pad->leftPosY;
        ClampCircle(&x, &y, INPUT_STICK_RADIUS, INPUT_STICK_DEADZONE);
        pad->clampLeftX = x;
        pad->clampLeftY = y;
        x = (s8)pad->rightPosX;
        y = (s8)pad->rightPosY;
        ClampCircle(&x, &y, INPUT_SUBSTICK_RADIUS, INPUT_STICK_DEADZONE);
        pad->clampRightX = x;
        pad->clampRightY = y;
        break;
    }
}

extern "C" GSinputPad* _inputFindDevice(s32 port)
{
    s32 i;

    for (i = 0; i < INPUT_DEVICE_MAX; i++) {
        GSinputPad* pad = &_inputData[i];

        if (pad->port == port) {
            return pad;
        }
    }
    return NULL;
}

extern "C" void ClampCircle(s8* x, s8* y, s8 radius, s8 deadzone)
{
    s32 dx = *x;
    s32 dy = *y;
    s32 squared;

    if (-deadzone < dx && dx < deadzone) {
        dx = 0;
    } else if (dx > 0) {
        dx -= deadzone;
    } else {
        dx += deadzone;
    }
    if (-deadzone < dy && dy < deadzone) {
        dy = 0;
    } else if (dy > 0) {
        dy -= deadzone;
    } else {
        dy += deadzone;
    }
    squared = dx * dx + dy * dy;
    if (radius * radius < squared) {
        s32 length = (s32)sqrtf(squared);

        dx = dx * radius / length;
        dy = dy * radius / length;
    }
    *x = dx;
    *y = dy;
}

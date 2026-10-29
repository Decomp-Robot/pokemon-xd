#ifndef GS_INPUT_H
#define GS_INPUT_H

#include <types.h>

typedef enum GSinputDevice {
    GS_INPUT_NONE = 0,
    GS_INPUT_PORT_1 = 1,
    GS_INPUT_PORT_2 = 2,
    GS_INPUT_PORT_3 = 3,
    GS_INPUT_PORT_4 = 4
} GSinputDevice;

#ifdef __cplusplus
extern "C" {
#endif

s8 GSinputGetRightStickXData(int pad, int filtered);
s8 GSinputGetRightStickYData(int pad, int filtered);
s8 GSinputGetLeftStickXData(int pad, int filtered);
s8 GSinputGetLeftStickYData(int pad, int filtered);
u32 GSinputButtonsChanged(GSinputDevice device);
u32 GSinputButtonsPressed(GSinputDevice device);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
void GSinputMotorStart(u32 chan, u8 mode, u8 strength, u32 duration, u8 decay);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
void GSinputInit(void);
s32 GSinputOpen(s32 port);
s32 GSinputRead(GSinputDevice device);
void GSinputReadStart(void);
void GSinputReadEnd(void);
u8 GSinputIsConnected(s32 device);
s32 GSinputGetSubType(s32 device);
void GSinputClearButtonData(GSinputDevice device, u32 buttons);
u32 GSinputButtonsRepeat(GSinputDevice device);
void GSinputSetClamping(GSinputDevice device, s32 clamping);
void GSinputSetSmoothingFunc(GSinputDevice device, s32 func);
void GSinputSetSmoothingRate(GSinputDevice device, u8 rate);
void GSinputResetRightSmoothing(GSinputDevice device, s8 x, s8 y);
void GSinputResetLeftSmoothing(GSinputDevice device, s8 x, s8 y);
#ifdef __cplusplus
}
#endif
#endif

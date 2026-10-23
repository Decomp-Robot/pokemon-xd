#ifndef GS_INPUT_H
#define GS_INPUT_H

#include <types.h>

enum GSinputDevice {
    GS_INPUT_NONE = 0,
    GS_INPUT_PORT_1 = 1,
    GS_INPUT_PORT_2 = 2,
    GS_INPUT_PORT_3 = 3,
    GS_INPUT_PORT_4 = 4
};

#ifdef __cplusplus
extern "C" {
#endif

u32 GSinputButtonsChanged(enum GSinputDevice device);
u32 GSinputButtonsPressed(enum GSinputDevice device);

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
#endif

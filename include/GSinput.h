#ifndef GS_INPUT_H
#define GS_INPUT_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Mode 0 requests rumble; duration counts down to motor stop.
void GSinputMotorStart(u32 chan, s32 mode, s32 strength, s32 duration, s32 decay);

#ifdef __cplusplus
}
#endif

#endif

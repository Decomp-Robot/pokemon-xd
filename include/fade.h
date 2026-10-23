#ifndef FADE_H
#define FADE_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns the fade state, optionally waiting for the active fade to finish.
s8 fadeCheck(u8 waitForCompletion);
void fadeSet(s32 kind, f32 duration);

#ifdef __cplusplus
}
#endif

#endif

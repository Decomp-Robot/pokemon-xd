#ifndef MENUOFFSCREEN_H
#define MENUOFFSCREEN_H

#include "types.h"

// Enable off-screen menu drawing and return the previous state.
#ifdef __cplusplus
extern "C" {
#endif
s32 menuOffScreenSetDrawEnable(s32 enable);
void menuCreateOffScreen(float frames, float frames2);
void menuReleaseOffScreen(float frames);
u8 menuDisableOffScreenCheck(void);
#ifdef __cplusplus
}
#endif

#endif

#ifndef MENUOFFSCREEN_H
#define MENUOFFSCREEN_H

#include "types.h"

// Set the menu drawing priority and return the previous priority.
extern "C" u8 menuOffScreenSetPriority(s32 priority);

// Enable off-screen menu drawing and return the previous state.
#ifdef __cplusplus
extern "C" {
#endif
u8 menuOffScreenFadeSync(s32 wait);
s32 menuOffScreenSetDisp(s32 enable);
void menuCreateOffScreen(float frames, float frames2);
void menuReleaseOffScreen(float frames);
u8 menuDisableOffScreenCheck(void);
#ifdef __cplusplus
}
#endif

#endif

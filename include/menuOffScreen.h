#ifndef MENUOFFSCREEN_H
#define MENUOFFSCREEN_H

#include "types.h"
#include <GStexture.h>

// Set the menu drawing priority and return the previous priority.
extern "C" u8 menuOffScreenSetPriority(u8 priority);

#ifdef __cplusplus
extern "C" {
#endif
// Enable off-screen menu drawing and return the previous state.
u8 menuOffScreenSetDisp(u8 enable);
void menuOffScreenDraw(void);
void menuOffScreenInit(void);
void menuOffScreenFadeSet(float endRate, float frames);
u8 menuOffScreenFadeSync(u8 wait);
u8 menuOffScreenCheckEnable(u8 wait);
void menuOffScreenCreate(u8 wait);
void menuOffScreenRelease(void);
u8 menuOffScreenIsDoing(void);
GStexture* menuOffScreenGetPtr(void);
void menuCreateOffScreen(float frames, float frames2);
void menuReleaseOffScreen(float frames);
u8 menuDisableOffScreenCheck(void);
#ifdef __cplusplus
}
#endif

#endif

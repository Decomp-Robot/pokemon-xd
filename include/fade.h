#ifndef FADE_H
#define FADE_H

#include <types.h>

enum FadeKind { FADE_IN_BLACK = 2, FADE_OUT_BLACK = 3 };

#ifdef __cplusplus
extern "C" {
#endif
void fadeSetToBlackOutIfNotBlack(f32 time);
void fadeSetToBlackInIfBlack(f32 time);

void fadeSet(u16 flags, f32 duration);
s8 fadeCheck(u8 wait);

#ifdef __cplusplus
}
#endif

#endif

#ifndef FADE_H
#define FADE_H

#include <types.h>

enum FadeKind { FADE_IN_BLACK = 2, FADE_OUT_BLACK = 3, FADE_OUT_EFFECT = 9 };

#ifdef __cplusplus
extern "C" {
#endif
void fadeEffectDokuStop(void);
void fadeEffectDokuStart(void);
int fadeGetType(void);
void fadeDaemon(void);
void fadeInit(void);
void fadeSetToBlackOutIfNotBlack(f32 time);
void fadeSetToBlackInIfBlack(f32 time);

typedef void (*FadeFunction)(void);
#ifdef __cplusplus
FadeFunction fadeSetEX(u16 flags, f32 duration, f32 effectTime, FadeFunction function, bool snapshotUse);
#else
FadeFunction fadeSetEX(u16 flags, f32 duration, f32 effectTime, FadeFunction function, u8 snapshotUse);
#endif
void fadeSetFunctionOnly(FadeFunction function);
void fadeSet(u16 flags, f32 duration);
s8 fadeCheck(u8 wait);

#ifdef __cplusplus
}
FadeFunction fadeSetFunction(FadeFunction function, bool snapshotUse, u16 flags, f32 effectTime);
#endif

#endif

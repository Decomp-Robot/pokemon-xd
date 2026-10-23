#ifndef SPRITE_H
#define SPRITE_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
void spriteClearEnv(s32 layer);
void spriteSetEnv(s32 layerID);
void spriteSetScissor(s32 layerID, s32 x, s32 y, s32 width, s32 height);
void spriteClearScissor(s32 layerID);
#ifdef __cplusplus
}
#endif
#endif

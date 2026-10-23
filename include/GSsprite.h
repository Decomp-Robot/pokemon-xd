#ifndef GS_SPRITE_H
#define GS_SPRITE_H

#include <types.h>
#include <GSgfx.h>

extern "C" {
void spriteSetOffset(s32 layer, s16 x, s16 y);
void spriteSetScissor(s32 layer, s32 x, s32 y, s32 width, s32 height);
void spriteClearScissor(s32 layer);
void spriteSetEnv(s32 layer);
void spriteClearEnv(s32 layer);
}

#endif

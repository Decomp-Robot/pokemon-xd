#ifndef SPRITE_H
#define SPRITE_H

#include <GSgfx.h>

extern "C" {
void spriteSetOffset(GSgfxLayerID layer, short x, short y);
void spriteClearScissor(GSgfxLayerID layer);
void spriteSetEnv(GSgfxLayerID layer);
void spriteClearEnv(GSgfxLayerID layer);
}

#endif

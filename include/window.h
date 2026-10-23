#ifndef WINDOW_H
#define WINDOW_H

#include <GSgfx.h>
#include <winSprite.h>

extern "C" {
void windowDrawTextureId(s16 x, s16 y, tagWINDOW_WORK *window, u16 textureId,
                         s32 drawMode, GSgfxLayerID layer);
}

#endif

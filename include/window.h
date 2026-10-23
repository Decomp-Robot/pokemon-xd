#ifndef WINDOW_H
#define WINDOW_H

#include <GSgfx.h>
#include <winSprite.h>

#ifdef __cplusplus
extern "C" {
#endif
tagWINDOW_WORK *windowSearchID(s32 id);
void windowSetParam(tagWINDOW_WORK *window, long index, long value);
long windowGetParam(tagWINDOW_WORK *window, long index);
u16 windowGetKeyTrg(void);
u16 windowGetKeyRepeat(void);
void windowDrawSprite(s16 x, s16 y, tagWINDOW_WORK *window, s32 textureId,
                         s32 drawMode, GSgfxLayerID layer);
void windowDrawSprite2(s32 x, s32 y, s32 width, s32 height, u32 color,
                       tagWINDOW_WORK *window, s32 textureId, s32 drawMode,
                       GSgfxLayerID layer);
#ifdef __cplusplus
}
#endif

#endif

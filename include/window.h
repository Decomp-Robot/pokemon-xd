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
void windowDrawSprite(s16 x, s16 y, tagWINDOW_WORK *window, u16 textureId,
                         s32 drawMode, GSgfxLayerID layer);
void windowDrawSprite2(s16 x, s16 y, s16 width, s16 height, unsigned long color,
                       tagWINDOW_WORK *window, u16 textureId, s32 drawMode,
                       GSgfxLayerID layer);
s32 windowGetActiveID();
u16 windowGetKeyOn();
void windowCloseMain(tagWINDOW_WORK *window);
#ifdef __cplusplus
}
#endif

#endif

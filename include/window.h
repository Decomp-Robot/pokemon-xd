#ifndef WINDOW_H
#define WINDOW_H

#include <GSgfx.h>
#include <winSprite.h>

struct WINDOW_SYS_WORK {
    u16 mWindowCapacity;
    u8 mUnknown02[0x0A];
    tagWINDOW_WORK* mWindowPool;
    u8 mUnknown10[4];
    u16 mDirectionRepeat;
    u8 mUnknown16[4];
    u16 mStickDirectionRepeat;
    u8 mUnknown1C[0x7A];
    u8 mEnablePortMask;
    u8 mUnknown97[9];
};

extern WINDOW_SYS_WORK winSysWork;
extern WINDOW_SYS_WORK winSysWork2;

enum WindowKeyMask {
    WINDOW_KEY_ACCEPT = 0x10,
    WINDOW_KEY_CANCEL = 0x20
};

#ifdef __cplusplus
extern "C" {
#endif
tagWINDOW_KEY* windowGetPortKeyInfo(u8 portMask);

void windowCreateCursorSprite(tagWINDOW_WORK* window);
tagWINDOW_WORK *windowSearchID(s32 id);
tagSPRITE_WORK* windowSearchItemID(tagWINDOW_WORK* window, unsigned short itemID);
u8 windowCheckCursor(long menu, u8 wait);
long windowGetValue(long menu);
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
s32 windowGetValue(long windowID);
u16 windowGetKeyOn();
void windowCloseMain(tagWINDOW_WORK *window);
#ifdef __cplusplus
}
#endif

#endif

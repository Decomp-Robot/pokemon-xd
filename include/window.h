#ifndef WINDOW_H
#define WINDOW_H

#include <GSgfx.h>
#include <winSprite.h>

struct WINDOW_SYS_WORK {
    u16 mWindowCapacity;
    u8 mUnknown02[6];
    s32 mActiveID;
    tagWINDOW_WORK* mWindowPool;
    tagWINDOW_WORK* mWindowList;
    u16 mDirectionRepeat;
    u8 mUnknown16[4];
    u16 mStickDirectionRepeat;
    u8 mUnknown1C[18];
    tagWINDOW_KEY mPortKeys[4];
    u8 mEnablePortMask;
    u8 mUnknown97[9];
};

extern WINDOW_SYS_WORK winSysWork;
extern WINDOW_SYS_WORK winSysWork2;

// Work area the free-form menu windows keep their own state in. It follows the
// window object, so the accessor below returns a pointer into the window.
struct WINDOW_FREE_WORK {
    u32 mMessageID;          // message the window's text printer is running.
    u8 mUnknown04[4];
    u32 mKeyTrigger;         // key trigger handed to the text printer.
};

#ifdef __cplusplus
extern "C" {
#endif
tagWINDOW_KEY* windowGetPortKeyInfo(u8 portMask);
tagWINDOW_KEY* windowGetPortKeyInfo2(u8 portMask);

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
void windowDrawSprite3(s16 x, s16 y, s16 width, s16 height, u32 color,
                       tagWINDOW_WORK *window, u16 textureId, s16 offsetX,
                       s16 offsetY, s16 clipWidth, s16 clipHeight, u32 flags,
                       GSgfxLayerID layer);
s32 windowGetActiveID();
s32 windowGetValue(long windowID);
u16 windowGetKeyOn();
enum WindowCloseFlags {
    WINDOW_CLOSE_CHILDREN_ONLY = 0x1,
    WINDOW_CLOSE_KEEP_CHILDREN = 0x2,
    WINDOW_CLOSE_RESPECT_MENU_FLAG = 0x4
};
long windowClose(tagWINDOW_WORK *window, unsigned long flags);
void windowCloseMain(tagWINDOW_WORK *window);
// Free work area of a window, or NULL when the window has none.
struct WINDOW_FREE_WORK *windowGetFreeWork(tagWINDOW_WORK *window);
// Work area the window allocates for its item hooks, or NULL when it has none.
void *windowGetAllocPtr(tagWINDOW_WORK *window);
#ifdef __cplusplus
}
#endif

#endif

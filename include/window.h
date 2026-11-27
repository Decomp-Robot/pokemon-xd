#ifndef WINDOW_H
#define WINDOW_H

#include <GSgfx.h>
#include <cursorBios.h>
#include <winSprite.h>
#include <stdarg.h>

struct WINDOW_SYS_WORK {
    u16 mWindowCapacity;
    u8 mUnknown02[6];
    s32 mActiveID;
    tagWINDOW_WORK* mWindowPool;
    tagWINDOW_WORK* mWindowList;
    tagWINDOW_KEY mKey;
    tagWINDOW_KEY mPortKeys[4];
    u8 mEnablePortMask;
    u8 mUnknown97[9];
};

extern WINDOW_SYS_WORK winSysWork;
extern WINDOW_SYS_WORK winSysWork2;

#ifdef __cplusplus
extern "C" {
#endif
void windowInit(u16 capacity);
tagWINDOW_WORK *windowOpen(s32 *cursor, s32 menuID, s32 parentID, u32 flags,
                           s32 argCount, va_list args);
tagWINDOW_KEY* windowGetPortKeyInfo(u8 portMask);
tagWINDOW_KEY* windowGetPortKeyInfo2(u8 portMask);

void windowCreateCursorSprite(tagWINDOW_WORK* window);
tagWINDOW_WORK *windowSearchID(s32 id);
tagSPRITE_WORK* windowSearchItemID(tagWINDOW_WORK* window, u16 itemID);
u8 windowCheckCursor(s32 menu, u8 wait);
void windowSetParam(tagWINDOW_WORK *window, s32 index, s32 value);
s32 windowGetParam(tagWINDOW_WORK *window, s32 index);
CURSOR windowGetCursor(s32 windowID);
u16 windowGetKeyTrg(void);
u16 windowGetKeyRepeat(void);
enum WindowDrawFlags {
    WINDOW_DRAW_FLIP_X = 0x1,
    WINDOW_DRAW_FLIP_Y = 0x2
};
void windowDrawSprite(s16 x, s16 y, tagWINDOW_WORK *window, u16 textureId,
                         s32 drawMode, GSgfxLayerID layer);
void windowDrawSprite2(s16 x, s16 y, s16 width, s16 height, u32 color,
                       tagWINDOW_WORK *window, u16 textureId, s32 drawMode,
                       GSgfxLayerID layer);
void windowDrawSprite3(s16 x, s16 y, s16 width, s16 height, u32 color,
                       tagWINDOW_WORK *window, u16 textureId, s16 offsetX,
                       s16 offsetY, s16 clipWidth, s16 clipHeight, u32 flags,
                       GSgfxLayerID layer);
s32 windowGetActiveID();
s32 windowGetValue(s32 windowID);
u16 windowGetKeyOn();
enum WindowCloseFlags {
    WINDOW_CLOSE_CHILDREN_ONLY = 0x1,
    WINDOW_CLOSE_KEEP_CHILDREN = 0x2,
    WINDOW_CLOSE_RESPECT_MENU_FLAG = 0x4
};
s32 windowClose(tagWINDOW_WORK *window, u32 flags);
void windowCloseMain(tagWINDOW_WORK *window);
// Free work area of a window, or NULL when the window has none.
struct WINDOW_FREE_WORK *windowGetFreeWork(tagWINDOW_WORK *window);
// Work area the window allocates for its item hooks, or NULL when it has none.
void *windowGetAllocPtr(tagWINDOW_WORK *window);
void *windowAllocMemory(tagWINDOW_WORK *window, s32 size);
#ifdef __cplusplus
}
#endif

#endif

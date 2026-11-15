#ifndef MENU_H
#define MENU_H

#include <GSgfxTypes.h>
#include <menuIds.h>
#include <types.h>
#include <bool.h>
#include <cursorBios.h>

struct Pokemon;
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

#ifdef __cplusplus
extern "C" {
#endif

enum MenuStatusFlags {
    MENU_STATUS_NO_CANCEL = 0x8,
    // Suppress the window daemon's default confirm/cancel key handling.
    MENU_STATUS_NO_DEFAULT_INPUT = 0x10
};
s32 menuSetStatus(s32 menuID, s32 status);
void menuSetPosition(s32 menuID, s16 x, s16 y);
void menuPlaySe(s32 menuID, s32 soundID);
u8 menuSetEnablePort(u8 portMask);
u8 menuGetEnablePort(void);
s32 menuGetLastError(void);
s32 menuGetCursor(s32 menuID);
s32 menuGetCursorItemID(s32 menuID);
u8 menuCursorCmp(CURSOR previous, CURSOR current);
void menuCursorNormal(struct tagWINDOW_WORK *window, s32 mode);
u8 menuIsCheck(s32 menuID);
f32 menuGetTotalTime(void);
f32 menuGetLastTime(void);
void menuButtonNormal(struct tagWINDOW_WORK *window);
void menuDataBiosSetXY(s32 id, s16 x, s16 y);
void menuDataBiosGetXY(s32 id, s16 *x, s16 *y);
u8 menuDataBiosGetDispMax(s32 id);

void windowCloseMain(struct tagWINDOW_WORK *window);
#ifdef __cplusplus
void menuSetDisp(s32 menuID, bool visible);
#else
void menuSetDisp(s32 menuID, u8 visible);
#endif
u16 windowGetKeyOn(void);
void windowDrawSprite2(s16 x, s16 y, s16 width, s16 height, u32 color,
                       struct tagWINDOW_WORK *window, u16 spriteID, s32 flags,
                       GSgfxLayerID layer);

bool menuPanelInputNumber(long initial, long *output, long lower, long upper,
                          bool closeOnExit);

s32 menuOpen(s32 menuID, s32 mode);
u16 windowGetKeyTrg(void);
enum { MENU_OPEN_FLAG_NO_ACTIVATE = 4 };
s32 menuOpenCustom(s32 menuID, s32 parentID, void *result, u32 flags,
                   bool wait,
                   s32 argCount, ...);
s32 menuClose(s32 menuID);
s32 menuCloseCustom(s32 menuID, s32 arg, bool wait);
s32 menuCloseSync(s32 menuID, bool wait);
u16 windowGetKeyRepeat(void);
s32 windowGetActiveID(void);
s32 windowGetValue(s32 menuID);
void windowDrawSprite(s16 x, s16 y, struct tagWINDOW_WORK *window, u16 spriteID,
                      s32 flags, GSgfxLayerID layer);

#ifdef __cplusplus
}
#endif

#endif

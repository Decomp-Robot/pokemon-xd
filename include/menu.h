#ifndef MENU_H
#define MENU_H

#include <GSgfxTypes.h>
#include <menuIds.h>
#include <types.h>
#include <bool.h>

struct Pokemon;
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

#ifdef __cplusplus
extern "C" {
#endif

void menuSetPosition(s32 menuID, s32 x, s32 y);
void menuPlaySe(s32 menuID, s32 soundID);
u8 menuSetEnablePort(s32 portMask);
s32 menuGetLastError(void);
s32 menuGetCursor(s32 menuID);
s32 menuGetCursorItemID(s32 menuID);
void menuCursorNormal(struct tagWINDOW_WORK *window, s32 mode);
u8 menuIsCheck(s32 menuID);
void menuButtonNormal(struct tagWINDOW_WORK *window);
u8 menuFaceCheckLoading(void);
void menuDataBiosSetXY(s32 id, s16 x, s16 y);
void menuDataBiosGetXY(s32 id, s16 *x, s16 *y);
u8 menuDataBiosGetDispMax(s32 id);
void menuFaceDispSub(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite, u16 faceID);

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

s32 menuOpen(s32 menuID, s32 mode);
u16 windowGetKeyTrg(void);
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

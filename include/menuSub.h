#ifndef MENU_SUB_H
#define MENU_SUB_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <dolphin/gx/GXStruct.h>
#ifdef __cplusplus
}
#endif

// The Yes/No sub dialog answer flag is a C++ bool in the menus that use it,
// which the compiler passes as a byte.
#ifdef __cplusplus
typedef bool MENU_SUB_ANSWER;
typedef bool MENU_SUB_CLOSE_ON_EXIT;
#else
typedef s32 MENU_SUB_ANSWER;
typedef s32 MENU_SUB_CLOSE_ON_EXIT;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Yes/No sub dialog: type selects the dialog, x/y position it.
s8 menuSubOpenYesNo(u8 type, s32 x, s32 y, MENU_SUB_ANSWER flag);
// Number entry sub dialog; value is both the initial value and the result.
u8 menuSubOpenNumberInputDigit(s32 kind, s32 *value);
// Number entry sub dialog with an explicit key mode and screen position;
// closeOnExit makes the dialog close once the entry is committed.
u8 menuSubOpenNumberInput(u32 value, u32 *out, u8 mode, s16 x, s16 y, MENU_SUB_CLOSE_ON_EXIT closeOnExit);
// Closes the number entry sub dialog.
void menuSubCloseNumberInput(void);
// Filled rectangle on a GSgfx layer.
void menuSubDrawTile(s32 layerID, s32 x, s32 y, s32 width, s32 height, GXColor *color);

#ifdef __cplusplus
}
#endif

#endif

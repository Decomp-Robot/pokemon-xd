#ifndef MENU_SUB_H
#define MENU_SUB_H

#include <types.h>
#include <GSgfxTypes.h>
#include <Pokemon.h>

#include <dolphin/gx/GXStruct.h>

#ifdef __cplusplus
typedef bool MENU_SUB_ANSWER;
typedef bool MENU_SUB_CLOSE_ON_EXIT;
#else
typedef s32 MENU_SUB_ANSWER;
typedef s32 MENU_SUB_CLOSE_ON_EXIT;
#endif

enum MenuSubNumberInputMode {
    MENU_SUB_NUMBER_SIGNED_DECIMAL = 0,
    MENU_SUB_NUMBER_UNSIGNED_DECIMAL = 1,
    MENU_SUB_NUMBER_HEXADECIMAL = 2
};

struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

// Cursor-back graphic sets. The plain set draws one fixed sprite pair; the
// variant set draws a second pair, or a third one when a colour variant is
// requested.
enum MenuSubCursorBackStyle {
    MENU_SUB_CURSOR_BACK_STYLE_PLAIN = 0,
    MENU_SUB_CURSOR_BACK_STYLE_VARIANT = 1,
};

#ifdef __cplusplus
extern "C" {
#endif
s32 menuSubOpenColosseumEnter(s32 type);
s8 menuSubOpenYesNoField(void);

u8 menuSubGetPokemonSexForFightDisp(struct Pokemon *pokemon);
u8 menuSubGetPokemonSexForDisp(struct Pokemon *pokemon);
void menuSubCursorAnimeValGet(f32 *values, s32 *animation);
s32 menuSubOpenSelect_gba(u8 type, const u32 *messages, s32 count, s32 x, s32 y,
                          s32 initialCursor, s32 displayedCount, u8 wrapCursor, u8 allowCancel);
s32 menuSubOpenSelect3(u8 type, const u32 *messages, s32 count, s32 x, s32 y,
                       s32 initialCursor, s32 displayedCount, u8 wrapCursor, u8 allowCancel);
s32 menuSubOpenSelect2(u8 type, u32 *messageIDs, s32 count, s32 x, s32 y,
                       s32 initialCursor, s32 displayedCount, u8 selectMode);
void menuSubSleep(f32 seconds);
s32 menuSubOpenSelect(u8 type, u32 *messages, int count,
                      s32 x, s32 y, s32 cursor);

// Yes/No sub dialog: type selects the dialog, x/y position it.
s8 menuSubOpenYesNo(u8 type, s16 x, s16 y, s32 answer);
// Unsigned decimal entry; result receives the edited value even on cancel.
u8 menuSubOpenNumberInputDigit(u32 initialValue, u32 *result);
// Number entry sub dialog with an explicit numeric mode and screen position;
// closeOnExit closes the dialog after either confirmation or cancellation.
u8 menuSubOpenNumberInput(u32 value, u32 *out, u8 mode, s16 x, s16 y,
                          MENU_SUB_CLOSE_ON_EXIT closeOnExit);
// Closes the number entry sub dialog.
void menuSubCloseNumberInput(void);
void menuSubCloseNumberInputWait(void);
// Waits for a menu key press on controller one.
void menuSubKeyWait(void);
u8 menuSubOpenNumberInputID(u32 value, u32 *out, u16 *(*getName)(u32));
// Filled rectangle on a GSgfx layer.
void menuSubDrawTile(s32 layerID, s32 x, s32 y, s32 width, s32 height, GXColor color);
// Framed window the list screens draw their rows in.
void menuSubDrawSimpleWindow(s32 layerID, s32 x, s32 y, s32 width, s32 height);
// Normal window frame.
void menuSubDrawNormalWindow2(s32 layer, s32 x, s32 y, s32 width, s32 height,
                              u8 alpha);
// Message window frame drawn around the text at x/y.
void menuSubDrawMsgWindow2(s32 layer, s32 x, s32 y, s32 width, s32 height,
                           u8 alpha, s32 mode);
// Scroll marker the list screens draw above and below the rows.
void menuSubDrawTriangle(s32 layerID, s32 x1, s32 y1, s32 x2, s32 y2,
                         s32 x3, s32 y3);
// Blended sprite color of a menu window item.
u32 menuSubCalcColor(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);

u8 menuSubAnimAlpha(void);
s32 menuSubCheckSelect(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);
void menuSubCursorAnimeInit(void);
void menuSubCursorAnimeMain(void);
void menuSubDrawCursorBackAnimeOnOffAuto(struct tagWINDOW_WORK *window,
    GSgfxLayerID layer, s32 x, s32 y, s32 width, s32 height, u8 alpha,
    u8 style);
void menuSubCursorAnimeStart(void);
void menuSubDrawCursorBack2(s32 layer, s32 x, s32 y, s32 width, s32 height,
                            u8 alpha, s32 leftStyle, s32 rightStyle);
// Cursor-back highlight drawn behind a menu entry in the given style.
void menuSubDrawCursorBack(GSgfxLayerID layer, s32 x, s32 y, s32 width,
                           s32 height, u8 alpha, u8 style);

#ifdef __cplusplus
}
#endif

#endif

#ifndef MENU_SUB_H
#define MENU_SUB_H

#include <types.h>
#include <GSgfxTypes.h>
#include <Pokemon.h>

// RGBA colour, same layout as the GX colour type.
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;

#ifdef __cplusplus
typedef bool MENU_SUB_ANSWER;
typedef bool MENU_SUB_CLOSE_ON_EXIT;
#else
typedef s32 MENU_SUB_ANSWER;
typedef s32 MENU_SUB_CLOSE_ON_EXIT;
#endif

struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

#ifdef __cplusplus
extern "C" {
#endif
s8 menuSubOpenYesNoField();

u8 menuSubGetPokemonSexForFightDisp(struct Pokemon *pokemon);
u8 menuSubGetPokemonSexForDisp(struct Pokemon *pokemon);
void menuSubCursorAnimeValGet(f32 *values, s32 *animation);
s32 menuSubOpenSelect2(s32 type, u32 *messageIDs, s32 count, s32 x, s32 y,
                       s32 initialCursor, s32 displayedCount, s32 selectMode);
void menuSubWait(f32 seconds);
s32 menuSubOpenSelect(s32 type, u32 *messages, int count,
                      s32 x, s32 y, s32 cursor);

// Yes/No sub dialog: type selects the dialog, x/y position it.
s8 menuSubOpenYesNo(u8 type, s16 x, s16 y, s32 answer);
// Number entry sub dialog; value is both the initial value and the result.
u8 menuSubOpenNumberInputDigit(s32 kind, s32 *value);
// Number entry sub dialog with an explicit key mode and screen position;
// closeOnExit makes the dialog close once the entry is committed.
u8 menuSubOpenNumberInput(u32 value, u32 *out, u8 mode, s16 x, s16 y,
                          MENU_SUB_CLOSE_ON_EXIT closeOnExit);
// Closes the number entry sub dialog.
void menuSubCloseNumberInput(void);
u8 menuSubOpenNumberInputID(u32 value, u32 *out, u16 *(*getName)(u32));
// Filled rectangle on a GSgfx layer.
void menuSubDrawTile(s32 layerID, s32 x, s32 y, s32 width, s32 height, GXColor *color);
// Blended sprite color of a menu window item.
u32 menuSubCalcColor(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);

u32 menuSubAnimAlpha(void);
void menuSubCursorAnimeInit(void);
void menuSubCursorAnimeMain(void);
void menuSubDrawCursorBackAnimeOnOffAuto(struct tagWINDOW_WORK *window,
    GSgfxLayerID layer, s32 x, s32 y, s32 width, s32 height, u8 alpha, u8 style);
void menuSubCursorAnimeStart(void);
void menuSubDrawCursorBack2(s32 layer, s32 x, s32 y, s32 width, s32 height,
                            u8 alpha, s32 leftStyle, s32 rightStyle);

#ifdef __cplusplus
}
#endif

#endif

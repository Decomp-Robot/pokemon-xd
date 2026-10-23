#ifndef MENU_H
#define MENU_H

#include <types.h>

enum {
    MENU_LIST_CURSOR_TEXTURE = 0xC2,
    MENU_ID_DEBUG_CAMERA = 0x06,
    MENU_ID_PEOPLE_EDITOR = 0x0A,
    MENU_ID_PARTY_INFO = 0x0C,
    MENU_ID_TITLE = 0x11,
    MENU_ID_DEBUG_MOVE_SELECT = 0x15,
    MENU_ID_DEBUG_TRAINER_AI_EDITOR = 0x18,
    MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR = 0x19,
    MENU_ID_DEBUG_TRAINER_AI_ITEM_EDITOR = 0x1A,
    MENU_ID_DEBUG_TRAINER_AI_COMBO_EDITOR = 0x1B,
    MENU_ID_DEBUG_TRAINER_AI_SWITCH_OUT_EDITOR = 0x1C,
    MENU_ID_DEBUG_TRAINER_AI_SWITCH_IN_EDITOR = 0x1D,
    MENU_ID_DEBUG_TRAINER_AI_MOVE_USE_EDITOR = 0x1E,
    MENU_ID_DEBUG_TRAINER_POKEMON_PART_EDITOR = 0x1F,
    MENU_ID_DEBUG_MOVE_EDITOR = 0x20,
    MENU_ID_DEBUG_LOG = 0x22,
    MENU_ID_FIELD_CAMERA_EDITOR = 0x23,
    MENU_ID_DEBUG_ITEM_CREATE = 0x24,
    MENU_ID_DEBUG_ENCOUNTER_EDITOR = 0x25,
    MENU_ID_DEBUG_POKEMON_SELECT = 0x26,
    MENU_ID_WORLD_MAP = 0x54,
    MENU_ID_DEBUG_EDIT_CONFIRMATION = 0x55
};

enum {
    MENU_ITEM_DEBUG_MOVE_TYPE_FIRST = 0x89,
    MENU_ITEM_DEBUG_MOVE_TYPE_SECOND = 0x8A,
    MENU_ITEM_DEBUG_MOVE_TYPE_THIRD = 0x8B
};

struct Pokemon;
struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

#ifdef __cplusplus
extern "C" {
#endif

enum {
    MENU_SE_CONFIRM = 0x11,
    MENU_SE_CANCEL = 0x12,
    MENU_SE_ERROR = 0x13,
};
s32 menuGetCursor(s32 menuId);
s32 menuGetCursorItemID(s32 menuId);
void menuButtonNormal(struct tagWINDOW_WORK *win);
u8 menuFaceCheckLoading(void);
void menuDataBiosSetXY(s32 id, s16 x, s16 y);
void menuDataBiosGetXY(s32 id, s16 *x, s16 *y);
u8 menuDataBiosGetDispMax(s32 id);
void menuFaceLoadPokemon(struct Pokemon *pokemon, void *(*callback)(void *), u16 *counter);
void menuFaceDisplay(struct tagWINDOW_WORK *win, struct tagSPRITE_WORK *sprite, u16 faceId);

// Opens a menu on the active window and optionally waits for a selection.
s32 menuOpen(s32 menuID, s32 flag);

s8 menuClose(int menuID);
s32 menuCloseSync(s32 menuID, s32 closeMode);
s8 menuCloseCustom(int menuID, int closeMode, int wait);

// Returns whether the menu window exists.
u8 menuIsOpen(s32 menuID);

#ifdef __cplusplus
long menuOpenCustom(int menuID, int parentID, void *cursor, int flags, bool wait, int mode, ...);
#else
long menuOpenCustom(int menuID, int parentID, void *cursor, int flags, u8 wait, int mode, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif

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

#ifdef __cplusplus
extern "C" {
#endif

// Opens a menu on the active window and optionally waits for a selection.
s32 menuOpen(s32 menuID, s32 waitMode);

s32 menuClose(s32 windowID);

// Returns 1 if the window is absent, 0 after closing it.
s32 menuCloseCustom(s32 windowID, s32 closeFlags, s32 waitMode);

// Returns whether the menu window exists.
u8 menuIsOpen(s32 menuID);

// Returns whether the window still exists; optionally waits for it to close.
u8 menuCloseSync(s32 windowID, u8 waitForClose);

// cursor receives the window cursor word when a selection is made.
s32 menuOpenCustom(s32 menuID, s32 parentWindowID, void *cursor,
                   s32 closeFlags, s32 waitMode, s32 argumentCount, ...);

#ifdef __cplusplus
}
#endif

#endif

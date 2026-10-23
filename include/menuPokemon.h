#ifndef MENU_POKEMON_H
#define MENU_POKEMON_H

#include <types.h>
#include <Pokemon.h>
#include <fightTrainerPokemon.h>
#include <menuPokemonStatus.h>

#include <fightTrainer.h>
#include <fightOutPokemon.h>


enum PartyConditionSprite {
    CONDITION_SPRITE_NONE = 0,
    CONDITION_SPRITE_FAINTED = 126,
    CONDITION_SPRITE_POISON = 0x83,
    CONDITION_SPRITE_BURN = 0x80,
    CONDITION_SPRITE_FREEZE = 0x81,
    CONDITION_SPRITE_PARALYSIS = 0x82,
    CONDITION_SPRITE_SLEEP = 0x85,
};

enum PartyConditionIcon {
    CONDITION_ICON_NONE = 0,
    CONDITION_ICON_FAINTED = 322,
    CONDITION_ICON_POISON = 326,
    CONDITION_ICON_BURN = 323,
    CONDITION_ICON_FREEZE = 324,
    CONDITION_ICON_PARALYSIS = 325,
    CONDITION_ICON_SLEEP = 328,
};

enum PartyPanelIcon {
    PARTY_ICON_PRIMARY = 351,
    PARTY_ICON_SECOND_PRIMARY = 350,
    PARTY_ICON_RESERVE_FIRST = 349,
    PARTY_ICON_RESERVE_SECOND = 348,
    PARTY_ICON_RESERVE_THIRD = 347,
    PARTY_ICON_RESERVE_FOURTH = 346,
    PARTY_ICON_RESERVE_FIFTH = PARTY_ICON_RESERVE_FOURTH,
};

typedef struct PokemonConditionMenuSprite {
    u16 condition;
    u16 spriteId;
} PokemonConditionMenuSprite;

extern const PokemonConditionMenuSprite pokemonConditionMenuSprites[];

// Party-menu entry modes and active submenu.
enum MenuPokemonMode {
    PARTY_MODE_FIELD = 1,
    PARTY_MODE_FIGHT = 2,
    PARTY_MODE_USE_ITEM_FIELD = 3,
    PARTY_MODE_USE_ITEM_FIGHT = 4,
    PARTY_MODE_GIVE_ITEM = 5,
    PARTY_MODE_SELECT_SLOT = 6,
    PARTY_MODE_SELECT_PURIFICATION_READY = 7,
    PARTY_MODE_MOVE_TUTOR = 8,
};

enum MenuPokemonPanelState {
    PARTY_PANEL_EMPTY = -1,
    PARTY_PANEL_NORMAL = 0,
    PARTY_PANEL_FAINTED = 1,
};

enum MenuPokemonPanelLayout {
    PARTY_LAYOUT_ONE_PRIMARY_FIVE_RESERVE = 0,
    PARTY_LAYOUT_TWO_PRIMARY_FOUR_RESERVE = 1,
};

enum MenuPokemonListResult {
    PARTY_LIST_DISABLED = -2,
    PARTY_LIST_CANCELLED = -1,
};

enum MenuPokemonPartySource {
    PARTY_SOURCE_HERO = 0,
    PARTY_SOURCE_FIGHT_TRAINER = 1,
    PARTY_SOURCE_INACTIVE_30_SLOT = 2,
};

enum MenuPokemonSubmenu {
    PARTY_SUBMENU_LIST = 0,
    PARTY_SUBMENU_COMMAND = 1,
    PARTY_SUBMENU_REARRANGE = 2,
    PARTY_SUBMENU_ITEM = 3,
};

enum MenuPokemonOpenState {
    PARTY_OPEN_OVERLAY = 0,
    PARTY_OPEN_FLOOR = 1,
};

enum MenuPokemonFadeState {
    PARTY_FADE_NORMAL = 0,
    PARTY_FADE_FIGHT = 1,
    PARTY_FADE_OFFSCREEN = 2,
};

// Menu ids the party menu opens (menu data table entries).
enum MenuPokemonMenuId {
    MENU_ID_WAZA_LIGHT     = 0x12, // move-panel light window closed with the menu
    MENU_ID_PARTY_RESERVE_FOURTH = 70,
    MENU_ID_PARTY_RESERVE_THIRD = 71,
    MENU_ID_PARTY_RESERVE_SECOND = 72,
    MENU_ID_PARTY_RESERVE_FIRST = 73,
    MENU_ID_PARTY_SECOND_PRIMARY = 74,
    MENU_ID_PARTY_RESERVE_FIFTH = 77,
    MENU_ID_PARTY_PRIMARY  = 0x4B, // first primary party panel
    MENU_ID_PARTY_LIST     = 0x4C, // the party list window
    MENU_ID_PARTY_FIELD_A  = 0x4F, // party field menu, first hero variant
    MENU_ID_PARTY_FIGHT_A  = 0x50, // party fight menu, first variant
    MENU_ID_PARTY_FIGHT_B  = 0x51, // party fight menu, second variant
    MENU_ID_PARTY_FIELD_B  = 0x5A, // party field menu, second hero variant
    MENU_ID_PARTY_HELD_ITEM      = 0x5D, // held-item commands
};


// Command item IDs for the two field-menu layouts.
enum MenuPokemonFieldCommand {
    PARTY_COMMAND_STATUS_A = 0x16F,
    PARTY_COMMAND_STATUS_B = 0x1AD,
    PARTY_COMMAND_REARRANGE_A = 0x171,
    PARTY_COMMAND_REARRANGE_B = 0x1AF,
    PARTY_COMMAND_ITEM = 0x173,
    PARTY_COMMAND_CANCEL_A = 0x175,
    PARTY_COMMAND_CANCEL_B = 0x1B1,
};

enum MenuPokemonHeldItemCommand {
    PARTY_HELD_ITEM_DISMISSED = -1,
    PARTY_HELD_ITEM_GIVE = 0,
    PARTY_HELD_ITEM_TAKE = 1,
    PARTY_HELD_ITEM_CANCEL = 2,
};

enum MenuPokemonPanelOperation {
    PARTY_PANEL_SHOW = 0,
    PARTY_PANEL_HIDE = 1,
    PARTY_PANEL_CHECK_MOVE = 2,
};

enum MenuPokemonListTarget {
    PARTY_LIST_TARGET_OPEN = 1,
    PARTY_LIST_TARGET_CLOSE = 7,
};

// Panel show/hide sequences for the two party panel columns.
enum MenuPokemonPanelTarget {
    PARTY_PANEL_TARGET_RIGHT_SHOW = 0x98,
    PARTY_PANEL_TARGET_RIGHT_HIDE   = 0x9C,
    PARTY_PANEL_TARGET_LEFT_SHOW  = 0xA0,
    PARTY_PANEL_TARGET_LEFT_HIDE    = 0xA4,
    PARTY_PANEL_TARGET_SPLIT_X      = 250,
};

// Floor pushed while the party menu owns the screen.
enum { FLOOR_ID_PARTY_MENU = 0x3A5 };

// Party-menu work block, shared by menuPokemonInitWork and the party-menu
// flow.
struct MenuPokemonWork {
    u8 mMode;             // 0x00: menu mode the party menu was opened in.
    bool mListEnabled;    // 0x01: when zero, the party list cannot be opened.
    u8 mState;            // 0x02: open/close sequence step.
    u8 mFadeState;        // 0x03: floor fade sequence step.
    s8 mPanelLayout;      // 0x04: primary/reserve panel layout.
    s8 mCursor;           // 0x05: cursor before the latest move.
    s8 mCursorIndex;      // 0x06: pokemon the menu currently points at.
    s8 mNextIndex;        // 0x07: requested cursor for the next open.
    s8 mRearrangeSourceSlot;         // 0x08: source party slot while rearranging.
    u8 _09[0x03];         // 0x09-0x0B: unknown.
    s32 mPartySource;     // 0x0C: pokemon list _menuPokemonGetPokemonPtr reads.
    FIGHT_TRAINER *mTrainer; // 0x10: battle party trainer.
    u8 mBagPocket;            // 0x14: bag pocket of the selected item.
    u8 mBagSlot;          // 0x15: bag slot of the item being given.
    u16 mItemId;          // 0x16: item the menu was opened with.
    u8 mSubMenu;          // 0x18: sub-menu step the action handlers report.
    u8 mFightFlag;        // 0x19: selects the fight menu variant.
    u8 _1A[0x02];         // 0x1A-0x1B: unknown.
    FightOutPokemon *mOutgoingPokemon; // 0x1C: deployed Pokemon being replaced.
    s32 mResult;          // 0x20: result the party menu closed with.
    u8 mSelecting;        // 0x24: set while the move-select sub menu runs.
    u8 _25[0x03];         // 0x25-0x27: unknown.
};

// One party panel record the panel windows carry as their parameter. Filled by
// pokemonToMenuPokemonStatus.
struct MENU_POKEMON_STATUS {
    u16 mName[11];        // 0x00: pokemon name, empty slot when cleared.
    u8 _16;               // 0x16: unknown.
    u8 mLevel;            // 0x17: level.
    s16 mMaxHp;           // 0x18: max HP.
    s16 mHp;              // 0x1A: current HP.
    union {
        u32 mLevelExpSpan; // next-level threshold minus current-level threshold.
        f32 mShadowGauge;
    } mProgressRange;     // 0x1C
    union {
        u32 mExpIntoLevel; // accumulated EXP above the current-level threshold.
        f32 mPurification; // pokemonGetDp value for shadow Pokemon.
    } mProgressValue;     // 0x20
    u16 mJoutaiId;        // 0x24: joutai texture drawn on the panel.
    u16 mFaceId;          // 0x26: ball face id from menuFaceGetID.
    u8 mSex;              // 0x28: gender mark id.
    u8 mBallType;         // 0x29: 0 normal, 1 shadow, 2 hyper shadow.
    u16 mHeldItem;        // 0x2A: held item id.
    u8 _2C[0x04];         // 0x2C-0x2F: unknown.
};

// The six party panel records, indexed by party slot.
struct MENU_POKEMON_STATUS_TBL {
    MENU_POKEMON_STATUS mStatus[6];
};

// One party panel window entry; menu_info holds one table per panel layout.
struct MENU_INFO {
    s16 mWindowId;        // 0x00: window opened for the slot.
    s16 mSlotIcon;        // 0x02: slot icon id passed to the window hook.
    s16 mX;               // 0x04: slot x position.
    s16 mY;               // 0x06: slot y position.
};

// Callback type the party status menu reports cursor moves through.
const MENU_POKEMON_CB_TYPE MENU_POKEMON_CB_NONE = (MENU_POKEMON_CB_TYPE)0;
const MENU_POKEMON_CB_TYPE MENU_POKEMON_CB_PREV = MENU_POKEMON_CB_CURSOR_LEFT;
const MENU_POKEMON_CB_TYPE MENU_POKEMON_CB_NEXT = MENU_POKEMON_CB_CURSOR_RIGHT;

// Ball sprite variants a party panel can show (MENU_POKEMON_STATUS::mBallType).
enum MenuPokemonBallType {
    POKEMON_BALL_TYPE_NORMAL = 0,
    POKEMON_BALL_TYPE_SHADOW = 1,
    POKEMON_BALL_TYPE_HYPER  = 2,
};

#ifdef __cplusplus
extern "C" {
#endif

extern MenuPokemonWork menuPokemonWork;
extern MENU_INFO menu_info[2][6];

// faceSelection is forwarded to the Pokemon face selector.
void pokemonToMenuPokemonStatus(Pokemon *pokemon, FightPokemon *fightPokemon,
                                MENU_POKEMON_STATUS *status, s32 faceSelection);

void menuPokemonInitWork(void);
s32 menuPokemonOpenItemGive(u8 bagPocket, u8 bagSlot, u16 itemId, FIGHT_TRAINER *trainer);
s32 menuPokemonOpenItemUse(u8 mode, bool listEnabled, u16 itemId, FIGHT_TRAINER *trainer);
u8 menuPokemonCheckPokemonChange(FightOutPokemon *outgoingPokemon, FIGHT_TRAINER *trainer, s32 index);
s32 menuPokemonOpenFight(u8 flag, u8 battleSelection, FIGHT_TRAINER *trainer, FightOutPokemon *outgoingPokemon);
s32 menuPokemonOpen(u8 mode, u16 itemId, FIGHT_TRAINER *trainer);
void menuPokemonClose(void);
u16 menuPokemonGetJoutaiSpriteID(Pokemon *pokemon, bool sprite);

// Party menu entry points the rest of the game and the menu data call.
void menuPokemonRefreshStatus(void);
s32 menuPokemonCtrlWaza(void);
void menuPokemonCloseWazaLight(void);
s32 menuPokemonOpenWazaLight(s8 index);
void menuPokemonDrawItem(tagWINDOW_WORK *win, tagSPRITE_WORK *sprite);
void menuPokemonDrawCur(tagWINDOW_WORK *win, tagSPRITE_WORK *sprite);
void menuPokemonDrawHelp(tagWINDOW_WORK *win, tagSPRITE_WORK *sprite);
void menuPokemonButton(tagWINDOW_WORK *win);
void menuPokemonCursor(tagWINDOW_WORK *win);
s32 menuPokemonCtrl(tagWINDOW_WORK *win);
s32 menuPokemonMain(void);

#ifdef __cplusplus
}
#endif

#endif

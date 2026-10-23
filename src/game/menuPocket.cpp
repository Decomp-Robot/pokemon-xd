#include <menuPokemon.h>
#include <libc/memory.h>
#include <GSmsg.h>
#include <sprite.h>
#include <GSsnd.h>
#include <winMsg.h>
#include <msgctrl.h>
#include <fade.h>
#include <menuSub.h>
#include <floor.h>
#include <floorManager.h>
#include <time.h>
#include <menu.h>
#include <menuPocket2.h>
#include <menuPocketBattleDisk.h>
#include <menuItemBios.h>
#include <itemData.h>
#include <winSprite.h>
#include <cursorBios.h>
#include <hero.h>
#include <heroBios.h>
#include <heroItem.h>
#include <pcbox.h>
#include <menuPokemon.h>
#include <GSgfx.h>
#include <menuPocketTool.h>
#include <menuOffScreen.h>
#include <menuShop.h>
#include <window.h>
#include <item.h>
#include <itemBios.h>
#include <menuSub.h>
#include <floor.h>
#include <waza.h>
#include <fightTrainerPokemon.h>
#include <winSeq.h>

extern "C" {
u16 windowGetKeyTrg(void);
u16 windowGetKeyRepeat(void);
GSgfxLayerID winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
}

// The pocket menu: the item list, the action menu an item opens and the give,
// sell, deposit and use flows behind it.
//
// Which items the list holds is decided by the tab it shows; one tab is one
// entry of lPocketTabData. A tab names the item kind whose array the hero
// holds it in, the cursor its list is drawn with, the message that explains
// it, the action menus it offers and the routine that prints one of its rows.

// One row of an action menu. Result 0 finishes a battle selection but keeps
// the field pocket open; 1 keeps either pocket open; 2 closes either pocket.
// The amount consumed is written through the third argument.
struct SActionMenuItem {
    enum ActionFlags {
        REQUIRE_USABLE_ITEM = 1,
        HIDE_FOR_IMPORTANT_ITEM = 2
    };

    int mMsgID; // 0x00: label message id.
    int (*mFunc)(int, int, int *); // 0x04: handler; null ends the table.
    u16 mFlags; // 0x08: bit0 only for usable items, bit1 hidden for
                //       important items.
};

// Handler of one action-menu row.
typedef int (*SActionMenuFunc)(int, int, int *);

// Print routine of one list row: layer to draw in, row position, tab and item
// index inside the tab.
typedef void (*PocketItemPrintFunc)(GSgfxLayerID layer, int x, int y, int tab, int index);

// One tab of the item list, 0x20 bytes.
struct PocketTabData {
    u8 mItemKind; // 0x00: item kind held by the tab; 0x80 and above mark the
                  //       items that live in the hidden-machine list.
    u8 mSwapAble; // 0x01: items of the tab may be swapped.
    u8 mSortAble; // 0x02: items of the tab may be sorted.
    u8 _03; // 0x03: unknown.
    u32 mTabItemID; // 0x04: tab menu item.
    u32 mActiveTabItemID; // 0x08: highlighted tab item.
    PocketItemPrintFunc mPrint; // 0x0C: prints one row of the tab.
    u16 mCursorID; // 0x10: cursor the tab's list is drawn with.
    u16 _12; // 0x12: unknown.
    u32 mTitleMsgID; // 0x14: title of the tab.
    const SActionMenuItem *mActMenu; // 0x18: rows offered in the field menu.
    const SActionMenuItem *mActMenu2; // 0x1C: rows offered in battle.
};

extern const PocketTabData lPocketTabData[];

// Cursor position (0, 0) a tab's cursor is put back to when its list shrank.
extern "C" CURSOR pocketTabCursorResetPosition;

// Measured columns of the action-menu rows.
struct ActMenuResizeEntry {
    u32 mItemID; // 0x00: menu-data id of the row.
    s16 mRowOffset; // 0x04: row y offset.
    s16 mColumnExtent; // 0x06: column layout extent.
    u8 mFlag; // 0x08: set for the rows the window draws itself.
    u8 _09[0x03]; // 0x09-0x0B: padding.
};

namespace {
enum NumberMenuItemID {
    NUMBER_TWO_DIGIT_TENS_CURSOR = 0xE5,
    NUMBER_TWO_DIGIT_UNITS_CURSOR = 0xE6,
    NUMBER_TWO_DIGIT_TENS = 0xE7,
    NUMBER_TWO_DIGIT_UNITS = 0xE8,
    NUMBER_THREE_DIGIT_HUNDREDS_CURSOR = 0xEE,
    NUMBER_THREE_DIGIT_TENS_CURSOR = 0xEF,
    NUMBER_THREE_DIGIT_UNITS_CURSOR = 0xF0,
    NUMBER_THREE_DIGIT_HUNDREDS = 0xF1,
    NUMBER_THREE_DIGIT_TENS = 0xF2,
    NUMBER_THREE_DIGIT_UNITS = 0xF3,
    PRICE_TWO_DIGIT_TENS_CURSOR = 0xFE,
    PRICE_TWO_DIGIT_UNITS_CURSOR = 0xFF,
    PRICE_TWO_DIGIT_TENS = 0x100,
    PRICE_TWO_DIGIT_UNITS = 0x101,
    PRICE_THREE_DIGIT_HUNDREDS_CURSOR = 0x3A7,
    PRICE_THREE_DIGIT_TENS_CURSOR = 0x3A8,
    PRICE_THREE_DIGIT_UNITS_CURSOR = 0x3A9,
    PRICE_THREE_DIGIT_HUNDREDS = 0x3AA,
    PRICE_THREE_DIGIT_TENS = 0x3AB,
    PRICE_THREE_DIGIT_UNITS = 0x3AC,
    ACTION_ROW_1_TEXT = 0xE0,
    ACTION_ROW_2_TEXT = 0xDF,
    ACTION_ROW_3_TEXT = 0xDE,
    ACTION_ROW_4_TEXT = 0xDD,
    ACTION_ROW_1_CURSOR = 0xDC,
    ACTION_ROW_2_CURSOR = 0xDB,
    ACTION_ROW_3_CURSOR = 0xDA,
    ACTION_ROW_4_CURSOR = 0xD9
};

ActMenuResizeEntry ActMenuResizeWork[7] = {
    {0xCF, 0, 0, 1, {0, 0, 0}},
    {0xD0, 0, 0, 0, {0, 0, 0}},
    {0xD1, 0, 0, 0, {0, 0, 0}},
    {0xD5, 0, 0, 1, {0, 0, 0}},
    {0xD4, 0, 0, 1, {0, 0, 0}},
    {0xD6, 0, 0, 0, {0, 0, 0}},
    {ACTION_ROW_4_CURSOR, 0, 0, 0, {0, 0, 0}},
};
} // namespace

// Window ids of the pocket menus.
enum {
    WIN_ID_SELECT_ITEM = 0x2C,
    WIN_ID_ACTION_MENU = 0x2D,
    WIN_ID_NUMBER_ITEM_TWO_DIGITS = 0x2E,
    WIN_ID_NUMBER_ITEM_THREE_DIGITS = 0x2F,
    WIN_ID_NUMBER_PRICE_TWO_DIGITS = 0x30,
    WIN_ID_NUMBER_PRICE_THREE_DIGITS = 0x31,
};

// Item-action message substitution variables.
enum {
    MSG_VAR_ITEM_ID = 0x2D,
    MSG_VAR_ITEM_QUANTITY = 0x2F,
};

// Message codes the shop messages are built with.
enum MENU_SHOP_MSG_CODE {
    MENU_MSG_ITEM_NUM = 0x0B,
    MENU_MSG_ITEM_PRICE = 0x0D,
    MENU_MSG_SELL_TOTAL = 0x09,
    MENU_MSG_SELL_CONFIRM = 0x0A,
};

// Menu-data id and row id the action-menu row layout is read from.
enum {
    MENU_DATA_ACT_ROW = 0x2C,
    MENU_DATA_ACT_ROW_ITEM = 0xB4,
};

// Quantity defaults and digit layout of the number-input windows.
enum {
    POCKET_NUMBER_INITIAL_QUANTITY = 1,
    POCKET_NUMBER_TWO_DIGITS = 2,
    POCKET_NUMBER_THREE_DIGITS = 3,
    POCKET_NUMBER_TWO_DIGIT_INITIAL_CURSOR = 1,
    POCKET_NUMBER_THREE_DIGIT_INITIAL_CURSOR = 2,
};

// Key item whose use result may close the pocket.
enum {
    ITEM_KEY_ITEM_CLOSE_EXCEPTION = 0x1F6,
};

enum PocketItemMessageID {
    POCKET_MSG_CANNOT_GIVE_ITEM = 0xC37A,
    POCKET_MSG_CANNOT_USE_ITEM = 0xC374,
    POCKET_MSG_QUANTITY_MARKER = 0x428F,
    POCKET_MSG_PC_ITEM_CAPACITY_EXCEEDED = 0x3AC0,
    POCKET_MSG_ITEM_DEPOSITED = 0xC37C,
    POCKET_MSG_BATTLE_DISK_PROMPT = 0x3C1B,
    POCKET_MSG_COLOGNE_PROMPT = 0x3AB8,
    POCKET_MSG_ITEM_ACTION_PROMPT = 0x4296,
    POCKET_MSG_DISCARD_QUANTITY_PROMPT = 0x3AAA,
    POCKET_MSG_DISCARD_CONFIRM_PROMPT = 0x3AAB,
    POCKET_MSG_ITEM_DISCARDED = 0xC379,
    POCKET_MSG_HIDDEN_MACHINE_PREFIX = 0x3AA0,
    POCKET_MSG_ITEM_NUMBER_PREFIX = 0x3A9F,
    POCKET_MSG_DEPOSIT_EXIT_LABEL = 0x3BBA,
    POCKET_MSG_EXIT_LABEL = 0x3A9E
};

// The cursor word is followed by the number window's argument words.
enum PocketNumberWindowArgument {
    POCKET_NUMBER_ARG_WINDOW_ID,
    POCKET_NUMBER_ARG_MAX_VALUE,
    POCKET_NUMBER_ARG_UNIT_PRICE,
    POCKET_NUMBER_ARG_COUNT
};

struct PocketNumberInputParam {
    int mCursor;
    int mWindowID;
    int mMaximumQuantity;
    int mUnitPrice;
};


// ---- engine and library entry points -------------------------------------



// ---- item, menu and hero data -------------------------------------------


extern "C" void heroAddPokedoru(Hero *hero, u32 money);
extern "C" u16 menuPocketCologne(int a);
// The throw-away menu is handed to _actionMenuOpen as one table.
struct SActionMenuTable {
    SActionMenuItem mItems[2];
};

extern const SActionMenuTable sActionMenuSuteru; // rows of the throw-away menu.
namespace { const f32 pocketScrollZero = 0.0f; }
namespace { const f32 pocketShopExitFadeTime = 0.5f; }


struct Pokemon;
struct FightPokemon;


struct PocketNumberDigit {
    int mDigitItemID;
    int mCursorTopItemID;
    int mCursorBottomItemID;
};
struct PocketNumberMenu {
    int mWindowID;
    const PocketNumberDigit *mDigits;
    int mDigitCount;
};
namespace { extern const PocketNumberMenu MenuPocket2Num[4]; }

enum {
    POCKET_MODE_SHOP_SELL = POCKET_MODE_SELL,
    POCKET_MODE_GIVE_RESTRICTED = POCKET_MODE_GIVE_FIXED_ORDER,
    POCKET_MODE_SNATCH_ONLY_BATTLE = 6,
    POKEMON_ITEM_USE_PARTY = 3,
    POKEMON_ITEM_USE_BATTLE = 4,
    PARTY_SLOT_INDEX_MASK = 0xFFFF,
    POCKET_NUMBER_MENU_COUNT = 4,
    POCKET_NUMBER_STATE_OPEN = 0,
    POCKET_NUMBER_STATE_CLOSE = 3,
    POCKET_PRICE_MSG = 0x434F,
    POCKET_DIGIT_MSG = 0x4270,
    MSG_VALUE_PRICE = 0x50,
    MSG_VALUE_DIGIT = 0x34,
    POCKET_NUMBER_OPEN_SEQ = 0x70,
    POCKET_NUMBER_CLOSE_SEQ = 0x74,
    POCKET_NUMBER_CURSOR_SOUND = 0x10,
    POCKET_NUMBER_UNUSED_CURSOR_SOUND = 0x44D,
    POCKET_KEY_UP = 1,
    POCKET_KEY_DOWN = 2,
    POCKET_KEY_LEFT = 4,
    POCKET_KEY_RIGHT = 8
};


namespace {
int _actTukauNormal(int tab, int menuPos, int *consumed);
int _actMotaseruNormal(int tab, int menuPos, int *consumed);
int _actSuteruNormal(int tab, int menuPos, int *consumed);
void _printItemTaisetsu(GSgfxLayerID layer, int x, int y, int tab, int index);

enum PocketMessageID {
    POCKET_ACTION_USE = 0x4292,
    POCKET_ACTION_GIVE = 0x4293,
    POCKET_ACTION_DISCARD = 0x4294,
    POCKET_ACTION_CANCEL = 0x4295,
    POCKET_DISCARD_CONFIRM = 0x426A,
    POCKET_DISCARD_CANCEL = 0x426B,
    POCKET_ITEMS_TAB_NAME = 0x3A99,
    POCKET_BALLS_TAB_NAME = 0x3A9A,
    POCKET_MACHINES_TAB_NAME = 0x3A9B,
    POCKET_BERRIES_TAB_NAME = 0x3A9C,
    POCKET_KEY_ITEMS_TAB_NAME = 0x3A9D,
    POCKET_FIELD_EMPTY_MESSAGE = 0x3AA1,
    POCKET_BATTLE_EMPTY_MESSAGE = 0x3AA2,
    POCKET_GIVE_EMPTY_MESSAGE = 0x3AA7,
    POCKET_SELL_EMPTY_MESSAGE = 0x3AA8,
    POCKET_DEPOSIT_EMPTY_MESSAGE = 0x3AB7
};

enum PocketItemKind {
    POCKET_BALLS = 1,
    POCKET_ITEMS = 2,
    POCKET_BERRIES = 3,
    POCKET_MACHINES = 4,
    POCKET_KEY_ITEMS = 5
};

enum PocketTabIndex {
    POCKET_TAB_KEY_ITEMS = 4
};

// Action results are translated to the item-selection loop result below.
enum PocketActionResult {
    POCKET_ACTION_COMPLETE = 0,
    POCKET_ACTION_CONTINUE = 1,
    POCKET_ACTION_CLOSE = 2
};

enum PocketSelectionResult {
    POCKET_SELECTION_AGAIN = 0,
    POCKET_SELECTION_LEAVE = 1
};

enum PocketTabMenuItemID {
    POCKET_ITEMS_TAB = 0x97,
    POCKET_BALLS_TAB = 0x98,
    POCKET_MACHINES_TAB = 0x99,
    POCKET_BERRIES_TAB = 0x9A,
    POCKET_KEY_ITEMS_TAB = 0x158,
    POCKET_ITEMS_ACTIVE_TAB = 0x9F,
    POCKET_BALLS_ACTIVE_TAB = 0x9E,
    POCKET_MACHINES_ACTIVE_TAB = 0x9D,
    POCKET_BERRIES_ACTIVE_TAB = 0x9C,
    POCKET_KEY_ITEMS_ACTIVE_TAB = 0x9B
};

enum PocketCursorID {
    POCKET_ITEMS_CURSOR = 3,
    POCKET_BALLS_CURSOR = 4,
    POCKET_MACHINES_CURSOR = 5,
    POCKET_BERRIES_CURSOR = 6,
    POCKET_KEY_ITEMS_CURSOR = 7
};

const SActionMenuItem FieldItemActions[] = {
    {POCKET_ACTION_USE, _actTukauNormal, 0},
    {POCKET_ACTION_GIVE, _actMotaseruNormal, SActionMenuItem::HIDE_FOR_IMPORTANT_ITEM},
    {POCKET_ACTION_DISCARD, _actSuteruNormal, 0},
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem FieldBallActions[] = {
    {POCKET_ACTION_GIVE, _actMotaseruNormal, 0},
    {POCKET_ACTION_DISCARD, _actSuteruNormal, 0},
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem FieldMachineActions[] = {
    {POCKET_ACTION_USE, _actTukauNormal, 0},
    {POCKET_ACTION_GIVE, _actMotaseruNormal, 0},
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem FieldBerryActions[] = {
    {POCKET_ACTION_USE, _actTukauNormal, 0},
    {POCKET_ACTION_GIVE, _actMotaseruNormal, 0},
    {POCKET_ACTION_DISCARD, _actSuteruNormal, 0},
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem FieldKeyItemActions[] = {
    {POCKET_ACTION_USE, _actTukauNormal, 0},
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem BattleItemActions[] = {
    {POCKET_ACTION_USE, _actTukauNormal, SActionMenuItem::REQUIRE_USABLE_ITEM},
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem BattleBallActions[] = {
    {POCKET_ACTION_USE, _actTukauNormal, 0},
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem BattleMachineActions[] = {
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem BattleBerryActions[] = {
    {POCKET_ACTION_USE, _actTukauNormal, 0},
    {POCKET_ACTION_CANCEL, 0, 0}
};
const SActionMenuItem BattleKeyItemActions[] = {
    {POCKET_ACTION_CANCEL, 0, 0}
};
} // namespace

namespace { void _printItemDouguBall(GSgfxLayerID layer, int x, int y, int tab, int index); }
namespace { void _printItemWazaMachine(GSgfxLayerID layer, int x, int y, int tab, int index); }
namespace { void _printItemKinomi(GSgfxLayerID layer, int x, int y, int tab, int index); }

const PocketTabData lPocketTabData[] = {
    {POCKET_ITEMS, 1, 1, 0, POCKET_ITEMS_TAB, POCKET_ITEMS_ACTIVE_TAB,
     _printItemDouguBall, POCKET_ITEMS_CURSOR, 0, POCKET_ITEMS_TAB_NAME,
     FieldItemActions, BattleItemActions},
    {POCKET_BALLS, 1, 1, 0, POCKET_BALLS_TAB, POCKET_BALLS_ACTIVE_TAB,
     _printItemDouguBall, POCKET_BALLS_CURSOR, 0, POCKET_BALLS_TAB_NAME,
     FieldBallActions, BattleBallActions},
    {POCKET_MACHINES, 0, 0, 0, POCKET_MACHINES_TAB, POCKET_MACHINES_ACTIVE_TAB,
     _printItemWazaMachine, POCKET_MACHINES_CURSOR, 0, POCKET_MACHINES_TAB_NAME,
     FieldMachineActions, BattleMachineActions},
    {POCKET_BERRIES, 0, 0, 0, POCKET_BERRIES_TAB, POCKET_BERRIES_ACTIVE_TAB,
     _printItemKinomi, POCKET_BERRIES_CURSOR, 0, POCKET_BERRIES_TAB_NAME,
     FieldBerryActions, BattleBerryActions},
    {POCKET_KEY_ITEMS, 1, 0, 0, POCKET_KEY_ITEMS_TAB, POCKET_KEY_ITEMS_ACTIVE_TAB,
     _printItemTaisetsu, POCKET_KEY_ITEMS_CURSOR, 0, POCKET_KEY_ITEMS_TAB_NAME,
     FieldKeyItemActions, BattleKeyItemActions}
};

namespace {



const PocketNumberDigit NumWinItem2[] = {
    {NUMBER_TWO_DIGIT_TENS, NUMBER_TWO_DIGIT_TENS_CURSOR, 0},
    {NUMBER_TWO_DIGIT_UNITS, NUMBER_TWO_DIGIT_UNITS_CURSOR, 0}
};
const PocketNumberDigit NumWinItem3[] = {
    {NUMBER_THREE_DIGIT_HUNDREDS, NUMBER_THREE_DIGIT_HUNDREDS_CURSOR, 0},
    {NUMBER_THREE_DIGIT_TENS, NUMBER_THREE_DIGIT_TENS_CURSOR, 0},
    {NUMBER_THREE_DIGIT_UNITS, NUMBER_THREE_DIGIT_UNITS_CURSOR, 0}
};
const PocketNumberDigit NumPriceWinItem2[] = {
    {PRICE_TWO_DIGIT_TENS, PRICE_TWO_DIGIT_TENS_CURSOR, 0},
    {PRICE_TWO_DIGIT_UNITS, PRICE_TWO_DIGIT_UNITS_CURSOR, 0}
};
const PocketNumberDigit NumPriceWinItem3[] = {
    {PRICE_THREE_DIGIT_HUNDREDS, PRICE_THREE_DIGIT_HUNDREDS_CURSOR, 0},
    {PRICE_THREE_DIGIT_TENS, PRICE_THREE_DIGIT_TENS_CURSOR, 0},
    {PRICE_THREE_DIGIT_UNITS, PRICE_THREE_DIGIT_UNITS_CURSOR, 0}
};
} // namespace
namespace {
const PocketNumberMenu MenuPocket2Num[] = {
    {WIN_ID_NUMBER_ITEM_TWO_DIGITS, NumWinItem2, POCKET_NUMBER_TWO_DIGITS},
    {WIN_ID_NUMBER_ITEM_THREE_DIGITS, NumWinItem3, POCKET_NUMBER_THREE_DIGITS},
    {WIN_ID_NUMBER_PRICE_TWO_DIGITS, NumPriceWinItem2, POCKET_NUMBER_TWO_DIGITS},
    {WIN_ID_NUMBER_PRICE_THREE_DIGITS, NumPriceWinItem3, POCKET_NUMBER_THREE_DIGITS}
};
} // namespace


namespace {
struct PocketExitMessageTable { u32 mMessages[7]; };
const PocketExitMessageTable PocketExitMessages = {
    {POCKET_FIELD_EMPTY_MESSAGE, POCKET_BATTLE_EMPTY_MESSAGE, POCKET_GIVE_EMPTY_MESSAGE, POCKET_SELL_EMPTY_MESSAGE, POCKET_DEPOSIT_EMPTY_MESSAGE, POCKET_GIVE_EMPTY_MESSAGE, POCKET_BATTLE_EMPTY_MESSAGE}
};
struct PocketActionItemTable { int mItems[4]; };
const PocketActionItemTable ActionTextItems = {{ACTION_ROW_1_TEXT, ACTION_ROW_2_TEXT, ACTION_ROW_3_TEXT, ACTION_ROW_4_TEXT}};
const PocketActionItemTable ActionCursorItems = {{ACTION_ROW_1_CURSOR, ACTION_ROW_2_CURSOR, ACTION_ROW_3_CURSOR, ACTION_ROW_4_CURSOR}};
struct PocketCursorAnimationValues { f32 mValues[2]; };
static inline PocketCursorAnimationValues pocketListCursorDefaults()
{
    const PocketCursorAnimationValues values = {{0, 0}};
    return values;
}

static inline PocketCursorAnimationValues pocketActionCursorDefaults()
{
    const PocketCursorAnimationValues values = {{0, 0}};
    return values;
}
} // namespace



const SActionMenuTable sActionMenuSuteru = {
    {{POCKET_DISCARD_CONFIRM, 0, 0}, {POCKET_DISCARD_CANCEL, 0, 0}}
};

// Item-script queue and the pocket window's shared state.
MenuPocket2ItemScript menuPocketWork;
MenuPocket2Work Data;
extern "C" CURSOR pocketTabCursorResetPosition = {0, 0};



namespace {
u16 _getItemIDFromMenuPos(int tab, int menuPos);
} // namespace




// Whether the sort cursor is drawn: the tab still has an item under the
// cursor and no action has been chosen yet.
extern "C" u8 menuPocketIsSortCursorDisp(tagWINDOW_WORK *window)
{
    CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);

    if (Data.mExchangeSlot >= 0) {
        return 0;
    }

    int menuPos = pos.x + pos.y;

    if (_getItemIDFromMenuPos(window->mCursor.y, menuPos) == 0 && menuPos == 0) {
        return 0;
    }
    return 1;
}

// Whether the swap cursor is drawn: an item under the cursor or an action
// already chosen.
extern "C" u8 menuPocketIsIrekaeCursorDisp(tagWINDOW_WORK *window)
{
    CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    int menuPos = pos.x + pos.y;

    if (_getItemIDFromMenuPos(window->mCursor.y, menuPos) == 0
        && Data.mExchangeSlot < 0) {
        return 0;
    }
    return 1;
}

// Pocket-menu entry points and file-local helpers.
namespace {
int _getNbItemSlot(int tab);
void _sortItemsThisTab(u8 tab);
void _swapItemSlot(int item, int from, int to);
u32 _getMoney(void);
int _actSuteruNormal(int tab, int menuPos, int *num);
int _actMotaseruNormal(int tab, int menuPos, int *num);
int _actTukauNormal(int tab, int menuPos, int *num);
} // namespace

namespace { u32 _getItemInstMsg(unsigned long itemID); }
namespace {
void _setExplanationMsgID(unsigned long msgID);
} // namespace

namespace { void _printItemList(GSgfxLayerID layer, int x, int tab, CURSOR &pos); }
namespace { void _printItemDouguBall(GSgfxLayerID layer, int x, int y, int tab, int index); }
namespace {
void _printItemTaisetsu(GSgfxLayerID layer, int x, int y, int tab, int index);
} // namespace

namespace { void _printItemWazaMachine(GSgfxLayerID layer, int x, int y, int tab, int index); }
namespace { void _printItemKinomi(GSgfxLayerID layer, int x, int y, int tab, int index); }
namespace {
u16 _menuPocketLoop(void);
} // namespace


// File-local helpers of the pocket menu, declared before use.
namespace {

u16 _getItemNumFromMenuPos(int tab, int menuPos);
int _getStockMaxFromTagID(int tab);
int _makeActMenu(SActionMenuItem *menu, int max, int tab, int menuPos);
int _actionMenuOpen(const SActionMenuItem *menu, int num);
int _openNumberInputMenu(int initialQuantity, int maxQuantity, int digitCount, int unitPrice);
u16 _selectItem(int &menuPos, long *slotID);
int _execActionMenu(int tab, u16 itemID, int menuPos, u16 *outNum);
int _execDeposit(int tab, u16 itemID, int menuPos);
int _execSell(int tab, u16 itemID, int menuPos);
int _execGive(int tab, u16 itemID, int menuPos);
int _afterDecideItem(int mode, int tab, u16 itemID, int menuPos, u16 *outNum);
void _setActMenuResizeWork(void);
void _decItem(u16 itemID, int kind, int menuPos, int count);
u8 _depositPC(u16 itemID, int count);
int _getItemPrice(int itemID);
PocketItemUseFunc _getUseFunc(unsigned long itemID);
int _openPokemonMenuForGiveItem(u16 itemID, u8 pocket, u8 menuItemIndex);
u8 _getItemKind(int itemID);
u8 _getItemDeposit(u16 itemID);
int _getItemNameMsg(unsigned long itemID);
}

namespace {

// Message callback of the error window the shop actions raise.
void _infoOnly(unsigned long msgID)
{
    winMsgOpen(2, msgID, 1, 0);
    winMsgClose(1);
}

// Plays the information beep and shows one message window.
void _infoBeep(unsigned long msgID)
{
    menuPtPlaySe(0x13);
    winMsgOpen(2, msgID, 1, 0);
    winMsgClose(1);
}

// Lays the action menu of the selected item out: the rows of the tab table
// the item may be offered. Returns the number of rows.
int _makeActMenu(SActionMenuItem *menu, int max, int tab, int menuPos)
{
    ItemData *data = itemDataBiosGetPtr(_getItemIDFromMenuPos(tab, menuPos));
    u8 usable;

    if (itemDataBiosGetBattleUseFunc(data) != 0) {
        usable = 1;
    } else {
        usable = 0;
    }

    int mode = Data.mMode;
    const PocketTabData *tabData = &lPocketTabData[Data.mTab];
    const SActionMenuItem *src;

    if (mode == POCKET_MODE_BATTLE || mode == POCKET_MODE_SNATCH_ONLY_BATTLE) {
        src = tabData->mActMenu2;
    } else if (mode == POCKET_MODE_FIELD) {
        src = tabData->mActMenu;
    }

    int num = 0;

    while (src[num].mFunc != 0) {
        num++;
    }
    num++;

    int count = 0;

    for (int i = 0; i < num; i++) {
        if ((src[i].mFlags & SActionMenuItem::REQUIRE_USABLE_ITEM) && !usable) {
            continue;
        }
        if (itemDataBiosGetImportant(data) != 0 && (src[i].mFlags & SActionMenuItem::HIDE_FOR_IMPORTANT_ITEM)) {
            continue;
        }
        menu[count] = src[i];
        menu[count].mFlags = 0;
        count++;
    }
    return count;
}

// Fills the measured columns of the action-menu rows from the item data.
void _setActMenuResizeWork(void)
{
    for (u32 i = 0; i < 7; i++) {
        ActMenuResizeWork[i].mRowOffset =
            menuItemBiosGetPtr(MENU_DATA_ACT_ROW, ActMenuResizeWork[i].mItemID)->mOffsetA;
        ActMenuResizeWork[i].mColumnExtent =
            menuItemBiosGetPtr(MENU_DATA_ACT_ROW, ActMenuResizeWork[i].mItemID)->mHeight;
    }
}

// Puts the chosen number of the item into the PC. Returns 0 when the action
// finished (or failed), 1 while the pocket should go on.
int _execDeposit(int tab, u16 itemID, int menuPos)
{
    if (!_getItemDeposit(itemID)) {
        msgctrlSetValue(MSG_VAR_ITEM_ID, itemID);
        _infoBeep(POCKET_MSG_CANNOT_DEPOSIT_ITEM);
        return 0;
    }

    int num;

    if ((u8)lPocketTabData[Data.mTab].mItemKind != POCKET_KEY_ITEMS) {
        _setExplanationMsgID(POCKET_MSG_DEPOSIT_QUANTITY);

        int digitCount;

        if (_getStockMaxFromTagID(tab) > 100) {
            digitCount = POCKET_NUMBER_THREE_DIGITS;
        } else {
            digitCount = POCKET_NUMBER_TWO_DIGITS;
        }

        num = _openNumberInputMenu(POCKET_NUMBER_INITIAL_QUANTITY, _getItemNumFromMenuPos(tab, menuPos), digitCount, 0);
    } else {
        num = 1;
    }

    if (num < 0) {
        return 0;
    }

    if (!_depositPC(itemID, num)) {
        msgctrlSetValue(MSG_VAR_ITEM_ID, itemID);
        _infoBeep(POCKET_MSG_PC_ITEM_CAPACITY_EXCEEDED);
        return 0;
    }

    _decItem(itemID, lPocketTabData[Data.mTab].mItemKind, menuPos, num);
    msgctrlSetValue(MSG_VAR_ITEM_ID, itemID);
    msgctrlSetValue(MSG_VAR_ITEM_QUANTITY, num);
    _infoOnly(POCKET_MSG_ITEM_DEPOSITED);
    return 1;
}

// Sells the chosen number of the item.
int _execSell(int tab, u16 itemID, int menuPos)
{
    int window = Data.mMsgWindowID;
    int price = _getItemPrice(itemID);
    u8 se;

    if (price <= 0) {
        menuPtPlaySe(0x13);
        int msg = menuShopGetMsgID(&se, window, MENU_MSG_ITEM_NUM, 0x2D, itemID, -1);
        winMsgOpenWithSE(2, msg, 1, 0, se);
        winMsgClose(1);
        return 0;
    }

    _setExplanationMsgID(menuShopGetMsgID(&se, window, MENU_MSG_ITEM_PRICE, 0x2D, itemID, -1));

    int digitCount;

    if (_getStockMaxFromTagID(tab) > 100) {
        digitCount = POCKET_NUMBER_THREE_DIGITS;
    } else {
        digitCount = POCKET_NUMBER_TWO_DIGITS;
    }

    int maxQuantity = _getItemNumFromMenuPos(tab, menuPos);
    int unitPrice = price / 2;
    int num = _openNumberInputMenu(POCKET_NUMBER_INITIAL_QUANTITY, maxQuantity, digitCount, unitPrice);

    if (num < 0) {
        return 0;
    }

    int total = unitPrice * num;

    int msg = menuShopGetMsgID(&se, window, MENU_MSG_SELL_TOTAL, 0x4B, total, -1);
    winMsgOpenWithSE(2, msg, 1, 0, se);

    s8 answer = (s8)menuSubOpenYesNo(0, -1, -1, 0);

    if (answer == 1 || answer == -1) {
        winMsgClose(1);
        return 0;
    }

    _decItem(itemID, lPocketTabData[Data.mTab].mItemKind, menuPos, num);
    GSsndPlay(0x1A, 0, 0);
    heroAddPokedoru(Data.mHero, total);
    msg = menuShopGetMsgID(&se, window, MENU_MSG_SELL_CONFIRM, 0x2D, itemID, 0x4B, total, -1);
    _infoOnly(msg);
    return 1;
}

// Hands the item to a pokemon.
int _execGive(int tab, u16 itemID, int menuPos)
{
    if (itemDataBiosGetImportant(itemDataBiosGetPtr(itemID)) != 0) {
        msgctrlSetValue(MSG_VAR_ITEM_ID, itemID);
        _infoBeep(POCKET_MSG_CANNOT_GIVE_ITEM);
        return 0;
    }

    Data.mLastSlotID = menuPos;
    return 1;
}

// Picks the row of the action menu and runs it.
int _execActionMenu(int tab, u16 itemID, int menuPos, u16 *outNum)
{
    SActionMenuItem menu[4];
    int id = itemID;
    int msgID;
    int num;

    *outNum = 0;

    if (Data.mMode == POCKET_MODE_FIELD) {
        if (itemID == POCKET_ITEM_BATTLE_DISK) {
            _setExplanationMsgID(POCKET_MSG_BATTLE_DISK_PROMPT);
            menuPocketBattleDiskMain(-1, Data.mTrainer, Data.mHero);
            return POCKET_ACTION_CONTINUE;
        }
        if (itemID == POCKET_ITEM_COLOGNE) {
            _setExplanationMsgID(POCKET_MSG_COLOGNE_PROMPT);

            u16 cologne = menuPocketCologne(-1);
            unsigned long cologneID = cologne;

            if (cologneID == 0) {
                return POCKET_ACTION_CONTINUE;
            }
            *outNum = cologne;

            PocketItemUseFunc useFunc = _getUseFunc(cologneID);

            if (useFunc == 0) {
                _infoBeep(POCKET_MSG_CANNOT_USE_ITEM);
                return POCKET_ACTION_COMPLETE;
            }
            if (useFunc(cologneID, &msgID) == POCKET_ITEM_USE_CLOSE) {
                if (msgID > 0) {
                    _decItem(cologne, 6, -1, msgID);
                }
                return POCKET_ACTION_CLOSE;
            }
            return POCKET_ACTION_CONTINUE;
        }
    }

    num = _makeActMenu(menu, 4, tab, menuPos);

    _setExplanationMsgID(POCKET_MSG_ITEM_ACTION_PROMPT);

    int row = _actionMenuOpen(menu, num);

    if (row < 0) {
        return POCKET_ACTION_CONTINUE;
    }

    SActionMenuFunc func = menu[row].mFunc;

    if (func == 0) {
        return POCKET_ACTION_CONTINUE;
    }

    switch (func(tab, menuPos, &msgID)) {
    case POCKET_ACTION_COMPLETE:
        if (msgID > 0) {
            _decItem(itemID, lPocketTabData[Data.mTab].mItemKind, menuPos, msgID);
        }
        Data.mLastSlotID = menuPos;
        return POCKET_ACTION_COMPLETE;

    case POCKET_ACTION_CONTINUE:
        return POCKET_ACTION_CONTINUE;

    case POCKET_ACTION_CLOSE:
        if (msgID > 0) {
            _decItem(itemID, lPocketTabData[Data.mTab].mItemKind, menuPos, msgID);
        }
        return POCKET_ACTION_CLOSE;
    }
    return POCKET_ACTION_COMPLETE;
}

// Runs the selected action; returns 1 to leave the item list, 0 to select again.
int _afterDecideItem(int mode, int tab, u16 itemID, int menuPos, u16 *outNum)
{
    u16 num = 0;
    int result;

    switch (mode) {
    case POCKET_MODE_FIELD:
    case POCKET_MODE_BATTLE:
    case POCKET_MODE_SNATCH_ONLY_BATTLE:
        result = _execActionMenu(tab, itemID, menuPos, &num);
        if (result == POCKET_ACTION_CLOSE) {
            result = POCKET_SELECTION_LEAVE;
        } else if (mode == POCKET_MODE_FIELD) {
            result = POCKET_SELECTION_AGAIN;
        } else if (result == POCKET_ACTION_COMPLETE) {
            result = POCKET_SELECTION_LEAVE;
        } else {
            result = POCKET_SELECTION_AGAIN;
        }
        break;

    case POCKET_MODE_GIVE:
    case POCKET_MODE_GIVE_RESTRICTED:
        if ((u8)_execGive(tab, itemID, menuPos) != 0) {
            result = POCKET_SELECTION_LEAVE;
        } else {
            result = POCKET_SELECTION_AGAIN;
        }
        break;

    case POCKET_MODE_SHOP_SELL:
        _execSell(tab, itemID, menuPos);
        result = POCKET_SELECTION_AGAIN;
        break;

    case POCKET_MODE_DEPOSIT:
        _execDeposit(tab, itemID, menuPos);
        result = POCKET_SELECTION_AGAIN;
        break;
    }

    *outNum = num;
    return result;
}

// Puts `count` of the item into the PC; whether it fitted is returned.
u8 _depositPC(u16 itemID, int count)
{
    if (pcboxGetItemCapacity(0, itemID) < count) {
        return 0;
    }
    pcboxAddItem(0, itemID, (u16)count);
    return 1;
}

} // namespace

// Pokedoru (money) the hero carries.
namespace {
u32 _getMoney(void)
{
    return heroGetStatus(Data.mHero, HERO_STATUS_MONEY, 0);
}
} // namespace


// Moves slot `from` of the tab to slot `dest`.
namespace {
void _swapItemSlot(int item, int from, int dest)
{
    u16 num;

    if (from == dest || from == dest - 1) {
        return;
    }

    ItemSlot *ary = heroItemGetItemKindToItemAryPtr(
        Data.mHero, (u8)item, &num, 0, 0, 0);
    ItemSlot entry = ary[from];

    if (from != num - 1) {
        memmove(ary + from, ary + from + 1, (num - from - 1) * 4);
    }
    if (from < dest) {
        dest--;
    }

    memmove(ary + dest + 1, ary + dest, (num - dest - 1) * 4);
    ary[dest] = entry;
}
} // namespace


// Items the hero owns on the tab, sorted.
namespace {
void _sortItemsThisTab(u8 tab)
{
    Data.mHero->itemSort(tab);
}

// Takes `count` of the item out of the hero list.
void _decItem(u16 itemID, int kind, int menuPos, int count)
{
    heroItemDecItemDataId(Data.mHero, itemID, count, menuPos);
}

// Opens the action menu and returns the row that was picked, -1 when it was
// left without picking one.
int _actionMenuOpen(const SActionMenuItem *menu, int num)
{
    u32 param[2];

    param[0] = (u32)menu;
    param[1] = (u32)num;

    int result = menuOpenCustom(WIN_ID_ACTION_MENU, windowGetActiveID(), 0, 0, 1, 1, param);

    menuClose(WIN_ID_ACTION_MENU);
    menuCloseSync(WIN_ID_ACTION_MENU, 1);

    if (result == -1) {
        return -1;
    }
    if (result < 0 || result >= num) {
        return -1;
    }
    return result;
}

// Opens the quantity menu and returns what the player entered, -1 when
// cancelled. A positive unit price also displays the sale total.
int _openNumberInputMenu(int initialQuantity, int maxQuantity, int digitCount, int unitPrice)
{
    PocketNumberInputParam param;

    if (maxQuantity < 0) {
        return 0;
    }

    int windowID;

    if (unitPrice > 0) {
        if (digitCount == POCKET_NUMBER_TWO_DIGITS) {
            param.mCursor = POCKET_NUMBER_TWO_DIGIT_INITIAL_CURSOR;
            windowID = WIN_ID_NUMBER_PRICE_TWO_DIGITS;
        } else {
            param.mCursor = POCKET_NUMBER_THREE_DIGIT_INITIAL_CURSOR;
            windowID = WIN_ID_NUMBER_PRICE_THREE_DIGITS;
        }
    } else {
        if (digitCount == POCKET_NUMBER_TWO_DIGITS) {
            param.mCursor = POCKET_NUMBER_TWO_DIGIT_INITIAL_CURSOR;
            windowID = WIN_ID_NUMBER_ITEM_TWO_DIGITS;
        } else {
            param.mCursor = POCKET_NUMBER_THREE_DIGIT_INITIAL_CURSOR;
            windowID = WIN_ID_NUMBER_ITEM_THREE_DIGITS;
        }
    }

    param.mWindowID = windowID;
    param.mMaximumQuantity = maxQuantity;
    param.mUnitPrice = unitPrice;
    Data.mNumInputValue = initialQuantity;

    int *windowArgs = (int *)&param + 1;
    int result = menuOpenCustom(windowID, windowGetActiveID(), &param, 0, 1, 1, windowArgs);

    menuClose(windowID);
    menuCloseSync(windowID, 1);

    if (result == -1 || Data.mNumInputValue == 0) {
        return -1;
    }
    return Data.mNumInputValue;
}

// Price the item is sold for.
int _getItemPrice(int itemID)
{
    return (u16)itemDataBiosGetPrice(itemDataBiosGetPtr((u16)itemID));
}

// Bag pocket the item belongs to, ItemData::mBagPocket.
u8 _getItemKind(int itemID)
{
    return itemDataBiosGetKind(itemDataBiosGetPtr((u16)itemID));
}

// Whether the item may be put into the PC, ItemData::mDepositAble.
u8 _getItemDeposit(u16 itemID)
{
    return itemDataBiosGetDeposit(itemDataBiosGetPtr(itemID));
}

// Routine the item runs when it is used.
PocketItemUseFunc _getUseFunc(unsigned long itemID)
{
    ItemData *data = itemDataBiosGetPtr((u16)itemID);

    if (Data.mMode == POCKET_MODE_FIELD) {
        return itemDataBiosGetFieldUseFunc(data);
    }
    return itemDataBiosGetBattleUseFunc(data);
}

// Count of the row `menuPos` of the tab.
u16 _getItemNumFromMenuPos(int tab, int menuPos)
{
    u16 num;
    int kind = lPocketTabData[tab].mItemKind;
    ItemSlot *ary;

    if (kind >= 0) {
        ary = heroItemGetItemKindToItemAryPtr(Data.mHero, (u8)kind, &num, 0, 0, 0);
    } else {
        ary = heroHizukiItemGetItemAryPtr(Data.mHero, &num, 0, 0, 0);
    }

    int i;
    int n = -1;

    for (i = 0; i < num; i++, ary++) {
        if (itemCheckValid(ary)) {
            if (++n >= menuPos) {
                return itemBiosGetNum(ary);
            }
        }
    }
    return 0;
}

// Number of slots the hero array of the tab kind offers.
int _getStockMaxFromTagID(int tab)
{
    u16 num;

    heroItemGetItemKindToItemAryPtr(Data.mHero, (u8)lPocketTabData[tab].mItemKind, 0, &num, 0, 0);
    return num;
}

// Name message id of the item, ItemData::mNameMsgID.
int _getItemNameMsg(unsigned long itemID)
{
    return itemDataBiosGetName(itemDataBiosGetPtr((u16)itemID));
}

} // namespace

// Number of rows the tab holds.
namespace {
int _getNbItemSlot(int tab)
{
    u16 num;
    int kind = lPocketTabData[tab].mItemKind;
    ItemSlot *ary;

    if (kind >= 0) {
        ary = heroItemGetItemKindToItemAryPtr(Data.mHero, (u8)kind, &num, 0, 0, 0);
    } else {
        ary = heroHizukiItemGetItemAryPtr(Data.mHero, &num, 0, 0, 0);
    }

    int i;
    int nb = 0;

    for (i = 0; i < num; i++, ary++) {
        if (itemCheckValid(ary)) {
            nb++;
        }
    }
    return nb;
}
} // namespace


// Throws the chosen number of the item away.
namespace {
int _actSuteruNormal(int tab, int menuPos, int *num)
{
    SActionMenuTable menu = sActionMenuSuteru;

    _setExplanationMsgID(POCKET_MSG_DISCARD_QUANTITY_PROMPT);

    int digitCount;

    if (_getStockMaxFromTagID(tab) > 100) {
        digitCount = POCKET_NUMBER_THREE_DIGITS;
    } else {
        digitCount = POCKET_NUMBER_TWO_DIGITS;
    }

    int count = _openNumberInputMenu(POCKET_NUMBER_INITIAL_QUANTITY, _getItemNumFromMenuPos(tab, menuPos), digitCount, 0);

    if (count < 0) {
        *num = 0;
        return 0;
    }

    u16 itemID = _getItemIDFromMenuPos(tab, menuPos);

    msgctrlSetValue(MSG_VAR_ITEM_ID, itemID);
    msgctrlSetValue(MSG_VAR_ITEM_QUANTITY, count);
    _setExplanationMsgID(POCKET_MSG_DISCARD_CONFIRM_PROMPT);

    int row = _actionMenuOpen(menu.mItems, 2);

    if (row == 1 || row == -1) {
        *num = 0;
        return 0;
    }

    msgctrlSetValue(MSG_VAR_ITEM_ID, itemID);
    msgctrlSetValue(MSG_VAR_ITEM_QUANTITY, count);
    _infoOnly(POCKET_MSG_ITEM_DISCARDED);
    *num = count;
    return 0;
}

// Opens the pokemon menu so the item can be given to a pokemon; whether the
// item was handed over is returned: the slot id the menu leaves behind is
// negative when it was left without handing the item over.
int _openPokemonMenuForGiveItem(u16 itemID, u8 pocket, u8 menuItemIndex)
{
    menuCloseCustom(WIN_ID_SELECT_ITEM, 0, 1);
    Data.mCurrentSlotID = menuPokemonOpenItemGive(pocket, menuItemIndex, itemID, 0);

    menuOffScreenSetPriority(0);
    menuPokemonClose();

    return (Data.mCurrentSlotID >> 31) ^ 1;
}

} // namespace

// Gives the item to a pokemon.
namespace {
int _actMotaseruNormal(int tab, int menuPos, int *num)
{
    u16 itemID = _getItemIDFromMenuPos(tab, menuPos);

    if (itemDataBiosGetHidenMachineNo(itemDataBiosGetPtr(itemID)) != 0xFF) {
        msgctrlSetValue(MSG_VAR_ITEM_ID, itemID);
        _infoBeep(POCKET_MSG_CANNOT_GIVE_ITEM);
        *num = 0;
        return 0;
    }

    u8 pocket = _getItemKind(itemID);

    if (!(u8)_openPokemonMenuForGiveItem(itemID, pocket, (u8)menuPos)) {
        return 1;
    }

    *num = 0;
    return 0;
}
} // namespace


// Uses the item: runs the routine the item data names.
namespace {
int _actTukauNormal(int tab, int menuPos, int *num)
{
    u16 itemID = _getItemIDFromMenuPos(tab, menuPos);
    int id = itemID;
    PocketItemUseFunc useFunc = _getUseFunc(id);

    if (useFunc == 0) {
        _infoBeep(POCKET_MSG_CANNOT_USE_ITEM);
        *num = 0;
        return POCKET_ACTION_CONTINUE;
    }

    int result = useFunc(id, num);
    int ret;

    switch (result) {
    case POCKET_ITEM_USE_COMPLETE:
        ret = POCKET_ACTION_COMPLETE;
        break;

    case POCKET_ITEM_USE_CONTINUE:
        ret = POCKET_ACTION_CONTINUE;
        break;

    case POCKET_ITEM_USE_CLOSE:
        if (tab == POCKET_TAB_KEY_ITEMS && itemID != ITEM_KEY_ITEM_CLOSE_EXCEPTION) {
            ret = POCKET_ACTION_COMPLETE;
        } else {
            ret = POCKET_ACTION_CLOSE;
        }
        break;
    }

    return ret;
}
} // namespace


// Prints the name of the item of one row.
namespace {
void _printItemDouguBall(GSgfxLayerID layer, int x, int y, int tab, int index)
{
    u32 msgID = _getItemNameMsg(_getItemIDFromMenuPos(tab, index));

    if (msgID != 0) {
        GSmsgPrint2(layer, x, y, -1, msgID);
    }

    int max = _getStockMaxFromTagID(tab);

    msgctrlSetValue(MSG_VALUE_DIGIT, max);

    s16 off = (s16)(GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16);

    off += (s16)(GSmsgGetMessageSize(POCKET_MSG_QUANTITY_MARKER) >> 16);

    int ox = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mWidth;

    GSmsgPrint2(layer, ox - off, y, -1, POCKET_MSG_QUANTITY_MARKER);

    msgctrlSetValue(MSG_VALUE_DIGIT, _getItemNumFromMenuPos(tab, index));

    s16 off2 = (s16)(GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16);

    int ox2 = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mWidth;

    GSmsgPrint2(layer, ox2 - off2 + 1, y, -1, POCKET_DIGIT_MSG);
}
} // namespace

// Prints one row of the technical-machine tab: the number of the machine,
// the move it teaches and the number of items held.
namespace {
void _printItemWazaMachine(GSgfxLayerID layer, int x, int y, int tab, int index)
{
    int x2;
    s16 off2;

    ItemData *data = itemDataBiosGetPtr(_getItemIDFromMenuPos(tab, index));
    int machine = itemDataBiosGetWazaMachineNo(data);
    int machineNo = machine + 1;
    u8 hidden = itemDataBiosGetHidenMachineNo(data);
    u8 hiddenNo = hidden + 1;
    int msgID;

    if (hidden != 0xFF) {
        msgID = POCKET_MSG_HIDDEN_MACHINE_PREFIX;
    } else {
        msgID = POCKET_MSG_ITEM_NUMBER_PREFIX;
    }

    GSmsgPrint2(layer, x, y, -1, msgID);

    x2 = x + (GSmsgGetMessageSize(msgID) >> 16);

    if (hidden != 0xFF) {
        msgctrlSetValue(MSG_VALUE_DIGIT, hiddenNo);
        GSmsgPrint2(layer, x2, y, -1, POCKET_DIGIT_MSG);
        x2 += GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16;
    } else {
        if (machineNo < 10) {
            msgctrlSetValue(MSG_VALUE_DIGIT, 0);
            GSmsgPrint2(layer, x2, y, -1, POCKET_DIGIT_MSG);
            x2 += GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16;
        }
        msgctrlSetValue(MSG_VALUE_DIGIT, machineNo);
        GSmsgPrint2(layer, x2, y, -1, POCKET_DIGIT_MSG);
        x2 += GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16;
    }

    u32 wazaID = itemDataBiosGetWazaIDByWazaMachineNo(machine);
    int nameID = wazaDataBiosGetName(wazaDataBiosGetPtr((u16)wazaID));

    GSmsgPrint2(layer, x2 + 9, y, -1, nameID);

    int max = _getStockMaxFromTagID(tab);

    msgctrlSetValue(MSG_VALUE_DIGIT, max);

    s16 off = (s16)(GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16);

    off += (s16)(GSmsgGetMessageSize(POCKET_MSG_QUANTITY_MARKER) >> 16);

    int ox = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mWidth;

    GSmsgPrint2(layer, ox - off, y, -1, POCKET_MSG_QUANTITY_MARKER);

    msgctrlSetValue(MSG_VALUE_DIGIT, _getItemNumFromMenuPos(tab, index));

    off2 = (s16)(GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16);

    int ox2 = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mWidth;

    GSmsgPrint2(layer, ox2 - off2 + 1, y, -1, POCKET_DIGIT_MSG);
}
} // namespace

// Prints one row of the berry tab: the number of berries and the name of the
// berry.
namespace {
void _printItemKinomi(GSgfxLayerID layer, int x, int y, int tab, int index)
{
    s16 off2;

    GSmsgPrint2(layer, x, y, -1, POCKET_MSG_ITEM_NUMBER_PREFIX);

    int ox = x + (GSmsgGetMessageSize(POCKET_MSG_ITEM_NUMBER_PREFIX) >> 16);

    u16 itemID = _getItemIDFromMenuPos(tab, index);
    u8 kinomi = itemDataBiosGetKinomiNo(itemID) + 1;

    if (kinomi < 10) {
        msgctrlSetValue(MSG_VALUE_DIGIT, 0);
        GSmsgPrint2(layer, ox, y, -1, POCKET_DIGIT_MSG);
        ox += GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16;
    }

    msgctrlSetValue(MSG_VALUE_DIGIT, kinomi);
    GSmsgPrint2(layer, ox, y, -1, POCKET_DIGIT_MSG);
    ox += GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16;

    int infoMsgID = _getItemNameMsg(itemID);

    GSmsgPrint2(layer, ox + 9, y, -1, infoMsgID);

    int max = _getStockMaxFromTagID(tab);

    msgctrlSetValue(MSG_VALUE_DIGIT, max);

    s16 off = (s16)(GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16);

    off += (s16)(GSmsgGetMessageSize(POCKET_MSG_QUANTITY_MARKER) >> 16);

    int ox2 = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mWidth;

    GSmsgPrint2(layer, ox2 - off, y, -1, POCKET_MSG_QUANTITY_MARKER);

    msgctrlSetValue(MSG_VALUE_DIGIT, _getItemNumFromMenuPos(tab, index));

    off2 = (s16)(GSmsgGetMessageSize(POCKET_DIGIT_MSG) >> 16);

    int ox3 = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mWidth;

    GSmsgPrint2(layer, ox3 - off2 + 1, y, -1, POCKET_DIGIT_MSG);
}
} // namespace

// Prints the name of the item of one row; used by the tabs whose rows only
// show a name, the key-item and TM tabs.
namespace {
void _printItemTaisetsu(GSgfxLayerID layer, int x, int y, int tab, int index)
{
    u32 msgID = _getItemNameMsg(_getItemIDFromMenuPos(tab, index));

    if (msgID != 0) {
        GSmsgPrint2(layer, x, y, -1, msgID);
    }
}
} // namespace


// Draws the rows of the tab list that are visible around the cursor.
namespace {
void _printItemList(GSgfxLayerID layer, int x, int tab, CURSOR &pos)
{
    int y = 0;
    const PocketTabData *tabData = &lPocketTabData[tab];
    int num = _getNbItemSlot(tab);
    int index = pos.x;
    int first = 0;
    int rows = 9;
    int scroll = 0;

    if (pocketScrollZero != Data.mListScroll && Data.mListScrollFlag != 0) {
        if (Data.mListScroll < pocketScrollZero) {
            index--;
            first = -1;
        } else {
            rows = 10;
        }
        scroll = (int)Data.mListScroll;
    }

    y = first * 30;

    int i = first;

    while (i < rows && index < num) {
        if (index >= 0) {
            tabData->mPrint(layer, x, y - scroll, tab, index);
        }
        y += 30;
        i++;
        index++;
    }

    if (i < rows) {
        int yPos = i * 30 - scroll;

        GSmsgPrint2(layer, x, yPos, -1, Data.mMode == POCKET_MODE_DEPOSIT ? POCKET_MSG_DEPOSIT_EXIT_LABEL : POCKET_MSG_EXIT_LABEL);
    }
}
} // namespace

// Sets the message shown under the item list.
namespace {
void _setExplanationMsgID(unsigned long msgID)
{
    Data.mExplanationMsgID = msgID;
}
} // namespace


// Message id of the item description.
namespace {
u32 _getItemInstMsg(unsigned long itemID)
{
    return itemDataBiosGetDoc(itemDataBiosGetPtr((u16)itemID));
}
} // namespace

// Item id of the row `menuPos` of the tab.
namespace {
u16 _getItemIDFromMenuPos(int tab, int menuPos)
{
    u16 num;
    PocketTabData *tabData = (PocketTabData *)&lPocketTabData[tab];
    ItemSlot *ary = heroItemGetItemKindToItemAryPtr(Data.mHero, tabData->mItemKind, &num, 0, 0, 0);
    int n = -1;

    for (int i = 0; i < num; i++, ary++) {
        if (itemCheckValid(ary)) {
            if (++n >= menuPos) {
                return itemBiosGetItemDataId(ary);
            }
        }
    }
    return 0;
}

// Opens the item-select window on `slotID` and returns the item the cursor
// is on; 0 when the window was left.
u16 _selectItem(int &menuPos, long *slotID)
{
    Data.mExchangeSlot = -1;

    int tab = menuOpenCustom(WIN_ID_SELECT_ITEM, windowGetActiveID(), slotID, 0, 1, 0);

    if (tab == -1) {
        return 0;
    }

    Data.mTab = tab;

    CURSOR pos = cursorBiosGetPos(lPocketTabData[tab].mCursorID);

    menuPos = pos.x + pos.y;
    return _getItemIDFromMenuPos(tab, menuPos);
}

} // namespace

// The pocket loop: draws the list, waits for the player to pick an item,
// runs the action menu on it and repeats until the pocket is left.
namespace {
u16 _menuPocketLoop(void)
{
    CURSOR pos;

    memset(&pos, 0, sizeof(pos));

    if (Data.mMode == POCKET_MODE_SNATCH_ONLY_BATTLE) {
        pos.y = 1;
    } else {
        pos = cursorBiosGetPos(2);
    }

    long slotID = pos.y;
    long *pSlotID = &slotID;

    for (int i = 0; i < 5; i++) {
        CURSOR tabPos = cursorBiosGetPos(lPocketTabData[i].mCursorID);

        if (tabPos.x + tabPos.y >= _getNbItemSlot(i) + 1) {
            cursorBiosSetPos(lPocketTabData[i].mCursorID, pocketTabCursorResetPosition);
        }
    }

    if (Data.mSkipActMenuResize == 0) {
        _setActMenuResizeWork();
    }

    u16 item = 0;
    int menuPos = 0;

    for (;;) {
        Data.mListActive = 1;
        item = _selectItem(menuPos, pSlotID);
        Data.mListActive = 0;
        pSlotID = 0;

        if (item == 0) {
            goto leave;
        }

        u16 out;
        int result = _afterDecideItem(Data.mMode, Data.mTab, item, menuPos, &out);

        if (out != 0) {
            item = out;
        }

        switch (result) {
        case POCKET_SELECTION_LEAVE:
            goto leave;
        case POCKET_SELECTION_AGAIN:
            continue;
        }
    }

leave:
    menuCloseCustom(WIN_ID_SELECT_ITEM, 0, 1);

    if (Data.mMode == POCKET_MODE_DEPOSIT) {
        fadeSet(3, pocketShopExitFadeTime);
        fadeCheck(1);
    }

    if (Data.mOwnsFloor != 0) {
        menuOffScreenSetPriority(1);
        GSfloorPop();
        floorSetFadeScript(0, 0);
    }

    return item;
}
} // namespace


// Whether the tab shown in the window may be sorted.
namespace {
u8 _IsSortAble(tagWINDOW_WORK *window)
{
    if (Data.mMode == POCKET_MODE_SHOP_SELL || Data.mMode == POCKET_MODE_DEPOSIT || Data.mMode == POCKET_MODE_GIVE_RESTRICTED) {
        return 0;
    }

    return lPocketTabData[window->mCursor.y].mSortAble;
}
} // namespace


// Whether the tab shown in the window may be swapped.
namespace {
u8 _IsIrekaeAble(tagWINDOW_WORK *window)
{
    if (Data.mMode == POCKET_MODE_SHOP_SELL || Data.mMode == POCKET_MODE_DEPOSIT || Data.mMode == POCKET_MODE_GIVE_RESTRICTED) {
        return 0;
    }

    return lPocketTabData[window->mCursor.y].mSwapAble;
}
} // namespace


extern "C" int menuPocket2PrintExplanation(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    u32 msgID = Data.mExplanationMsgID;
    if (msgID != 0) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, -1, msgID);
    }
    return 0;
}




// menuPocket2GetHero -- fetch the hero and battle-trainer contexts the
// pocket menu latched in Data.
#pragma force_active on
void menuPocket2GetHero(Hero **hero, FIGHT_TRAINER **trainer)
{
    *hero = Data.mHero;
    *trainer = Data.mTrainer;
}
#pragma force_active reset

// menuPocket2GetCurrentTemochiPokemonSlotID -- snapshot the selected
// temochi pokemon slot ID into the current slot field of Data,
// returning the snapshotted ID.
#pragma force_active on
u32 menuPocket2GetCurrentTemochiPokemonSlotID(void)
{
    u32 slot;

    slot = Data.mSelectedSlotID;
    Data.mCurrentSlotID = slot;
    return slot;
}
#pragma force_active reset

extern "C" void menuPocket2GetTemochiPokemon(u32 slot, Pokemon **pokemon, FightPokemon **fightPokemon)
{
    Pokemon *partyPokemon;
    FightPokemon *battlePokemon;
    if (Data.mMode == POCKET_MODE_BATTLE || Data.mMode == POCKET_MODE_SNATCH_ONLY_BATTLE) {
        battlePokemon = fightTrainerBiosGetFightPokemonPtr(Data.mTrainer, (u16)slot);
        partyPokemon = fightPokemonGetPokemonPtr(battlePokemon);
    } else {
        partyPokemon = (Pokemon *)heroGetStatus(Data.mHero, HERO_STATUS_POKEMON, (u16)slot);
        battlePokemon = 0;
    }
    *pokemon = partyPokemon;
    *fightPokemon = battlePokemon;
}

#pragma force_active on
extern "C" void menuPocket2ClosePokemonMenu(s32 slot)
{
    Data.mCurrentSlotID = slot;
    menuPokemonClose();
}
#pragma force_active reset

#pragma force_active on
extern "C" u32 menuPocket2OpenPokemonMenu(u32 itemID, s32 wait)
{
    menuCloseCustom(MENU_DATA_ACT_ROW, 0, 1);
    u8 kind;
    if (Data.mMode == POCKET_MODE_BATTLE || Data.mMode == POCKET_MODE_SNATCH_ONLY_BATTLE) {
        kind = POKEMON_ITEM_USE_BATTLE;
    } else {
        kind = POKEMON_ITEM_USE_PARTY;
    }
    u8 waitFlag = wait != 0;
    s32 slot = menuPokemonOpenItemUse(kind, waitFlag, itemID, Data.mTrainer);
    if (wait == 0) {
        slot = -1;
    }
    return slot;
}
#pragma force_active reset

extern "C" int menuPocket2DrawNumMenuCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 *param = (s32 *)window->mArgs[0];
    const PocketNumberMenu *menu = MenuPocket2Num;
    u32 i;
    for (i = 0; i < POCKET_NUMBER_MENU_COUNT; menu++, i++) {
        if (param[POCKET_NUMBER_ARG_WINDOW_ID] == menu->mWindowID) {
            break;
        }
    }
    if (i >= POCKET_NUMBER_MENU_COUNT) {
        return 0;
    }
    if (window->mCursor.y < 0 || window->mCursor.y >= MenuPocket2Num[i].mDigitCount) {
        return 0;
    }
    const PocketNumberDigit *digit = MenuPocket2Num[i].mDigits;
    digit += window->mCursor.y;
    if (digit->mCursorTopItemID == sprite->mItemID || digit->mCursorBottomItemID == sprite->mItemID) {
        sprite->mRgba.mAlpha = 255;
    } else {
        sprite->mRgba.mAlpha = 0;
    }
    return 0;
}

extern "C" int menuPocket2PrintPrice(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 *param = (s32 *)window->mArgs[0];
    if (param[POCKET_NUMBER_ARG_UNIT_PRICE] <= 0) {
        return 0;
    }
    msgctrlSetValue(MSG_VALUE_PRICE, param[POCKET_NUMBER_ARG_UNIT_PRICE] * Data.mNumInputValue);
    s16 width = GSmsgGetMessageSize(POCKET_PRICE_MSG) >> 16;
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), sprite->mWidth - width, 0, -1, POCKET_PRICE_MSG);
    return 0;
}

extern "C" int menuPocket2PrintNumMenu(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 *param = (s32 *)window->mArgs[0];
    const PocketNumberMenu *menu = MenuPocket2Num;
    u32 i;
    for (i = 0; i < POCKET_NUMBER_MENU_COUNT; menu++, i++) {
        if (param[POCKET_NUMBER_ARG_WINDOW_ID] == menu->mWindowID) {
            break;
        }
    }
    if (i >= POCKET_NUMBER_MENU_COUNT) {
        return 0;
    }
    const PocketNumberDigit *digit = MenuPocket2Num[i].mDigits;
    int index;
    for (index = 0; index < MenuPocket2Num[i].mDigitCount; index++, digit++) {
        if (sprite->mItemID == digit->mDigitItemID) {
            break;
        }
    }
    if (index >= MenuPocket2Num[i].mDigitCount) {
        return 0;
    }
    int n;
    int divisor = 1;
    for (n = 0; n < MenuPocket2Num[i].mDigitCount - index - 1; n++) {
        divisor *= 10;
    }
    int value = Data.mNumInputValue / divisor;
    value %= 10;
    msgctrlSetValue(MSG_VALUE_DIGIT, value);
    GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, -1, POCKET_DIGIT_MSG);
    return 0;
}

extern "C" int menuPocketInputNumber(tagWINDOW_WORK *window, int value, int max, u16 repeatKeys, int digits, int sound, int unusedCursorSound)
{
    u16 keys = repeatKeys;
    int place = digits - window->mCursor.y - 1;
    if (keys & (POCKET_KEY_UP | POCKET_KEY_DOWN | POCKET_KEY_LEFT | POCKET_KEY_RIGHT)) {
        int oldValue = value;
        int divisor = 1;
        for (int i = 0; i < place; i++) {
            divisor *= 10;
        }
        int digit = (value / divisor) % 10;
        int tens = (value / (divisor * 10)) % 10;
        int hundreds = (value / (divisor * 100)) % 10;
        if (keys & POCKET_KEY_UP) {
            if (max != 0) {
                if (value == max) {
                    value = 0;
                } else if (digit < 9) {
                    value += divisor;
                } else if (place >= digits - 1) {
                    value = 0;
                    for (int i = 0; i < digits; i++) {
                        value *= 10;
                        value += 9;
                    }
                } else if (tens < 9) {
                    value += divisor;
                } else if (place >= digits - 2) {
                    value = 0;
                    for (int i = 0; i < digits; i++) {
                        value *= 10;
                        value += 9;
                    }
                } else if (hundreds < 9) {
                    value += divisor;
                } else {
                    value = 0;
                }
                if (value > max) {
                    value = max;
                }
            }
        } else if (keys & POCKET_KEY_DOWN) {
            if (max != 0) {
                if (value == 0) {
                    value = max;
                } else if (digit >= 1) {
                    value -= divisor;
                } else if (place >= digits - 1) {
                    value = 0;
                } else if (tens >= 1) {
                    value -= divisor;
                } else if (place >= digits - 2) {
                    value = 0;
                } else if (hundreds >= 1) {
                    value -= divisor;
                } else {
                    value = max;
                }
            }
        } else if (keys & POCKET_KEY_RIGHT) {
            if (++window->mCursor.y >= digits) {
                window->mCursor.y = (s8)(digits - 1);
            }
        } else if (keys & POCKET_KEY_LEFT) {
            if (--window->mCursor.y < 0) {
                window->mCursor.y = 0;
            }
        }
        if (oldValue != value) {
            menuPtPlaySe(sound);
        }
    }
    return value;
}

extern "C" int menuPocket2NumCursor(tagWINDOW_WORK *window)
{
    s32 *param = (s32 *)window->mArgs[0];
    u16 keys = windowGetKeyRepeat();
    const PocketNumberMenu *menu = MenuPocket2Num;
    u32 i;
    for (i = 0; i < POCKET_NUMBER_MENU_COUNT; menu++, i++) {
        if (param[POCKET_NUMBER_ARG_WINDOW_ID] == menu->mWindowID) {
            break;
        }
    }
    if (i >= POCKET_NUMBER_MENU_COUNT) {
        return 0;
    }
    Data.mNumInputValue = menuPocketInputNumber(window, Data.mNumInputValue,
        param[POCKET_NUMBER_ARG_MAX_VALUE], keys, MenuPocket2Num[i].mDigitCount,
        POCKET_NUMBER_CURSOR_SOUND, POCKET_NUMBER_UNUSED_CURSOR_SOUND);
    return 0;
}

extern "C" int menuPocket2NumCtrl(tagWINDOW_WORK *window)
{
    s32 *param = (s32 *)window->mArgs[0];
    switch (window->mState) {
    case POCKET_NUMBER_STATE_OPEN:
        if (window->mFlag == 0) {
            winSeqSetMenu(param[POCKET_NUMBER_ARG_WINDOW_ID], POCKET_NUMBER_OPEN_SEQ);
            window->mFlag = 1;
        }
        break;
    case POCKET_NUMBER_STATE_CLOSE:
        if (window->mFlag == 0) {
            winSeqSetMenu(param[POCKET_NUMBER_ARG_WINDOW_ID], POCKET_NUMBER_CLOSE_SEQ);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}


struct PocketActionWindowParam {
    const SActionMenuItem *mItems;
    int mCount;
};



int _actionMenuLineHeight(tagWINDOW_WORK *window);

enum {
    POCKET_ACTION_ROWS = 4,
    POCKET_TAB_COUNT = 5,
    POCKET_LIST_ROWS = 9,
    POCKET_ROW_HEIGHT = 30,
    POCKET_ACTION_CURSOR_ROW_0 = 0xDC,
    POCKET_ACTION_CURSOR_ROW_1 = 0xDB,
    POCKET_ACTION_CURSOR_ROW_2 = 0xDA,
    POCKET_ACTION_TEXT_ROW_0 = 0xE0,
    POCKET_ACTION_TEXT_ROW_1 = 0xDF,
    POCKET_ACTION_TEXT_ROW_2 = 0xDE,
    POCKET_ACTION_TEXT_ROW_3 = 0xDD,
    POCKET_ACTION_OPEN_SEQUENCE = 0x68,
    POCKET_ACTION_CLOSE_SEQUENCE = 0x6C,
    POCKET_LIST_SCISSOR_ITEM = 0xB6,
    POCKET_TITLE_SCISSOR_ITEM = 0xB7,
    POCKET_MAIN_LIST_CURSOR = 0xB1,
};

extern "C" int menuPocket2DrawActionCursorLine(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    PocketActionWindowParam *param = (PocketActionWindowParam *)window->mArgs[0];
    int rowOffset = POCKET_ACTION_ROWS - param->mCount;
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    int spacing = _actionMenuLineHeight(window);
    int y = window->mCursor.y + rowOffset;
    y *= spacing;
    sprite->mY = item->mOffsetA + y;
    return 0;
}

extern "C" int menuPocket2DrawActionCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    PocketActionItemTable cursorItems = ActionCursorItems;
    PocketActionWindowParam *param = (PocketActionWindowParam *)window->mArgs[0];
    int rowOffset = POCKET_ACTION_ROWS - param->mCount;
    int row = window->mCursor.y + rowOffset;
    if (row < 0 || row >= POCKET_ACTION_ROWS) {
        return 0;
    }
    if (sprite->mItemID == cursorItems.mItems[row]) {
        PocketCursorAnimationValues values = pocketActionCursorDefaults();
        s32 animation;


        animation = -1;
        if (window->mMenuID == windowGetActiveID()) {
            menuSubCursorAnimeValGet(values.mValues, &animation);
        }
        int layer = winSpriteGetLayerID(window, sprite);
        windowDrawSpriteCursor(0, 0, -1, window, MENU_LIST_CURSOR_TEXTURE, 0, layer, values.mValues, animation);
    }
    winSpriteSetDisp(sprite, 0);
    return 0;
}

extern "C" int menuPocket2DrawActionText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    PocketActionWindowParam *param = (PocketActionWindowParam *)window->mArgs[0];
    int textItems[POCKET_ACTION_ROWS];
    for (int row = 0; row < POCKET_ACTION_ROWS; ++row) {
        textItems[row] = ActionTextItems.mItems[row];
    }
    int row;
    for (row = 0; row < POCKET_ACTION_ROWS; ++row) {
        if (sprite->mItemID == textItems[row]) {
            break;
        }
    }
    row -= POCKET_ACTION_ROWS - param->mCount;
    if (row < 0 || row >= param->mCount) {
        return 0;
    }
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, -1, param->mItems[row].mMsgID);
    return 0;
}

extern "C" int menuPocket2DrawActionMenu(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    PocketActionWindowParam *param = (PocketActionWindowParam *)window->mArgs[0];
    if (param->mCount < POCKET_ACTION_ROWS) {
        unsigned int row;
        for (row = 0; row < 7; ++row) {
            if (sprite->mItemID == ActMenuResizeWork[row].mItemID) {
                break;
            }
        }
        if (row < 7) {
            int spacing = _actionMenuLineHeight(window);
            int offset = (POCKET_ACTION_ROWS - param->mCount) * spacing;
            sprite->mY = ActMenuResizeWork[row].mRowOffset + offset;
            if (ActMenuResizeWork[row].mFlag) {
                sprite->mHeight = ActMenuResizeWork[row].mColumnExtent - offset;
            }
        }
    }
    return 0;
}

int _actionMenuLineHeight(tagWINDOW_WORK *window)
{
    tagMENU_ITEM_DATA *second = menuItemBiosGetPtr(window->mMenuID, POCKET_ACTION_CURSOR_ROW_2);
    tagMENU_ITEM_DATA *first = menuItemBiosGetPtr(window->mMenuID, ACTION_ROW_4_CURSOR);
    int spacing = second->mOffsetA - first->mOffsetA;
    if (spacing < 0) {
        spacing = -spacing;
    }
    return spacing;
}

extern "C" int menuPocket2ActionCursor(tagWINDOW_WORK *window)
{
    PocketActionWindowParam *param = (PocketActionWindowParam *)window->mArgs[0];
    u16 keys = windowGetKeyRepeat();
    CURSOR old = window->mCursor;
    int count = param->mCount;
    if (keys & POCKET_KEY_DOWN) {
        int row = window->mCursor.y + 1;
        if (row >= count) {
            row = 0;
        }
        window->mCursor.y = (s8)row;
    }
    if (keys & POCKET_KEY_UP) {
        int row = window->mCursor.y - 1;
        if (row < 0) {
            row = count - 1;
        }
        window->mCursor.y = (s8)row;
    }
    if (old.y != window->mCursor.y || old.x != window->mCursor.x) {
        menuSubCursorAnimeStart();
    }
    return 0;
}

extern "C" int menuPocket2ActionCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case 0:
        if (window->mFlag == 0) {
            winSeqSetMenu(WIN_ID_ACTION_MENU, POCKET_ACTION_OPEN_SEQUENCE);
            Data.mExchangeTime = 0.0f;
            window->mFlag = 1;
            menuSubCursorAnimeInit();
        }
        break;
    case 3:
        if (window->mFlag == 0) {
            winSeqSetMenu(WIN_ID_ACTION_MENU, POCKET_ACTION_CLOSE_SEQUENCE);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

extern "C" int menuPocket2PrintItemList(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    tagMENU_ITEM_DATA *scissor = menuItemBiosGetPtr(window->mMenuID, POCKET_LIST_SCISSOR_ITEM);
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    spriteSetScissor(winSpriteGetLayerID(window, sprite), scissor->mMessageX - item->mMessageX, scissor->mOffsetA - item->mOffsetA, scissor->mWidth, scissor->mHeight);
    CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    _printItemList(winSpriteGetLayerID(window, sprite), Data.mTabScroll, window->mCursor.y, pos);
    if (0.0f != Data.mTabScroll) {
        int tab;
        f32 offset;
        if (Data.mTabScroll > 0.0f) {
            tab = window->mCursor.y - 1;
            if (tab < 0) {
                tab = POCKET_TAB_COUNT - 1;
            }
            offset = Data.mTabScroll - menuItemBiosGetPtr(WIN_ID_SELECT_ITEM, POCKET_LIST_SCISSOR_ITEM)->mWidth;
        } else {
            tab = window->mCursor.y + 1;
            if ((unsigned int)tab >= POCKET_TAB_COUNT) {
                tab = 0;
            }
            offset = Data.mTabScroll + menuItemBiosGetPtr(WIN_ID_SELECT_ITEM, POCKET_LIST_SCISSOR_ITEM)->mWidth;
        }
        CURSOR neighbor = cursorBiosGetPos(lPocketTabData[tab].mCursorID);
        _printItemList(winSpriteGetLayerID(window, sprite), offset, tab, neighbor);
    }
    spriteClearScissor(winSpriteGetLayerID(window, sprite));
    spriteSetEnv(winSpriteGetLayerID(window, sprite));
    return 0;
}

extern "C" int menuPocket2PrintItemKind(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s16 nextWidth;
    tagMENU_ITEM_DATA *scissor = menuItemBiosGetPtr(window->mMenuID, POCKET_TITLE_SCISSOR_ITEM);
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    spriteSetScissor(winSpriteGetLayerID(window, sprite), scissor->mMessageX - item->mMessageX, scissor->mOffsetA - item->mOffsetA, scissor->mWidth, scissor->mHeight);
    int tab;
    int center = sprite->mWidth / 2;
    u32 message = lPocketTabData[window->mCursor.y].mTitleMsgID;
    s16 width = GSmsgGetMessageSize(message) >> 16;
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), center - width / 2 + (int)Data.mTabScroll, 0, -1, message);
    if (0.0f != Data.mTabScroll) {
        f32 offset;
        if (Data.mTabScroll > 0.0f) {
            tab = window->mCursor.y - 1;
            if (tab < 0) {
                tab = POCKET_TAB_COUNT - 1;
            }
            offset = Data.mTabScroll - menuItemBiosGetPtr(WIN_ID_SELECT_ITEM, POCKET_LIST_SCISSOR_ITEM)->mWidth;
        } else {
            tab = window->mCursor.y + 1;
            if ((unsigned int)tab >= POCKET_TAB_COUNT) {
                tab = 0;
            }
            offset = Data.mTabScroll + menuItemBiosGetPtr(WIN_ID_SELECT_ITEM, POCKET_LIST_SCISSOR_ITEM)->mWidth;
        }
        message = lPocketTabData[tab].mTitleMsgID;
        nextWidth = (s16)(GSmsgGetMessageSize(message) >> 16);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), center - nextWidth / 2 + (int)offset, 0, -1, message);
    }
    spriteClearScissor(winSpriteGetLayerID(window, sprite));
    spriteSetEnv(winSpriteGetLayerID(window, sprite));
    return 0;
}

extern "C" int menuPocket2DrawRollDown(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (!Data.mListActive) {
        return 0;
    }
    CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    int count = _getNbItemSlot(window->mCursor.y) + 1;
    if (pos.x + POCKET_LIST_ROWS < count) {
        sprite->mRgba.mAlpha = 255.0f * (1.0f - Data.mRollPhase);
    } else {
        sprite->mRgba.mAlpha = 0;
    }
    return 0;
}

extern "C" int menuPocket2DrawTitleLR(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    winSpriteSetDisp(sprite, Data.mMode != POCKET_MODE_SNATCH_ONLY_BATTLE);
    return 0;
}

extern "C" int menuPocket2DrawRollUp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (!Data.mListActive) {
        return 0;
    }
    CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    if (pos.x > 0) {
        sprite->mRgba.mAlpha = 255.0f * (1.0f - Data.mRollPhase);
    } else {
        sprite->mRgba.mAlpha = 0;
    }
    return 0;
}

extern "C" int menuPocket2DrawExchangeCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    int y = pos.y * POCKET_ROW_HEIGHT;
    if (!Data.mListScrollFlag) {
        y += (s16)Data.mListScroll;
    }
    if (Data.mExchangeSlot >= 0) {
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
        sprite->mY = item->mOffsetA + y;
        winSpriteSetDisp(sprite, 1);
        static f32 pulsePeriod = 0.5f;
        Data.mExchangeTime += timeGetLastFrameSec();
        if (Data.mExchangeTime > pulsePeriod) {
            Data.mExchangeTime = 0.0f;
        }
        f32 pulse = Data.mExchangeTime / pulsePeriod;
        if (pulse > 0.5f) {
            pulse = 1.0f - pulse;
        }
        pulse *= 2.0f;
        sprite->mRgba.mAlpha = 100.0f + 155.0f * pulse;
    } else {
        winSpriteSetDisp(sprite, 0);
    }
    return 0;
}

extern "C" int menuPocket2DrawBallCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    int y = pos.y * POCKET_ROW_HEIGHT;
    if (!Data.mListScrollFlag) {
        y += (s16)Data.mListScroll;
    }
    if ((u8)(Data.mExchangeSlot >= 0) ^ (u8)(sprite->mItemID == POCKET_MAIN_LIST_CURSOR)) {
        if (0.0f == Data.mTabScroll) {
            PocketCursorAnimationValues values = pocketListCursorDefaults();
            s32 animation;


            animation = -1;
            if (window->mMenuID == windowGetActiveID()) {
                menuSubCursorAnimeValGet(values.mValues, &animation);
            }
            int layer = winSpriteGetLayerID(window, sprite);
            windowDrawSpriteCursor(0, y, -1, window, MENU_LIST_CURSOR_TEXTURE, 0, layer, values.mValues, animation);
        }
    }
    winSpriteSetDisp(sprite, 0);
    return 0;
}



enum {
};

extern "C" int menuPocket2DrawLineCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    int y;
    u8 visible;
    if (Data.mExchangeSlot >= 0) {
        y = (Data.mExchangeSlot - pos.x) * POCKET_ROW_HEIGHT;
        if (Data.mListScrollFlag) {
            y -= (int)Data.mListScroll;
        }
        visible = 0;
        if (y >= 0 && y < POCKET_LIST_ROWS * POCKET_ROW_HEIGHT) {
            visible = 1;
        }
    } else {
        y = pos.y * POCKET_ROW_HEIGHT;
        if (!Data.mListScrollFlag) {
            y += (int)Data.mListScroll;
        }
        visible = 1;
    }
    if (0.0f != Data.mTabScroll) {
        visible = 0;
    }
    if (visible) {
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
        sprite->mY = (s16)(item->mOffsetA + y);
        if (Data.mExchangeSlot >= 0) {
            winSpriteSetDisp(sprite, 0);
            tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
            menuSubDrawCursorBack2(winSpriteGetLayerID(window, sprite), 0, 0, item->mWidth, item->mHeight, sprite->mRgba.mAlpha, 1, 1);
        } else {
            winSpriteSetDisp(sprite, 1);
        }
    } else {
        winSpriteSetDisp(sprite, 0);
    }
    return 0;
}

extern "C" u8 menuPocketIsSortCursorDisp(tagWINDOW_WORK *window);
extern "C" u8 menuPocketIsIrekaeCursorDisp(tagWINDOW_WORK *window);
void menuPocketToolFloatToZero(float &value, float step);


enum {
    POCKET_STATE_OPEN = 0,
    POCKET_STATE_ACTIVE = 2,
    POCKET_STATE_CLOSE = 3,
    POCKET_SFX_MOVE = 0x10,
    POCKET_SFX_CONFIRM = 0x11,
    POCKET_SFX_CANCEL = 0x12,
    POCKET_SFX_OPEN = 0x14,
    POCKET_SFX_CLOSE = 0x15,
    POCKET_SFX_TAB = 0x44D,
    POCKET_KEY_CONFIRM = 0x10,
    POCKET_KEY_CANCEL = 0x20,
    POCKET_KEY_SWAP = 0xC0,
    POCKET_KEY_SORT = 0x600,
    POCKET_MSG_EXIT_FIELD = 0x3AA1,
    POCKET_MSG_EXIT_BATTLE = 0x3AA2,
    POCKET_MSG_EXIT_GIVE = 0x3AA7,
    POCKET_MSG_EXIT_SELL = 0x3AA8,
    POCKET_MSG_EXIT_DEPOSIT = 0x3AB7,
    POCKET_MSG_SWAP = 0x3AAD,
    POCKET_MSG_MONEY = 0x434E,
    POCKET_MSG_VAR_MONEY = 0x50,
    POCKET_MENU_FLOOR = 0x38F,
    POCKET_INITIAL_SLOT_LIMIT = 5,
    POCKET_VISIBLE_ROWS = 9,
    POCKET_CURSOR_TAB = 2,
    POCKET_SEQUENCE_OPEN = 0x60,
    POCKET_SEQUENCE_CLOSE = 0x64,
    POCKET_ITEM_TAB_LAYOUT = 0xB6,
    POCKET_INITIAL_WORK_SIZE = 0x4C
};


extern "C" int menuPocket2Cursor(tagWINDOW_WORK *window)
{
    u16 repeatKeys = windowGetKeyRepeat();
    u16 triggerKeys = windowGetKeyTrg();
    int itemIndex;
    u8 moved = 0;
    u32 sound = 0;
    PocketExitMessageTable exitMessages = PocketExitMessages;

    if (0.0f != Data.mTabScroll || 0.0f != Data.mListScroll || winSeqCheckMove(window->mMenuID)) return 0;

    CURSOR position;
    position = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    _getNbItemSlot(window->mCursor.y);
    itemIndex = position.x + position.y;

    if (Data.mExchangeSlot < 0) {
        if (triggerKeys & POCKET_KEY_SWAP) {
            if (_IsIrekaeAble(window) && _getItemIDFromMenuPos(window->mCursor.y, itemIndex)) {
                Data.mExchangeSlot = itemIndex;
                sound = POCKET_SFX_CONFIRM;
            }
        }
    } else if (triggerKeys & (POCKET_KEY_CONFIRM | POCKET_KEY_SWAP)) {
        _swapItemSlot(lPocketTabData[window->mCursor.y].mItemKind, Data.mExchangeSlot, itemIndex);
        Data.mExchangeSlot = -1;
        sound = POCKET_SFX_CONFIRM;
    } else if (triggerKeys & (POCKET_KEY_CANCEL | POCKET_KEY_SWAP)) {
        Data.mExchangeSlot = -1;
        sound = POCKET_SFX_CANCEL;
    }

    position = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
    CURSOR oldPosition = position;
    int rowCount = _getNbItemSlot(window->mCursor.y) + 1;

    if (repeatKeys & POCKET_KEY_DOWN) {
        position.y++;
        if (position.y + position.x >= rowCount) position.y--;
        else {
            if (position.y >= POCKET_VISIBLE_ROWS) {
                position.x++;
                position.y--;
                Data.mListScrollFlag = 1;
                sound = POCKET_SFX_MOVE;
            } else {
                Data.mListScrollFlag = 0;
                sound = POCKET_SFX_MOVE;
            }
            Data.mListScroll = -30.0f;
        }
        moved = 1;
    }
    if (repeatKeys & POCKET_KEY_UP) {
        if (position.y > 0 || position.x > 0) {
            position.y--;
            if (position.y < 0) {
                position.y = 0;
                position.x--;
                Data.mListScrollFlag = 1;
                sound = POCKET_SFX_MOVE;
            } else {
                Data.mListScrollFlag = 0;
                sound = POCKET_SFX_MOVE;
            }
            Data.mListScroll = 30.0f;
        }
        moved = 1;
    }
    cursorBiosSetPos(lPocketTabData[window->mCursor.y].mCursorID, position);
    if (oldPosition.y != position.y || oldPosition.x != position.x) menuSubCursorAnimeStart();

    if (Data.mExchangeSlot < 0 && !moved) {
        if ((repeatKeys & POCKET_KEY_RIGHT) && Data.mMode != POCKET_MODE_SNATCH_ONLY_BATTLE) {
            if ((u32)++window->mCursor.y >= POCKET_TAB_COUNT) window->mCursor.y = 0;
            Data.mTabScroll = menuItemBiosGetPtr(WIN_ID_SELECT_ITEM, POCKET_ITEM_TAB_LAYOUT)->mWidth;
            sound = POCKET_SFX_TAB;
        } else if ((repeatKeys & POCKET_KEY_LEFT) && Data.mMode != POCKET_MODE_SNATCH_ONLY_BATTLE) {
            if (--window->mCursor.y < 0) window->mCursor.y = POCKET_TAB_COUNT - 1;
            Data.mTabScroll = -menuItemBiosGetPtr(WIN_ID_SELECT_ITEM, POCKET_ITEM_TAB_LAYOUT)->mWidth;
            sound = POCKET_SFX_TAB;
        } else if ((triggerKeys & POCKET_KEY_SORT) && _IsSortAble(window)) {
            sound = POCKET_SFX_CONFIRM;
            _sortItemsThisTab(lPocketTabData[window->mCursor.y].mItemKind);
        }
        cursorBiosSetPos(POCKET_CURSOR_TAB, window->mCursor);
    }
    if (sound) menuPtPlaySe(sound);
    u32 message;
    if (Data.mExchangeSlot >= 0) message = POCKET_MSG_SWAP;
    else {
        position = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
        itemIndex = position.x + position.y;
        u16 item = _getItemIDFromMenuPos(window->mCursor.y, itemIndex);
        if (item) message = _getItemInstMsg(item);
        else message = exitMessages.mMessages[Data.mMode];
    }
    _setExplanationMsgID(message);
    return 0;
}

extern "C" void menuPocket2Button(tagWINDOW_WORK *window)
{
    if (Data.mExchangeSlot >= 0 || winSeqCheckMove(window->mMenuID)) return;
    u16 keys = windowGetKeyTrg();
    if (keys & POCKET_KEY_CONFIRM) menuPtPlaySe(POCKET_SFX_CONFIRM);
    menuButtonNormal(window);
}

extern "C" int menuPocket2DrawSortButtonText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool display = false;
    if (_IsSortAble(window) && Data.mListActive && menuPocketIsSortCursorDisp(window)) display = 1;
    winSpriteSetDisp(sprite, display);
    return 0;
}
extern "C" int menuPocket2DrawExchangeButtonText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool display = false;
    if (_IsIrekaeAble(window) && Data.mListActive && menuPocketIsIrekaeCursorDisp(window)) display = 1;
    winSpriteSetDisp(sprite, display);
    return 0;
}
extern "C" int menuPocket2DrawSortButton(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool display = false;
    if (_IsSortAble(window) && Data.mListActive && menuPocketIsSortCursorDisp(window)) display = 1;
    winSpriteSetDisp(sprite, display);
    return 0;
}
extern "C" int menuPocket2DrawExchangeButton(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool display = false;
    if (_IsIrekaeAble(window) && Data.mListActive && menuPocketIsIrekaeCursorDisp(window)) display = 1;
    winSpriteSetDisp(sprite, display);
    return 0;
}
extern "C" int menuPocket2PrintMoney(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u32 money = _getMoney();
    if (Data.mMode != POCKET_MODE_SHOP_SELL) return 0;
    msgctrlSetValue(POCKET_MSG_VAR_MONEY, money);
    s16 width = (s16)(GSmsgGetMessageSize(POCKET_MSG_MONEY) >> 16);
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
    GSmsgPrint2(layer, sprite->mWidth - width, 0, -1, POCKET_MSG_MONEY);
    return 0;
}
extern "C" int menuPocket2DrawMoneyBack(tagWINDOW_WORK *, tagSPRITE_WORK *sprite)
{
    if (Data.mMode != POCKET_MODE_SHOP_SELL) sprite->mRgba.mAlpha = 0;
    else sprite->mRgba.mAlpha = 255;
    return 0;
}
extern "C" int menuPocket2DrawMoney(tagWINDOW_WORK *, tagSPRITE_WORK *sprite)
{
    if (Data.mMode != POCKET_MODE_SHOP_SELL) sprite->mRgba.mAlpha = 0;
    else sprite->mRgba.mAlpha = 255;
    return 0;
}
extern "C" int menuPocket2DrawActiveTab(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (sprite->mItemID == (s32)lPocketTabData[window->mCursor.y].mActiveTabItemID) sprite->mColor = 0xFFFFFFFF;
    else sprite->mRgba.mAlpha = 0;
    return 0;
}
extern "C" int menuPocket2Ctrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case POCKET_STATE_OPEN:
        if (!window->mFlag) {
            menuPtPlaySe(POCKET_SFX_OPEN);
            winSeqSetMenu(WIN_ID_SELECT_ITEM, POCKET_SEQUENCE_OPEN);
            CURSOR pos = cursorBiosGetPos(lPocketTabData[window->mCursor.y].mCursorID);
            _setExplanationMsgID(_getItemInstMsg(_getItemIDFromMenuPos(window->mCursor.y, pos.x + pos.y)));
            window->mFlag = 1;
            menuSubCursorAnimeInit();
        }
        break;
    case POCKET_STATE_ACTIVE:
        menuPocketToolFloatToZero(Data.mTabScroll, 23.0f);
        menuPocketToolFloatToZero(Data.mListScroll, 7.0f);
        Data.mRollPhase += 1.0f / 30.0f;
        if (Data.mRollPhase > 1.0f) Data.mRollPhase = 0.0f;
        menuSubCursorAnimeMain();
        break;
    case POCKET_STATE_CLOSE:
        if (!window->mFlag) {
            menuPtPlaySe(POCKET_SFX_CLOSE);
            winSeqSetMenu(WIN_ID_SELECT_ITEM, POCKET_SEQUENCE_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}
#pragma force_active on
extern "C" void menuPocketMain(void)
{
    Data.mResult = _menuPocketLoop();
}
#pragma force_active reset
extern "C" u16 menuPocket2Call(s32 mode, u32 messageWindowID, Hero *hero)
{
    memset(&Data, 0, POCKET_INITIAL_WORK_SIZE);
    Data.mMode = mode;
    Data.mHero = hero;
    Data.mMsgWindowID = messageWindowID;
    return _menuPocketLoop();
}
extern "C" u16 menuPocket2Open(s32 mode, s32 *slotID, void *actor)
{
    memset(&Data, 0, POCKET_INITIAL_WORK_SIZE);
    Data.mMode = mode;
    if (mode == POCKET_MODE_BATTLE || mode == POCKET_MODE_SNATCH_ONLY_BATTLE) {
        Data.mTrainer = (FIGHT_TRAINER *)actor;
        Data.mHero = fightTrainerBiosGetHeroPtr((FIGHT_TRAINER *)actor);
        Data.mCurrentSlotID = *slotID;
        Data.mSelectedSlotID = Data.mCurrentSlotID;
        if (*slotID < 0 || *slotID >= POCKET_INITIAL_SLOT_LIMIT) Data.mCurrentSlotID = 0;
    } else {
        Data.mHero = (Hero *)actor;
        Data.mCurrentSlotID = -1;
    }
    Data.mOwnsFloor = 1;
    GSfloorPush(POCKET_MENU_FLOOR);
    floorSetFadeScript(0, 0);
    floorWaitForChange();
    if (Data.mResult == 0) return 0;
    if (slotID) *slotID = Data.mCurrentSlotID;
    return Data.mResult;
}



// MSL memory routine used to clear the item-script queue.



#pragma peephole off

// Return the last selected pocket slot.
u8 menuPocket2GetLastSlotID(void)
{
    return Data.mLastSlotID;
}

// menuPocketClearItemScript -- drop every queued item script.
void menuPocketClearItemScript(void)
{
    memset(menuPocketWork.mEntries, 0, sizeof(menuPocketWork.mEntries));
    menuPocketWork.mCount = 0;
}

// menuPocket2GetItemScript -- script queued for the given item, or -1 when
// the item has none queued.
s32 menuPocket2GetItemScript(u16 id)
{
    s32 script = -1;
    s32 i;

    for (i = 0; i < menuPocketWork.mCount; i++) {
        if (menuPocketWork.mEntries[i].mID == id) {
            script = menuPocketWork.mEntries[i].mScript;
            break;
        }
    }

    return script;
}

// menuPocket2AddItemScript -- queue the script registered for the given
// item, unless that pair is already queued or the queue is full.
void menuPocket2AddItemScript(u32 item, u32 script)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        MenuPocket2ItemScriptEntry *entry = &menuPocketWork.mEntries[i];
        if (entry->mID == (u16)item && entry->mScript == script) {
            return;
        }
    }

    if (menuPocketWork.mCount >= 7) {
        return;
    }

    menuPocketWork.mEntries[menuPocketWork.mCount].mID = (u16)item;
    menuPocketWork.mEntries[menuPocketWork.mCount].mScript = script;
    menuPocketWork.mCount++;
}

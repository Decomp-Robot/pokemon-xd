#include <menuPocket2.h>
#include <menuItemBios.h>
#include <winSprite.h>
#include <PocketVec.h>
#include <hero.h>
#include <GSgfx.h>

// The pocket menu: the item list, the action menu an item opens and the give,
// sell, deposit and use flows behind it.
//
// Which items the list holds is decided by the tab it shows; one tab is one
// entry of lPocketTabData. A tab names the item kind whose array the hero
// holds it in, the cursor its list is drawn with, the message that explains
// it, the action menus it offers and the routine that prints one of its rows.

// One row of an action menu. The handler returns 1 to leave the pocket, 0 to
// stay and 2 to stay when the pocket was entered from the field; the amount
// the action consumed is written through the third argument.
struct SActionMenuItem {
    s32 mMsgID; // 0x00: label message id.
    int (*mFunc)(int, int, int *); // 0x04: handler; null ends the table.
    u16 mFlags; // 0x08: bit0 only for usable items, bit1 hidden for
                //       important items.
};

// Handler of one action-menu row.
typedef int (*SActionMenuFunc)(int, int, int *);

// Routine the item data names for the item; run when the item is used and
// given the number of items the use consumed.
typedef int (*PocketItemUseFunc)(int, int *);

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
    u32 _04; // 0x04: unknown; window id of the tab.
    u32 _08; // 0x08: unknown; window id of the tab.
    PocketItemPrintFunc mPrint; // 0x0C: prints one row of the tab.
    u16 mCursorID; // 0x10: cursor the tab's list is drawn with.
    u16 _12; // 0x12: unknown.
    u32 mExplanationMsgID; // 0x14: explanation shown for the tab.
    SActionMenuItem *mActMenu; // 0x18: rows offered in the field menu.
    SActionMenuItem *mActMenu2; // 0x1C: rows offered in the shop menus.
};

extern PocketTabData lPocketTabData[];

// Cursor position (0, 0) a tab's cursor is put back to when its list shrank.
extern "C" PocketVec<short> lbl_804E7EE8;

// Frame hooks the meta-menu keeps for the menu it manages. The meta-menu's
// draw hook runs the third for records that fill it, its tutorial prompt
// gates on the fourth, the per-frame wait of an info window runs the fifth
// and an A press on a tutorial message runs the last one. The pocket record
// fills only the last three. No record in the game fills the first two.
struct PocketMenuProcess {
    void (*mInit)(); // 0x00: unused.
    void (*mExit)(); // 0x04: unused.
    void (*mDraw)(); // 0x08: extra drawing.
    int (*mIsAnimationStateNone)(); // 0x0C: gate of the tutorial prompt.
    void (*mMain)(); // 0x10: one frame of the menu.
    void (*mDecide)(); // 0x14: A press on a tutorial message.
};

extern "C" int menuPocketIsAnimationStateNone(void);
extern "C" void menuPocketTutorialDecideProc(void);
// One waited frame of the pocket menu: hands the thread back to the menu
// system. Owned by the menu bios.
extern "C" void zz_002010c_(void);

// The pocket menu's record. The menu has no item animations, so the gate
// always answers "none" and confirming a tutorial message does nothing.
extern "C" PocketMenuProcess lPocketMenuProcess = {
    0,
    0,
    0,
    menuPocketIsAnimationStateNone,
    zz_002010c_,
    menuPocketTutorialDecideProc,
};

// The rows the action menu is measured and laid out in, 0x54 bytes.
// _setActMenuResizeWork fills the two measured columns from the menu data of
// the row's item; the flag that follows belongs to the row's window.
struct ActMenuResizeEntry {
    u32 mItemID; // 0x00: menu-data id of the row.
    s16 mField4; // 0x04: measured value.
    s16 mField6; // 0x06: measured value.
    u8 mFlag; // 0x08: set for the rows the window draws itself.
    u8 _09[0x03]; // 0x09-0x0B: padding.
};

namespace {

__declspec(weak) ActMenuResizeEntry ActMenuResizeWork[7] = {
    {0xCF, 0, 0, 1, {0, 0, 0}},
    {0xD0, 0, 0, 0, {0, 0, 0}},
    {0xD1, 0, 0, 0, {0, 0, 0}},
    {0xD5, 0, 0, 1, {0, 0, 0}},
    {0xD4, 0, 0, 1, {0, 0, 0}},
    {0xD6, 0, 0, 0, {0, 0, 0}},
    {0xD9, 0, 0, 0, {0, 0, 0}},
};

}

// One entry of the item table the item-data accessors below read, 0x28 bytes.
// This is not the menu-item record of menuItemBios.h: the item table is strided
// 0x28 and the menu-item table 0x1C, and the two name different fields. The
// pocket menu only hands the pointer on and never reads a field itself, so the
// layout stays partial; the offsets are the ones the accessors read.
struct ItemData {
    u8 mBagPocket; // 0x00: bag pocket the item belongs to.
    u8 mImportant; // 0x01: item cannot be thrown away or sold.
    u8 _02; // 0x02: unknown.
    u8 mDepositAble; // 0x03: item may be put into the PC.
    u8 _04[0x02]; // 0x04-0x05: unknown.
    u16 mPrice; // 0x06: price the item is sold for.
    u8 _08[0x08]; // 0x08-0x0F: unknown.
    u32 mDescriptionMsgID; // 0x10: message id describing the item.
    u32 mNameMsgID; // 0x14: message id of the item name.
    u32 mMachineNo; // 0x18: machine number in the low byte.
    u32 mFieldUseFunc; // 0x1C: routine the item runs in the field.
    u32 mBattleUseFunc; // 0x20: routine the item runs in battle.
    u8 _24[0x04]; // 0x24-0x27: unknown.
};

// One entry of a hero item array: the item held and how many of it.
struct PocketItemSlot {
    u16 mItemID; // 0x00: item of the entry.
    u16 mNum; // 0x02: how many of it the hero holds.
};

// Window ids of the pocket menus.
enum {
    WIN_ID_SELECT_ITEM = 0x2C,
    WIN_ID_ACTION_MENU = 0x2D,
    WIN_ID_MESSAGE = 0x2D,
    WIN_ID_NUMBER_ITEM = 0x2E,
    WIN_ID_NUMBER_ITEM_MIN = 0x2F,
    WIN_ID_NUMBER_PRICE = 0x30,
    WIN_ID_NUMBER_PRICE_MIN = 0x31,
};

// Message codes the shop messages are built with.
enum {
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

// The two kinds of number the number menu edits.
enum {
    NUM_INPUT_ITEM = 1,
    NUM_INPUT_PRICE = 2,
};

// Parameter block of the number-input window.
struct NumInputParam {
    int mKind; // 0x00: which of the two windows is used.
    int mWindowID; // 0x04: window id of the number menu.
    int mValue; // 0x08: value the menu starts with.
    int mMin; // 0x0C: smallest value the menu accepts.
};

// ---- engine and library entry points -------------------------------------

extern "C" void winMsgOpen(int mode, u32 msgID, int a, int b);
extern "C" void winMsgOpenWithSE(int mode, u32 msgID, int a, int b, u32 se);
extern "C" void winMsgClose(int a);
extern "C" void menuClose(int windowID);
extern "C" int menuCloseSync(int windowID, int a);
extern "C" int menuCloseCustom(int windowID, int a, int b);
extern "C" int menuOpenCustom(int menuID, int parentID, void *param, int a, int b, int c, ...);
extern "C" int windowGetActiveID(void);
extern "C" int windowGetValue(int windowID, long *out);
extern "C" void msgctrlSetValue(int id, int value);
extern "C" int menuSubOpenYesNo(int a, int b, int c, int d);
extern "C" void GSgfxResetRestore(int a);
extern "C" void GSmsgPrint2(int layer, int x, u32 y, int a, u32 msgID);
extern "C" void GSsndPlay(int a, int b, int c);
extern "C" void zz_0020dd8_(u32 seID); // plays one sound effect.
extern "C" u32 zz_0060d08_(u8 *pSeOut, int a, int msgCode, int b, ...); // builds a shop message.
extern "C" unsigned long GSmsgGetMessageSize(u32 msgID); // width of one message (size packed: width in the high half).
extern "C" void floorPop(void);
extern "C" void fadeSet(int a, f32 value);
extern "C" void fadeCheck(int a);
extern "C" void floorSetFadeScript(int a, int b);
extern "C" void *memset(void *p, int value, u32 size);
extern "C" void *memmove(void *dst, const void *src, u32 size);
extern "C" PocketVec<short> cursorBiosGetPos(int cursorID);
extern "C" PocketVec<short> cursorBiosSetPos(int cursorID, PocketVec<short> pos);

// ---- item, menu and hero data -------------------------------------------

extern "C" ItemData *itemDataBiosGetPtr(u16 itemID);
extern "C" u8 itemDataBiosGetImportant(ItemData *data);
extern "C" u8 itemDataBiosGetDeposit(ItemData *data);
extern "C" u8 itemDataBiosGetKinomiNo(u16 itemID);
extern "C" u8 itemDataBiosGetHidenMachineNo(ItemData *data);
extern "C" u8 itemDataBiosGetWazaMachineNo(ItemData *data);
extern "C" int itemDataBiosGetWazaIDByWazaMachineNo(u8 machineNo);
extern "C" void *wazaDataBiosGetPtr(u16 wazaID);
extern "C" u8 itemCheckValid(void *itemPtr);
extern "C" u8 zz_item_data_get_bag_pocket(ItemData *data);
extern "C" u32 zz_item_data_get_name_id(ItemData *data);
extern "C" u32 zz_move_data_get_name_id(void *data);


// Item array of one hero pocket `kind`: `outNum` receives the number of slots
// the pocket offers and `outMax` its stock maximum.
extern "C" void *heroItemGetItemKindToItemAryPtr(Hero *hero, u8 kind, u16 *outNum, u16 *outMax, int a, int b);
extern "C" void *heroHizukiItemGetItemAryPtr(Hero *hero, u16 *outNum, int a, int b, int c);
extern "C" u16 zz_015e6d0_(void *itemSlot);
extern "C" u16 zz_015e6b8_(void *itemSlot);
extern "C" int zz_015ea84_(ItemData *data);
extern "C" int zz_015eb1c_(ItemData *data);
extern "C" u32 zz_item_data_get_function_pointer(ItemData *data);
extern "C" u32 zz_item_data_get_function_pointer_2(ItemData *data);
extern "C" u32 heroGetStatus(Hero *hero, int statusID, int fallback);
extern "C" void heroAddPokedoru(Hero *hero, u32 money);
extern "C" void heroItemDecItemDataId(Hero *hero, u16 itemID, u16 count, s16 menuPos);
extern "C" u16 pcboxGetItemCapacity(int a, u16 itemID);
extern "C" void pcboxAddItem(int a, u16 itemID, u16 count);
extern "C" u32 menuPokemonOpenItemGive(u8 kind, u8 flag, u16 itemID, int a);
extern "C" void menuPokemonClose(void);
extern "C" void menuPocketBattleDiskMain(int a, FIGHT_TRAINER *trainer, Hero *hero);
extern "C" u16 menuPocketCologne(int a);
// The throw-away menu is handed to _actionMenuOpen as one table.
struct SActionMenuTable {
    SActionMenuItem mItems[2];
};

extern SActionMenuTable sActionMenuSuteru; // rows of the throw-away menu.
extern "C" f32 lbl_804EC098; // zero the list scroll is compared against.
extern "C" f32 lbl_804EC09C; // fade value the shop leaves the screen with.

namespace {

u16 _getItemIDFromMenuPos(int tab, int menuPos);

} // namespace

// The pocket menu animates nothing, so its record in lPocketMenuProcess
// always reports "no animation running".
extern "C" int menuPocketIsAnimationStateNone(void)
{
    return 1;
}

// The pocket menu adds nothing to the A press that confirms a tutorial
// message.
extern "C" void menuPocketTutorialDecideProc(void)
{
}

// Whether the sort cursor is drawn: the tab still has an item under the
// cursor and no action has been chosen yet.
extern "C" u8 menuPocketIsSortCursorDisp(tagWINDOW_WORK *window)
{
    PocketVec<short> pos = cursorBiosGetPos(lPocketTabData[window->cursor].mCursorID);

    if (menuPocket2Work.mSelectedAction >= 0) {
        return 0;
    }

    s32 menuPos = pos.x + pos.y;

    if (_getItemIDFromMenuPos(window->cursor, menuPos) == 0 && menuPos == 0) {
        return 0;
    }
    return 1;
}

// Whether the swap cursor is drawn: an item under the cursor or an action
// already chosen.
extern "C" u8 menuPocketIsIrekaeCursorDisp(tagWINDOW_WORK *window)
{
    PocketVec<short> pos = cursorBiosGetPos(lPocketTabData[window->cursor].mCursorID);
    s32 menuPos = pos.x + pos.y;

    if (_getItemIDFromMenuPos(window->cursor, menuPos) == 0
        && menuPocket2Work.mSelectedAction < 0) {
        return 0;
    }
    return 1;
}

// File-local helpers of the pocket menu, declared before use.
namespace {

u16 _getItemNumFromMenuPos(int tab, int menuPos);
s32 _getNbItemSlot(int tab);
s32 _getStockMaxFromTagID(int tab);
s32 _makeActMenu(SActionMenuItem *menu, int max, int tab, int menuPos);
s32 _actionMenuOpen(const SActionMenuItem *menu, int num);
s32 _openNumberInputMenu(int value, int max, int kind, int min);
u16 _selectItem(int &menuPos, long *slotID);
s32 _execActionMenu(int tab, u16 itemID, int menuPos, u16 *outNum);
s32 _execDeposit(int tab, u16 itemID, int menuPos);
s32 _execSell(int tab, u16 itemID, int menuPos);
s32 _execGive(int tab, u16 itemID, int menuPos);
s32 _afterDecideItem(int mode, int tab, u16 itemID, int menuPos, u16 *outNum);
void _setActMenuResizeWork(void);
void _decItem(u16 itemID, int kind, int menuPos, int count);
void _sortItemsThisTab(u8 tab);
u8 _depositPC(u16 itemID, int count);
void _swapItemSlot(int item, int from, int to);
s32 _getItemPrice(int itemID);
int _getUseFunc(unsigned long itemID);
u32 _getMoney(void);
s32 _actSuteruNormal(int tab, int menuPos, int *num);
s32 _actMotaseruNormal(int tab, int menuPos, int *num);
s32 _actTukauNormal(int tab, int menuPos, int *num);
s32 _openPokemonMenuForGiveItem(u16 itemID, u8 pocket, u8 kind);
extern "C" u8 _getItemBagPocket(int itemID);
extern "C" u8 _getDepositAble(u16 itemID);
extern "C" int _getItemDescriptionMsgID(int itemID);
extern "C" u32 zz_0023c68_(int itemID);
void _setExplanationMsgID(unsigned long msgID);
extern "C" void zz_0023b20_(GSgfxLayerID layer, int x, int tab, unsigned short *pos);
extern "C" void zz_0023560_(GSgfxLayerID layer, int x, int y, int tab, int index);
void _printItemTaisetsu(GSgfxLayerID layer, int x, int y, int tab, int index);
extern "C" void zz_0023690_(GSgfxLayerID layer, int x, int y, int tab, int index);
extern "C" void zz_00238e4_(GSgfxLayerID layer, int x, int y, int tab, int index);
u16 _menuPocketLoop(void);
}

// Message callback of the error window the shop actions raise.
void _menuCB_errorMsgDisp2(unsigned long msgID)
{
    winMsgOpen(2, msgID, 1, 0);
    winMsgClose(1);
}

namespace {

// Plays the information beep and shows one message window.
void _infoBeep(unsigned long msgID)
{
    zz_0020dd8_(0x13);
    winMsgOpen(2, msgID, 1, 0);
    winMsgClose(1);
}

// Lays the action menu of the selected item out: the rows of the tab table
// the item may be offered. Returns the number of rows.
s32 _makeActMenu(SActionMenuItem *menu, int max, int tab, int menuPos)
{
    ItemData *data = itemDataBiosGetPtr(_getItemIDFromMenuPos(tab, menuPos));
    u8 usable;

    if (zz_item_data_get_function_pointer_2(data) != 0) {
        usable = 1;
    } else {
        usable = 0;
    }

    s32 mode = menuPocket2Work.mMode;
    PocketTabData *tabData = &lPocketTabData[menuPocket2Work.mTab];
    SActionMenuItem *src;

    if (mode == 1 || mode == 6) {
        src = tabData->mActMenu2;
    } else if (mode == 0) {
        src = tabData->mActMenu;
    }

    s32 num = 0;

    while (src[num].mFunc != 0) {
        num++;
    }
    num++;

    s32 count = 0;

    for (s32 i = 0; i < num; i++) {
        if ((src[i].mFlags & 1) && !usable) {
            continue;
        }
        if (itemDataBiosGetImportant(data) != 0 && (src[i].mFlags & 2)) {
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
        ActMenuResizeWork[i].mField4 =
            menuItemBiosGetPtr(MENU_DATA_ACT_ROW, ActMenuResizeWork[i].mItemID)->mOffsetA;
        ActMenuResizeWork[i].mField6 =
            menuItemBiosGetPtr(MENU_DATA_ACT_ROW, ActMenuResizeWork[i].mItemID)->mOffsetC;
    }
}

// Puts the chosen number of the item into the PC. Returns 0 when the action
// finished (or failed), 1 while the pocket should go on.
s32 _execDeposit(int tab, u16 itemID, int menuPos)
{
    if (!_getDepositAble(itemID)) {
        msgctrlSetValue(WIN_ID_MESSAGE, itemID);
        _infoBeep(0xC37B);
        return 0;
    }

    s32 num;

    if ((u8)lPocketTabData[menuPocket2Work.mTab].mItemKind != 5) {
        _setExplanationMsgID(0x3AAC);

        s32 kind;

        if (_getStockMaxFromTagID(tab) > 100) {
            kind = 3;
        } else {
            kind = 2;
        }

        num = _openNumberInputMenu(NUM_INPUT_ITEM, _getItemNumFromMenuPos(tab, menuPos), kind, 0);
    } else {
        num = 1;
    }

    if (num < 0) {
        return 0;
    }

    if (!_depositPC(itemID, num)) {
        msgctrlSetValue(WIN_ID_MESSAGE, itemID);
        _infoBeep(0x3AC0);
        return 0;
    }

    _decItem(itemID, lPocketTabData[menuPocket2Work.mTab].mItemKind, menuPos, num);
    msgctrlSetValue(WIN_ID_MESSAGE, itemID);
    msgctrlSetValue(WIN_ID_NUMBER_ITEM_MIN, num);
    _menuCB_errorMsgDisp2(0xC37C);
    return 1;
}

// Sells the chosen number of the item.
s32 _execSell(int tab, u16 itemID, int menuPos)
{
    s32 window = menuPocket2Work.mMsgWindowID;
    s32 price = _getItemPrice(itemID);
    u8 se;

    if (price <= 0) {
        zz_0020dd8_(0x13);
        s32 msg = zz_0060d08_(&se, window, MENU_MSG_ITEM_NUM, 0x2D, itemID, -1);
        winMsgOpenWithSE(2, msg, 1, 0, se);
        winMsgClose(1);
        return 0;
    }

    _setExplanationMsgID(zz_0060d08_(&se, window, MENU_MSG_ITEM_PRICE, 0x2D, itemID, -1));

    s32 kind;

    if (_getStockMaxFromTagID(tab) > 100) {
        kind = 3;
    } else {
        kind = 2;
    }

    s32 max = _getItemNumFromMenuPos(tab, menuPos);
    s32 each = price / 2;
    s32 num = _openNumberInputMenu(NUM_INPUT_ITEM, max, kind, each);

    if (num < 0) {
        return 0;
    }

    s32 total = each * num;

    s32 msg = zz_0060d08_(&se, window, MENU_MSG_SELL_TOTAL, 0x4B, total, -1);
    winMsgOpenWithSE(2, msg, 1, 0, se);

    s8 answer = (s8)menuSubOpenYesNo(0, -1, -1, 0);

    if (answer == 1 || answer == -1) {
        winMsgClose(1);
        return 0;
    }

    _decItem(itemID, lPocketTabData[menuPocket2Work.mTab].mItemKind, menuPos, num);
    GSsndPlay(0x1A, 0, 0);
    heroAddPokedoru(menuPocket2Work.mHero, total);
    msg = zz_0060d08_(&se, window, MENU_MSG_SELL_CONFIRM, 0x2D, itemID, 0x4B, total, -1);
    _menuCB_errorMsgDisp2(msg);
    return 1;
}

// Hands the item to a pokemon.
s32 _execGive(int tab, u16 itemID, int menuPos)
{
    if (itemDataBiosGetImportant(itemDataBiosGetPtr(itemID)) != 0) {
        msgctrlSetValue(WIN_ID_MESSAGE, itemID);
        _infoBeep(0xC37A);
        return 0;
    }

    menuPocket2Work.mLastSlotID = menuPos;
    return 1;
}

// Picks the row of the action menu and runs it.
s32 _execActionMenu(int tab, u16 itemID, int menuPos, u16 *outNum)
{
    SActionMenuItem menu[4];
    int id = itemID;
    int msgID;
    s32 num;

    *outNum = 0;

    if (menuPocket2Work.mMode == 0) {
        if (itemID == 0x215) {
            _setExplanationMsgID(0x3C1B);
            menuPocketBattleDiskMain(-1, menuPocket2Work.mTrainer, menuPocket2Work.mHero);
            return 1;
        }
        if (itemID == 0x200) {
            _setExplanationMsgID(0x3AB8);

            u16 cologne = menuPocketCologne(-1);
            u32 cologneID = cologne;

            if (cologneID == 0) {
                return 1;
            }
            *outNum = cologne;

            PocketItemUseFunc useFunc = (PocketItemUseFunc)_getUseFunc(cologneID);

            if (useFunc == 0) {
                _infoBeep(0xC374);
                return 0;
            }
            if (useFunc(cologneID, &msgID) == 2) {
                if (msgID > 0) {
                    _decItem(cologne, 6, -1, msgID);
                }
                return 2;
            }
            return 1;
        }
    }

    num = _makeActMenu(menu, 4, tab, menuPos);

    _setExplanationMsgID(0x4296);

    s32 row = _actionMenuOpen(menu, num);

    if (row < 0) {
        return 1;
    }

    SActionMenuFunc func = menu[row].mFunc;

    if (func == 0) {
        return 1;
    }

    switch (func(tab, menuPos, &msgID)) {
    case 0:
        if (msgID > 0) {
            _decItem(itemID, lPocketTabData[menuPocket2Work.mTab].mItemKind, menuPos, msgID);
        }
        menuPocket2Work.mLastSlotID = menuPos;
        return 0;

    case 1:
        return 1;

    case 2:
        if (msgID > 0) {
            _decItem(itemID, lPocketTabData[menuPocket2Work.mTab].mItemKind, menuPos, msgID);
        }
        return 2;
    }
    return 0;
}

// Runs the action the player picked from the menu of the selected item.
s32 _afterDecideItem(int mode, int tab, u16 itemID, int menuPos, u16 *outNum)
{
    u16 num = 0;
    s32 result;

    switch (mode) {
    case 0:
    case 1:
    case 6:
        result = _execActionMenu(tab, itemID, menuPos, &num);
        if (result == 2) {
            result = 1;
        } else if (mode == 0) {
            result = 0;
        } else if (result == 0) {
            result = 1;
        } else {
            result = 0;
        }
        break;

    case 2:
    case 5:
        if ((u8)_execGive(tab, itemID, menuPos) != 0) {
            result = 1;
        } else {
            result = 0;
        }
        break;

    case 3:
        _execSell(tab, itemID, menuPos);
        result = 0;
        break;

    case 4:
        _execDeposit(tab, itemID, menuPos);
        result = 0;
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

// Pokedoru (money) the hero carries.
__declspec(weak) u32 _getMoney(void)
{
    return heroGetStatus(menuPocket2Work.mHero, 0xC, 0);
}

// Moves slot `from` of the tab to slot `dest`.
__declspec(weak) void _swapItemSlot(int item, int from, int dest)
{
    u16 num;

    if (from == dest || from == dest - 1) {
        return;
    }

    PocketItemSlot *ary = (PocketItemSlot *)heroItemGetItemKindToItemAryPtr(
        menuPocket2Work.mHero, (u8)item, &num, 0, 0, 0);
    PocketItemSlot entry = ary[from];

    if (from != num - 1) {
        memmove(ary + from, ary + from + 1, (num - from - 1) * 4);
    }
    if (from < dest) {
        dest--;
    }

    memmove(ary + dest + 1, ary + dest, (num - dest - 1) * 4);
    ary[dest] = entry;
}

// Items the hero owns on the tab, sorted.
__declspec(weak) void _sortItemsThisTab(u8 tab)
{
    menuPocket2Work.mHero->itemSort(tab);
}

// Takes `count` of the item out of the hero list.
void _decItem(u16 itemID, int kind, int menuPos, int count)
{
    heroItemDecItemDataId(menuPocket2Work.mHero, itemID, count, menuPos);
}

// Opens the action menu and returns the row that was picked, -1 when it was
// left without picking one.
s32 _actionMenuOpen(const SActionMenuItem *menu, int num)
{
    u32 param[2];

    param[0] = (u32)menu;
    param[1] = (u32)num;

    s32 result = menuOpenCustom(WIN_ID_ACTION_MENU, windowGetActiveID(), 0, 0, 1, 1, param);

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

// Opens the number menu on `value` and returns what the player entered, -1
// when the menu was left. A negative maximum refuses to open the menu.
s32 _openNumberInputMenu(int value, int max, int kind, int min)
{
    NumInputParam param;

    if (max < 0) {
        return 0;
    }

    s32 windowID;

    if (min > 0) {
        if (kind == NUM_INPUT_PRICE) {
            param.mKind = 1;
            windowID = WIN_ID_NUMBER_PRICE;
        } else {
            param.mKind = 2;
            windowID = WIN_ID_NUMBER_PRICE_MIN;
        }
    } else {
        if (kind == NUM_INPUT_PRICE) {
            param.mKind = 1;
            windowID = WIN_ID_NUMBER_ITEM;
        } else {
            param.mKind = 2;
            windowID = WIN_ID_NUMBER_ITEM_MIN;
        }
    }

    param.mWindowID = windowID;
    param.mValue = max;
    param.mMin = min;
    menuPocket2Work.mNumInputValue = value;

    // The window routine is handed the word the window id was stored in.
    s32 *pWindowID = (s32 *)&param + 1;
    s32 result = menuOpenCustom(windowID, windowGetActiveID(), &param, 0, 1, 1, pWindowID);

    menuClose(windowID);
    menuCloseSync(windowID, 1);

    if (result == -1 || menuPocket2Work.mNumInputValue == 0) {
        return -1;
    }
    return menuPocket2Work.mNumInputValue;
}

// Price the item is sold for.
s32 _getItemPrice(int itemID)
{
    return (u16)zz_015ea84_(itemDataBiosGetPtr((u16)itemID));
}

// Bag pocket the item belongs to, ItemData::mBagPocket.
extern "C" u8 _getItemBagPocket(int itemID)
{
    return zz_item_data_get_bag_pocket(itemDataBiosGetPtr((u16)itemID));
}

// Whether the item may be put into the PC, ItemData::mDepositAble.
extern "C" u8 _getDepositAble(u16 itemID)
{
    return itemDataBiosGetDeposit(itemDataBiosGetPtr(itemID));
}

// Routine the item runs when it is used.
int _getUseFunc(unsigned long itemID)
{
    ItemData *data = itemDataBiosGetPtr((u16)itemID);

    if (menuPocket2Work.mMode == 0) {
        return zz_item_data_get_function_pointer(data);
    }
    return zz_item_data_get_function_pointer_2(data);
}

// Count of the row `menuPos` of the tab.
u16 _getItemNumFromMenuPos(int tab, int menuPos)
{
    u16 num;
    int kind = lPocketTabData[tab].mItemKind;
    void *ary;

    if (kind >= 0) {
        ary = heroItemGetItemKindToItemAryPtr(menuPocket2Work.mHero, (u8)kind, &num, 0, 0, 0);
    } else {
        ary = heroHizukiItemGetItemAryPtr(menuPocket2Work.mHero, &num, 0, 0, 0);
    }

    s32 i;
    s32 n = -1;

    for (i = 0; i < num; i++, ary = (u8 *)ary + 4) {
        if (itemCheckValid(ary)) {
            if (++n >= menuPos) {
                return zz_015e6b8_(ary);
            }
        }
    }
    return 0;
}

// Number of slots the hero array of the tab kind offers.
s32 _getStockMaxFromTagID(int tab)
{
    u16 num;

    heroItemGetItemKindToItemAryPtr(menuPocket2Work.mHero, (u8)lPocketTabData[tab].mItemKind, 0, &num, 0, 0);
    return num;
}

// Description message id of the item, ItemData::mDescriptionMsgID.
extern "C" int _getItemDescriptionMsgID(int itemID)
{
    return zz_015eb1c_(itemDataBiosGetPtr((u16)itemID));
}

// Number of rows the tab holds.
__declspec(weak) s32 _getNbItemSlot(int tab)
{
    u16 num;
    int kind = lPocketTabData[tab].mItemKind;
    void *ary;

    if (kind >= 0) {
        ary = heroItemGetItemKindToItemAryPtr(menuPocket2Work.mHero, (u8)kind, &num, 0, 0, 0);
    } else {
        ary = heroHizukiItemGetItemAryPtr(menuPocket2Work.mHero, &num, 0, 0, 0);
    }

    s32 i;
    s32 nb = 0;

    for (i = 0; i < num; i++, ary = (u8 *)ary + 4) {
        if (itemCheckValid(ary)) {
            nb++;
        }
    }
    return nb;
}

// Throws the chosen number of the item away.
__declspec(weak) s32 _actSuteruNormal(int tab, int menuPos, int *num)
{
    SActionMenuTable menu = sActionMenuSuteru;

    _setExplanationMsgID(0x3AAA);

    s32 kind;

    if (_getStockMaxFromTagID(tab) > 100) {
        kind = 3;
    } else {
        kind = 2;
    }

    s32 count = _openNumberInputMenu(NUM_INPUT_ITEM, _getItemNumFromMenuPos(tab, menuPos), kind, 0);

    if (count < 0) {
        *num = 0;
        return 0;
    }

    u16 itemID = _getItemIDFromMenuPos(tab, menuPos);

    msgctrlSetValue(WIN_ID_MESSAGE, itemID);
    msgctrlSetValue(WIN_ID_NUMBER_ITEM_MIN, count);
    _setExplanationMsgID(0x3AAB);

    s32 row = _actionMenuOpen(menu.mItems, 2);

    if (row == 1 || row == -1) {
        *num = 0;
        return 0;
    }

    msgctrlSetValue(WIN_ID_MESSAGE, itemID);
    msgctrlSetValue(WIN_ID_NUMBER_ITEM_MIN, count);
    _menuCB_errorMsgDisp2(0xC379);
    *num = count;
    return 0;
}

// Opens the pokemon menu so the item can be given to a pokemon; whether the
// item was handed over is returned: the slot id the menu leaves behind is
// negative when it was left without handing the item over.
s32 _openPokemonMenuForGiveItem(u16 itemID, u8 pocket, u8 flag)
{
    menuCloseCustom(WIN_ID_SELECT_ITEM, 0, 1);
    menuPocket2Work.mCurrentSlotID = menuPokemonOpenItemGive(pocket, flag, itemID, 0);

    GSgfxResetRestore(0);
    menuPokemonClose();

    return (menuPocket2Work.mCurrentSlotID >> 31) ^ 1;
}

// Gives the item to a pokemon.
__declspec(weak) s32 _actMotaseruNormal(int tab, int menuPos, int *num)
{
    u16 itemID = _getItemIDFromMenuPos(tab, menuPos);

    if (itemDataBiosGetHidenMachineNo(itemDataBiosGetPtr(itemID)) != 0xFF) {
        msgctrlSetValue(WIN_ID_MESSAGE, itemID);
        _infoBeep(0xC37A);
        *num = 0;
        return 0;
    }

    u8 pocket = _getItemBagPocket(itemID);

    if (!(u8)_openPokemonMenuForGiveItem(itemID, pocket, (u8)menuPos)) {
        return 1;
    }

    *num = 0;
    return 0;
}

// Uses the item: runs the routine the item data names.
__declspec(weak) s32 _actTukauNormal(int tab, int menuPos, int *num)
{
    u16 itemID = _getItemIDFromMenuPos(tab, menuPos);
    int id = itemID;
    PocketItemUseFunc useFunc = (PocketItemUseFunc)_getUseFunc(id);

    if (useFunc == 0) {
        _infoBeep(0xC374);
        *num = 0;
        return 1;
    }

    s32 result = useFunc(id, num);
    s32 ret;

    switch (result) {
    case 0:
        ret = 0;
        break;

    case 1:
        ret = 1;
        break;

    case 2:
        if (tab == 4 && itemID != 0x1F6) {
            ret = 0;
        } else {
            ret = 2;
        }
        break;
    }

    return ret;
}

// Prints the name of the item of one row.
extern "C" __declspec(weak) void zz_0023560_(GSgfxLayerID layer, int x, int y, int tab, int index)
{
    u32 msgID = _getItemDescriptionMsgID(_getItemIDFromMenuPos(tab, index));

    if (msgID != 0) {
        GSmsgPrint2(layer, x, y, -1, msgID);
    }

    s32 max = _getStockMaxFromTagID(tab);

    msgctrlSetValue(0x34, max);

    s16 off = (s16)(GSmsgGetMessageSize(0x4270) >> 16);

    off += (s16)(GSmsgGetMessageSize(0x428F) >> 16);

    s32 ox = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mOffsetB;

    GSmsgPrint2(layer, ox - off, y, -1, 0x428F);

    msgctrlSetValue(0x34, _getItemNumFromMenuPos(tab, index));

    s16 off2 = (s16)(GSmsgGetMessageSize(0x4270) >> 16);

    s32 ox2 = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mOffsetB;

    GSmsgPrint2(layer, ox2 - off2 + 1, y, -1, 0x4270);
}

// Prints one row of the technical-machine tab: the number of the machine,
// the move it teaches and the number of items held.
extern "C" __declspec(weak) void zz_0023690_(GSgfxLayerID layer, int x, int y, int tab, int index)
{
    s32 x2;
    s16 off2;

    ItemData *data = itemDataBiosGetPtr(_getItemIDFromMenuPos(tab, index));
    int machine = itemDataBiosGetWazaMachineNo(data);
    int machineNo = machine + 1;
    u8 hidden = itemDataBiosGetHidenMachineNo(data);
    u8 hiddenNo = hidden + 1;
    s32 msgID;

    if (hidden != 0xFF) {
        msgID = 0x3AA0;
    } else {
        msgID = 0x3A9F;
    }

    GSmsgPrint2(layer, x, y, -1, msgID);

    x2 = x + (GSmsgGetMessageSize(msgID) >> 16);

    if (hidden != 0xFF) {
        msgctrlSetValue(0x34, hiddenNo);
        GSmsgPrint2(layer, x2, y, -1, 0x4270);
        x2 += GSmsgGetMessageSize(0x4270) >> 16;
    } else {
        if (machineNo < 10) {
            msgctrlSetValue(0x34, 0);
            GSmsgPrint2(layer, x2, y, -1, 0x4270);
            x2 += GSmsgGetMessageSize(0x4270) >> 16;
        }
        msgctrlSetValue(0x34, machineNo);
        GSmsgPrint2(layer, x2, y, -1, 0x4270);
        x2 += GSmsgGetMessageSize(0x4270) >> 16;
    }

    s32 nameID = zz_move_data_get_name_id(wazaDataBiosGetPtr(
        itemDataBiosGetWazaIDByWazaMachineNo(machine)));

    GSmsgPrint2(layer, x2 + 9, y, -1, nameID);

    s32 max = _getStockMaxFromTagID(tab);

    msgctrlSetValue(0x34, max);

    s16 off = (s16)(GSmsgGetMessageSize(0x4270) >> 16);

    off += (s16)(GSmsgGetMessageSize(0x428F) >> 16);

    s32 ox = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mOffsetB;

    GSmsgPrint2(layer, ox - off, y, -1, 0x428F);

    msgctrlSetValue(0x34, _getItemNumFromMenuPos(tab, index));

    off2 = (s16)(GSmsgGetMessageSize(0x4270) >> 16);

    s32 ox2 = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mOffsetB;

    GSmsgPrint2(layer, ox2 - off2 + 1, y, -1, 0x4270);
}

// Prints one row of the berry tab: the number of berries and the name of the
// berry.
extern "C" __declspec(weak) void zz_00238e4_(GSgfxLayerID layer, int x, int y, int tab, int index)
{
    s16 off2;

    GSmsgPrint2(layer, x, y, -1, 0x3A9F);

    s32 ox = x + (GSmsgGetMessageSize(0x3A9F) >> 16);

    u16 itemID = _getItemIDFromMenuPos(tab, index);
    u8 kinomi = itemDataBiosGetKinomiNo(itemID) + 1;

    if (kinomi < 10) {
        msgctrlSetValue(0x34, 0);
        GSmsgPrint2(layer, ox, y, -1, 0x4270);
        ox += GSmsgGetMessageSize(0x4270) >> 16;
    }

    msgctrlSetValue(0x34, kinomi);
    GSmsgPrint2(layer, ox, y, -1, 0x4270);
    ox += GSmsgGetMessageSize(0x4270) >> 16;

    s32 infoMsgID = _getItemDescriptionMsgID(itemID);

    GSmsgPrint2(layer, ox + 9, y, -1, infoMsgID);

    s32 max = _getStockMaxFromTagID(tab);

    msgctrlSetValue(0x34, max);

    s16 off = (s16)(GSmsgGetMessageSize(0x4270) >> 16);

    off += (s16)(GSmsgGetMessageSize(0x428F) >> 16);

    s32 ox2 = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mOffsetB;

    GSmsgPrint2(layer, ox2 - off, y, -1, 0x428F);

    msgctrlSetValue(0x34, _getItemNumFromMenuPos(tab, index));

    off2 = (s16)(GSmsgGetMessageSize(0x4270) >> 16);

    s32 ox3 = x + menuItemBiosGetPtr(MENU_DATA_ACT_ROW, MENU_DATA_ACT_ROW_ITEM)->mOffsetB;

    GSmsgPrint2(layer, ox3 - off2 + 1, y, -1, 0x4270);
}

// Prints the name of the item of one row; used by the tabs whose rows only
// show a name, the key-item and TM tabs.
__declspec(weak) void _printItemTaisetsu(GSgfxLayerID layer, int x, int y, int tab, int index)
{
    u32 msgID = _getItemDescriptionMsgID(_getItemIDFromMenuPos(tab, index));

    if (msgID != 0) {
        GSmsgPrint2(layer, x, y, -1, msgID);
    }
}

// Draws the rows of the tab list that are visible around the cursor.
extern "C" __declspec(weak) void zz_0023b20_(GSgfxLayerID layer, int x, int tab, unsigned short *pos)
{
    s32 y = 0;
    PocketTabData *tabData = &lPocketTabData[tab];
    s32 num = _getNbItemSlot(tab);
    s32 index = *(s16 *)pos;
    s32 first = 0;
    s32 rows = 9;
    s32 scroll = 0;

    if (lbl_804EC098 != menuPocket2Work.mListScroll && menuPocket2Work.mListScrollFlag != 0) {
        if (menuPocket2Work.mListScroll < lbl_804EC098) {
            index--;
            first = -1;
        } else {
            rows = 10;
        }
        scroll = (s32)menuPocket2Work.mListScroll;
    }

    y = first * 30;

    s32 i = first;

    while (i < rows && index < num) {
        if (index >= 0) {
            tabData->mPrint(layer, x, y - scroll, tab, index);
        }
        y += 30;
        i++;
        index++;
    }

    if (i < rows) {
        s32 yPos = i * 30 - scroll;

        GSmsgPrint2(layer, x, yPos, -1, menuPocket2Work.mMode == 4 ? 0x3BBA : 0x3A9E);
    }
}

// Sets the message shown under the item list.
__declspec(weak) void _setExplanationMsgID(unsigned long msgID)
{
    menuPocket2Work.mExplanationMsgID = msgID;
}

// Message id of the item name.
extern "C" __declspec(weak) u32 zz_0023c68_(int itemID)
{
    return zz_item_data_get_name_id(itemDataBiosGetPtr((u16)itemID));
}

// Item id of the row `menuPos` of the tab.
__declspec(weak) u16 _getItemIDFromMenuPos(int tab, int menuPos)
{
    u16 num;
    void *ary = heroItemGetItemKindToItemAryPtr(menuPocket2Work.mHero, lPocketTabData[tab].mItemKind,
                                                &num, 0, 0, 0);
    s32 n = -1;

    for (s32 i = 0; i < num; i++, ary = (u8 *)ary + 4) {
        if (itemCheckValid(ary)) {
            if (++n >= menuPos) {
                return zz_015e6d0_(ary);
            }
        }
    }
    return 0;
}

// Opens the item-select window on `slotID` and returns the item the cursor
// is on; 0 when the window was left.
u16 _selectItem(int &menuPos, long *slotID)
{
    menuPocket2Work.mSelectedAction = -1;

    s32 tab = menuOpenCustom(WIN_ID_SELECT_ITEM, windowGetActiveID(), slotID, 0, 1, 0);

    if (tab == -1) {
        return 0;
    }

    menuPocket2Work.mTab = tab;

    PocketVec<short> pos = cursorBiosGetPos(lPocketTabData[tab].mCursorID);

    menuPos = pos.x + pos.y;
    return _getItemIDFromMenuPos(tab, menuPos);
}

// The pocket loop: draws the list, waits for the player to pick an item,
// runs the action menu on it and repeats until the pocket is left.
__declspec(weak) u16 _menuPocketLoop(void)
{
    PocketVec<short> pos;

    memset(&pos, 0, sizeof(pos));

    if (menuPocket2Work.mMode == 6) {
        pos.y = 1;
    } else {
        pos = cursorBiosGetPos(2);
    }

    long slotID = pos.y;
    long *pSlotID = &slotID;

    for (s32 i = 0; i < 5; i++) {
        PocketVec<short> tabPos = cursorBiosGetPos(lPocketTabData[i].mCursorID);

        if (tabPos.x + tabPos.y >= _getNbItemSlot(i) + 1) {
            cursorBiosSetPos(lPocketTabData[i].mCursorID, lbl_804E7EE8);
        }
    }

    if (menuPocket2Work.mSkipActMenuResize == 0) {
        _setActMenuResizeWork();
    }

    u16 item = 0;
    s32 menuPos = 0;

    for (;;) {
        menuPocket2Work.mSelecting = 1;
        item = _selectItem(menuPos, pSlotID);
        menuPocket2Work.mSelecting = 0;
        pSlotID = 0;

        if (item == 0) {
            goto leave;
        }

        u16 out;
        s32 result = _afterDecideItem(menuPocket2Work.mMode, menuPocket2Work.mTab, item, menuPos, &out);

        if (out != 0) {
            item = out;
        }

        switch (result) {
        case 1:
            goto leave;
        case 0:
            continue;
        }
    }

leave:
    menuCloseCustom(WIN_ID_SELECT_ITEM, 0, 1);

    if (menuPocket2Work.mMode == 4) {
        fadeSet(3, lbl_804EC09C);
        fadeCheck(1);
    }

    if (menuPocket2Work.mFromField != 0) {
        GSgfxResetRestore(1);
        floorPop();
        floorSetFadeScript(0, 0);
    }

    return item;
}

// Whether the tab shown in the window may be sorted.
__declspec(weak) u8 _IsSortAble(tagWINDOW_WORK *window)
{
    if (menuPocket2Work.mMode == 3 || menuPocket2Work.mMode == 4 || menuPocket2Work.mMode == 5) {
        return 0;
    }

    return lPocketTabData[window->cursor].mSortAble;
}

// Whether the tab shown in the window may be swapped.
__declspec(weak) u8 _IsIrekaeAble(tagWINDOW_WORK *window)
{
    if (menuPocket2Work.mMode == 3 || menuPocket2Work.mMode == 4 || menuPocket2Work.mMode == 5) {
        return 0;
    }

    return lPocketTabData[window->cursor].mSwapAble;
}

} // namespace

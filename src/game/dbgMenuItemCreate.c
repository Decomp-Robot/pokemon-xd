#include <heroItem.h>
#include <menu.h>
#include <types.h>
#include <winSprite.h>

/* Debug "create item" menu: picks an item id and an amount and hands them to
 * the hero's item list.  dbgItemCreateInitialized is the once-only setup flag, dbgItemCreateSelectedItem
 * the item id cursor and dbgItemCreateQuantity the amount cursor; itemNameTableCount points at
 * the item name table count. */

extern s32 dbgItemCreateInitialized;
extern s32 dbgItemCreateSelectedItem;
extern s32 dbgItemCreateQuantity;
extern s32 *itemNameTableCount;

u8 itemDataCheckValid(u16 itemID);

enum {
    DEBUG_ITEM_PREVIOUS_FIELD = 0x1,
    DEBUG_ITEM_NEXT_FIELD = 0x2,
    DEBUG_ITEM_DECREMENT_ONE = 0x4,
    DEBUG_ITEM_INCREMENT_ONE = 0x8,
    DEBUG_ITEM_DECREMENT_TEN = 0x200,
    DEBUG_ITEM_INCREMENT_TEN = 0x400
};

#pragma peephole off
#pragma dont_inline on

/* Amount cursor: 1..999 in steps of 1 or 10. */
void nbItemCtrl(void)
{
    struct tagWINDOW_KEY *key = windowGetKeyInfo();

    if (key->mKeyRepeat & DEBUG_ITEM_INCREMENT_ONE) {
        dbgItemCreateQuantity++;
        if (dbgItemCreateQuantity > 999) {
            dbgItemCreateQuantity = 999;
        }
    }
    if (key->mKeyRepeat & DEBUG_ITEM_INCREMENT_TEN) {
        dbgItemCreateQuantity += 10;
        if (dbgItemCreateQuantity > 999) {
            dbgItemCreateQuantity = 999;
        }
    }
    if (key->mKeyRepeat & DEBUG_ITEM_DECREMENT_ONE) {
        dbgItemCreateQuantity--;
        if (dbgItemCreateQuantity < 0) {
            dbgItemCreateQuantity = 0;
        }
    }
    if (key->mKeyRepeat & DEBUG_ITEM_DECREMENT_TEN) {
        dbgItemCreateQuantity -= 10;
        if (dbgItemCreateQuantity < 0) {
            dbgItemCreateQuantity = 0;
        }
    }
}

void itemNameCtrl(void)
{
    struct tagWINDOW_KEY *key = windowGetKeyInfo();

    if (key->mKeyRepeat & DEBUG_ITEM_INCREMENT_ONE) {
        if (++dbgItemCreateSelectedItem >= *itemNameTableCount) {
            dbgItemCreateSelectedItem = *itemNameTableCount - 1;
        }
    }
    if (key->mKeyRepeat & DEBUG_ITEM_INCREMENT_TEN) {
        if ((dbgItemCreateSelectedItem += 10) >= *itemNameTableCount) {
            dbgItemCreateSelectedItem = *itemNameTableCount - 1;
        }
    }
    if (key->mKeyRepeat & DEBUG_ITEM_DECREMENT_ONE) {
        dbgItemCreateSelectedItem--;
        if (dbgItemCreateSelectedItem < 0) {
            dbgItemCreateSelectedItem = 0;
        }
    }
    if (key->mKeyRepeat & DEBUG_ITEM_DECREMENT_TEN) {
        dbgItemCreateSelectedItem -= 10;
        if (dbgItemCreateSelectedItem < 0) {
            dbgItemCreateSelectedItem = 0;
        }
    }
}

s32 menuDbgItemCreateCursor(struct tagWINDOW_WORK *window)
{
    struct tagWINDOW_KEY *key = windowGetKeyInfo();

    if (key->mKeyRepeat & DEBUG_ITEM_PREVIOUS_FIELD) {
        if (--window->mCursorState.mSelection.mCursor < 0) {
            window->mCursorState.mSelection.mCursor = 0;
        }
    }
    if (key->mKeyRepeat & DEBUG_ITEM_NEXT_FIELD) {
        if (++window->mCursorState.mSelection.mCursor > 1) {
            window->mCursorState.mSelection.mCursor = 1;
        }
    }
    switch (window->mCursorState.mSelection.mCursor) {
    case 0:
        itemNameCtrl();
        break;
    case 1:
        nbItemCtrl();
        break;
    }
    return 0;
}

void menuDbgItemCreate(void)
{
    s32 ret;

    if (dbgItemCreateInitialized == 0) {
        dbgItemCreateSelectedItem = 1;
        dbgItemCreateInitialized = 1;
    }
    dbgItemCreateQuantity = 1;
    for (;;) {
        if (menuOpen(MENU_ID_DEBUG_ITEM_CREATE, 1) == -1) {
            break;
        }
        if (!itemDataCheckValid((u16)dbgItemCreateSelectedItem)) {
            continue;
        }
        if (dbgItemCreateQuantity < 1 || dbgItemCreateQuantity > 999) {
            continue;
        }
        ret = menuOpen(MENU_ID_DEBUG_EDIT_CONFIRMATION, 1);
        menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
        if (ret != 0) {
            continue;
        }
        heroItemAddItemDataId(0, (u16)dbgItemCreateSelectedItem, (u16)dbgItemCreateQuantity, -1);
    }
    menuClose(MENU_ID_DEBUG_ITEM_CREATE);
    menuCloseSync(MENU_ID_DEBUG_ITEM_CREATE, 1);
}

#pragma dont_inline reset

#pragma peephole reset

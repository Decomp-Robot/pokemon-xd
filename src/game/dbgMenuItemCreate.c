#include <hero.h>
#include <menu.h>
#include <types.h>
#include <itemDataBios.h>
#include <winSprite.h>

/* Debug "create item" menu: picks an item id and an amount and hands them to
 * the hero's item list. */

extern s32 initFlag;
extern s32 ItemID;
extern s32 Num;
extern s32 *item_data_index_number;


#pragma peephole off
#pragma dont_inline on

/* Amount cursor: 1..999 in steps of 1 or 10. */
void nbItemCtrl(void)
{
    struct tagWINDOW_KEY *key = windowGetKeyInfo();

    if (key->mKeyRepeat & 0x8) {
        Num++;
        if (Num > 999) {
            Num = 999;
        }
    }
    if (key->mKeyRepeat & 0x400) {
        Num += 10;
        if (Num > 999) {
            Num = 999;
        }
    }
    if (key->mKeyRepeat & 0x4) {
        Num--;
        if (Num < 0) {
            Num = 0;
        }
    }
    if (key->mKeyRepeat & 0x200) {
        Num -= 10;
        if (Num < 0) {
            Num = 0;
        }
    }
}

void itemNameCtrl(void)
{
    struct tagWINDOW_KEY *key = windowGetKeyInfo();

    if (key->mKeyRepeat & 0x8) {
        if (++ItemID >= *item_data_index_number) {
            ItemID = *item_data_index_number - 1;
        }
    }
    if (key->mKeyRepeat & 0x400) {
        if ((ItemID += 10) >= *item_data_index_number) {
            ItemID = *item_data_index_number - 1;
        }
    }
    if (key->mKeyRepeat & 0x4) {
        ItemID--;
        if (ItemID < 0) {
            ItemID = 0;
        }
    }
    if (key->mKeyRepeat & 0x200) {
        ItemID -= 10;
        if (ItemID < 0) {
            ItemID = 0;
        }
    }
}

s32 menuDbgItemCreateCursor(struct tagWINDOW_WORK *window)
{
    struct tagWINDOW_KEY *key = windowGetKeyInfo();

    if (key->mKeyRepeat & 0x1) {
        if (--window->mCursor.y < 0) {
            window->mCursor.y = 0;
        }
    }
    if (key->mKeyRepeat & 0x2) {
        if (++window->mCursor.y > 1) {
            window->mCursor.y = 1;
        }
    }
    switch (window->mCursor.y) {
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

    if (initFlag == 0) {
        ItemID = 1;
        initFlag = 1;
    }
    Num = 1;
    for (;;) {
        if (menuOpen(MENU_ID_DEBUG_ITEM_CREATE, 1) == -1) {
            break;
        }
        if (!itemDataCheckValid((u16)ItemID)) {
            continue;
        }
        if (Num < 1 || Num > 999) {
            continue;
        }
        ret = menuOpen(MENU_ID_DEBUG_EDIT_CONFIRMATION, 1);
        menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
        if (ret != 0) {
            continue;
        }
        heroItemAddItemDataId(0, (u16)ItemID, (u16)Num, -1);
    }
    menuClose(MENU_ID_DEBUG_ITEM_CREATE);
    menuCloseSync(MENU_ID_DEBUG_ITEM_CREATE, 1);
}

#pragma dont_inline reset

#pragma peephole reset

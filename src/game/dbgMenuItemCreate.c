#include <types.h>
#include <winSprite.h>

/* Debug "create item" menu: picks an item id and an amount and hands them to
 * the hero's item list.  lbl_804EA6A8 is the once-only setup flag, lbl_804EA6B0
 * the item id cursor and lbl_804EA6AC the amount cursor; lbl_804E8A00 points at
 * the item name table count. */

extern s32 lbl_804EA6A8;
extern s32 lbl_804EA6B0;
extern s32 lbl_804EA6AC;
extern s32 *lbl_804E8A00;

s32 menuOpen(s32 id, s32 mode);
void menuClose(s32 id);
void menuCloseSync(s32 id, s32 mode);
void menuCloseCustom(s32 id, s32 a, s32 b);
u8 itemDataCheckValid(u16 itemID);
void heroItemAddItemDataId(s32 slot, u16 itemID, u16 count, s32 flag);

#pragma peephole off
#pragma dont_inline on

/* Amount cursor: 1..999 in steps of 1 or 10. */
void nbItemCtrl(void)
{
    struct tagWINDOW_KEY *key = windowGetKeyState();

    if (key->mKeyRepeat & 0x8) {
        lbl_804EA6AC++;
        if (lbl_804EA6AC > 999) {
            lbl_804EA6AC = 999;
        }
    }
    if (key->mKeyRepeat & 0x400) {
        lbl_804EA6AC += 10;
        if (lbl_804EA6AC > 999) {
            lbl_804EA6AC = 999;
        }
    }
    if (key->mKeyRepeat & 0x4) {
        lbl_804EA6AC--;
        if (lbl_804EA6AC < 0) {
            lbl_804EA6AC = 0;
        }
    }
    if (key->mKeyRepeat & 0x200) {
        lbl_804EA6AC -= 10;
        if (lbl_804EA6AC < 0) {
            lbl_804EA6AC = 0;
        }
    }
}

void itemNameCtrl(void)
{
    struct tagWINDOW_KEY *key = windowGetKeyState();

    if (key->mKeyRepeat & 0x8) {
        if (++lbl_804EA6B0 >= *lbl_804E8A00) {
            lbl_804EA6B0 = *lbl_804E8A00 - 1;
        }
    }
    if (key->mKeyRepeat & 0x400) {
        if ((lbl_804EA6B0 += 10) >= *lbl_804E8A00) {
            lbl_804EA6B0 = *lbl_804E8A00 - 1;
        }
    }
    if (key->mKeyRepeat & 0x4) {
        lbl_804EA6B0--;
        if (lbl_804EA6B0 < 0) {
            lbl_804EA6B0 = 0;
        }
    }
    if (key->mKeyRepeat & 0x200) {
        lbl_804EA6B0 -= 10;
        if (lbl_804EA6B0 < 0) {
            lbl_804EA6B0 = 0;
        }
    }
}

s32 menuDbgItemCreateCursor(struct tagWINDOW_WORK *window)
{
    struct tagWINDOW_KEY *key = windowGetKeyState();

    if (key->mKeyRepeat & 0x1) {
        if (--window->cursor < 0) {
            window->cursor = 0;
        }
    }
    if (key->mKeyRepeat & 0x2) {
        if (++window->cursor > 1) {
            window->cursor = 1;
        }
    }
    switch (window->cursor) {
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

    if (lbl_804EA6A8 == 0) {
        lbl_804EA6B0 = 1;
        lbl_804EA6A8 = 1;
    }
    lbl_804EA6AC = 1;
    for (;;) {
        if (menuOpen(0x24, 1) == -1) {
            break;
        }
        if (!itemDataCheckValid((u16)lbl_804EA6B0)) {
            continue;
        }
        if (lbl_804EA6AC < 1 || lbl_804EA6AC > 999) {
            continue;
        }
        ret = menuOpen(0x55, 1);
        menuCloseCustom(0x55, 0, 1);
        if (ret != 0) {
            continue;
        }
        heroItemAddItemDataId(0, (u16)lbl_804EA6B0, (u16)lbl_804EA6AC, -1);
    }
    menuClose(0x24);
    menuCloseSync(0x24, 1);
}

#pragma dont_inline reset

#pragma peephole reset

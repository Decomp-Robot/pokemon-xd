#include <menuSystem.h>
#include <sprite.h>
#include <GSmsg.h>
#include <GSvec.h>
#include <menuPocketBattleDisk.h>
#include <menuItemBios.h>
#include <itemDataBios.h>
#include <itemBios.h>
#include <menuPda.h>
#include <menuPdaSub.h>
#include <menuSpriteBios.h>
#include <menuPocketTool.h>
#include <hero.h>
#include <fightTrainer.h>

extern "C" void *memset(void *dst, int value, unsigned int size);
extern "C" void winSeqSetMenu(long menuID, s32 sequence);
extern "C" void menuSubCursorAnimeInit(void);
extern "C" void menuSubCursorAnimeMain(void);

u8 deckDataVirtualDB_GetFlagData(u16 index);
extern "C" void menuSubCursorAnimeStart(void);


void menuPocketButtonWait(void);
extern "C" void menuBattleDiskOpenDoc(u8 itemKind);
extern "C" void menuBattleDiskCloseDoc(void);

namespace {
// State of the battle-disk list menu.
struct MenuPocketBattleDiskWork {
    FIGHT_TRAINER *mTrainer; // trainer passed by the parent menu.
    Hero *mHero;             // hero whose item slots are displayed.
    s32 mItemCount;          // list item count.
    GSivec2 mCursor;         // x: first visible row; y: cursor row.
    f32 mScroll;             // scroll offset.
    u8 mHold;                // hold flag; when nonzero the scroll is ignored.
    u8 _15[3];
    f32 mRollProgress;       // roll animation progress.
    u8 mActive;              // whether list scrolling is active.
    u8 _1D[3];
    s32 mSelectedPosition;   // selected list position.
    u8 _24[4];
};

MenuPocketBattleDiskWork Data;
}

namespace {
void _controlCursor(void);
u16 selectItem(void);
s32 getNbItemSlot(void);
void printItemList(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void diskUsedMarkDisplay(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, int y);
u16 getItemIDFromMenuPos(int menuPosition);
long getItemNameMsg(unsigned long itemID);
}

extern "C" void menuSubCursorAnimeValGet(f32 *values, s32 *anime);
extern "C" void windowDrawSpriteCursor(s16 x, s16 y, u32 color, tagWINDOW_WORK *window, u16 itemID, u32 flags, s32 layer,
                            f32 *animeValues, u32 animeColor);


extern "C" s32 menuPocketBattleDiskDrawLineCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    tagMENU_ITEM_DATA *item;

    item = menuItemBiosGetPtr(window->mMenuID, work->mItemID);
    work->mY = (s16)(item->mOffsetA + getCursorPositionY());
    return 0;
}

extern "C" s32 menuPocketBattleDiskDrawBallCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    f32 vals[2] = { 0.0f, 0.0f };
    s32 anime;
    s32 layer;

    anime = -1;

    if (window->mMenuID == windowGetActiveID()) {
        menuSubCursorAnimeValGet(vals, &anime);
    }
    layer = winSpriteGetLayerID(window, work);
    windowDrawSpriteCursor(0, getCursorPositionY(), -1, window, 194, 0, layer, vals, anime);
    winSpriteSetDisp(work, 0);
    return 0;
}

// getCursorPositionY -- cursor Y for the battle-disk list.
// The row index advances in 30-dot steps; while the list is held the
// fractional scroll offset is ignored, otherwise it is added.
s32 getCursorPositionY(void)
{
    s32 y = (s16)(Data.mCursor.y * 30);

    if (Data.mHold == 0) {
        y += (s16)(s32)Data.mScroll;
    }
    return y;
}

extern "C" s32 menuPocketBattleDiskDrawRollDown(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    (void)window;
    MenuPocketBattleDiskWork *work = &Data;
    if (work->mActive == 0) {
        return 0;
    }
    if (work->mCursor.x + 8 < work->mItemCount + 1) {
        sprite->mRgba.mAlpha = (u8)(255.0f * (1.0f - work->mRollProgress));
    } else {
        sprite->mRgba.mAlpha = 0;
    }
    return 0;
}

extern "C" s32 menuPocketBattleDiskDrawRollUp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    (void)window;
    MenuPocketBattleDiskWork *work = &Data;
    if (work->mActive == 0) {
        return 0;
    }
    if (work->mCursor.x > 0) {
        sprite->mRgba.mAlpha = (u8)(255.0f * (1.0f - work->mRollProgress));
    } else {
        sprite->mRgba.mAlpha = 0;
    }
    return 0;
}

extern "C" s32 menuPocketBattleDiskPageBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    menuPdaSubScrollBar(window, sprite, window->mMenuID, 0x110, 0xF06422FF,
                        Data.mItemCount + 1, 8,
                        &Data.mCursor);
    winSpriteSetDisp(sprite, 0);
    return 0;
}

extern "C" s32 menuPocketBattleDiskCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case 0:
        if (window->mFlag == 0) {
            winSeqSetMenu(0x3B, 0x60);
            window->mFlag = 1;
            menuSubCursorAnimeInit();
        }
        break;
    case 2:
        _controlCursor();
        menuPocketToolFloatToZero(Data.mScroll, 7.0f);
        Data.mRollProgress += 0.033333335f;
        if (Data.mRollProgress > 1.0f) {
            Data.mRollProgress = 0.0f;
        }
        menuSubCursorAnimeMain();
        break;
    case 3:
        if (window->mFlag == 0) {
            winSeqSetMenu(0x3B, 0x64);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

extern "C" s32 menuPocketBattleDiskPrintItemKind(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s16 width = (s16)(GSmsgGetMessageSize(0x14D3) >> 16);
    s32 layer = winSpriteGetLayerID(window, sprite);
    GSmsgPrint2(layer, (sprite->mWidth - width) / 2, 0, 0xFFFFFFFF, 0x14D3);
    return 0;
}

extern "C" s32 menuPocketBattleDiskPrintItemList(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    tagMENU_ITEM_DATA *listArea = menuItemBiosGetPtr(window->mMenuID, 0x11B);
    tagMENU_ITEM_DATA *rowArea = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    s32 layer = winSpriteGetLayerID(window, sprite);
    spriteSetScissor(layer, listArea->mMessageX - rowArea->mMessageX,
                     listArea->mOffsetA - rowArea->mOffsetA,
                     listArea->mWidth, listArea->mHeight);
    spriteSetEnv(winSpriteGetLayerID(window, sprite));
    printItemList(window, sprite);
    spriteSetOffset(winSpriteGetLayerID(window, sprite), 0, 0);
    spriteClearScissor(winSpriteGetLayerID(window, sprite));
    spriteSetEnv(winSpriteGetLayerID(window, sprite));
    return 0;
}

extern "C" s32 menuPocketBattleDiskCursor(void)
{
    return 0;
}

extern "C" u16 menuPocketBattleDiskMain(s32 unused, FIGHT_TRAINER *trainer, Hero *hero)
{
    (void)unused;
    memset(&Data, 0, 0x24);
    Data.mTrainer = trainer;
    Data.mHero = hero;
    Data.mItemCount = getNbItemSlot();

    u16 itemID;
    for (;;) {
        Data.mActive = 1;
        itemID = selectItem();
        Data.mActive = 0;
        if (itemID == 0) {
            break;
        }
        menuBattleDiskOpenDoc((u8)(itemID - 0x215));
        menuPocketButtonWait();
        menuBattleDiskCloseDoc();
    }

    menuCloseCustom(0x3B, 0, 1);
    return itemID;
}

namespace {

void _controlCursor(void)
{
    u16 key = windowGetKeyRepeat();
    u32 sound = 0;
    s32 total = Data.mItemCount + 1;
    s32 pageCount = (total + 7) / 8;

    if (0.0f != Data.mScroll || Data.mActive == 0) {
        return;
    }

    GSivec2 oldCursor = Data.mCursor;
    if ((key & 8) != 0) {
        s16 lastTop = (s16)(total - 8);
        if (pageCount == 1) {
            Data.mCursor.y = (s16)(total - 1);
        } else if (Data.mCursor.x == lastTop) {
            Data.mCursor.y = 7;
        } else {
            Data.mCursor.x += 8;
            if (Data.mCursor.x > lastTop) {
                Data.mCursor.x = (s16)lastTop;
            }
        }
        if (oldCursor.x != Data.mCursor.x || oldCursor.y != Data.mCursor.y) {
            menuPtPlaySe(0x44D);
        }
    } else if ((key & 4) != 0) {
        if (Data.mCursor.x == 0) {
            Data.mCursor.y = 0;
        } else {
            Data.mCursor.x -= 8;
            if (Data.mCursor.x < 0) {
                Data.mCursor.x = 0;
            }
        }
        if (oldCursor.x != Data.mCursor.x || oldCursor.y != Data.mCursor.y) {
            menuPtPlaySe(0x44D);
        }
    } else if ((key & 2) != 0) {
        ++Data.mCursor.y;
        if (Data.mCursor.y + Data.mCursor.x >= total) {
            --Data.mCursor.y;
        } else {
            if (Data.mCursor.y >= 8) {
                ++Data.mCursor.x;
                --Data.mCursor.y;
                sound = 0x10;
                Data.mHold = 1;
            } else {
                sound = 0x10;
                Data.mHold = 0;
            }
            Data.mScroll = -30.0f;
        }
    }

    if ((key & 1) != 0 && (Data.mCursor.y > 0 || Data.mCursor.x > 0)) {
        --Data.mCursor.y;
        if (Data.mCursor.y < 0) {
            Data.mCursor.y = 0;
            --Data.mCursor.x;
            sound = 0x10;
            Data.mHold = 1;
        } else {
            sound = 0x10;
            Data.mHold = 0;
        }
        Data.mScroll = 30.0f;
    }

    if (oldCursor.x != Data.mCursor.x || oldCursor.y != Data.mCursor.y) {
        menuSubCursorAnimeStart();
    }
    if (sound != 0) {
        menuPtPlaySe(sound);
    }
}

}



namespace {
void printItemList(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MenuPocketBattleDiskWork *work = &Data;
    f32 scroll = work->mScroll;
    s32 itemCount = work->mItemCount;
    s32 firstItem = work->mCursor.x;
    s32 start = 0;
    s32 rows = 8;
    s32 scrollOffset = 0;
    if (0.0f != scroll && work->mHold != 0) {
        if (scroll < 0.0f) {
            --firstItem;
            start = -1;
        } else {
            rows = 9;
        }
        scrollOffset = (s32)scroll;
    }
    s32 row;
    for (row = start; row < rows && firstItem < itemCount; ++row, ++firstItem) {
        if (firstItem < 0) {
            continue;
        }
        s32 y = row * 30 - scrollOffset;
        u16 itemID = getItemIDFromMenuPos(firstItem);
        u32 descriptionID = (u32)getItemNameMsg(itemID);
        if (descriptionID == 0) {
            continue;
        }
        if (deckDataVirtualDB_GetFlagData((u16)(itemID - 0x215L)) != 0) {
            diskUsedMarkDisplay(window, sprite, y);
        }
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, y, 0xFFFFFFFF, descriptionID);
    }
    if (row < rows) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, row * 30 - scrollOffset, 0xFFFFFFFF, 0x3A9E);
    }
}

void diskUsedMarkDisplay(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, int y)
{
    tagMENU_ITEM_DATA *row = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    tagMENU_ITEM_DATA *icon = menuItemBiosGetPtr(window->mMenuID, 0x117);
    s32 iconX = icon->mMessageX - row->mMessageX;
    s32 iconY = icon->mOffsetA - row->mOffsetA;
    s16 *texture = (s16 *)menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(0xD9));
    s32 layer = winSpriteGetLayerID(window, sprite);
    windowDrawSprite2((s16)iconX, (s16)(iconY + y), texture[2], texture[3],
                       (u32)-1, window, (u16)0xD9, 0, (GSgfxLayerID)layer);
}
}


namespace {
u16 selectItem(void)
{
    if (menuOpenCustom(0x3B, windowGetActiveID(), 0, 0, 1, 0) == -1) {
        return 0;
    }
    Data.mSelectedPosition = Data.mCursor.x + Data.mCursor.y;
    return getItemIDFromMenuPos(Data.mSelectedPosition);
}

u16 getItemIDFromMenuPos(int menuPosition)
{
    u16 itemCount;
    ItemSlot *itemSlots = heroItemGetItemKindToItemAryPtr(
        Data.mHero, 7, &itemCount, 0, 0, 0);
    s32 validPosition = -1;
    for (s32 i = 0; i < itemCount; ++i, ++itemSlots) {
        if (itemCheckValid(itemSlots)) {
            ++validPosition;
            if (validPosition >= menuPosition) {
                return itemBiosGetItemDataId(itemSlots);
            }
        }
    }
    return 0;
}

s32 getNbItemSlot(void)
{
    u16 itemCount;
    ItemSlot *itemSlots = heroItemGetItemKindToItemAryPtr(
        Data.mHero, 7, &itemCount, 0, 0, 0);
    s32 validCount = 0;
    for (s32 i = 0; i < itemCount; ++i, ++itemSlots) {
        if (itemCheckValid(itemSlots)) {
            ++validCount;
        }
    }
    return validCount;
}

}
namespace {
long getItemNameMsg(unsigned long itemID)
{
    return itemDataBiosGetName(itemDataBiosGetPtr((u16)itemID));
}
}

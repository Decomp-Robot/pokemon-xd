#include <menu.h>
#include <menuPocketBattleDisk.h>
#include <winSprite.h>
#include <window.h>
#include <menuItemBios.h>

#pragma peephole off

struct CursorAnimationOffsets {
    f32 mValues[2];
};

extern "C" {
u16 windowGetKeyTrg(void);
u8 menuOffScreenSetDisp(s32 flag);
GSgfxLayerID winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 windowGetActiveID(void);
void menuSubCursorAnimeValGet(f32 *values, s32 *anime);
void windowDrawCursorSprite(s16 x, s16 y, s32 a, tagWINDOW_WORK *window, u16 item, u32 flags, s32 layer, f32 *vals, s32 anime);
}

// Cursor hooks registered by the item tables.
extern "C" s32 menuItemSetListCursorY(tagWINDOW_WORK *window, tagSPRITE_WORK *work);
extern "C" s32 menuItemDrawListCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *work);

// Item cursor hook: place the sprite work on the item's y plus the list
// cursor y.
s32 menuItemSetListCursorY(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    tagMENU_ITEM_DATA *item;

    item = menuItemBiosGetPtr(window->mMenuID, work->mItemID);
    work->mY = (s16)(item->mOffsetA + getCursorPositionY());
    return 0;
}

// Item cursor hook: draw the cursor sprite for the window's item, then hide
// the sprite work itself.
s32 menuItemDrawListCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    CursorAnimationOffsets offsets = {{0.0f, 0.0f}};
    s32 anime;
    s32 layer;

    anime = -1;

    if (window->mMenuID == windowGetActiveID()) {
        menuSubCursorAnimeValGet(offsets.mValues, &anime);
    }
    layer = winSpriteGetLayerID(window, work);
    windowDrawCursorSprite(0, getCursorPositionY(), -1, window, MENU_LIST_CURSOR_TEXTURE, 0, layer, offsets.mValues, anime);
    winSpriteSetDisp(work, 0);
    return 0;
}

#pragma peephole reset

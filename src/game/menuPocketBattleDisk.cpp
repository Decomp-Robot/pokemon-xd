#include <sprite.h>
#include <menuSub.h>
#include <menuPocketBattleDisk.h>
#include <menu.h>
#include <winSprite.h>
#include <menuItemBios.h>


extern "C" {
s32 menuPocketBattleDiskDrawLineCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *work);
s32 menuPocketBattleDiskDrawBallCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *work);
}

// Item cursor hook: place the sprite work on the item's y plus the list
// cursor y.
s32 menuPocketBattleDiskDrawLineCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    tagMENU_ITEM_DATA *item;

    item = menuItemBiosGetPtr(window->mMenuID, work->mItemID);
    work->mY = (s16)(item->mOffsetA + getCursorPositionY());
    return 0;
}

// Item cursor hook: draw the cursor sprite for the window's item, then hide
// the sprite work itself.
s32 menuPocketBattleDiskDrawBallCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    f32 vals[2] = {0.0f, 0.0f};
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
    s32 y = (s16)(menuPocketBattleDiskWork.mRow * 30);

    if (menuPocketBattleDiskWork.mHold == 0) {
        y += (s16)(s32)menuPocketBattleDiskWork.mScroll;
    }
    return y;
}

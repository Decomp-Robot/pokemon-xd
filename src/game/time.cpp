#include <time.h>
#include <pause.h>
#include <menuPocketBattleDisk.h>
#include <winSprite.h>
#include <menuItemBios.h>

#pragma peephole off

// Half a timer tick, added before the render time is truncated to a count.
extern const f32 half_804EBE10;
// Default cursor anime pair, owned by the menu code that fills them in.
extern u32 lbl_804EFC88;
extern u32 lbl_804EFC8C;

extern "C" {
f32 GSgfxVideoGetLastRenderTime(void);
u8 GSgfxVideoGetVsyncRate(void);
s32 windowGetActiveID(void);
void menuSubCursorAnimeValGet(f32 *values, s32 *anime);
s32 winSpriteGetLayerID(tagWINDOW_WORK *window, tagSPRITE_WORK *work);
void winSpriteSetDisp(tagSPRITE_WORK *work, s32 disp);
void zz_0114ea8_(s16 x, s16 y, s32 a, tagWINDOW_WORK *window, u16 item, u32 flags, s32 layer, f32 *vals, s32 anime);
}

// Cursor hooks registered by the item tables.
extern "C" s32 zz_000e97c_(tagWINDOW_WORK *window, tagSPRITE_WORK *work);
extern "C" s32 zz_000e9d4_(tagWINDOW_WORK *window, tagSPRITE_WORK *work);

// Last frame render time in timer units; frozen at 0 while paused.
u32 timeGetLastFrameTime(void)
{
    u32 paused = zz_000e794_();

    if (paused != 0) {
        return 0;
    }
    return (u32)(GSgfxVideoGetLastRenderTime() + half_804EBE10);
}

// Last frame render time in seconds; frozen at 0.0f while paused.
f32 timeGetLastFrameSec(void)
{
    u32 paused = zz_000e794_();

    if (paused != 0) {
        return 0.0f;
    }
    return GSgfxVideoGetLastRenderTime() / (f32)GSgfxVideoGetVsyncRate();
}

// Item cursor hook: place the sprite work on the item's y plus the list
// cursor y.
s32 zz_000e97c_(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    tagMENU_ITEM_DATA *item;

    item = menuItemBiosGetPtr(window->mMenuID, work->mItemID);
    work->mCursorY = (s16)(item->mY + getCursorPositionY());
    return 0;
}

// Item cursor hook: draw the cursor sprite for the window's item, then hide
// the sprite work itself.
s32 zz_000e9d4_(tagWINDOW_WORK *window, tagSPRITE_WORK *work)
{
    u32 vals[2];
    s32 anime;
    s32 layer;

    vals[0] = lbl_804EFC88;
    vals[1] = lbl_804EFC8C;
    anime = -1;

    if (window->mMenuID == windowGetActiveID()) {
        menuSubCursorAnimeValGet((f32 *)vals, &anime);
    }
    layer = winSpriteGetLayerID(window, work);
    zz_0114ea8_(0, getCursorPositionY(), -1, window, 194, 0, layer, (f32 *)vals, anime);
    winSpriteSetDisp(work, 0);
    return 0;
}

#pragma peephole reset

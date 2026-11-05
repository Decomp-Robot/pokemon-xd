#include <types.h>
#include <menu.h>
#include <gamedata.h>
#include <GSinput.h>
#include <GSgfx.h>
#include <GSflag.h>
#include <winSprite.h>
#include <winSeq.h>
#include <floor.h>
#include <floorBios.h>

enum {
    FLOOR_KIND_FIELD = 1
};

#include <floorManager.h>
#include <place.h>
#include <time.h>
#include <GSsndPlayback.h>
#include <math.h>

enum {
    MENU_ID_UNDER_TAKER = 193,
    MENU_INT_SEQ_OPEN = 369,
    MENU_INT_SEQ_CLOSE = 375,
};

/* Interrupt kind announced through menu 151: mail received, Esa helped
 * survive, and the player was revived. */
enum MenuInterruptKind {
    MENU_INTERRUPT_MAIL = 0,
    MENU_INTERRUPT_ESA = 1,
    MENU_INTERRUPT_RELIVE = 2,
};

f32 _menuIntFrameCount;
f32 _menuIntFrameCount2;

extern "C" {
u8 menuIntUnderTakerMenuOpenFlag = 1;
u8 menuIntUnderTakerIconDispFlag = 1;
}

u8 _menuIntGetAlpha(void);
u8 _menuIntGetAlpha2(void);
void _menuIntVibSet(void);

extern "C" {

void menuIntUnderTakerOpenIcon(void)
{
    if (menuIsCheck(MENU_ID_UNDER_TAKER) != 0) {
        menuClose(MENU_ID_UNDER_TAKER);
    } else {
        menuOpenCustom(MENU_ID_UNDER_TAKER, 0, NULL, 0, true, 0);
    }
}

void menuIntUnderTakerMenuClose(void)
{
    if (menuIsCheck(MENU_ID_UNDER_TAKER) != 0) {
        menuClose(MENU_ID_UNDER_TAKER);
        menuCloseSync(MENU_ID_UNDER_TAKER, true);
    }
}

void menuIntUnderTakerMenuOpen(void)
{
    menuIntUnderTakerMenuOpenFlag = 1;
    menuOpenCustom(MENU_ID_UNDER_TAKER, 0, NULL, 0, true, 0);
}

void menuIntUnderTakerDispIconRestore2(void)
{
    menuIntUnderTakerIconDispFlag = 1;
}

void menuIntUnderTakerDispIconOff2(void)
{
    menuIntUnderTakerIconDispFlag = 0;
}

void menuIntUnderTakerHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    GSgfxLayerFindByID(winSpriteGetLayerID(window, sprite));
    u8 floorKind = floorDataBiosGetFloorKind(floorDataBiosGetPtr(GSfloorGetCurrent()));
    if (menuIntUnderTakerIconDispFlag != 0 && GSflagTest(((struct { u8 _00[0x24]; u32 mIconFlag; } *)gPlaceWork)->mIconFlag) != 0 && floorKind == FLOOR_KIND_FIELD) {
        winSpriteSetDisp(sprite, true);
        switch (sprite->mItemID) {
        case 1628:
            if (window->mState != WINDOW_STATE_INIT) {
                sprite->mRgba.mAlpha = _menuIntGetAlpha2();
            }
            break;
        }
    } else {
        winSpriteSetDisp(sprite, false);
    }
}

} // extern "C"

u8 _menuIntGetAlpha2(void)
{
    f32 c = (f32)cos(6.2831855f * _menuIntFrameCount2 / 1.5f);
    u16 alpha = ((1.0f + c) / 2.0f) * 300.0f;
    if (alpha > 255) {
        alpha = 255;
    }
    if (alpha < 50) {
        alpha = 50;
    }
    _menuIntFrameCount2 += timeGetLastFrameSec();
    return alpha;
}

extern "C" {

s32 menuIntUnderTakerMainCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        GSsndPlayNow(1460);
        if (menuIntUnderTakerMenuOpenFlag != 0) {
            _menuIntVibSet();
        }
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_INT_SEQ_OPEN);
            window->mFlag = 1;
        }
        menuIntUnderTakerMenuOpenFlag = 1;
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_INT_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

void menuIntHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 *kind = (s32 *)window->mArgs[0];
    switch (sprite->mItemID) {
    case 1214:
        if (*kind != MENU_INTERRUPT_MAIL) {
            winSpriteSetDisp(sprite, false);
        } else {
            winSpriteSetDisp(sprite, true);
            if (window->mState != WINDOW_STATE_INIT) {
                sprite->mRgba.mAlpha = _menuIntGetAlpha();
            }
        }
        break;
    case 1285:
        if (*kind != MENU_INTERRUPT_ESA) {
            winSpriteSetDisp(sprite, false);
        } else {
            winSpriteSetDisp(sprite, true);
            if (window->mState != WINDOW_STATE_INIT) {
                sprite->mRgba.mAlpha = _menuIntGetAlpha();
            }
        }
        break;
    case 1286:
        if (*kind != MENU_INTERRUPT_RELIVE) {
            winSpriteSetDisp(sprite, false);
        } else {
            winSpriteSetDisp(sprite, true);
            if (window->mState != WINDOW_STATE_INIT) {
                sprite->mRgba.mAlpha = _menuIntGetAlpha();
            }
        }
        break;
    }
}

} // extern "C"

u8 _menuIntGetAlpha(void)
{
    f32 c = (f32)cos(6.2831855f * _menuIntFrameCount / 1.5f);
    u16 alpha = ((1.0f + c) / 2.0f) * 300.0f;
    if (alpha > 255) {
        alpha = 255;
    }
    if (alpha < 50) {
        alpha = 50;
    }
    _menuIntFrameCount += timeGetLastFrameSec();
    return alpha;
}

extern "C" {

void menuIntButton(tagWINDOW_WORK *window)
{
    if (winSeqCheckMove(window->mMenuID) == 0) {
        menuButtonNormal(window);
    }
}

s32 menuIntMainCtrl(tagWINDOW_WORK *window)
{
    s32 *kind = (s32 *)window->mArgs[0];
    switch (window->mState) {
    case WINDOW_STATE_INIT: {
        _menuIntVibSet();
        u32 soundID = 0;
        switch (*kind) {
        case MENU_INTERRUPT_MAIL:
            soundID = 1382;
            break;
        case MENU_INTERRUPT_ESA:
            soundID = 1380;
            break;
        case MENU_INTERRUPT_RELIVE:
            soundID = 1381;
            break;
        }
        if (soundID != 0) {
            GSsndPlayNow(soundID);
        }
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_INT_SEQ_OPEN);
            window->mFlag = 1;
        }
        break;
    }
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_INT_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

} // extern "C"

void _menuIntVibSet(void)
{
    if ((s32)gamedatasaveGetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION) == 0) {
        GSinputMotorStart(GS_INPUT_PORT_1, 0, 255, 30, 0);
    }
}

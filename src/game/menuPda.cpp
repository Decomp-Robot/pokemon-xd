#include <menuDataBios.h>
#include <menuOffScreen.h>
#include <flashsystem.h>
#include <GSflag.h>
#include <GSmem.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <fade.h>
#include <hero.h>
#include <savedata.h>
#include <math.h>
#include <camera.h>
#include <menu.h>
#include <GSsnd.h>
#include <floor.h>
#include <GSmodel.h>
#include <GSres.h>
#include <floorManager.h>
#include "GSgfx.h"
#include "GSmodel.h"
#include "GSres.h"
#include "menuItemBios.h"
#include "menuPda.h"
#include "menuOffScreen.h"
#include "menuSub.h"
#include "time.h"
#include "winSprite.h"

enum { PDA_OPEN_SOUND = 0x44E };

static s16 _cursor_index[5];

// PDA front end. The PDA script (menuPdaScript) and the PDA screen (menuPda)
// share the fade in _pdaScriptAddFrame and the PDA frame menu; the helpers behind
// them drive the model, the top menu and the list cursors.

extern "C" {
double acos(double value);
void *windowGetCursorToItem(tagWINDOW_WORK *window);
void GSthreadSwitch(void);
void scriptLightSetDisp(unsigned long res, s32 on);

void GSfloorPop(void);
s32 pokecoloGetRomVersion(void);
s32 menuPdaMemoXDGetItem(void);
void menuPdaDPMonList(void);
void menuPdaMemoList(void);
void menuPdaMailListXD(void);
void menuPdaSearcher(void);
void menuButtonNormal(tagWINDOW_WORK *window);

int sprintf(char *buf, const char *format, ...);
}

// PDA model and cursor helpers.
void _menuPdaInit(void);
void _menuPdaStartEvent(void);
void _menuPdaTopClose(void);
void _menuPdaZoomIn(void);
void _menuPdaZoomOut(void);
void _menuPdaExit(void);
void _menuPdaSetMenuItem(void);
void _menPdaFadeOutSet(void);
s32 _menPdaFadeOutWait(void);
void _menPdaFadeInWait(void);
void _menuPdaSetMotion(unsigned long res, s16 motion, f32 frame, bool loop);
void _menuPdaWaitMotion(unsigned long res);
void _menuPdaStopMotion(unsigned long res);
void _menuPdaModelVisibility(unsigned long res, bool visible);
void _menuPdaAddFunc(void);
bool _menuPdaWaitMotion2(unsigned long res);
s32 _menuPdaModelGetIndex(unsigned long res);
void _menuPdaFree(void *buffer);
s32 _SelectCursorSub(s8 select, GSvec2 *vertices);
s32 _SelectCursorCalcPositon(GSvec2 *vertices);
s8 _menuPdaGetCurrentID(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _menuPdaModelCursorAnime(void);
s32 _SelectCursorPositionCheck(void);
void _menuPdaDrawFace(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _SelectCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s8 select);
u32 _menuPdaGetMoney(void);
u32 _menuPdaGetPokecoupon(void);
extern "C" {
u32 pokecoloGetPlayTimeValue(void);
}

// Entry points of this object.
extern "C" {
void menuPdaScript(u8 mode);
s32 menuPdaCursorCtrl(tagWINDOW_WORK *window);
s32 menuPdaDrawTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPdaButton(tagWINDOW_WORK *window);
void menuPdaInit(void);
void menuPdaMain(void);
void menuPda(void);
s32 menuPdaBackDrawBackXD(void);
}

PDATOP_MENU_RES topMenu(PDATOP_MENU_RES res);
s32 _getMenuIndex(PDATOP_MENU_RES res);

static inline void _menuPdaSetTitleMessage(tagSPRITE_WORK *sprite)
{
    u32 messages[5] = {15187, 15188, 15189, 15363, 15190};
    sprite->mMsgID = messages[menuPdaGetRes()];
}

static inline void _menuPdaSetChapterMessage(tagSPRITE_WORK *sprite)
{
    u32 chapter = GSflagGet(0x6C2);
    u32 messages[7] = {15218, 15219, 15220, 15221, 15222, 15223, 15406};
    if (chapter > 0 && chapter <= 8) {
        sprite->mMsgID = messages[chapter - 1];
    } else {
        sprite->mMsgID = 0;
    }
}

// Run the PDA from a floor script: play the open flash animation of the mode,
// open the PDA frame and input menus, wait for the animation and fade the
// PDA away again.
void menuPdaScript(u8 mode)
{
    s32 value = 0;
    PDA_FRAME_ARG arg;
    unsigned long wait;
    f32 dt;
    f32 fade;

    menuPdaMode = mode;
    if (mode == 0) {
        _menuPdaStatus.mRes = PDATOP_MENU_RES_MEMO;
        _menuPdaStatus.mPreviousRes = 1;
        FlashSystemSetPlayFrameLabel(0x1FBD3000, "1_start", "1_end");
        FlashSystemSetNowFrameLabel(0x1FBD3000, "1_start");
        wait = FlashSystemGetFrameFromLabel(0x1FBD3000, "1_end");
    } else {
        _menuPdaStatus.mRes = PDATOP_MENU_RES_DP_MON;
        _menuPdaStatus.mPreviousRes = 0;
        FlashSystemSetPlayFrameLabel(0x1FBD3000, "2_start", "2_end");
        FlashSystemSetNowFrameLabel(0x1FBD3000, "2_start");
        wait = FlashSystemGetFrameFromLabel(0x1FBD3000, "2_end");
    }
    _SelectCursorCalcAngle(_menuPdaStatus.mRes);
    _menuPdaStatus.mCursorY = _menuPdaStatus.mCursorTargetY;
    FlashSystemSetLoopFlag(0x1FBD3000, 0);
    _menuPdaStatus.mSubmenuFlag = 0;
    _menuPdaSetMenuItemScript(mode);
    menuOpen(0xA8, 0);
    menuOpenCustom(0x6C, windowGetActiveID(), 0, 0, 0, 1, &arg);
    menuOpenCustom(0x107, windowGetActiveID(), &value, 0, 0, 0);
    while (FlashSystemGetNowFrame(0x1FBD3000) < wait) {
        GSthreadSwitch();
    }
    _menuPdaScriptCalcAlphaInit();
    GSthreadSwitch();
    if (mode == 0) {
        menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 2, 1);
    } else {
        menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 5, 1);
    }
    fade = 2.0f;
    do {
        dt = timeGetLastFrameSec();
        _menuPdaScriptCalcAlpha(dt);
        fade = fade - dt;
        GSthreadSwitch();
    } while (fade >= 0.0f);
    menuClose(0x107);
    menuCloseSync(0x107, 1);
    menuClose(0xA8);
    menuClose(0x6C);
    menuCloseSync(0x6C, 1);
    menuOffScreenSetPriority(1);
    GSfloorPop();
}

// Alpha of the PDA script fade as a 0-255 fade step.
u8 _menuPdaScriptGetAlpha(void)
{
    f32 frame = _pdaScriptAddFrame;
    f32 full;
    f32 limit;
    f32 fade;

    limit = _pdaScriptTime;
    if (frame >= limit) {
        return 0xFF;
    }
    full = 255.0f;
    fade = full * frame / limit;
    if (fade > full) {
        fade = full;
    }
    return fade;
}

// Advance the PDA script fade by dt.
void _menuPdaScriptCalcAlpha(f32 dt)
{
    _pdaScriptAddFrame = _pdaScriptAddFrame + dt;
}

// Reset the PDA script fade.
void _menuPdaScriptCalcAlphaInit(void)
{
    _pdaScriptAddFrame = 0.0f;
}

// Selectable state of the PDA frame items for the script mode.
void _menuPdaSetMenuItemScript(u8 mode)
{
    if (mode == 0) {
        menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 2, 0);
        menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 5, 0);
    } else {
        menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 2, 1);
        menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 5, 0);
    }
    menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 3, 1);
    menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 4, 1);
    menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 6, 1);
}

// Per-frame hook of the PDA wait loops (motion and zoom waits): nothing to do
// in the release build.
void menuPdaDrawDirectModel(void)
{
}

// Point the PDA menus at the PDA model resource.
void menuPdaReleaseModelPointer(void)
{
    _pModel = GSresGetResourceAllGroup(0x17CF1000);
}

// Script-side twin of menuPdaReleaseModelPointer: the PDA script and the mail
// flow each register their own entry that points the menus at the PDA model.
void menuPdaSetModelPointer(void)
{
    _pModel = GSresGetResourceAllGroup(0x17CF1000);
}

// List cursor of the PDA frame menu. Moves the window cursor through the
// shown rows of the item chain; the step grows while the key repeats until
// the cursor distance covers the screen.
s32 menuPdaCursorCtrl(tagWINDOW_WORK *window)
{
    u16 keys;
    tagMENU_ITEM_DATA *item;
    MenuData *menuData;
    MenuPdaWork *work;
    s32 up;
    s32 down;
    s16 count;
    s32 stepX;
    s32 stepY;

    if (window == 0) {
        return 0;
    }
    if (_checkAddFunc(window) != false) {
        return 0;
    }
    menuData = menuDataBiosGetPtr(window->mMenuID);
    if (windowGetCursorToItem(window) == 0) {
        window->mCursor.y = 0;
        return 0;
    }
    stepX = 0x10;
    stepY = 0xC;
    keys = windowGetKeyRepeat();
    up = keys & 1;
    work = &_menuPdaStatus;
    down = keys & 2;
    for (;;) {
        count = 0;
        item = menuItemBiosGetPtr(window->mMenuID, menuData->mFirstItemID);
        if (up != 0) {
            for (;;) {
                if (item->mShownFlag) {
                    count++;
                }
                if (item->mChainEndFlag) {
                    break;
                }
                item = menuItemBiosGetPtr(window->mMenuID, item->mNextID);
            }
            if (window->mCursor.y > 0) {
                window->mCursor.y--;
                work->mCursorMoved = 0;
            } else {
                window->mCursor.y = count - 1;
                work->mCursorMoved = 1;
            }
        } else if (down != 0) {
            for (;;) {
                if (item->mShownFlag) {
                    count++;
                }
                if (item->mChainEndFlag) {
                    break;
                }
                item = menuItemBiosGetPtr(window->mMenuID, item->mNextID);
            }
            if (window->mCursor.y < count - 1) {
                window->mCursor.y++;
                work->mCursorMoved = 0;
            } else {
                window->mCursor.y = 0;
                work->mCursorMoved = 1;
            }
        }
        if (window->cursorTo != window->mCursor.y) {
            break;
        }
        stepX = stepX + 0x10;
        stepY = stepY + 0xC;
        if (stepX >= 0x280) {
            stepX = 0x280;
        }
        if (stepY >= 0x1E0) {
            stepY = 0x1E0;
        }
        if (stepX == 0x280 && stepY == 0x1E0) {
            break;
        }
    }
    return 0;
}

// Button sound of the PDA frame menu; silent while a fight result shows.
void menuPdaButton(tagWINDOW_WORK *window)
{
    if (floorGetCurrent() != 0x38C) {
        menuButtonNormal(window);
    }
}

// Show/hide the title sprite of a PDA list window for its leader and submenu
// that the PDA script selected.
s32 menuPdaDrawTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 selected = menuPdaGetRes();
    s32 subres = menuPdaGetSubres();
    s16 item = sprite->mItemID;

    switch (item) {
    case 0x1CA:
    case 0x1CB:
    case 0x1CC:
        if (selected == 0 && subres == 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x16:
        if (selected == 0) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x17:
        if (selected == 2) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x18:
        if (selected == 1 && subres == 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1A:
        if (selected == 1 && subres > 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1B:
        if (selected == 1 && subres > 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1C:
        if (selected == 1 && subres > 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1D:
        if (selected == 1 && subres > 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1E:
        if (selected == 1 && subres > 1 && menuPdaMemoXDGetItem() == 0) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1F:
        if (selected == 1 && subres > 1 && menuPdaMemoXDGetItem() == 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x20:
        if (selected == 1 && subres > 1 && menuPdaMemoXDGetItem() == 2) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x21:
        if (selected == 1 && subres > 1 && menuPdaMemoXDGetItem() == 3) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x19:
        if (selected == 3) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    }
    return 0;
}

// Empty callback of a PDA item record: the record only needs its sprite set.
s32 menuPdaBackDrawBackXD(void)
{
    return 0;
}

// The PDA screen: open the PDA frame, run the top menu loop with the zoomed
// submenus and close everything again.
void menuPda(void)
{
    PDATOP_MENU_RES res = PDATOP_MENU_RES_DP_MON;
    _GSmodel *model;
    PDA_FRAME_ARG arg;

    scriptLightSetDisp(0x17CF1600, 0);
    scriptLightSetDisp(0x17CF1601, 0);
    if (pokecoloGetRomVersion() == 1) {
        GSflagOn(0x3F2);
    }
    FlashSystemSetLoopFlag(0x1FBD3000, 1);
    FlashSystemSetLoopFlag(0x20543000, 0);
    menuOpen(0xA8, 0);
    model = (_GSmodel*)GSresGetResourceAllGroup(0x17CF1000);
    _pModel = model;
    GSmodelSetVisibility(model, 0);
    GSthreadSwitch();
    _menuPdaStatus.mExitFlag = 0;
    _menuPdaInit();
    _menuPdaStartEvent();
    _menuPdaSetMenuItem();
    _SelectCursorCalcAngle(_cursor_index[0]);
    _menuPdaStatus.mCursorY = _menuPdaStatus.mCursorTargetY;
    arg.mTitleMessageID = &TitleMsgID;
    arg.mUnk00 = 0;
    arg.mUnk01 = 0xFF;
    arg.mUnk02 = 0;
    arg.mPreviousRes = &LineAnimTime;
    menuOpenCustom(0x6C, windowGetActiveID(), 0, 0, 0, 1, &arg);
    while (_menuPdaStatus.mExitFlag == 0) {
        _menuPdaSetMenuItem();
        res = topMenu(res);
        _menuPdaStatus.mSubmenuFlag = 1;
        _menuPdaStatus.mRes = res;
        switch (res) {
        case PDATOP_MENU_RES_DP_MON:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            menuPdaDPMonList();
            _menuPdaStatus.mSubmenuFlag = 0;
            _menuPdaZoomOut();
            break;
        case PDATOP_MENU_RES_MEMO:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            menuPdaMemoList();
            _menuPdaStatus.mSubmenuFlag = 0;
            _menuPdaZoomOut();
            break;
        case PDATOP_MENU_RES_MAIL:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            menuPdaMailListXD();
            _menuPdaStatus.mSubmenuFlag = 0;
            _menuPdaZoomOut();
            break;
        case PDATOP_MENU_RES_SEARCHER:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            menuPdaSearcher();
            _menuPdaStatus.mSubmenuFlag = 0;
            _menuPdaZoomOut();
            break;
        default:
            _menPdaFadeOutSet();
            _menuPdaExit();
            _menuPdaStatus.mExitFlag = 1;
            break;
        }
    }
    _menPdaFadeOutWait();
    if (_menuPdaStatus.mFadeOutDone != 1) {
        menuOffScreenSetDisp(1);
        _menuPdaStatus.mFadeOutDone = 1;
    }
    _menuPdaTopClose();
    menuClose(0x6C);
    menuCloseSync(0x6C, 1);
    menuClose(0xA8);
    menuCloseSync(0xA8, 1);
    _menuPdaModelVisibility(0x17CF1000, false);
    _menPdaFadeInWait();
    menuOffScreenSetPriority(1);
    GSfloorPop();
    floorSetFadeScript(0, 0);
}

// Select the page flash labels for a PDA top menu entry: entry 0 gets the
// "1_" labels and so on, and the leave entry (4) has no page, so the flash is
// left alone. Called with the entry of the row under the list cursor.
void _menuPdaSelectAnimeSet(long res)
{
    char label[0x40];
    char text[0x40];

    if (res >= 4) {
        menuPdaPageDisp = 0;
        return;
    }
    menuPdaPageDisp = 1;
    sprintf(label, "%d_start", res + 1);
    sprintf(text, "%d_end", res + 1);
    FlashSystemSetPlayFrameLabel(0x20543000, label, text);
    FlashSystemSetNowFrameLabel(0x20543000, label);
}

void menuPdaReset(void)
{
    _menuPdaStatus.ResetTransform();
}

// Idle thread of the PDA screen.
__declspec(export) void menuPdaMain(void)
{
    for (;;) {
        GSthreadSwitch();
    }
}

// Load the PDA model and hide it; the PDA jingle plays once unlocked.
__declspec(export) void menuPdaInit(void)
{
    _GSmodel *model = (_GSmodel*)GSresGetResourceAllGroup(0x17CF1000);

    _pModel = model;
    if (model != 0) {
        GSmodelSetVisibility(model, 0);
        _menuPdaSetMotion(0x17CF1000, 0, 0.0f, false);
    }
    if (menuPdaSoundEnable != 0) {
        GSsndPlayNow(PDA_OPEN_SOUND);
    }
}

// Intro sequence of the PDA screen: render the PDA model on the event layer,
// play its intro motion and start the floor talk bgm.
void _menuPdaStartEvent(void)
{
    GSgfxLayerID layer = menuPdaEventLayer[0];
    _GSmodel *model = (_GSmodel*)GSresGetResourceAllGroup(0x17CF1000);

    GSmodelSetRenderLayer(model, 1, &layer);
    cameraPlayAnime(0x17, 0x17CF1800, 0.0f, 0);
    GSmodelSetRenderCamera(model, (_GScamera*)GSresGetResource(0x17, 0x17CF1800));
    GSmodelSetVisibility(model, 1);
    GSmodelSetBoundCheck(model, 0);
    _menuPdaSetMotion(0x17CF1000, 0, 0.0f, false);
    menuOffScreenSetPriority(0);
    GSthreadSwitch();
    _menuPdaWaitMotion(0x17CF1000);
    _menuPdaStopMotion(0x17CF1000);
    _menuPdaSetMotion(0x17CF1000, 1, 0.0f, true);
}

// Close the PDA top menu.
void _menuPdaTopClose(void)
{
    menuClose(MENU_ID_PDA_TOP);
    menuCloseSync(MENU_ID_PDA_TOP, 1);
}

// Open the PDA top menu with the cursor on res and return the entry the
// player chose; -1 when the menu could not open.
PDATOP_MENU_RES topMenu(PDATOP_MENU_RES res)
{
    s32 index = _getMenuIndex(res);

    if (menuOpenCustom(MENU_ID_PDA_TOP, windowGetActiveID(), &index, 0, 1, 0) == -1) {
        return (PDATOP_MENU_RES)-1;
    }
    return _menuPdaStatus.mRes;
}

// Row index of res in the shown top menu rows; 0 when not listed.
s32 _getMenuIndex(PDATOP_MENU_RES res)
{
    for (s16 i = 0; i < _cursor_index_max; i++) {
        if (res == _cursor_index[i]) {
            return i;
        }
    }
    return 0;
}

extern "C" s32 menuPdaBackDrawTitleText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool added = _checkAddFunc(window);
    switch (sprite->mItemID) {
    case 7: {
        u32 playTime = pokecoloGetPlayTimeValue();
        msgctrlSetValue(0x4C, playTime);
        break;
    }
    case 9:
        msgctrlSetValue(0x69, _menuPdaGetPokecoupon());
        break;
    case 11:
        msgctrlSetValue(0x69, _menuPdaGetMoney());
        break;
    case 14:
    case 15:
        break;
    case 13: {
        u32 trainerID;
        s32 digit;
        s32 x;
        trainerID = heroGetStatus(0, HERO_STATUS_TRAINER_RND, 0) & 0xFFFF;
        x = sprite->mWidth;
        x -= 18;
        for (digit = 0; digit < 5; digit++) {
            u32 digitValue = trainerID % 10;
            trainerID /= 10;
            msgctrlSetValue(0x34, digitValue);
            GSmsgPrint2(window->mLayerID, x, 0, window->mRgba.mColorA | 0xFFFFFF00, 0x426F);
            x -= GSmsgGetMessageSize(0x426F) >> 16;
        }
        break;
    }
    case 2:
        if (added && menuPdaMode == 0) {
            sprite->mRgba.mAlpha = _menuPdaScriptGetAlpha();
        }
        _SelectCursor(window, sprite, 0);
        break;
    case 3:
        _SelectCursor(window, sprite, 1);
        break;
    case 4:
        _SelectCursor(window, sprite, 2);
        break;
    case 5:
        if (added && menuPdaMode == 1) {
            sprite->mRgba.mAlpha = _menuPdaScriptGetAlpha();
        }
        _SelectCursor(window, sprite, 3);
        break;
    case 6:
        _SelectCursor(window, sprite, 4);
        break;
    case 1:
        _menuPdaDrawFace(window, sprite);
        _menuPdaGetCurrentID(window, sprite);
        _menuPdaModelCursorAnime();
        _SelectCursorPositionCheck();
        if (_menuPdaStatus.mRes >= 0) {
            _menuPdaStatus.mPreviousRes = _menuPdaStatus.mRes;
        }
        break;
    case 0x173: {
        _menuPdaSetTitleMessage(sprite);
        break;
    }
    case 0x1C6: {
        _menuPdaSetChapterMessage(sprite);
        break;
    }
    case 0x1C8:
        winSpriteSetDisp(sprite, added);
        break;
    case 0x1C9:
        winSpriteSetDisp(sprite, menuPdaPageDisp);
        break;
    }
    return 0;
}

bool _checkAddFunc(tagWINDOW_WORK *window)
{
    return window->mMenuID == 0x107;
}

void _SelectCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s8 select)
{
    s32 i;
    s32 alpha;
    s32 count;
    tagMENU_ITEM_DATA *item;
    GSvec2 vertices[72];
    _checkAddFunc(window);
    f32 halfHeight = _next_height / 2.0f;
    if (menuItemBiosGetSelectFlag(0x6B, sprite->mItemID)) {
        winSpriteSetDisp(sprite, 1);
    } else {
        winSpriteSetDisp(sprite, 0);
    }
    if (_menuPdaStatus.mRes != select) {
        return;
    }
    {
        GSgfxLayer *layer = GSgfxLayerFindByID((GSgfxLayerID)window->mLayerID);
        if (layer != 0) {
            f32 distance = (f32)__fabs(_menuPdaStatus.mCursorTargetY - _menuPdaStatus.mCursorY);
            if (distance > _next_height) {
                distance = _next_height;
            }
            alpha = 255.0f * ((f32)__fabs(distance - halfHeight) / halfHeight);
            count = _SelectCursorSub(select, vertices);
            item = menuItemBiosGetPtr(0x6B, sprite->mItemID);
            for (i = 0; i < count; i++) {
                vertices[i].x -= item->mMessageX;
                vertices[i].y -= item->mOffsetA;
            }
            layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_ONE, true);
            layer->beginSurface(GSGFX_PRIM_TRIANGLE_FAN, GSgfxVFGetPredef(3), count);
            for (i = 0; i < count; i++) {
                GXWGFifo.s16 = (s16)vertices[i].x;
                GXWGFifo.s16 = (s16)vertices[i].y;
                GXWGFifo.u32 = 0x405A2600 | (u8)alpha;
            }
            layer->endSurface();
            layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
        }
    }
}

void _menuPdaDrawFace(tagWINDOW_WORK *window, tagSPRITE_WORK *)
{
    u16 portrait = 0;
    switch (((Hero *)savedataGetStatus(0, 2))->getHeroStyle()) {
    case 0: portrait = 0x123; break;
    case 2: portrait = 0x3A1; break;
    case 1: portrait = 0x3A2; break;
    }
    windowDrawSprite(0, 0, window, portrait, 0, window->mLayerID);
}

s32 _SelectCursorSub(s8 select, GSvec2 *vertices)
{
    _SelectCursorCalcAngle(select);
    return _SelectCursorCalcPositon(vertices);
}

s32 _SelectCursorCalcPositon(GSvec2 *vertices)
{
    s32 i;
    f32 outerAngle;
    s32 count = 0;
    f32 outerStep = (_menuPdaStatus.mOuterEndAngle - _menuPdaStatus.mOuterStartAngle) / 16.0f;
    f32 innerStep = (_menuPdaStatus.mInnerEndAngle - _menuPdaStatus.mInnerStartAngle) / 16.0f;
    vertices[count].x = _r1 * (f32)sin(_menuPdaStatus.mOuterStartAngle) + menuPdaCursorCenterOuter.x;
    vertices[count].y = _r1 * (f32)cos(_menuPdaStatus.mOuterStartAngle) + menuPdaCursorCenterOuter.y;
    count++;
    for (i = 0; i < 16; i++) {
        f32 angle = _menuPdaStatus.mInnerStartAngle + (f32)i * innerStep;
        vertices[count].x = _r2 * (f32)sin(angle) + menuPdaCursorCenterInner.x;
        vertices[count].y = _r2 * (f32)cos(angle) + menuPdaCursorCenterInner.y;
        count++;
    }
    s32 lastInner = count - 1;
    for (i = 0; i < 16; i++) {
        outerAngle = _menuPdaStatus.mOuterStartAngle + outerStep * (15 - i);
        vertices[count].x = _r1 * (f32)sin(outerAngle) + menuPdaCursorCenterOuter.x;
        if (i == 0) {
            vertices[count].y = vertices[lastInner].y;
        } else {
            vertices[count].y = _r1 * (f32)cos(outerAngle) + menuPdaCursorCenterOuter.y;
        }
        count++;
    }
    return count;
}

extern s32 _SelectLabel[];
void _SelectCursorCalcAngle(s8 select)
{
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(0x6B, _SelectLabel[select]);
    _menuPdaStatus.mCursorTargetY = (s16)(item->mOffsetA - 5);
    _menuPdaStatus.mOuterStartAngle = acos((_menuPdaStatus.mCursorY - menuPdaCursorCenterOuter.y) / _r1);
    {
        f32 height = 30.0f;
        _menuPdaStatus.mOuterEndAngle = acos(((_menuPdaStatus.mCursorY + height) - menuPdaCursorCenterOuter.y) / _r1);
    }
    _menuPdaStatus.mInnerStartAngle = acos((_menuPdaStatus.mCursorY - menuPdaCursorCenterInner.y) / _r2);
    {
        f32 height = 30.0f;
        _menuPdaStatus.mInnerEndAngle = acos(((_menuPdaStatus.mCursorY + height) - menuPdaCursorCenterInner.y) / _r2);
    }
    _menuPdaStatus.mStartAngleDelta = _menuPdaStatus.mOuterEndAngle - _menuPdaStatus.mOuterStartAngle;
    _menuPdaStatus.mEndAngleDelta = _menuPdaStatus.mInnerEndAngle - _menuPdaStatus.mInnerStartAngle;
}

s32 _SelectCursorPositionCheck(void)
{
    f32 dt = timeGetLastFrameSec();
    s32 done = 0;
    if (_menuPdaStatus.mCursorMoved == 1) {
        _menuPdaStatus.mCursorY = _menuPdaStatus.mCursorTargetY;
        done = 1;
    } else if (_menuPdaStatus.mCursorTargetY != _menuPdaStatus.mCursorY) {
        f32 step = _menuPdaStatus.mCursorTargetY - _menuPdaStatus.mCursorY;
        _menuPdaStatus.mCursorDistance = step;
        step = step * dt / 0.1f;
        if (0.0f == dt) {
            step = 0.0f;
        } else if ((f32)__fabs(step) <= 0.2f) {
            if (step >= 0.0f) step = 0.2f;
            else step = -0.2f;
        }
        _menuPdaStatus.mCursorY += step;
        f64 stepMagnitude = __fabs(step);
        if ((f32)__fabs(_menuPdaStatus.mCursorTargetY - _menuPdaStatus.mCursorY) < (f32)stepMagnitude) {
            _menuPdaStatus.mCursorY = _menuPdaStatus.mCursorTargetY;
            _menuPdaStatus.mCursorDistance = 0.0f;
            done = 1;
        }
    } else {
        done = 1;
    }
    return done;
}

struct PdaItemEntry { s32 flag; s32 item; };
extern PdaItemEntry pdaitemtable[5];
void _menuPdaSetMenuItem(void)
{
    PdaItemEntry *entry = pdaitemtable;
    s16 count = 0;
    for (s16 i = 0; i < 5U; i++) {
        if (entry->flag == 0) {
            menuItemBiosSetSelectFlag(0x6B, entry->item, 1);
            _cursor_index[count++] = i;
        } else if (GSflagGet(entry->flag) != 0) {
            menuItemBiosSetSelectFlag(0x6B, entry->item, 1);
            _cursor_index[count++] = i;
        } else {
            menuItemBiosSetSelectFlag(0x6B, entry->item, 0);
        }
        entry++;
    }
    _cursor_index_max = count;
}

s8 _menuPdaGetCurrentID(tagWINDOW_WORK *window, tagSPRITE_WORK *)
{
    if (_checkAddFunc(window)) return 0;
    u16 item = menuGetCursorItemID(window->mMenuID);
    if (window->cursorTo != window->mCursor.y || menuPdaCursorInit != 0) {
        _menuPdaSelectAnimeSet(_cursor_index[window->mCursor.y]);
        menuPdaCursorInit = 0;
    }
    switch (item) {
    case 2: _menuPdaStatus.mRes = PDATOP_MENU_RES_DP_MON; break;
    case 3: _menuPdaStatus.mRes = PDATOP_MENU_RES_MEMO; break;
    case 4: _menuPdaStatus.mRes = PDATOP_MENU_RES_MAIL; break;
    case 5: _menuPdaStatus.mRes = PDATOP_MENU_RES_SEARCHER; break;
    case 6: _menuPdaStatus.mRes = PDATOP_MENU_RES_EXIT; break;
    }
    return _menuPdaStatus.mRes;
}

void _menuPdaModelCursorAnime(void)
{
    if (_menuPdaStatus.mRes != _menuPdaStatus.mPreviousRes) {
        if (_menuPdaStatus.mRes > _menuPdaStatus.mPreviousRes) {
            _menuPdaSetMotion(0x17CF1000, 2, 0.0f, false);
        } else {
            _menuPdaSetMotion(0x17CF1000, 3, 0.0f, false);
        }
    }
    if (_menuPdaModelGetIndex(0x17CF1000) == 3) {
        if (!_menuPdaWaitMotion2(0x17CF1000)) {
            _menuPdaSetMotion(0x17CF1000, 1, 0.0f, true);
        }
    }
    if (_menuPdaModelGetIndex(0x17CF1000) == 2) {
        if (!_menuPdaWaitMotion2(0x17CF1000)) {
            _menuPdaSetMotion(0x17CF1000, 1, 0.0f, true);
        }
    }
}

extern "C" s32 menuPdaDummyCtrl(tagWINDOW_WORK *)
{
    menuPdaDrawDirectModel();
    return 0;
}

extern "C" s32 menuPdaTopCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case 0:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x132);
            window->mFlag = 1;
        }
        break;
    case 3:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x136);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

extern "C" void menuPdaModelVisibility(unsigned long res, bool visible) { _menuPdaModelVisibility(res, visible); }
extern "C" void menuPdaAddFunc(void) { _menuPdaAddFunc(); }
extern "C" void menuPdaZoomIn(void) { _menuPdaZoomIn(); }

void _menuPdaZoomOut(void)
{
    _menuPdaSetMotion(0x17CF1000, 7, 0.0f, false);
    _menuPdaWaitMotion(0x17CF1000);
    _menuPdaSetMotion(0x17CF1000, 1, 0.0f, true);
}

void _menuPdaZoomIn(void)
{
    _menuPdaSetMotion(0x17CF1000, 4, 0.0f, false);
    _menuPdaWaitMotion(0x17CF1000);
    _menuPdaSetMotion(0x17CF1000, 5, 0.0f, false);
    _menuPdaWaitMotion(0x17CF1000);
    _menuPdaSetMotion(0x17CF1000, 6, 0.0f, true);
}

void _menuPdaAddFunc(void)
{
    _menuPdaSetMotion(0x17CF1000, 8, 0.0f, false);
    _menuPdaWaitMotion(0x17CF1000);
    GSsndPlayNow(0x44E);
    _menuPdaSetMotion(0x17CF1000, 0, 0.0f, false);
    _menuPdaWaitMotion(0x17CF1000);
    _menuPdaSetMotion(0x17CF1000, 1, 0.0f, true);
}

void _menPdaFadeInWait(void)
{
    s32 state = 0;
    s32 running = 1;
    do {
        switch (state) {
        case 0: fadeSet(2, 0.5f); state = 1; break;
        case 1:
            if (fadeCheck(0) == 0) state = 100;
            else GSthreadSwitch();
            break;
        case 100: running = 0; break;
        }
    } while (running);
}

s32 _menPdaFadeOutWait(void)
{
    s32 state = 0;
    s32 running = 1;
    do {
        switch (state) {
        case 0:
            if (fadeCheck(0) == 0) state = 100;
            else GSthreadSwitch();
            break;
        case 100: running = 0; break;
        }
    } while (running);
    return 1;
}

void _menPdaFadeOutSet(void) { fadeSet(3, 0.8f); }
u32 _menuPdaGetMoney(void) { return heroGetStatus(0, HERO_STATUS_MONEY, 0); }
u32 _menuPdaGetPokecoupon(void) { return heroGetStatus(0, 13, 0); }

extern "C" void menuPdaSetSubres(s32 res, s32 subres)
{
    if (res >= 0) _menuPdaStatus.mRes = (PDATOP_MENU_RES)res;
    if (subres >= 0) _menuPdaStatus.mSubmenuFlag = subres;
}
extern "C" s32 menuPdaGetRes(void) { return _menuPdaStatus.mRes; }
extern "C" s32 menuPdaGetSubres(void) { return _menuPdaStatus.mSubmenuFlag; }

bool _menuPdaWaitMotion2(unsigned long res)
{
    _GSmodel *model = (_GSmodel *)GSresGetResourceAllGroup(res);
    if (model == 0) return false;
    return GSmodelIsAnimating(model);
}

s32 _menuPdaModelGetIndex(unsigned long res)
{
    _GSmodel *model = (_GSmodel *)GSresGetResourceAllGroup(res);
    if (model == 0) return -1;
    u32 index, next;
    GSmodelGetAnimIndex(model, &index, &next);
    return index;
}

void _menuPdaModelVisibility(unsigned long res, bool visible)
{
    _GSmodel *model = (_GSmodel *)GSresGetResourceAllGroup(res);
    if (model != 0) GSmodelSetVisibility(model, visible);
}

void _menuPdaStopMotion(unsigned long res)
{
    _GSmodel *model = (_GSmodel *)GSresGetResourceAllGroup(res);
    if (model != 0) GSmodelStopAnimation(model);
}

void _menuPdaWaitMotion(unsigned long res)
{
    _GSmodel *model = (_GSmodel *)GSresGetResourceAllGroup(res);
    if (model == 0) return;
    while (GSmodelIsAnimating(model)) {
        menuPdaDrawDirectModel();
        GSthreadSwitch();
    }
}

void _menuPdaSetMotion(unsigned long res, s16 motion, f32 frame, bool loop)
{
    _GSmodel *model = (_GSmodel *)GSresGetResourceAllGroup(res);
    if (model != 0) {
        GSmodelSetAnimIndex(model, motion);
        GSmodelSetAnimFrame(model, frame);
        GSmodelSetAnimRate(model, 0.5f);
        if (loop) GSmodelSetAnimType(model, 1);
        else GSmodelSetAnimType(model, 0);
        GSmodelStartAnimation(model);
    }
}

void _menuPdaExit(void)
{
    _menuPdaStatus.mExitPosition.x = 400.0f;
    _menuPdaStatus.mExitPosition.y = -100.0f;
    if (_menuPdaFlashBuffer != 0) _menuPdaFlashBuffer = 0;
    if (_menuPdaTextureBuffer != 0) {
        _menuPdaFree(_menuPdaTextureBuffer);
        _menuPdaTextureBuffer = 0;
    }
}

void _menuPdaInit(void)
{
    _menuPdaStatus.mRes = PDATOP_MENU_RES_DP_MON;
    _menuPdaStatus.mPreviousRes = 0;
    _menuPdaStatus.mSubmenuFlag = 0;
    _menuPdaStatus.ResetTransform();
    _menuPdaStatus.mMenuPosition.x = 300.0f;
    _menuPdaStatus.mMenuPosition.y = -100.0f;
    _menuPdaStatus.mExitPosition.x = 0.0f;
    _menuPdaStatus.mExitPosition.y = 0.0f;
    _menuPdaStatus.mExitPosition.z = 0.0f;
    _menuPdaStatus.mUnk3C = 0;
    _menuPdaStatus.mFadeOutDone = 1;
    _menuPdaStatus.mExitFlag = 0;
    _menuPdaStatus.mCursorMoved = 0;
    _SelectCursorCalcAngle(0);
    menuPdaCursorInit = 1;
    _menuPdaStatus.mCursorY = _menuPdaStatus.mCursorTargetY;
}

void _menuPdaFree(void *buffer) { GSmemFree(buffer); }

extern "C" void menuPdaOpen(void)
{
    GSfloorPush(0x392);
    floorSetFadeScript(0, 0);
    floorWaitForChange();
}

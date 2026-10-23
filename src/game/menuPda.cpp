#include "GSgfx.h"
#include "gsFlag.h"
#include "menuItemBios.h"
#include "menuPda.h"
#include "menuSub.h"
#include "pdaMemoList.h"
#include "time.h"
#include "winSprite.h"

// PDA front end. The PDA script (menuPdaScript) and the PDA screen (menuPda)
// share the fade in menuPdaScriptFade and the PDA frame menu; the helpers behind
// them drive the model, the top menu and the list cursors.

// Callees shared with the rest of the game. The zz_ names are the
// still-unrecovered symbols of the neighbouring objects (floor, model, save
// and PDA sub-menu code); they are called here with the roles the call sites
// and their arguments show.
extern "C" {
void menuOpen(s32 menuId, s32 mode);
void menuClose(s32 menuId);
void menuCloseSync(s32 menuId, s32 flag);
long menuOpenCustom(s32 menuId, long windowId, ...);
long windowGetActiveID(void);
u32 windowGetKeyRepeat(void);
void *windowGetCursorToItem(tagWINDOW_WORK *window);
u8 *menuDataBiosGetPtr(long dataId);
void menuItemBiosSetSelectFlag(s32 dataId, s32 itemId, s32 flag);
void *GSresGetResourceAllGroup(unsigned long res);
void *GSresGetResource(s32 group, unsigned long res);
void GSthreadSwitch(void);
void GSgfxResetRestore(int mode);
void GSmodelSetVisibility(void *model, s32 visible);
void GSmodelSetRenderLayer(void *model, s32 flag, GSgfxLayerID *layerId);
void GSmodelSetBoundCheck(void *model, s32 on);
void GSsndPlayNow(s32 id);
void scriptLightSetDisp(unsigned long res, s32 on);
void fightOutPokemonSetVisibility(unsigned long res, s32 visible);
void floorSetFadeScript(s32 floor, s32 script);
void zz_0114b9c_(s32 flag);
void floorPop(void);
s32 zz_005c228_(void);
u32 zz_01262fc_(void);
void zz_01971c8_(s32 flag, unsigned long res, f32 frame, s32 unk);
void GSgfxLayerSetCamera(void *model, void *res);
void zz_0039d2c_(s8 select);
s32 menuPdaMemoXDGetItem(void);
void menuPdaDPMonList(void);
void menuPdaMemoList(void);
void menuPdaMailListXD(void);
void zz_0042c2c_(void);
void menuButtonNormal(tagWINDOW_WORK *window);
void winSpriteSetDisp(tagSPRITE_WORK *sprite, s32 on);
s32 heroMoveGetLeader(void);
s32 floorGetNextFloorID(void);
void peopleTalkMessageBgmStart(unsigned long res);
void FlashSystemSetPlayFrameLabel(unsigned long flash, const char *start, const char *end);
void FlashSystemSetNowFrameLabel(unsigned long flash, const char *label);
void FlashSystemSetLoopFlag(unsigned long flash, s32 loop);
unsigned long FlashSystemGetFrameFromLabel(unsigned long flash, const char *label);
unsigned long FlashSystemGetNowFrame(unsigned long flash);
int sprintf(char *buf, const char *format, ...);
}

// Helpers of the PDA menus recovered in this object and its neighbours.
void _menuPdaInit(void);
void _menuPdaStartEvent(void);
void _menuPdaTopClose(void);
void _menuPdaZoomIn(void);
void _menuPdaZoomOut(void);
void _menuPdaExit(void);
void _menuPdaSetMenuItem(void);
void _menPdaFadeOutSet(void);
void _menPdaFadeOutWait(void);
void _menPdaFadeInWait(void);
void _menuPdaSetMotion(unsigned long res, s16 motion, f32 frame, bool loop);
void _menuPdaWaitMotion(unsigned long res);
extern "C" bool zz_003930c_(tagWINDOW_WORK *window);

// Entry points of this object.
extern "C" {
void menuPdaScript(u8 mode);
s32 menuPdaScriptGetAlpha(void);
void peopleTalkEnterFloor(void);
void menuPdaScriptSelectItems(u8 mode);
s32 menuPdaCursorCtrl(tagWINDOW_WORK *window);
s32 menuPdaDrawTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPdaButton(tagWINDOW_WORK *window);
void menuPdaInit(void);
void menuPdaMain(void);
void menuPda(void);
void menuPdaSetPageFlash(s32 page);
s32 menuPdaNullHook(void);
}

void _menuPdaScriptCalcAlpha(f32 dt);
PDATOP_MENU_RES topMenu(PDATOP_MENU_RES res);
s32 _getMenuIndex(PDATOP_MENU_RES res);

// Fade constants shared with the rest of the PDA object (sdata2).
extern "C" const f32 menuPdaScriptFadeTime;
extern "C" const f32 menuPdaZero;
extern "C" const f32 menuPdaAlphaFull;

// pdaMemoList::_menuClose(long)
void pdaMemoList::_menuClose(long menu)
{
    menuClose(menu);
    menuCloseSync(menu, 1);
}

extern "C" {

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
        menuPdaWork.mRes = PDATOP_MENU_RES_MEMO;
        menuPdaWork.mUnk08 = 1;
        FlashSystemSetPlayFrameLabel(0x1FBD3000, menuPdaFlashMemoStart, menuPdaFlashMemoEnd);
        FlashSystemSetNowFrameLabel(0x1FBD3000, menuPdaFlashMemoStart);
        wait = FlashSystemGetFrameFromLabel(0x1FBD3000, menuPdaFlashMemoEnd);
    } else {
        menuPdaWork.mRes = PDATOP_MENU_RES_DP_MON;
        menuPdaWork.mUnk08 = 0;
        FlashSystemSetPlayFrameLabel(0x1FBD3000, menuPdaFlashDPMonStart, menuPdaFlashDPMonEnd);
        FlashSystemSetNowFrameLabel(0x1FBD3000, menuPdaFlashDPMonStart);
        wait = FlashSystemGetFrameFromLabel(0x1FBD3000, menuPdaFlashDPMonEnd);
    }
    zz_0039d2c_(menuPdaWork.mRes);
    menuPdaWork.mFadeFrom = menuPdaWork.mFadeTo;
    FlashSystemSetLoopFlag(0x1FBD3000, 0);
    menuPdaWork.mSubmenuFlag = 0;
    menuPdaScriptSelectItems(mode);
    menuOpen(0xA8, 0);
    menuOpenCustom(0x6C, windowGetActiveID(), 0, 0, 0, 1, &arg);
    menuOpenCustom(0x107, windowGetActiveID(), &value, 0, 0, 0);
    while (FlashSystemGetNowFrame(0x1FBD3000) < wait) {
        GSthreadSwitch();
    }
    peopleTalkEnterFloor();
    GSthreadSwitch();
    if (mode == 0) {
        menuItemBiosSetSelectFlag(0x107, 2, 1);
    } else {
        menuItemBiosSetSelectFlag(0x107, 5, 1);
    }
    fade = menuPdaScriptFadeTime;
    do {
        dt = timeGetLastFrameSec();
        _menuPdaScriptCalcAlpha(dt);
        fade = fade - dt;
        GSthreadSwitch();
    } while (fade >= menuPdaZero);
    menuClose(0x107);
    menuCloseSync(0x107, 1);
    menuClose(0xA8);
    menuClose(0x6C);
    menuCloseSync(0x6C, 1);
    GSgfxResetRestore(1);
    floorPop();
}

// Alpha of the PDA script fade as a 0-255 fade step.
s32 menuPdaScriptGetAlpha(void)
{
    f32 frame = menuPdaScriptFade;
    f32 full;
    f32 limit;
    f32 fade;

    limit = menuPdaScriptFadeLimit;
    if (frame >= limit) {
        return 0xFF;
    }
    full = menuPdaAlphaFull;
    fade = full * frame / limit;
    if (fade > full) {
        fade = full;
    }
    return fade;
}

// Advance the PDA script fade by dt.
void _menuPdaScriptCalcAlpha(f32 dt)
{
    menuPdaScriptFade = menuPdaScriptFade + dt;
}

// Reset the PDA script fade; run when entering a floor.
void peopleTalkEnterFloor(void)
{
    menuPdaScriptFade = menuPdaZero;
}

// Selectable state of the PDA frame items for the script mode.
void menuPdaScriptSelectItems(u8 mode)
{
    if (mode == 0) {
        menuItemBiosSetSelectFlag(0x107, 2, 0);
        menuItemBiosSetSelectFlag(0x107, 5, 0);
    } else {
        menuItemBiosSetSelectFlag(0x107, 2, 1);
        menuItemBiosSetSelectFlag(0x107, 5, 0);
    }
    menuItemBiosSetSelectFlag(0x107, 3, 1);
    menuItemBiosSetSelectFlag(0x107, 4, 1);
    menuItemBiosSetSelectFlag(0x107, 6, 1);
}

// Per-frame hook of the PDA wait loops (motion and zoom waits): nothing to do
// in the release build.
void menuPdaWaitFrame(void)
{
}

// Point the PDA menus at the PDA model resource.
void menuPdaSetModelPointer(void)
{
    menuPdaModel = GSresGetResourceAllGroup(0x17CF1000);
}

// Script-side twin of menuPdaSetModelPointer: the PDA script and the mail
// flow each register their own entry that points the menus at the PDA model.
void menuPdaScriptSetModel(void)
{
    menuPdaModel = GSresGetResourceAllGroup(0x17CF1000);
}

// List cursor of the PDA frame menu. Moves the window cursor through the
// shown rows of the item chain; the step grows while the key repeats until
// the cursor distance covers the screen.
s32 menuPdaCursorCtrl(tagWINDOW_WORK *window)
{
    u16 keys;
    tagMENU_ITEM_DATA *item;
    u8 *menuData;
    MenuPdaWork *work;
    s32 up;
    s32 down;
    s16 count;
    s32 stepX;
    s32 stepY;

    if (window == 0) {
        return 0;
    }
    if (zz_003930c_(window) != false) {
        return 0;
    }
    menuData = menuDataBiosGetPtr(window->mMenuID);
    if (windowGetCursorToItem(window) == 0) {
        window->cursor = 0;
        return 0;
    }
    stepX = 0x10;
    stepY = 0xC;
    keys = windowGetKeyRepeat();
    up = keys & 1;
    work = &menuPdaWork;
    down = keys & 2;
    for (;;) {
        count = 0;
        item = menuItemBiosGetPtr(window->mMenuID, *(s16 *)(menuData + 6));
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
            if (window->cursor > 0) {
                window->cursor--;
                work->mCursorMoved = 0;
            } else {
                window->cursor = count - 1;
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
            if (window->cursor < count - 1) {
                window->cursor++;
                work->mCursorMoved = 0;
            } else {
                window->cursor = 0;
                work->mCursorMoved = 1;
            }
        }
        if (window->cursorTo != window->cursor) {
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
    if (zz_01262fc_() != 0x38C) {
        menuButtonNormal(window);
    }
}

// Show/hide the title sprite of a PDA list window for the leader and floor
// the player is on.
s32 menuPdaDrawTitle(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 leader = heroMoveGetLeader();
    s32 floor = floorGetNextFloorID();
    s16 item = sprite->mItemID;

    switch (item) {
    case 0x1CA:
    case 0x1CB:
    case 0x1CC:
        if (leader == 0 && floor == 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x16:
        if (leader == 0) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x17:
        if (leader == 2) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x18:
        if (leader == 1 && floor == 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1A:
        if (leader == 1 && floor > 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1B:
        if (leader == 1 && floor > 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1C:
        if (leader == 1 && floor > 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1D:
        if (leader == 1 && floor > 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1E:
        if (leader == 1 && floor > 1 && menuPdaMemoXDGetItem() == 0) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x1F:
        if (leader == 1 && floor > 1 && menuPdaMemoXDGetItem() == 1) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x20:
        if (leader == 1 && floor > 1 && menuPdaMemoXDGetItem() == 2) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x21:
        if (leader == 1 && floor > 1 && menuPdaMemoXDGetItem() == 3) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 0x19:
        if (leader == 3) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    }
    return 0;
}

// Empty callback of a PDA item record: the record only needs its sprite set.
s32 menuPdaNullHook(void)
{
    return 0;
}

// The PDA screen: open the PDA frame, run the top menu loop with the zoomed
// submenus and close everything again.
void menuPda(void)
{
    PDATOP_MENU_RES res = PDATOP_MENU_RES_DP_MON;
    void *model;
    PDA_FRAME_ARG arg;

    scriptLightSetDisp(0x17CF1600, 0);
    scriptLightSetDisp(0x17CF1601, 0);
    if (zz_005c228_() == 1) {
        flagSetTrue(0x3F2);
    }
    FlashSystemSetLoopFlag(0x1FBD3000, 1);
    FlashSystemSetLoopFlag(0x20543000, 0);
    menuOpen(0xA8, 0);
    model = GSresGetResourceAllGroup(0x17CF1000);
    menuPdaModel = model;
    GSmodelSetVisibility(model, 0);
    GSthreadSwitch();
    menuPdaWork.mExitFlag = 0;
    _menuPdaInit();
    _menuPdaStartEvent();
    _menuPdaSetMenuItem();
    zz_0039d2c_(menuPdaResTable[0]);
    menuPdaWork.mFadeFrom = menuPdaWork.mFadeTo;
    arg.mUnk04 = &menuPdaFrameValue0;
    arg.mUnk00 = 0;
    arg.mUnk01 = 0xFF;
    arg.mUnk02 = 0;
    arg.mUnk08 = &menuPdaFrameValue1;
    menuOpenCustom(0x6C, windowGetActiveID(), 0, 0, 0, 1, &arg);
    while (menuPdaWork.mExitFlag == 0) {
        _menuPdaSetMenuItem();
        res = topMenu(res);
        menuPdaWork.mSubmenuFlag = 1;
        menuPdaWork.mRes = res;
        switch (res) {
        case PDATOP_MENU_RES_DP_MON:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            menuPdaDPMonList();
            menuPdaWork.mSubmenuFlag = 0;
            _menuPdaZoomOut();
            break;
        case PDATOP_MENU_RES_MEMO:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            menuPdaMemoList();
            menuPdaWork.mSubmenuFlag = 0;
            _menuPdaZoomOut();
            break;
        case PDATOP_MENU_RES_MAIL:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            menuPdaMailListXD();
            menuPdaWork.mSubmenuFlag = 0;
            _menuPdaZoomOut();
            break;
        case PDATOP_MENU_RES_EXTRA:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            zz_0042c2c_();
            menuPdaWork.mSubmenuFlag = 0;
            _menuPdaZoomOut();
            break;
        default:
            _menPdaFadeOutSet();
            _menuPdaExit();
            menuPdaWork.mExitFlag = 1;
            break;
        }
    }
    _menPdaFadeOutWait();
    if (menuPdaWork.mFadeOutDone != 1) {
        zz_0114b9c_(1);
        menuPdaWork.mFadeOutDone = 1;
    }
    _menuPdaTopClose();
    menuClose(0x6C);
    menuCloseSync(0x6C, 1);
    menuClose(0xA8);
    menuCloseSync(0xA8, 1);
    fightOutPokemonSetVisibility(0x17CF1000, 0);
    _menPdaFadeInWait();
    GSgfxResetRestore(1);
    floorPop();
    floorSetFadeScript(0, 0);
}

// Select the page flash labels for a PDA top menu entry: entry 0 gets the
// "1_" labels and so on, and the leave entry (4) has no page, so the flash is
// left alone. Called with the entry of the row under the list cursor.
void menuPdaSetPageFlash(s32 res)
{
    char label[0x40];
    char text[0x40];

    if (res >= 4) {
        menuPdaPageDisp = 0;
        return;
    }
    menuPdaPageDisp = 1;
    sprintf(label, menuPdaPageStartFormat, res + 1);
    sprintf(text, menuPdaPageEndFormat, res + 1);
    FlashSystemSetPlayFrameLabel(0x20543000, label, text);
    FlashSystemSetNowFrameLabel(0x20543000, label);
}

// Idle thread of the PDA screen; the script system reaches it without a
// direct call. The original object exports it (tdesc descriptor, kept so the
// link reproduces the original DOL).
__declspec(export) void menuPdaMain(void)
{
    for (;;) {
        GSthreadSwitch();
    }
}

// Load the PDA model and hide it; the PDA jingle plays once unlocked.
// Exported like menuPdaMain.
__declspec(export) void menuPdaInit(void)
{
    void *model = GSresGetResourceAllGroup(0x17CF1000);

    menuPdaModel = model;
    if (model != 0) {
        GSmodelSetVisibility(model, 0);
        _menuPdaSetMotion(0x17CF1000, 0, menuPdaZero, false);
    }
    if (menuPdaSoundEnable != 0) {
        GSsndPlayNow(0x44E);
    }
}
} // extern "C"

// Intro sequence of the PDA screen: render the PDA model on the event layer,
// play its intro motion and start the floor talk bgm.
void _menuPdaStartEvent(void)
{
    GSgfxLayerID layer = menuPdaEventLayer[0];
    void *model = GSresGetResourceAllGroup(0x17CF1000);

    GSmodelSetRenderLayer(model, 1, &layer);
    zz_01971c8_(0x17, 0x17CF1800, menuPdaZero, 0);
    GSgfxLayerSetCamera(model, GSresGetResource(0x17, 0x17CF1800));
    GSmodelSetVisibility(model, 1);
    GSmodelSetBoundCheck(model, 0);
    _menuPdaSetMotion(0x17CF1000, 0, menuPdaZero, false);
    GSgfxResetRestore(0);
    GSthreadSwitch();
    _menuPdaWaitMotion(0x17CF1000);
    peopleTalkMessageBgmStart(0x17CF1000);
    _menuPdaSetMotion(0x17CF1000, 1, menuPdaZero, true);
}

// Close the PDA top menu.
void _menuPdaTopClose(void)
{
    menuClose(0x6B);
    menuCloseSync(0x6B, 1);
}

// Open the PDA top menu with the cursor on res and return the entry the
// player chose; -1 when the menu could not open.
PDATOP_MENU_RES topMenu(PDATOP_MENU_RES res)
{
    s32 index = _getMenuIndex(res);

    if (menuOpenCustom(0x6B, windowGetActiveID(), &index, 0, 1, 0) == -1) {
        return (PDATOP_MENU_RES)-1;
    }
    return menuPdaWork.mRes;
}

// Row index of res in the shown top menu rows; 0 when not listed.
s32 _getMenuIndex(PDATOP_MENU_RES res)
{
    for (s16 i = 0; i < menuPdaResCount; i++) {
        if (res == menuPdaResTable[i]) {
            return i;
        }
    }
    return 0;
}

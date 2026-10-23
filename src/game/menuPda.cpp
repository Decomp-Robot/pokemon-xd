#include <menu.h>
#include <GSsnd.h>
#include <floorRoom.h>
#include <pokecolo.h>
#include <gsFlag.h>
#include <camera.h>
#include "GSgfx.h"
#include "GSmodel.h"
#include "GSres.h"
#include "menuItemBios.h"
#include "menuPda.h"
#include "menuOffScreen.h"
#include "menuSub.h"
#include "time.h"
#include "winSprite.h"

enum {
    PDA_MODEL_RESOURCE = 0x17CF1000,
    PDA_EVENT_LIGHT_1_RESOURCE = 0x17CF1600,
    PDA_EVENT_LIGHT_2_RESOURCE = 0x17CF1601,
    PDA_CAMERA_RESOURCE = 0x17CF1800,
    PDA_OPEN_FLASH = 0x1FBD3000,
    PDA_PAGE_FLASH = 0x20543000,
    PDA_OPEN_SOUND = 0x44E,
};

// PDA front end. The PDA script (menuPdaScript) and the PDA screen (menuPda)
// share the fade in menuPdaScriptFade and the PDA frame menu; the helpers behind
// them drive the model, the top menu and the list cursors.

extern "C" {
u16 windowGetKeyRepeat(void);
long windowGetActiveID(void);
void *windowGetCursorToItem(tagWINDOW_WORK *window);
u8 *menuDataBiosGetPtr(long dataId);
void menuItemBiosSetSelectFlag(s32 dataId, s32 itemId, s32 flag);
void GSthreadSwitch(void);
void scriptLightSetDisp(unsigned long res, s32 on);
void fightOutPokemonSetVisibility(unsigned long res, s32 visible);
void floorSetFadeScript(s32 floor, s32 script);
void GSfloorPop(void);
s32 menuPdaMemoXDGetItem(void);
void menuPdaDPMonList(void);
void menuPdaMemoList(void);
void menuPdaMailListXD(void);
void menuPdaSearcher(void);
void menuButtonNormal(tagWINDOW_WORK *window);
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

// PDA menu helpers.
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
        FlashSystemSetPlayFrameLabel(PDA_OPEN_FLASH, menuPdaFlashMemoStart, menuPdaFlashMemoEnd);
        FlashSystemSetNowFrameLabel(PDA_OPEN_FLASH, menuPdaFlashMemoStart);
        wait = FlashSystemGetFrameFromLabel(PDA_OPEN_FLASH, menuPdaFlashMemoEnd);
    } else {
        menuPdaWork.mRes = PDATOP_MENU_RES_DP_MON;
        menuPdaWork.mUnk08 = 0;
        FlashSystemSetPlayFrameLabel(PDA_OPEN_FLASH, menuPdaFlashDPMonStart, menuPdaFlashDPMonEnd);
        FlashSystemSetNowFrameLabel(PDA_OPEN_FLASH, menuPdaFlashDPMonStart);
        wait = FlashSystemGetFrameFromLabel(PDA_OPEN_FLASH, menuPdaFlashDPMonEnd);
    }
    _SelectCursorCalcAngle(menuPdaWork.mRes);
    menuPdaWork.mFadeFrom = menuPdaWork.mFadeTo;
    FlashSystemSetLoopFlag(PDA_OPEN_FLASH, 0);
    menuPdaWork.mSubmenuFlag = 0;
    menuPdaScriptSelectItems(mode);
    menuOpen(MENU_ID_PDA_BACKDROP, 0);
    menuOpenCustom(MENU_ID_PDA_FRAME, windowGetActiveID(), 0, 0, 0, 1, &arg);
    menuOpenCustom(MENU_ID_PDA_SCRIPT_INPUT, windowGetActiveID(), &value, 0, 0, 0);
    while (FlashSystemGetNowFrame(PDA_OPEN_FLASH) < wait) {
        GSthreadSwitch();
    }
    peopleTalkEnterFloor();
    GSthreadSwitch();
    if (mode == 0) {
        menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 2, 1);
    } else {
        menuItemBiosSetSelectFlag(MENU_ID_PDA_SCRIPT_INPUT, 5, 1);
    }
    fade = menuPdaScriptFadeTime;
    do {
        dt = timeGetLastFrameSec();
        _menuPdaScriptCalcAlpha(dt);
        fade = fade - dt;
        GSthreadSwitch();
    } while (fade >= menuPdaZero);
    menuClose(MENU_ID_PDA_SCRIPT_INPUT);
    menuCloseSync(MENU_ID_PDA_SCRIPT_INPUT, 1);
    menuClose(MENU_ID_PDA_BACKDROP);
    menuClose(MENU_ID_PDA_FRAME);
    menuCloseSync(MENU_ID_PDA_FRAME, 1);
    menuOffScreenSetPriority(1);
    GSfloorPop();
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
void menuPdaWaitFrame(void)
{
}

// Point the PDA menus at the PDA model resource.
void menuPdaSetModelPointer(void)
{
    menuPdaModel = GSresGetResourceAllGroup(PDA_MODEL_RESOURCE);
}

// Script-side twin of menuPdaSetModelPointer: the PDA script and the mail
// flow each register their own entry that points the menus at the PDA model.
void menuPdaScriptSetModel(void)
{
    menuPdaModel = GSresGetResourceAllGroup(PDA_MODEL_RESOURCE);
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
    if (_checkAddFunc(window) != false) {
        return 0;
    }
    menuData = menuDataBiosGetPtr(window->mMenuID);
    if (windowGetCursorToItem(window) == 0) {
        window->mCursorState.mSelection.mCursor = 0;
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
            if (window->mCursorState.mSelection.mCursor > 0) {
                window->mCursorState.mSelection.mCursor--;
                work->mCursorMoved = 0;
            } else {
                window->mCursorState.mSelection.mCursor = count - 1;
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
            if (window->mCursorState.mSelection.mCursor < count - 1) {
                window->mCursorState.mSelection.mCursor++;
                work->mCursorMoved = 0;
            } else {
                window->mCursorState.mSelection.mCursor = 0;
                work->mCursorMoved = 1;
            }
        }
        if (window->cursorTo != window->mCursorState.mSelection.mCursor) {
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
    if (fieldGetCurrentRoomID() != 0x38C) {
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

    scriptLightSetDisp(PDA_EVENT_LIGHT_1_RESOURCE, 0);
    scriptLightSetDisp(PDA_EVENT_LIGHT_2_RESOURCE, 0);
    if (pokecoloGetRomVersion() == 1) {
        flagSetTrue(0x3F2);
    }
    FlashSystemSetLoopFlag(PDA_OPEN_FLASH, 1);
    FlashSystemSetLoopFlag(PDA_PAGE_FLASH, 0);
    menuOpen(MENU_ID_PDA_BACKDROP, 0);
    model = GSresGetResourceAllGroup(PDA_MODEL_RESOURCE);
    menuPdaModel = model;
    GSmodelSetVisibility((_GSmodel*)model, 0);
    GSthreadSwitch();
    menuPdaWork.mExitFlag = 0;
    _menuPdaInit();
    _menuPdaStartEvent();
    _menuPdaSetMenuItem();
    _SelectCursorCalcAngle(menuPdaResTable[0]);
    menuPdaWork.mFadeFrom = menuPdaWork.mFadeTo;
    arg.mUnk04 = &menuPdaFrameValue0;
    arg.mUnk00 = 0;
    arg.mUnk01 = 0xFF;
    arg.mUnk02 = 0;
    arg.mUnk08 = &menuPdaFrameValue1;
    menuOpenCustom(MENU_ID_PDA_FRAME, windowGetActiveID(), 0, 0, 0, 1, &arg);
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
        case PDATOP_MENU_RES_SEARCHER:
            _menuPdaTopClose();
            _menuPdaZoomIn();
            menuPdaSearcher();
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
        menuOffScreenSetDisp(1);
        menuPdaWork.mFadeOutDone = 1;
    }
    _menuPdaTopClose();
    menuClose(MENU_ID_PDA_FRAME);
    menuCloseSync(MENU_ID_PDA_FRAME, 1);
    menuClose(MENU_ID_PDA_BACKDROP);
    menuCloseSync(MENU_ID_PDA_BACKDROP, 1);
    fightOutPokemonSetVisibility(PDA_MODEL_RESOURCE, 0);
    _menPdaFadeInWait();
    menuOffScreenSetPriority(1);
    GSfloorPop();
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
    FlashSystemSetPlayFrameLabel(PDA_PAGE_FLASH, label, text);
    FlashSystemSetNowFrameLabel(PDA_PAGE_FLASH, label);
}

// Idle thread of the PDA screen, entered by the script system.
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
    void *model = GSresGetResourceAllGroup(PDA_MODEL_RESOURCE);

    menuPdaModel = model;
    if (model != 0) {
        GSmodelSetVisibility((_GSmodel*)model, 0);
        _menuPdaSetMotion(PDA_MODEL_RESOURCE, 0, menuPdaZero, false);
    }
    if (menuPdaSoundEnable != 0) {
        GSsndPlayNow(PDA_OPEN_SOUND);
    }
}
} // extern "C"

// Intro sequence of the PDA screen: render the PDA model on the event layer,
// play its intro motion and start the floor talk bgm.
void _menuPdaStartEvent(void)
{
    GSgfxLayerID layer = menuPdaEventLayer[0];
    void *model = GSresGetResourceAllGroup(PDA_MODEL_RESOURCE);

    GSmodelSetRenderLayer((_GSmodel*)model, 1, &layer);
    cameraPlayAnime(0x17, PDA_CAMERA_RESOURCE, menuPdaZero, 0);
    GSmodelSetRenderCamera((_GSmodel*)model, GSresGetResource(0x17, PDA_CAMERA_RESOURCE));
    GSmodelSetVisibility((_GSmodel*)model, 1);
    GSmodelSetBoundCheck((_GSmodel*)model, 0);
    _menuPdaSetMotion(PDA_MODEL_RESOURCE, 0, menuPdaZero, false);
    menuOffScreenSetPriority(0);
    GSthreadSwitch();
    _menuPdaWaitMotion(PDA_MODEL_RESOURCE);
    peopleTalkMessageBgmStart(PDA_MODEL_RESOURCE);
    _menuPdaSetMotion(PDA_MODEL_RESOURCE, 1, menuPdaZero, true);
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

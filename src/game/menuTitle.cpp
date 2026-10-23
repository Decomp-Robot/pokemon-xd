#include <menuDataBios.h>
#include <menuTop.h>
#include <GSsnd.h>
#include <GSthread.h>
#include <winMsg.h>
#include <menuTool.h>
#include <flashsystem.h>
#include <GSres.h>
#include <floorManager.h>
#include <gamedata.h>
#include <fade.h>
#include <camera.h>
#include <GSfsys.h>
#include <menu.h>
#include <sound.h>
#include <floor.h>
#include <winSprite.h>
#include <WorldMap.h>
#include <fightEncountBios.h>
#include "GSmodel.h"

#pragma force_active on

/*
 * menuTitle.cpp
 *
 * Title screen: the intro flash movie, the Orre map fly-around with the
 * press-start prompt, and the mode select flash that hands control to the
 * chosen entry menu. The map models reuse the world-map model helpers.
 */

// Flash movies the title screen plays (label-addressed frames).
#define TITLE_FLASH 0x1FE03000     // flash layer the title screen plays
#define TITLE_BGM_ID 1506          // title BGM stream
#define TITLE_MAP_MODEL_RES 0x22140400 // map model resource the fly-around shows
#define TITLE_MAP_RES_A 0x22141000  // map model resource: the Orre landmass
#define TITLE_MAP_RES_B 0x22141001  // map model resource: the buildings
#define TITLE_MAP_RES_C 0x22141002  // map model resource: the landmarks
#define TITLE_MAP_RES_D 0x22141003  // map model resource: the foliage
#define TITLE_MAP_CAM_RES 0x22161800   // map camera resource of the fly-around
#define TITLE_SELECT_RES 0xA8C     // select menu resource group

/* Camera types accepted by cameraSetType. */
#define TITLE_CAM_TYPE_MAP 4 // the fly-around map camera

// States of menuTitleMainSub, kept in _menuTitleStatus.
enum {
    TITLE_STATE_0 = 0,
    TITLE_STATE_FLASH_BACK = 1,
    TITLE_STATE_FLASH_START = 30,
    TITLE_STATE_CONFIRM = 31,
    TITLE_STATE_WAIT_FLASH = 32,
    TITLE_STATE_MAP = 40,
    TITLE_STATE_MAP_FADE = 41,
    TITLE_STATE_SELECT = 200,
    TITLE_STATE_SELECT_IN = 201,
    TITLE_STATE_QUIT = 1000,
};

// Window menu id the title screen opens.
#define TITLE_MENU_ID 17
/* Additional windows retained by the mode-select screen. */
#define TITLE_SELECT_MENU_ID_1 0x116
#define TITLE_SELECT_MENU_ID_2 0x118
/* Confirm sound played when the title prompt is accepted. */
#define TITLE_SE_CONFIRM 17
/* Start plus one other pad key: the keys that accept a menu entry. */
#define TITLE_ACCEPT_KEYS 0x810
/* Title BGM fade-out time handed to the stream player. */
#define TITLE_BGM_FADE_TIME 1000
/* Room whose floor the map fly-around unlinks when the map state hands over
 * to the mode-select menu, and the room the preread thread waits for. */
#define TITLE_UNLINK_ROOM_ID 0x39C
#define TITLE_PREREAD_ROOM_ID 0x397
/* Address of the waza select menu resource. */
#define TITLE_WAZA_SELECT_RES 0x200A3000

/* Mode-select entry defaults the battle select screen starts from. */
struct TitleSelectWork {
    u8 _00[0x10];
    f32 mRows[4][3];
    s16 mPages[5]; // +0x40..+0x48: entry pages, -1 until filled
};

#ifdef __cplusplus
extern "C" {
#endif

/* Title-screen work variables. */
extern s32 _menuTitleStartStatus;      // mode select entry recorded by menuTitleSetStartStatus
extern s32 _menuTitleDiskCoverOpen;     // battle-select menu open attempts
extern u32 _menuTitleStatus;              // menuTitleMainSub state machine step
extern f32 _menuTitleTimer;      // seconds left of the flash fade hold
extern f32 _menuTitleFrameSec;       // last frame time, for the map fade
extern f32 _menuTitleSeFrame;     // accumulated flash fade seconds
extern u32 _menuTitleSeCount;    // frames the flash end holds
extern u32 _menuTitleType;     // set when a select entry is confirmed
extern f32 _menuTitleFrameCount;         // press-start prompt fade 0..1
extern f32 _menuTitleAutoDemoCount;     // bgm fade-out seconds accumulated
extern u32 _gsthread_ptr_fight_reg_wzx;    // preread thread id, 0 when none runs
extern u32 _menuTitleSkipFlag;           // cleared when the title flow initializes
extern s16 _titleCursorTimer;       // press-start pulse frame, advanced % 1000

/* Mode-select work block the entry menus read back. */
extern TitleSelectWork _menuTitleWork;

/* Battle-select slot the next entry uses, 0..3. */
extern s32 _menuTitleWorkPat;

#ifdef __cplusplus
}
#endif

extern "C" {
void dbgMenuSetOpenFlag(s32 flag);
u8 GSsndIsPlayingStream(u32 streamID);
f32 timeGetLastFrameSec(void);
double sin(double x);
u32 roomDataGetFsysGidForLanguage(void *mapData);
void fightRegWzxLoadAll(s32 id);
void savedataCreate(s32 a, s32 b);
s32 memcardTaskCall(s32 a, s32 b, s32 c);
/* Records whether the mode select screen may keep running. */
void menuTopSetInitCursor(s32 a);
s32 pokecoloGetRomVersion(void);
u8 menuIsOpen(s32 id);
/* Sets the battle CD id on the encounter data the select screen hands on. */
}

/* Functions of this module. */
void _menuTitleNormalButton(tagWINDOW_WORK *window);
s32 _selectMenu(void);
void _CreateThread(void);
void _menuTitlePreReadFightRegWzxData(void);
void _menuTitleSetSE(u32 id);
void _menuTitleClose2(s32 id, u32 flag);

extern "C" {
void menuTitleLoadMap(void);
void menuTitleSetupModel(u32 resourceID, s16 animationIndex, u8 animationType, f32 startFrame);

void menuTitleModelCtor(u32 group, u32 modelID, bool visible);
void menuTitleModelDtor(u32 group, u32 modelID);
u8 menuTitlePlayBGMWait(u8 wait);
void menuTitlePlayBGM(void);
void menuTitleOpenSelectAnime2(void);
void menuTitleOpenSelectAnime(void);
void menuTitleExit(void);
void menuTitleBattleSelectFree(void);
void menuTitleMain(void);
void menuTitleStartBattleSelect(void);
void menuTitleReadWazaSelectMenu(void);
void menuTitleLoadMapModels(void);
void menuTitleSetupMapCamera(void);
void menuTitleUnloadMapModels(void);
void menuTitleCursorAnimeMain(void);
void menuTitleCursorAnime(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTitleCursorShowSelected(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTitleFunc(void);
void menuTitleButton(tagWINDOW_WORK *window);
void menuTitleHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTitleMainSub(void);
void menuTitleBattlePreReadCheck(void);
void menuTitleSetStartStatus(s32 entry);

void menuTitleCloseSelectAnime(void);
}

void _menuTitleNormalButton(tagWINDOW_WORK *window);
s32 _selectMenu(void);
void _menuTitlePreReadFightRegWzxData(void);
void _CreateThread(void);

/* -------------------------------------------------------------------------
 * Title screen flow
 * ---------------------------------------------------------------------- */



/* Open the title window over the currently active one. */
extern "C" void menuTitleOpenMenu(void)
{
    menuTitleLoadMap();
    menuOpenCustom(TITLE_MENU_ID, windowGetActiveID(), 0, 0, 0, 0);
}

/* Selected entry of the mode-select menu. */
extern "C" s16 menuTitleGetSelect(s32 entry)
{
    TitleSelectWork *select = &_menuTitleWork;

    return select->mPages[entry];
}

extern "C" void menuTitleSetSelect(s32 entry, s16 value)
{
    TitleSelectWork *select = &_menuTitleWork;

    select->mPages[entry] = value;
}

/* Per-frame housekeeping of the title window: pulse the press-start prompt
 * and, once the map fade has run long enough, move on to the map state. */
extern "C" void menuTitleFunc(void)
{
    s32 state;

    menuTitleCursorAnimeMain();
    _menuTitleFrameSec = timeGetLastFrameSec();
    _menuTitleFrameCount += _menuTitleFrameSec / 2.0f;
    if (_menuTitleFrameCount >= 1.0f) {
        _menuTitleFrameCount = 0.0f;
    }

    state = (s32)_menuTitleStatus;
    if (state < 30) {
        return;
    }
    if (state == TITLE_STATE_MAP_FADE) {
        _menuTitleFrameCount = 0.0f;
        return;
    }
    if (state == TITLE_STATE_SELECT) {
        return;
    }
    if (pokecoloGetRomVersion() == 1) {
        return;
    }
    _menuTitleAutoDemoCount += _menuTitleFrameSec;
    if (!(_menuTitleAutoDemoCount >= 60.5f)) {
        return;
    }
    _menuTitleAutoDemoCount = 0.0f;
    state = (s32)_menuTitleStatus;
    /* The flow ends once it quits; until then the flash confirm step has
     * nothing left to do either. */
    if (state == TITLE_STATE_QUIT) {
        return;
    }
    if (state < TITLE_STATE_QUIT) {
        if (state == TITLE_STATE_CONFIRM) {
            return;
        }
    }
    _menuTitleStatus = TITLE_STATE_MAP;
    menuTitleSetStartStatus(TITLE_STATE_0);
}

/* Button hook of the title window: run the standard handler once the intro
 * flash has played through. */
extern "C" void menuTitleButton(tagWINDOW_WORK *window)
{
    if (fadeCheck(0) != 0) {
        return;
    }
    dbgMenuSetOpenFlag(1);
    u32 end = FlashSystemGetFrameFromLabel(TITLE_FLASH, "end");
    if (FlashSystemGetNowFrame(TITLE_FLASH) >= end) {
        _menuTitleNormalButton(window);
    }
}

/* Standard title button handling: confirm on Start/A. */
void _menuTitleNormalButton(tagWINDOW_WORK *window)
{
    struct tagWINDOW_KEY *key;

    if ((s32)_menuTitleStatus >= TITLE_STATE_MAP_FADE) {
        return;
    }
    if (window == 0) {
        return;
    }
    menuDataBiosGetPtr(window->mMenuID);
    key = windowGetKeyInfo();
    if ((key->mKeyTrg & TITLE_ACCEPT_KEYS) == 0) {
        return;
    }
    if (window->mAccept == 0) {
        _menuTitleSetSE(TITLE_SE_CONFIRM);
    }
    window->mAccept = 1;
}


/* Prompt cursor pulse: fade the entry sprite out while the 60-frame cycle is
 * in its first half, unless the title window is not the active one. */
extern "C" void menuTitleCursorAnime(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 phase;
    f32 angle;
    f32 alpha;

    if (window == windowSearchID(windowGetActiveID())) {
        s32 frame = _titleCursorTimer;

        phase = frame % 60;
        sprite->mColor = -1;
        if (phase < 30) {
            angle = 3.14f * ((f32)phase / 30.0f);
            alpha = (f32)sin(angle);
            alpha = 128.0f * alpha;
            sprite->mRgba.mAlpha -= (u8)alpha;
        }
    } else {
        sprite->mRgba.mAlpha = 255;
    }
}


/* Advance the press-start cycle counter from the frame time. */
extern "C" void menuTitleCursorAnimeMain(void)
{
    s32 frame;
    f32 seconds;
    f32 cycles;

    seconds = timeGetLastFrameSec();
    frame = _titleCursorTimer;
    cycles = 60.0f * seconds + (f32)frame;
    frame = (s32)cycles;
    _titleCursorTimer = (s16)(frame % 1000);
}

/* Cursor hook of the press-start entries (item ids 61-63, 66, 67):
 * pulse and select-highlight the entry sprites. */
extern "C" void menuTitleHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 itemID = sprite->mItemID;

    if (itemID < 66) {
        if (itemID >= 64) {
            return;
        }
        if (itemID < 61) {
            return;
        }
    } else if (itemID >= 68) {
        return;
    }
    menuTitleCursorAnime(window, sprite);
    menuTitleCursorShowSelected(window, sprite);
}

/* Show the entry the select cursor sits on, hide the others. */
extern "C" void menuTitleCursorShowSelected(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 ids[5] = {61, 62, 63, 66, 67};
    s16 cur = window->mCursor.y;
    s32 entry;

    for (entry = 0; entry < 5; entry++) {
        if (ids[entry] == sprite->mItemID) {
            break;
        }
    }

    if (entry == cur) {
        winSpriteSetDisp(sprite, 1);
    } else {
        winSpriteSetDisp(sprite, 0);
    }
}

/* Main title state machine: intro flash, map fly-around, mode select. */
extern "C" void menuTitleMainSub(void)
{
    s32 status;
    s32 running = 1;

    _menuTitleStatus = TITLE_STATE_0;
    _menuTitleSeFrame = _menuTitleTimer = 0.0f;
    _menuTitleSeCount = 0;
    _menuTitleSkipFlag = 0;
    menuTitlePlayBGM();
    menuTitlePlayBGMWait(1);
    if (_menuTitleStartStatus == 1) {
        _menuTitleStatus = TITLE_STATE_FLASH_BACK;
        _menuTitleStartStatus = 0;
        _menuTitleType = 1;
    } else {
        _menuTitleType = 0;
    }
    menuTitleLoadMap();

    do {
        switch (_menuTitleStatus) {
        case TITLE_STATE_0:
            FlashSystemSetLoopFlag(TITLE_FLASH, 0);
            FlashSystemSetPlayFrameLabel(TITLE_FLASH, "start", "end");
            FlashSystemSetNowFrameLabel(TITLE_FLASH, "start");
            _menuTitleStatus = TITLE_STATE_FLASH_START;
            break;

        case TITLE_STATE_FLASH_BACK:
            FlashSystemSetLoopFlag(TITLE_FLASH, 0);
            FlashSystemSetPlayFrameLabel(TITLE_FLASH, "back", "end");
            FlashSystemSetNowFrameLabel(TITLE_FLASH, "back");
            _menuTitleStatus = TITLE_STATE_FLASH_START;
            break;

        case TITLE_STATE_MAP:
            _menuTitleStatus = TITLE_STATE_QUIT;
            break;

        case TITLE_STATE_FLASH_START:
            _menuTitleStatus = TITLE_STATE_WAIT_FLASH;
            break;

        case TITLE_STATE_WAIT_FLASH:
            if (menuOpenCustom(TITLE_MENU_ID, windowGetActiveID(), 0, 0, 1, 0) >= 0) {
                FlashSystemSetLoopFlag(TITLE_FLASH, 0);
                FlashSystemSetPlayFrameLabel(TITLE_FLASH, "end", "window");
                _menuTitleStatus = TITLE_STATE_CONFIRM;
                _menuTitleTimer = 0.0f;
            }
            break;

        case TITLE_STATE_CONFIRM:
            while (_menuTitleTimer < 1.0f) {
                _menuTitleTimer += timeGetLastFrameSec();
                GSthreadSwitch();
            }
            FlashSystemSetPlayFrameLabel(TITLE_FLASH, "window", "window");
            FlashSystemSetNowFrameLabel(TITLE_FLASH, "window");
            _menuTitleStatus = TITLE_STATE_SELECT;
            break;

        case TITLE_STATE_SELECT_IN:
            break;

        case TITLE_STATE_SELECT:
            if (pokecoloGetRomVersion() == 1) {
                status = 5;
            } else {
                status = memcardTaskCall(1, 2, 0);
            }
            if (status == -1) {
                _menuTitleStatus = TITLE_STATE_WAIT_FLASH;
                FlashSystemSetPlayFrameLabel(TITLE_FLASH, "back", "end");
                FlashSystemSetNowFrameLabel(TITLE_FLASH, "back");
            } else {
                if (status != 3) {
                    savedataCreate(0, 0);
                }
                if (gamedatasaveGetStatus(0, 4) != 0U) {
                    menuTopSetInitCursor(1);
                } else {
                    menuTopSetInitCursor(0);
                }
                _menuTitleStatus = TITLE_STATE_MAP_FADE;
            }
            break;

        case TITLE_STATE_MAP_FADE:
            s32 selResult = _selectMenu();
            if (selResult < 0 || selResult == 4) {
                menuTitleCloseSelectAnime();
                _menuTitleStatus = TITLE_STATE_FLASH_BACK;
                _menuTitleAutoDemoCount = 0.0f;
            }
            break;

        case TITLE_STATE_QUIT:
            running = 0;
            break;
        }
    } while (running);
}

/* Load the four map models the fly-around shows. */
extern "C" void menuTitleLoadMapModels(void)
{
    menuTitleSetupModel(TITLE_MAP_RES_A, 0, 1, 0.0f);
    menuTitleSetupModel(TITLE_MAP_RES_B, 0, 1, 0.0f);
    menuTitleSetupModel(TITLE_MAP_RES_C, 0, 1, 0.0f);
    menuTitleSetupModel(TITLE_MAP_RES_D, 0, 1, 0.0f);
}

/* Point the fly-around camera at the map group. */
extern "C" void menuTitleSetupMapCamera(void)
{
    cameraSetType(4);
    cameraPlayAnime(floorGetCurrentGroupID(), TITLE_MAP_CAM_RES, 0.0f, 1);
}

/* Stream in the title map group and start its models and camera. */
extern "C" void menuTitleLoadMap(void)
{
    menuTitleModelCtor(floorGetCurrentGroupID(), TITLE_MAP_MODEL_RES, 1);
    menuTitleLoadMapModels();
    menuTitleSetupMapCamera();
}

/* Release the title map group's models. */
extern "C" void menuTitleUnloadMapModels(void)
{
    menuTitleModelDtor(floorGetCurrentGroupID(), TITLE_MAP_MODEL_RES);
}

/* Set up one fly-around model: read it, set its base frame and rate, and
 * start it at 60fps. */
extern "C" void menuTitleSetupModel(u32 resourceID, s16 animationIndex, u8 animationType, f32 startFrame)
{
    _GSmodel *model;

    model = (_GSmodel *)GSresGetResourceAllGroup(resourceID);
    if (model == 0) {
        return;
    }
    GSmodelSetAnimIndex(model, animationIndex);
    GSmodelSetAnimFrame(model, startFrame);
    GSmodelSetAnimRate(model, 0.5f);
    GSmodelGetAnimFrameCount(model, (s32)animationIndex);
    if (animationType != 0) {
        GSmodelSetAnimType(model, 1);
    } else {
        GSmodelSetAnimType(model, 0);
    }
    GSmodelStartAnimation(model);
    GSmodelSetForceUseFractionalFramesFlag(model, 1);
    GSmodelSet60fpsAnimFlag(model, 1);
    GSmodelSetVisibility(model, 1);
    GSmodelSetBoundCheck(model, 0);
}

/* Drop a fly-around model holder once the model in it is no longer needed. */
static inline void menuTitleModelFree(WorldMapModel *model)
{
    if (model != 0) {
        delete model;
    }
}

/* Read the requested map model in through a throw-away holder. */
extern "C" void menuTitleModelCtor(u32 group, u32 modelID, bool visible)
{
    WorldMapModel *model = new WorldMapModel;

    model->SetModel(group, modelID, visible);
    menuTitleModelFree(model);
}

/* Detach the map model through a throw-away holder. */
extern "C" void menuTitleModelDtor(u32 group, u32 modelID)
{
    WorldMapModel *model = new WorldMapModel;

    model->UnloadModel(group, modelID);
    menuTitleModelFree(model);
}

/* Wait until the title BGM stream has faded out (wait != 0), or just report
 * whether it is still playing (wait == 0). */
extern "C" u8 menuTitlePlayBGMWait(u8 wait)
{
    f32 elapsed;

    if (wait == 0) {
        return GSsndIsPlayingStream(TITLE_BGM_ID);
    }
    elapsed = 0.0f;
    for (;;) {
        if (GSsndIsPlayingStream(TITLE_BGM_ID) != 1) {
            elapsed = elapsed + timeGetLastFrameSec();
            if (elapsed >= 2.0f) {
                break;
            }
            GSthreadSwitch();
            continue;
        }
        break;
    }
    return 1;
}

/* Start the title BGM if it is not already running. */
extern "C" void menuTitlePlayBGM(void)
{
    if (soundGetBGMID() != TITLE_BGM_ID) {
        soundOpenPlay(TITLE_BGM_ID, 0, 255);
    }
    _menuTitleAutoDemoCount = 0.0f;
}

/* Play the mode-select flash movie from menu_play to menu_end, wait for the
 * end frame, then hand over to the select screen. */
s32 _selectMenu(void)
{
    u32 end;

    u32 now;

    FlashSystemSetPlayFrameLabel(TITLE_FLASH, "menu_play", "menu_end");
    FlashSystemSetLoopFlag(TITLE_FLASH, 0);
    FlashSystemSetNowFrameLabel(TITLE_FLASH, "menu_play");
    end = FlashSystemGetFrameFromLabel(TITLE_FLASH, "menu_end");
    do {
        now = FlashSystemGetNowFrame(TITLE_FLASH);
        GSthreadSwitch();
    } while (now < end);
    /* The select screen reports the chosen mode through its return value. */
    return menuTopSelectMode();
}

/* Play the select-menu return flash (back_play..back_end) and wait. */
extern "C" void menuTitleCloseSelectAnime(void)
{
    FlashSystemPlayAndWait(TITLE_FLASH, "back_play", "back_end");
}

/* Play the mode-select close flash, which starts and ends on menu_end. */
extern "C" void menuTitleOpenSelectAnime2(void)
{
    FlashSystemPlayAndWait(TITLE_FLASH, "menu_end", "menu_end");
}

/* Play the mode-select open flash (menu_play..menu_end) and wait. */
extern "C" void menuTitleOpenSelectAnime(void)
{
    FlashSystemPlayAndWait(TITLE_FLASH, "menu_play", "menu_end");
}

/* Play a UI sound effect. */
void _menuTitleSetSE(u32 id)
{
    GSsndPlay(id, 0, 0);
}

/* Free the select-menu resources and close its windows; called when the
 * mode select screen ends. */
extern "C" void menuTitleExit(void)
{
    GSfsysWait(TITLE_SELECT_RES);
    if (menuTopIsRetainingResources() == 0) {
        GSfsysFree(TITLE_SELECT_RES);
        GSresFreeGroup(TITLE_SELECT_RES);
    }
    _menuTitleClose2(TITLE_MENU_ID, 2);
    _menuTitleClose2(TITLE_SELECT_MENU_ID_1, 2);
    _menuTitleClose2(TITLE_SELECT_MENU_ID_2, 2);
    winMsgClose(1);
    menuTitleBattlePreReadCheck();
}

/* Free the battle-select resources. */
extern "C" void menuTitleBattleSelectFree(void)
{
    GSfsysWait(TITLE_SELECT_RES);
    GSfsysFree(TITLE_SELECT_RES);
    GSresFreeGroup(TITLE_SELECT_RES);
}

/* Stop the title's loader thread if one is running. */
extern "C" void menuTitleBattlePreReadCheck(void)
{
    if (_gsthread_ptr_fight_reg_wzx != 0) {
        GSthreadTerminate(_gsthread_ptr_fight_reg_wzx);
        _gsthread_ptr_fight_reg_wzx = 0;
    }
}

/* Wait until the loader thread clears its busy flag. */
extern "C" void menuTitlePreReadWait(void)
{
    while (_gsthread_ptr_fight_reg_wzx != 0) {
        GSthreadSwitch();
    }
}

/* Title main loop: fade the BGM into the select menu once the map state
 * asked for it. */
extern "C" void menuTitleMain(void)
{
    for (;;) {
        if ((s32)_menuTitleStatus == (s32)TITLE_STATE_MAP) {
            soundOpenPlayFadeOutIn(TITLE_BGM_ID, TITLE_BGM_FADE_TIME, 0, 127);
            floorLink(TITLE_UNLINK_ROOM_ID, 0);
            _menuTitleStatus = TITLE_STATE_QUIT;
            continue;
        }
        GSthreadSwitch();
    }
}

/* Battle-select startup: read the waza select menu, fill the encounter data
 * and start the loader thread that prereads the fight data. */
extern "C" void menuTitleStartBattleSelect(void)
{
    void *encounter;

    _menuTitleDiskCoverOpen = 0;
    _menuTitleAutoDemoCount = 0.0f;
    _menuTitleFrameSec = 0.0f;
    menuTitleReadWazaSelectMenu();
    encounter = fightEncountDataBiosGetPtr(14);
    fightEncountDataBiosSetDiskId(encounter, 0);
    fightEncountDataBiosSetFightTrainerDataId(encounter, 1, 2);
    _CreateThread();
    if (GSfloorIsPop() == 0) {
        TitleSelectWork *select;
        s32 slot = _menuTitleWorkPat;

        _menuTitleStatus = TITLE_STATE_0;
        _menuTitleWorkPat = slot + 1;
        if (_menuTitleWorkPat >= 4) {
            _menuTitleWorkPat = 0;
        }
        select = &_menuTitleWork;
        select->mRows[0][0] = 300.0f;
        select->mRows[1][0] = -300.0f;
        select->mRows[2][0] = 0.0f;
        select->mRows[3][0] = 0.0f;
        select->mRows[0][1] = 400.0f;
        select->mRows[1][1] = -400.0f;
        select->mRows[2][1] = 0.0f;
        select->mRows[3][1] = 0.0f;
        select->mRows[0][2] = 500.0f;
        select->mRows[1][2] = -500.0f;
        select->mRows[2][2] = 0.0f;
        select->mRows[3][2] = 0.0f;
        select->mPages[0] = -1;
        select->mPages[1] = -1;
        select->mPages[2] = -1;
        select->mPages[3] = -1;
        select->mPages[4] = -1;
        menuToolInit();
    }
}

/* Start the title loader thread that prereads the fight data. */
void _CreateThread(void)
{
    _gsthread_ptr_fight_reg_wzx = GSthreadCreate(20, GSfloorGetCurrentFloorGroupID(), 8192, 1, 1,
        _menuTitlePreReadFightRegWzxData);
}

/* Body of the preread thread: hand the BGM over, wait for the map data to be
 * streamed in, then load the fight registry and clear the thread id. */
void _menuTitlePreReadFightRegWzxData(void)
{
    for (;;) {
        if (soundGetBGMID() == TITLE_BGM_ID) {
            break;
        }
        GSthreadSwitch();
    }
    menuTitlePlayBGMWait(1);
    GSfsysPreReadWait(roomDataGetFsysGidForLanguage(floorDataBiosGetPtr(TITLE_PREREAD_ROOM_ID)));
    fightRegWzxLoadAll(-1);
    _gsthread_ptr_fight_reg_wzx = 0;
}

/* Make sure the select menu resource group is in memory. */
extern "C" void menuTitleReadWazaSelectMenu(void)
{
    if (GSresGetResourceAllGroup(TITLE_WAZA_SELECT_RES) == 0) {
        GSfsysReadWait(TITLE_SELECT_RES);
    }
}

/* Record the mode select entry the player settled on. */
extern "C" void menuTitleSetStartStatus(s32 entry)
{
    _menuTitleStartStatus = entry;
}

/* Close a menu that is still open. */
void _menuTitleClose2(s32 id, u32 flag)
{
    s32 closeFlags;
    s32 menuID;

    menuID = id;
    closeFlags = flag;
    if (menuIsOpen(menuID) == 1) {
        menuCloseCustom(menuID, closeFlags, 1);
    }
}

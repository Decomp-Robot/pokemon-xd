#include <winSprite.h>
#include <WorldMap.h>

#pragma force_active on

// C++-linkage engine accessors (defined in other units).
extern void *deckDataVirtual_GetPtr(u16 id);

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

// States of menuTitleMainSub, kept in titleState.
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
/* The two further window menus the mode-select screen leaves open behind it;
 * nothing recovered so far says which screen part each one belongs to. */
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

#ifdef __cplusplus
extern "C" {
#endif

/* Title-screen work variables. */
extern s32 titleEntrySelected;      // mode select entry recorded by menuTitleSetState
extern s32 titleBgmWaitCounter;     // battle-select menu open attempts
extern u32 titleState;              // menuTitleMainSub state machine step
extern f32 titleFlashWaitTime;      // seconds left of the flash fade hold
extern f32 titleFrameSeconds;       // last frame time, for the map fade
extern f32 titleFlashFadeAlpha;     // accumulated flash fade seconds
extern u32 titleFlashHoldFrames;    // frames the flash end holds
extern u32 titleEntryConfirmed;     // set when a select entry is confirmed
extern f32 titleCursorFade;         // press-start prompt fade 0..1
extern f32 titleBgmFadeSeconds;     // bgm fade-out seconds accumulated
extern u32 titlePrereadThreadId;    // preread thread id, 0 when none runs
extern u32 titleInitFlag;           // cleared when the title flow initializes
extern s16 titleCursorFrame;       // press-start pulse frame, advanced % 1000

/* rodata: flash frame labels and the press-start item id table. The frame
 * labels are named after the labels themselves ("menu_play"/"menu_end" for
 * the mode select movie, "back_play"/"back_end" for the confirm movie). */
extern const char cLabelEnd[4];      // "end"
extern const char cLabelStart[6];    // "start"
extern const char cLabelBack[5];     // "back"
extern const char cLabelWindow[7];   // "window"
extern const char cLabelMenuPlay[10]; // "menu_play"
extern const char cLabelMenuEnd[9];  // "menu_end"
extern const char cLabelBackPlay[10]; // "back_play"
extern const char cLabelBackEnd[9];  // "back_end"
extern const s32 titleCursorItemIDs[5]; // 0x802EBBB0

/* sdata2 constants shared with the title flow. */
extern const f32 cHalfSecond;       // 0.5f
extern const f32 cZeroFloat;        // 0.0f
extern const f32 cOneFloat;         // 1.0f
extern const f32 cSixtyHalfFloat;   // 60.5f
extern const f32 cPiFloat;          // pi
extern const f32 cThirtyFloat;      // 30.0f
extern const f32 cAlphaBaseFloat;   // 128.0f
extern const f32 cSixtyFloat;       // 60.0f
extern const f32 cTwoFloat;         // 2.0f
extern const f32 cPos300Float;      // 300.0f
extern const f32 cNeg300Float;      // -300.0f
extern const f32 cPos400Float;      // 400.0f
extern const f32 cNeg400Float;      // -400.0f
extern const f32 cPos500Float;      // 500.0f
extern const f32 cNeg500Float;      // -500.0f

/* Mode-select work block the entry menus read back (bss 0x8043D2A8). */
extern u8 titleSelectWork[];

/* Battle-select slot the next entry uses, 0..3 (sdata 0x804E80B8). */
extern s32 titleSelectSlot;

#ifdef __cplusplus
}
#endif

extern "C" {
void GSthreadSwitch(void);
void GSthreadTerminate(u32 threadId);
u32 GSthreadCreate(s32 priority, u32 groupID, u32 stackSize, s32 usesFPU, s32 closeProtected,
    void (*entry)(void));
s32 windowGetActiveID(void);
tagWINDOW_WORK *windowSearchID(s32 id);
struct tagWINDOW_KEY *windowGetKeyState(void);
s32 menuOpenCustom(s32 menuID, s32 windowID, ...);
signed char fadeCheck(s32 id);
void dbgMenuSetOpenFlag(s32 flag);
void *menuDataBiosGetPtr(s32 menuID);
void GSsndPlay(s32 id, s32 a, s32 b);
u32 soundGetBGMID(void);
void soundOpenPlay(u32 soundID, s32 a, s32 b);
void soundOpenPlayFadeOutIn(u32 soundID, u32 next, s32 a, s32 b);
u8 GSsndIsPlayingStream(u32 streamID);
f32 timeGetLastFrameSec(void);
double sin(double x);
void *GSresGetResourceAllGroup(void *group);
void GSresFreeGroup(u32 group);
void GSfsysFree(u32 id);
void GSfsysPreReadWait(u32 id);
void GSfsysReadWait(u32 id);
void *roomIDGetMapData(s32 roomID);
u32 roomDataGetFsysGidForLanguage(void *mapData);
void fightRegWzxLoadAll(s32 id);
void GSmodelSetAnimFrame(void *model, f32 frame);
void GSmodelSetAnimRate(void *model, f32 rate);
s32 GSmodelGetAnimFrameCount(void *model, s16 anim);
void GSmodelSetAnimType(void *model, s32 type);
void GSmodelStartAnimation(void *model);
void GSmodelSetForceUseFractionalFramesFlag(void *model, s32 flag);
void GSmodelSet60fpsAnimFlag(void *model, s32 flag);
void GSmodelSetVisibility(void *model, s32 visible);
void GSmodelSetBoundCheck(void *model, s32 flag);
/* Selects the animation a map model plays. */
void zz_00f399c_(void *model, s16 id);
u32 GSfsysWait(u32 id);
/* Returns the menuTop work struct's "a screen transition is running" flag,
 * which gates whether the mode-select resources may be freed. */
u8 fn_8002FC70(void);
void winMsgClose(s32 id);
u8 cameraSetType(s32 type);
s32 getCurrentMapGroupId(void);
/* Loads a camera resource for a map group. */
void zz_01971c8_(s32 group, void *res, s32 flag, f32 rate);
void savedataCreate(s32 a, s32 b);
s32 memcardTaskCall(s32 a, s32 b, s32 c);
/* Tests whether a save slot holds data. */
u32 zz_0157d74_(s32 a, s32 b);
/* Records whether the mode select screen may keep running. */
void zz_0030324_(s32 a);
/* Returns the entry the mode select screen settled on. */
s32 zz_002f718_(void);
/* Reports whether a memcard task is still in flight. */
s32 zz_005c228_(void);
void floorUnlinkWithSE(s32 a, s32 b);
/* Returns the thread group id of the floor group on top of the stack. */
u32 floorGetStackGroupID(void);
/* Reports whether the preread loader is still busy. */
u8 zz_0125b44_(void);
void _HSD_AObjForgetMemory(void);
u8 menuIsOpen(s32 id);
void menuCloseCustom(s32 id, s32 a, s32 b);
/* Sets the battle CD id on the encounter data the select screen hands on. */
void zz_battle_set_battle_cd_id(void *data, s32 id);
void fightEncountDataBiosSetFightTrainerDataId(void *data, s32 player, u16 id);
void FlashSystemSetPlayFrameLabel(u32 flash, const char *start, const char *end);
void FlashSystemSetLoopFlag(u32 flash, s32 flag);
void FlashSystemSetNowFrameLabel(u32 flash, const char *label);
s32 FlashSystemGetFrameFromLabel(u32 flash, const char *label);
s32 FlashSystemGetNowFrame(u32 flash);
void FlashSystemPlayAndWait(u32 flash, const char *start, const char *end);
void winSpriteSetDisp(tagSPRITE_WORK *sprite, s32 disp);
}

/* Functions of this module. */
extern "C" {
void menuTitleLoadMap(void);
void menuTitleSetupModel(void *res, s32 anim, u8 type, f32 frame);

void menuTitleModelCtor(unsigned int group, unsigned int modelID, bool visible);
void menuTitleModelDtor(unsigned int group, unsigned int modelID);
u8 menuTitleWaitBgmFade(u8 wait);
void menuTitlePlayBGM(void);
void menuTitleOpenSelectAnime2(void);
void menuTitlePlaySelectOpenFlash(void);
void menuTitleCleanupSelect(void);
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
void menuTitleUpdate(void);
void menuTitleButton(tagWINDOW_WORK *window);
void menuTitleCursorHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTitleMainSub(void);
void menuTitlePlaySe(s32 id);
void menuTitleStopThread(void);
void menuTitleSetState(s32 entry);
void menuTitleCloseMenuIfOpen(s32 id, s32 flag);

void menuTitleOpenSelectAnime(void);
}

/* Functions of this module that are compiled as C++. */
void menuTitleNormalButton(tagWINDOW_WORK *window);
s32 selectMenu(void);
void menuTitlePreReadFightRegWzxData(void);
void menuTitleCreateThread(void);

/* -------------------------------------------------------------------------
 * Title screen flow
 * ---------------------------------------------------------------------- */

/* Mode-select entry defaults the battle select screen starts from. */
struct TitleSelectWork {
    u8 _00[0x10];
    /* +0x10..+0x3C: twelve floats in four rows of three - the 300/400/500
     * distances, their negation, and two all-zero rows. What they select is
     * not established by the callers seen so far. */
    f32 mRows[4][3];
    s16 mPages[5]; // +0x40..+0x48: entry pages, -1 until filled
};

/* Open the title window over the currently active one. */
extern "C" void menuTitleOpen(void)
{
    menuTitleLoadMap();
    menuOpenCustom(TITLE_MENU_ID, windowGetActiveID(), 0, 0, 0, 0);
}

/* Selected entry of the mode-select menu. */
extern "C" s16 menuTitleGetSelect(s32 entry)
{
    TitleSelectWork *select = (TitleSelectWork *)titleSelectWork;

    return select->mPages[entry];
}

extern "C" void menuTitleSetSelect(s32 entry, s16 value)
{
    TitleSelectWork *select = (TitleSelectWork *)titleSelectWork;

    select->mPages[entry] = value;
}

/* Per-frame housekeeping of the title window: pulse the press-start prompt
 * and, once the map fade has run long enough, move on to the map state. */
extern "C" void menuTitleUpdate(void)
{
    s32 state;

    menuTitleCursorAnimeMain();
    titleFrameSeconds = timeGetLastFrameSec();
    titleCursorFade += titleFrameSeconds * cHalfSecond;
    if (titleCursorFade >= cOneFloat) {
        titleCursorFade = cZeroFloat;
    }

    state = (s32)titleState;
    if (state < 30) {
        return;
    }
    if (state == TITLE_STATE_MAP_FADE) {
        titleCursorFade = cZeroFloat;
        return;
    }
    if (state == TITLE_STATE_SELECT) {
        return;
    }
    if (zz_005c228_() == 1) {
        return;
    }
    titleBgmFadeSeconds += titleFrameSeconds;
    if (!(titleBgmFadeSeconds >= cSixtyHalfFloat)) {
        return;
    }
    titleBgmFadeSeconds = cZeroFloat;
    state = (s32)titleState;
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
    titleState = TITLE_STATE_MAP;
    menuTitleSetState(TITLE_STATE_0);
}

/* Button hook of the title window: run the standard handler once the intro
 * flash has played through. */
extern "C" void menuTitleButton(tagWINDOW_WORK *window)
{
    if (fadeCheck(0) != 0) {
        return;
    }
    dbgMenuSetOpenFlag(1);
    u32 end = (u32)FlashSystemGetFrameFromLabel(TITLE_FLASH, cLabelEnd);
    if ((u32)FlashSystemGetNowFrame(TITLE_FLASH) >= end) {
        menuTitleNormalButton(window);
    }
}

/* Standard title button handling: confirm on Start/A. */
void menuTitleNormalButton(tagWINDOW_WORK *window)
{
    struct tagWINDOW_KEY *key;

    if ((s32)titleState >= TITLE_STATE_MAP_FADE) {
        return;
    }
    if (window == 0) {
        return;
    }
    menuDataBiosGetPtr(window->mMenuID);
    key = windowGetKeyState();
    if ((key->mKeyTrg & TITLE_ACCEPT_KEYS) == 0) {
        return;
    }
    if (window->mAccept == 0) {
        menuTitlePlaySe(TITLE_SE_CONFIRM);
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
        s32 frame = titleCursorFrame;

        phase = frame % 60;
        sprite->mColor = -1;
        if (phase < 30) {
            angle = cPiFloat * ((f32)phase / cThirtyFloat);
            alpha = cAlphaBaseFloat * (f32)sin(angle);
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
    frame = titleCursorFrame;
    cycles = cSixtyFloat * seconds + (f32)frame;
    frame = (s32)cycles;
    titleCursorFrame = (s16)(frame % 1000);
}

/* Cursor hook of the press-start entries (item ids 61-63, 66, 67):
 * pulse and select-highlight the entry sprites. */
extern "C" void menuTitleCursorHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
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
    s32 ids[5];
    s16 cur = window->cursor;
    s32 entry;
    s32 i;

    /* Snapshot the item id table, then find the entry the sprite belongs to. */
    for (i = 0; i < 5; i++) {
        ids[i] = titleCursorItemIDs[i];
    }
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

    titleState = TITLE_STATE_0;
    titleFlashFadeAlpha = titleFlashWaitTime = cZeroFloat;
    titleFlashHoldFrames = 0;
    titleInitFlag = 0;
    menuTitlePlayBGM();
    menuTitleWaitBgmFade(1);
    if (titleEntrySelected == 1) {
        titleState = TITLE_STATE_FLASH_BACK;
        titleEntrySelected = 0;
        titleEntryConfirmed = 1;
    } else {
        titleEntryConfirmed = 0;
    }
    menuTitleLoadMap();

    do {
        switch (titleState) {
        case TITLE_STATE_0:
            FlashSystemSetLoopFlag(TITLE_FLASH, 0);
            FlashSystemSetPlayFrameLabel(TITLE_FLASH, cLabelStart, cLabelEnd);
            FlashSystemSetNowFrameLabel(TITLE_FLASH, cLabelStart);
            titleState = TITLE_STATE_FLASH_START;
            break;

        case TITLE_STATE_FLASH_BACK:
            FlashSystemSetLoopFlag(TITLE_FLASH, 0);
            FlashSystemSetPlayFrameLabel(TITLE_FLASH, cLabelBack, cLabelEnd);
            FlashSystemSetNowFrameLabel(TITLE_FLASH, cLabelBack);
            titleState = TITLE_STATE_FLASH_START;
            break;

        case TITLE_STATE_MAP:
            titleState = TITLE_STATE_QUIT;
            break;

        case TITLE_STATE_FLASH_START:
            titleState = TITLE_STATE_WAIT_FLASH;
            break;

        case TITLE_STATE_WAIT_FLASH:
            if (menuOpenCustom(TITLE_MENU_ID, windowGetActiveID(), 0, 0, 1, 0) >= 0) {
                FlashSystemSetLoopFlag(TITLE_FLASH, 0);
                FlashSystemSetPlayFrameLabel(TITLE_FLASH, cLabelEnd, cLabelWindow);
                titleState = TITLE_STATE_CONFIRM;
                titleFlashWaitTime = cZeroFloat;
            }
            break;

        case TITLE_STATE_CONFIRM:
            while (titleFlashWaitTime < cOneFloat) {
                titleFlashWaitTime += timeGetLastFrameSec();
                GSthreadSwitch();
            }
            FlashSystemSetPlayFrameLabel(TITLE_FLASH, cLabelWindow, cLabelWindow);
            FlashSystemSetNowFrameLabel(TITLE_FLASH, cLabelWindow);
            titleState = TITLE_STATE_SELECT;
            break;

        case TITLE_STATE_SELECT_IN:
            break;

        case TITLE_STATE_SELECT:
            if (zz_005c228_() == 1) {
                status = 5;
            } else {
                status = memcardTaskCall(1, 2, 0);
            }
            if (status == -1) {
                titleState = TITLE_STATE_WAIT_FLASH;
                FlashSystemSetPlayFrameLabel(TITLE_FLASH, cLabelBack, cLabelEnd);
                FlashSystemSetNowFrameLabel(TITLE_FLASH, cLabelBack);
            } else {
                if (status != 3) {
                    savedataCreate(0, 0);
                }
                if (zz_0157d74_(0, 4) != 0U) {
                    zz_0030324_(1);
                } else {
                    zz_0030324_(0);
                }
                titleState = TITLE_STATE_MAP_FADE;
            }
            break;

        case TITLE_STATE_MAP_FADE:
            s32 selResult = selectMenu();
            if (selResult < 0 || selResult == 4) {
                menuTitleOpenSelectAnime();
                titleState = TITLE_STATE_FLASH_BACK;
                titleBgmFadeSeconds = cZeroFloat;
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
    menuTitleSetupModel((void *)TITLE_MAP_RES_A, 0, 1, cZeroFloat);
    menuTitleSetupModel((void *)TITLE_MAP_RES_B, 0, 1, cZeroFloat);
    menuTitleSetupModel((void *)TITLE_MAP_RES_C, 0, 1, cZeroFloat);
    menuTitleSetupModel((void *)TITLE_MAP_RES_D, 0, 1, cZeroFloat);
}

/* Point the fly-around camera at the map group. */
extern "C" void menuTitleSetupMapCamera(void)
{
    cameraSetType(TITLE_CAM_TYPE_MAP);
    zz_01971c8_(getCurrentMapGroupId(), (void *)TITLE_MAP_CAM_RES, 1, cZeroFloat);
}

/* Stream in the title map group and start its models and camera. */
extern "C" void menuTitleLoadMap(void)
{
    menuTitleModelCtor(getCurrentMapGroupId(), TITLE_MAP_MODEL_RES, 1);
    menuTitleLoadMapModels();
    menuTitleSetupMapCamera();
}

/* Release the title map group's models. */
extern "C" void menuTitleUnloadMapModels(void)
{
    menuTitleModelDtor(getCurrentMapGroupId(), TITLE_MAP_MODEL_RES);
}

/* Set up one fly-around model: read it, set its base frame and rate, and
 * start it at 60fps. */
extern "C" void menuTitleSetupModel(void *res, s32 anim, u8 type, f32 frame)
{
    void *model;

    model = GSresGetResourceAllGroup(res);
    if (model == 0) {
        return;
    }
    zz_00f399c_(model, anim);
    GSmodelSetAnimFrame(model, frame);
    GSmodelSetAnimRate(model, cHalfSecond);
    GSmodelGetAnimFrameCount(model, anim);
    if (type != 0) {
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

/* A fly-around map model together with its offscreen texture. */
class TitleMapModel : public WorldMapModel {
public:
    TitleMapModel() { mOffscreenTexture = 0; }
    ~TitleMapModel() { if (mOffscreenTexture != 0) { releaseOffscreenData(); } }
};

/* Drop a fly-around model holder once the model in it is no longer needed. */
static inline void menuTitleModelFree(TitleMapModel *model)
{
    if (model != 0) {
        delete model;
    }
}

/* Read the requested map model in through a throw-away holder. */
extern "C" void menuTitleModelCtor(u32 group, u32 modelID, bool visible)
{
    TitleMapModel *model = new TitleMapModel;

    model->SetModel(group, modelID, visible);
    menuTitleModelFree(model);
}

/* Detach the map model through a throw-away holder. */
extern "C" void menuTitleModelDtor(u32 group, u32 modelID)
{
    TitleMapModel *model = new TitleMapModel;

    model->UnloadModel(group, modelID);
    menuTitleModelFree(model);
}

/* Wait until the title BGM stream has faded out (wait != 0), or just report
 * whether it is still playing (wait == 0). */
extern "C" u8 menuTitleWaitBgmFade(u8 wait)
{
    f32 elapsed;

    if (wait == 0) {
        return GSsndIsPlayingStream(TITLE_BGM_ID);
    }
    elapsed = cZeroFloat;
    for (;;) {
        if (GSsndIsPlayingStream(TITLE_BGM_ID) != 1) {
            elapsed = elapsed + timeGetLastFrameSec();
            if (elapsed >= cTwoFloat) {
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
    titleBgmFadeSeconds = cZeroFloat;
}

/* Play the mode-select flash movie from menu_play to menu_end, wait for the
 * end frame, then hand over to the select screen. */
s32 selectMenu(void)
{
    s32 end;

    s32 now;

    FlashSystemSetPlayFrameLabel(TITLE_FLASH, cLabelMenuPlay, cLabelMenuEnd);
    FlashSystemSetLoopFlag(TITLE_FLASH, 0);
    FlashSystemSetNowFrameLabel(TITLE_FLASH, cLabelMenuPlay);
    end = FlashSystemGetFrameFromLabel(TITLE_FLASH, cLabelMenuEnd);
    do {
        now = FlashSystemGetNowFrame(TITLE_FLASH);
        GSthreadSwitch();
    } while ((u32)now < (u32)end);
    /* The select screen reports the chosen mode through its return value. */
    return zz_002f718_();
}

/* Play the select-menu open flash (back_play..back_end) and wait. */
extern "C" void menuTitleOpenSelectAnime(void)
{
    FlashSystemPlayAndWait(TITLE_FLASH, cLabelBackPlay, cLabelBackEnd);
}

/* Play the mode-select close flash, which starts and ends on menu_end. */
extern "C" void menuTitleOpenSelectAnime2(void)
{
    FlashSystemPlayAndWait(TITLE_FLASH, cLabelMenuEnd, cLabelMenuEnd);
}

/* Play the mode-select open flash (menu_play..menu_end) and wait. */
extern "C" void menuTitlePlaySelectOpenFlash(void)
{
    FlashSystemPlayAndWait(TITLE_FLASH, cLabelMenuPlay, cLabelMenuEnd);
}

/* Play a UI sound effect. */
extern "C" void menuTitlePlaySe(s32 id)
{
    GSsndPlay(id, 0, 0);
}

/* Free the select-menu resources and close its windows; called when the
 * mode select screen ends. */
extern "C" void menuTitleCleanupSelect(void)
{
    GSfsysWait(TITLE_SELECT_RES);
    if (fn_8002FC70() == 0) {
        GSfsysFree(TITLE_SELECT_RES);
        GSresFreeGroup(TITLE_SELECT_RES);
    }
    menuTitleCloseMenuIfOpen(TITLE_MENU_ID, 2);
    menuTitleCloseMenuIfOpen(TITLE_SELECT_MENU_ID_1, 2);
    menuTitleCloseMenuIfOpen(TITLE_SELECT_MENU_ID_2, 2);
    winMsgClose(1);
    menuTitleStopThread();
}

/* Free the battle-select resources. */
extern "C" void menuTitleBattleSelectFree(void)
{
    GSfsysWait(TITLE_SELECT_RES);
    GSfsysFree(TITLE_SELECT_RES);
    GSresFreeGroup(TITLE_SELECT_RES);
}

/* Stop the title's loader thread if one is running. */
extern "C" void menuTitleStopThread(void)
{
    if (titlePrereadThreadId != 0) {
        GSthreadTerminate(titlePrereadThreadId);
        titlePrereadThreadId = 0;
    }
}

/* Wait until the loader thread clears its busy flag. */
extern "C" void menuTitlePreReadWait(void)
{
    while (titlePrereadThreadId != 0) {
        GSthreadSwitch();
    }
}

/* Title main loop: fade the BGM into the select menu once the map state
 * asked for it. */
extern "C" void menuTitleMain(void)
{
    for (;;) {
        if ((s32)titleState == (s32)TITLE_STATE_MAP) {
            soundOpenPlayFadeOutIn(TITLE_BGM_ID, TITLE_BGM_FADE_TIME, 0, 127);
            floorUnlinkWithSE(TITLE_UNLINK_ROOM_ID, 0);
            titleState = TITLE_STATE_QUIT;
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

    titleBgmWaitCounter = 0;
    titleBgmFadeSeconds = cZeroFloat;
    titleFrameSeconds = cZeroFloat;
    menuTitleReadWazaSelectMenu();
    encounter = deckDataVirtual_GetPtr(14);
    zz_battle_set_battle_cd_id(encounter, 0);
    fightEncountDataBiosSetFightTrainerDataId(encounter, 1, 2);
    menuTitleCreateThread();
    if (zz_0125b44_() == 0) {
        TitleSelectWork *select;
        s32 slot = titleSelectSlot;

        titleState = TITLE_STATE_0;
        titleSelectSlot = slot + 1;
        if (titleSelectSlot >= 4) {
            titleSelectSlot = 0;
        }
        select = (TitleSelectWork *)&titleSelectWork[0];
        select->mRows[0][0] = cPos300Float;
        select->mRows[1][0] = cNeg300Float;
        select->mRows[2][0] = cZeroFloat;
        select->mRows[3][0] = cZeroFloat;
        select->mRows[0][1] = cPos400Float;
        select->mRows[1][1] = cNeg400Float;
        select->mRows[2][1] = cZeroFloat;
        select->mRows[3][1] = cZeroFloat;
        select->mRows[0][2] = cPos500Float;
        select->mRows[1][2] = cNeg500Float;
        select->mRows[2][2] = cZeroFloat;
        select->mRows[3][2] = cZeroFloat;
        select->mPages[0] = -1;
        select->mPages[1] = -1;
        select->mPages[2] = -1;
        select->mPages[3] = -1;
        select->mPages[4] = -1;
        _HSD_AObjForgetMemory();
    }
}

/* Start the title loader thread that prereads the fight data. */
void menuTitleCreateThread(void)
{
    titlePrereadThreadId = GSthreadCreate(20, floorGetStackGroupID(), 8192, 1, 1,
        menuTitlePreReadFightRegWzxData);
}

/* Body of the preread thread: hand the BGM over, wait for the map data to be
 * streamed in, then load the fight registry and clear the thread id. */
void menuTitlePreReadFightRegWzxData(void)
{
    for (;;) {
        if (soundGetBGMID() == TITLE_BGM_ID) {
            break;
        }
        GSthreadSwitch();
    }
    menuTitleWaitBgmFade(1);
    GSfsysPreReadWait(roomDataGetFsysGidForLanguage(roomIDGetMapData(TITLE_PREREAD_ROOM_ID)));
    fightRegWzxLoadAll(-1);
    titlePrereadThreadId = 0;
}

/* Make sure the select menu resource group is in memory. */
extern "C" void menuTitleReadWazaSelectMenu(void)
{
    if (GSresGetResourceAllGroup((void *)TITLE_WAZA_SELECT_RES) == 0) {
        GSfsysReadWait(TITLE_SELECT_RES);
    }
}

/* Record the mode select entry the player settled on. */
extern "C" void menuTitleSetState(s32 entry)
{
    titleEntrySelected = entry;
}

/* Close a menu that is still open. */
extern "C" void menuTitleCloseMenuIfOpen(s32 id, s32 flag)
{
    if (menuIsOpen(id) == 1) {
        menuCloseCustom(id, flag, 1);
    }
}

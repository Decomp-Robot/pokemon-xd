
#include <GSmsg.h>
#include <flashsystem.h>
#include <fade.h>
#include <camera.h>
#include <pokecolo.h>
#include <savedata.h>
#include <gamedata.h>
#include <hero.h>
#include <menuItemBios.h>
#include <menuModel.h>
#include <menuNameEntry.h>
#include <menuPokemon.h>
#include <menuSub.h>
#include <peopleInfoBios.h>
#include <menu.h>
#include <menuTitle.h>
#include <menuTop.h>
#include <floor.h>
#include <winMsg.h>
#include <msgctrl.h>
extern "C" {
#include <sound.h>
}
#include <types.h>
#include <winSprite.h>

enum MenuTopPhase {
    MENU_TOP_BEGIN = 0,
    MENU_TOP_SELECT = 1,
    MENU_TOP_CONTINUE = 2,
    MENU_TOP_LOAD = 3,
    MENU_TOP_OPTIONS = 4,
    MENU_TOP_NEW_GAME = 5,
    MENU_TOP_DONE = 100
};

enum {
    MENU_TOP_SELECT_ID = 0x116,
    MENU_TOP_TITLE_ID = 0x11,
    MENU_TOP_FIRST_ITEM = 0x44,
    MENU_TOP_ENTRY_COUNT = 5,
    MENU_TOP_NEW_GAME_ENTRY = 0,
    MENU_TOP_CONTINUE_ENTRY = 1,
    MENU_TOP_BATTLE_ENTRY = 2,
    MENU_TOP_OPTIONS_ENTRY = 3,
    MENU_TOP_DEMO_ROM = 1,
    MENU_TOP_CONFIRM_NEW_GAME_SAVED = 0x8B,
    MENU_TOP_CONFIRM_NEW_GAME_EMPTY = 0x42D9,
    MENU_TOP_NO_CARD_MESSAGE = 0x82,
    MENU_TOP_NO_SAVE_MESSAGE = 0x98,
    MENU_TOP_START_SOUND = 8,
    MENU_TOP_NEW_GAME_FLOOR = 0xB6,
    MENU_TOP_DEMO_FLOOR = 0x47
};

enum {
    BATTLE_SELECT_LEFT = 0,
    BATTLE_SELECT_RIGHT = 1,
    BATTLE_SELECT_BACK = 2,
    BATTLE_SELECT_LEFT_CURSOR = 0x70,
    BATTLE_SELECT_RIGHT_CURSOR = 0x71,
    BATTLE_SELECT_BACK_CURSOR = 0x72,
    BATTLE_SELECT_LEFT_LABEL = 0x76,
    BATTLE_SELECT_RIGHT_LABEL = 0x77,
    MSG_BATTLE_SELECT_LEFT = 0x445A,
    MSG_BATTLE_SELECT_RIGHT = 0x4459,
};

struct MenuTopState {
    s32 cursor;
    s32 mode;
    u8 keepSelectResources;
    u8 padding[3];
    s32 battleSelectMode;
};

struct MenuTopBattleSelectFlashData {
    u32 itemIDs[5];
    char titleStart[12];
    char leftBackStart[12];
    char leftBackEnd[12];
    char rightBackStart[12];
    char rightBackEnd[12];
};

void _menuTopClose(long menuID);
void _menuTopClose2(long menuID, unsigned long flags);
void _menuTopExit(void);
void _menuTopExitPokemon(void);
void _menuTopNewGame(u16* name);
void _menuTopInitPokemon(s16 width, s16 height);
void _menuTopLoadPokemon(void);
s32 _menuTopLoadCheck(void);
void _menuTopSetLastCursor(void);
void _menuTopInit(void);
extern "C" void menuTopPlayToolBGM(void);

extern "C" {
MenuTopState _menuTopWork;
MENU_MODEL _menuTopMenuModel;
s32 _menuTopSelectCursor;
extern const MenuTopBattleSelectFlashData menuTopBattleSelectFlashData = {
    {0x44, 0x45, 0x46, 0x47, 0x48},
    "top_start",
    "1back_start",
    "1back_end",
    "2back_start",
    "2back_end",
};

void menuTopInit(void);
void menuTopExit(void);
s32 menuTopGetBattleSelectMode(void);
void menuTopSetBattleSelectMode(s32 mode);
u8 menuItemBiosGetSelectFlag(s32 menuID, s32 itemID);
void menuTitleSetSelect(s32 entry, s16 value);

s32 menuTopRunBattleSelect(void);
void menuTitleSetStartStatus(s32 state);


void menuTitleCloseSelectAnime(void);
void menuTitleOpenSelectAnime2(void);
void menuTitleOpenSelectAnime(void);
void menuTitleOpenMenu(void);
void menuTitleBattlePreReadCheck(void);
void menuTitlePreReadWait(void);
void menuTitleBattleSelectFree(void);

void cursorBiosInit(void);
void heroMoveSyncWithHero(void);
void pokecoloStartCountPlayTime(s32 enabled);
u8 memcardIsMemCardValid(void);
void menuTitleOption(void);

void winSeqSetMenu(long menuID, s32 sequence);
s32 menuTopSelect(void);
s32 _menuTopResID;
void scriptModelClose(s32 modelID);
s16 menuTitleGetSelect(s32 entry);
s32 pokecoloGetRomVersion(void);
extern const u32 _menuTopMsgTable[5];
extern const u32 _menuTopMsgTableBattleSelect[3];
void menuTopPrintItemMessage(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, s32 msgID);


void floorSetContinueMode(s32 mode);
void floorClearReturnPoint(void);
void floorReturnSavePoint(void);
void menuTitleUnloadMapModels(void);

void GSthreadSwitch(void);
void menuTitleLoadMap(void);
void menuTitlePlayBGM(void);
void menuColosseumBattle(s32 enabled);

}

#pragma dont_inline on
extern "C" {
void menuTopJumpVSMode(void)
{
    menuOpen(0x119, 0);
}
}
#pragma dont_inline reset

extern "C" {
void menuTopSelectCtrl(tagWINDOW_WORK* control)
{
    s32 state = control->mState;
    u8* ready = (u8*)control->mArg;

    switch (state) {
    case 0: {
        if (control->mFlag == 0) {
            if (*ready != 0) {
                winSeqSetMenu(control->mMenuID, 0x24);
            }
            control->mFlag = 1;
        }
        break;
    }

    case 3: {
        if (control->mFlag == 0) {
            winSeqSetMenu(control->mMenuID, 0x17B);
            control->mFlag = 1;
        }
        break;
    }
    }
}
}

extern "C" {
void menuTopLoadHook(void)
{
    s16 boundsA[2] = {250, 30};
    s16 boundsB[2] = {320, 400};
    f32 angle[3];
    angle[0] = 0.0f;
    angle[1] = -0.2f;
    angle[2] = 0.0f;

    if ((u8)menuModelCheck(&_menuTopMenuModel, 0) == 0) {
        menuModelSetSize(&_menuTopMenuModel, boundsA[0], boundsA[1],
                         boundsB[0], boundsB[1]);
        menuModelAdjustCamera(&_menuTopMenuModel);
        menuModelRender(&_menuTopMenuModel);
        menuModelSetAngle(&_menuTopMenuModel, angle);
        menuModelSetZoom(&_menuTopMenuModel, 1.5f);
    }
}
}

extern "C" {
void menuTopSelectMessageHook(tagWINDOW_WORK* window, tagSPRITE_WORK*)
{
    CURSOR cursor = window->mCursor;

    if (pokecoloGetRomVersion() != 1 && cursor.y >= 0) {
        s16 selection = menuTitleGetSelect(cursor.y);
        GSmsgPrint2(window->mLayerID, 0, -8,
                    0xFFFFFF00 | window->mRgba.mColorA,
                    _menuTopMsgTable[selection]);
    }
}
}

extern "C" {
void menuTopBattleSelectMessageHook(tagWINDOW_WORK* window, tagSPRITE_WORK*)
{
    CURSOR cursor = window->mCursor;

    if (cursor.y >= 0) {
        GSmsgPrint2(window->mLayerID, 0, -8,
                    0xFFFFFF00 | window->mRgba.mColorA,
                    _menuTopMsgTableBattleSelect[cursor.y]);
    }
}
}

extern "C" {
void menuTopBattleSelectHook(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    s16 itemID = sprite->mItemID;
    CURSOR cursor = window->mCursor;

    switch (itemID) {
    case BATTLE_SELECT_LEFT_CURSOR:
        if (cursor.y == BATTLE_SELECT_LEFT) {
            menuTitleCursorAnime(window, sprite);
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;

    case BATTLE_SELECT_RIGHT_CURSOR:
        if (cursor.y == BATTLE_SELECT_RIGHT) {
            menuTitleCursorAnime(window, sprite);
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;

    case BATTLE_SELECT_BACK_CURSOR:
        if (cursor.y == BATTLE_SELECT_BACK) {
            menuTitleCursorAnime(window, sprite);
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;

    case BATTLE_SELECT_LEFT_LABEL:
        menuTopPrintItemMessage(window, sprite, MSG_BATTLE_SELECT_LEFT);
        break;

    case BATTLE_SELECT_RIGHT_LABEL:
        menuTopPrintItemMessage(window, sprite, MSG_BATTLE_SELECT_RIGHT);
        break;
    }
}
}

extern "C" {
void menuTopPrintItemMessage(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, s32 msgID)
{
    u32 size = GSmsgGetMessageSize(msgID);
    tagMENU_ITEM_DATA* item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    s32 x = (item->mWidth - (s16)(size >> 16)) / 2;
    s32 y = (item->mHeight - (s16)(size & 0xFFFF)) / 2;

    GSmsgPrint2(window->mLayerID, x, y,
                0xFFFFFF00 | window->mRgba.mColorA, msgID);
}
}

extern "C" {
extern "C" s32 menuTopSelect(void)
{
    u8 playOpenFlash = 1;
    s32 phase = MENU_TOP_BEGIN;
    s32 running = 1;
    s32 answer;

    menuTopInit();
    _menuTopWork.mode = MENU_TOP_SELECT;
    _menuTopWork.keepSelectResources = 0;

    do {
        switch (phase) {
        case MENU_TOP_BEGIN:
            phase = MENU_TOP_SELECT;
            break;

        case MENU_TOP_SELECT: {
            _menuTopWork.mode = MENU_TOP_SELECT;
            s32 itemIDs[MENU_TOP_ENTRY_COUNT];
            for (s16 entry = 0; entry < MENU_TOP_ENTRY_COUNT; entry++) {
                itemIDs[entry] = menuTopBattleSelectFlashData.itemIDs[entry];
            }
            for (s16 entry = 0; entry < MENU_TOP_ENTRY_COUNT; entry++) {
                menuItemBiosSetSelectFlag(MENU_TOP_SELECT_ID, (s16)itemIDs[entry], 1);
            }

            if (pokecoloGetRomVersion() == MENU_TOP_DEMO_ROM) {
                menuItemBiosSetSelectFlag(MENU_TOP_SELECT_ID, MENU_TOP_FIRST_ITEM, 0);
                menuItemBiosSetSelectFlag(MENU_TOP_SELECT_ID, MENU_TOP_FIRST_ITEM + 2, 0);
                menuItemBiosSetSelectFlag(MENU_TOP_SELECT_ID, MENU_TOP_FIRST_ITEM + 3, 0);
            }

            s16 visibleEntry = 0;
            for (s16 entry = 0; entry < MENU_TOP_ENTRY_COUNT; entry++) {
                if (menuItemBiosGetSelectFlag(MENU_TOP_SELECT_ID, itemIDs[entry])) {
                    menuTitleSetSelect(visibleEntry++, entry);
                }
            }

            s32 selection = menuOpenCustom(MENU_TOP_SELECT_ID, windowGetActiveID(),
                                            &_menuTopWork.cursor, 0, 1, 1, &playOpenFlash);
            playOpenFlash = 0;
            if (selection >= 0) {
                selection = menuTitleGetSelect(selection);
                _menuTopWork.cursor = selection;
            }
            if (selection < 0) {
                _menuTopWork.cursor = -1;
            }

            if (selection == MENU_TOP_NEW_GAME_ENTRY) {
                phase = MENU_TOP_NEW_GAME;
            } else {
                switch (_menuTopWork.cursor) {
                case MENU_TOP_NEW_GAME_ENTRY:
                    break;
                case MENU_TOP_BATTLE_ENTRY:
                    if (menuTopRunBattleSelect() >= 0) {
                        _menuTopWork.keepSelectResources = 1;
                        phase = MENU_TOP_DONE;
                    } else {
                        _menuTopWork.keepSelectResources = 0;
                    }
                    continue;
                case MENU_TOP_CONTINUE_ENTRY:
                    phase = MENU_TOP_CONTINUE;
                    break;
                case MENU_TOP_OPTIONS_ENTRY:
                    _menuTopWork.mode = MENU_TOP_OPTIONS;
                    phase = MENU_TOP_OPTIONS;
                    break;
                default:
                    menuTitleSetStartStatus(1);
                    phase = MENU_TOP_DONE;
                    break;
                }
            }
            fadeCheck(1);
            break;
        }

        case MENU_TOP_NEW_GAME: {
            _menuTopClose(MENU_TOP_SELECT_ID);
            menuTitleCloseSelectAnime();

            if (pokecoloGetRomVersion() == MENU_TOP_DEMO_ROM) {
                answer = 0;
            } else {
                s32 message = gamedatasaveGetStatus(0, 4)
                    ? MENU_TOP_CONFIRM_NEW_GAME_SAVED : MENU_TOP_CONFIRM_NEW_GAME_EMPTY;
                winMsgOpen(2, message, 1, 1);
                _menuTopWork.mode = MENU_TOP_NEW_GAME;
                answer = menuSubOpenYesNo(0, 60, 166, true);
                winMsgClose(1);
            }

            if (answer == 0) {
                u16 name[20];
                menuTopExit();
                menuTitleBattlePreReadCheck();
                menuNameEntryOpen(0, 0);
                menuNameEntryGetLastName(name);
                if (name[0] != 0) {
                    cursorBiosInit();
                    _menuTopNewGame(name);
                    heroMoveSyncWithHero();
                    soundOpenPlay(MENU_TOP_START_SOUND, 1000, 255);
                    floorLink(MENU_TOP_NEW_GAME_FLOOR, 0);
                    phase = MENU_TOP_DONE;
                } else {
                    phase = MENU_TOP_SELECT;
                    menuTitleOpenMenu();
                    menuTitleOpenSelectAnime2();
                }
            } else {
                playOpenFlash = 1;
                phase = MENU_TOP_SELECT;
                menuTitleOpenSelectAnime();
            }
            break;
        }

        case MENU_TOP_CONTINUE:
            if (pokecoloGetRomVersion() == MENU_TOP_DEMO_ROM) {
                u16 name[20];
                GScharMakeFromSJIS(name, "MICHAEL");
                cursorBiosInit();
                menuPokemonInitWork();
                _menuTopNewGame(name);
                soundOpenPlay(MENU_TOP_START_SOUND, 1000, 255);
                floorLink(MENU_TOP_DEMO_FLOOR, 0);
                phase = MENU_TOP_DONE;
            } else {
                pokecoloStartCountPlayTime(0);
                if (gamedatasaveGetStatus(0, 4)) {
                    phase = MENU_TOP_LOAD;
                } else {
                    _menuTopClose(MENU_TOP_SELECT_ID);
                    menuTitleCloseSelectAnime();
                    if (!memcardIsMemCardValid()) {
                        winMsgOpen(2, MENU_TOP_NO_CARD_MESSAGE, 1, 0);
                    } else {
                        winMsgOpen(2, MENU_TOP_NO_SAVE_MESSAGE, 1, 0);
                    }
                    winMsgClose(1);
                    phase = MENU_TOP_SELECT;
                    menuTitleOpenMenu();
                    menuTitleOpenSelectAnime();
                }
            }
            break;

        case MENU_TOP_LOAD:
            _menuTopClose(MENU_TOP_SELECT_ID);
            menuTitleCloseSelectAnime();
            fadeSet(3, 0.5f);
            fadeCheck(1);
            _menuTopClose(MENU_TOP_TITLE_ID);
            _menuTopInitPokemon(640, 480);
            menuTitlePreReadWait();
            _menuTopLoadPokemon();
            if (_menuTopLoadCheck() == 0) {
                cursorBiosInit();
                menuTitleBattleSelectFree();
            } else {
                phase = MENU_TOP_SELECT;
                menuTitleOpenMenu();
                menuTitleOpenSelectAnime2();
                fadeSet(2, 0.5f);
            }
            break;

        case MENU_TOP_OPTIONS:
            _menuTopClose(MENU_TOP_SELECT_ID);
            menuTitleCloseSelectAnime();
            fadeSet(3, 0.5f);
            fadeCheck(1);
            _menuTopClose(MENU_TOP_TITLE_ID);
            menuTitleOption();
            fadeCheck(1);
            _menuTopWork.mode = MENU_TOP_SELECT;
            phase = MENU_TOP_SELECT;
            menuTitleOpenMenu();
            menuTitleOpenSelectAnime2();
            fadeSet(2, 0.5f);
            break;

        case MENU_TOP_DONE:
            _menuTopSetLastCursor();
            running = 0;
            break;
        }
    } while (running);

    menuTopExit();
    return _menuTopWork.cursor;
}
}

extern "C" {
u8 menuTopShouldKeepSelectResources(void)
{
    return _menuTopWork.keepSelectResources;
}
}

extern "C" {
s32 menuTopRunBattleSelect(void)
{
    const unsigned long flashID = 0x200A3000;
    u8 running = 1;
    const MenuTopBattleSelectFlashData* const flashData = &menuTopBattleSelectFlashData;

    menuTopSetBattleSelectMode(0);
    menuTopJumpVSMode();
    FlashSystemSetLoopFlag(flashID, 0);
    FlashSystemSetPlayFrameLabel(flashID, flashData->titleStart,
                                  "top_end");
    FlashSystemSetNowFrameLabel(flashID, flashData->titleStart);
    u32 endFrame = FlashSystemGetFrameFromLabel(flashID, "top_end");
    u32 nowFrame;
    do {
        nowFrame = FlashSystemGetNowFrame(flashID);
        GSthreadSwitch();
    } while (nowFrame < endFrame);
    menuTopPlayToolBGM();

    s32 result;
    do {
        s32 mode = menuTopGetBattleSelectMode();
        result = menuOpenCustom(0x118, windowGetActiveID(), &mode, 0, 1, 0);
        switch (result) {
        case 0: {
            _menuTopClose2(0x118, 2);
            _menuTopClose2(0x116, 2);
            _menuTopClose2(0x11, 2);
            menuTitleBattlePreReadCheck();
            menuColosseumBattle(1);
            tagWINDOW_WORK* window = windowSearchID(0x119);
            if (window != 0) {
                window->mOpenPriority = 0;
            }
            FlashSystemPlayAndWait(flashID, flashData->leftBackStart,
                                    flashData->leftBackEnd);
            break;
        }

        case 1: {
            _menuTopClose2(0x118, 2);
            _menuTopClose2(0x116, 2);
            _menuTopClose2(0x11, 2);
            menuTitleBattlePreReadCheck();
            menuColosseumBattle(0);
            tagWINDOW_WORK* window = windowSearchID(0x119);
            if (window != 0) {
                window->mOpenPriority = 0;
            }
            FlashSystemPlayAndWait(flashID, flashData->rightBackStart,
                                    flashData->rightBackEnd);
            break;
        }

        default: {
            result = -1;
            fadeSet(3, 0.5f);
            fadeCheck(1);
            _menuTopClose(0x119);
            _menuTopClose(0x118);
            menuTitleLoadMap();
            menuOpenCustom(0x11, windowGetActiveID(), 0, 0, 0, 0);
            menuTitleOpenSelectAnime();
            menuTitlePlayBGM();
            fadeSet(2, 0.5f);
            running = 0;
            break;
        }
        }
    } while (running);

    return result;
}
}

extern "C" {
void menuTopBattleSelectCtrl(tagWINDOW_WORK* control)
{
    menuTitleCursorAnimeMain();
    s32 state = control->mState;

    switch (state) {
    case 0:
        if (control->mFlag == 0) {
            winSeqSetMenu(control->mMenuID, 0x24);
            control->mFlag = 1;
        }
        break;

    case 3:
        if (control->mFlag == 0) {
            winSeqSetMenu(control->mMenuID, 0x17B);
            control->mFlag = 1;
        }
        break;
    }
}
}

#pragma dont_inline on
extern "C" {
extern "C" void menuTopPlayToolBGM(void)
{
    if (soundGetBGMID() != 0x568) {
        soundOpenPlay(0x568, 0, 0xFF);
    }
}
}
#pragma dont_inline reset

void _menuTopNewGame(u16 *name)
{
    savedataCreate(0, name);
    pokecoloStartCountPlayTime(1);
    pokecoloSetPlayTime(0.0);
}

extern "C" {
void menuTopExit(void)
{
    _menuTopExit();
}
}

#pragma force_active on
extern "C" {
s32 menuTopMain(void)
{
    return menuTopSelect();
}
}
#pragma force_active reset

extern "C" {
void menuTopInit(void)
{
    _menuTopInit();
}
}

#pragma dont_inline on
void _menuTopSetLastCursor(void)
{
    s32 cursor = _menuTopWork.cursor;
    if (cursor >= 5 || cursor < 0) {
        cursor = 0;
    }
    _menuTopSelectCursor = cursor;
}
#pragma dont_inline reset

#pragma dont_inline on
s32 _menuTopLoadCheck(void)
{
    s32 result;
    s32 value;
    s32 meetCount;
    unsigned long playTime;

    playTime = pokecoloGetPlayTimeValue();
    meetCount = darkPokemonGetReliveNum();
    value = darkPokemonGetSnatchSuccessNum();

    FlashSystemSetPlayFrameLabel(0x22063000, "start", "end");
    FlashSystemSetLoopFlag(0x22063000, 0);
    FlashSystemSetNowFrameLabel(0x22063000, "start");
    msgctrlSetValue(0x4C, (unsigned long)playTime);
    msgctrlSetValue(0x2F, (unsigned long)value);
    msgctrlSetValue(0x30, (unsigned long)meetCount);
    menuOpen(0xDB, 0);
    winMsgOpen(1, 0x42EE, 0, 1);

    if (menuSubOpenYesNo(0, 0x1C2, 0xC0, true) == 0) {
        heroMoveSyncWithHero();
        floorSetContinueMode(1);
        floorClearReturnPoint();
        floorReturnSavePoint();
        result = 100;
        pokecoloStartCountPlayTime(1);
    } else {
        result = 1;
        pokecoloStartCountPlayTime(1);
    }

    menuTitleUnloadMapModels();
    _menuTopExitPokemon();
    _menuTopClose(0xDB);
    winMsgClose(1);
    fadeCheck(1);
    return result;
}
#pragma dont_inline reset

#pragma dont_inline on
void _menuTopClose(long menuID)
{
    menuCloseCustom(menuID, 0, 1);
}

void _menuTopClose2(long menuID, unsigned long flags)
{
    if (menuIsCheck(menuID) == 1) {
        menuCloseCustom(menuID, flags, 1);
    }
}
#pragma dont_inline reset

void _menuTopInitPokemon(s16 width, s16 height)
{
    menuModelInit(&_menuTopMenuModel, width, height);
}

void _menuTopExitPokemon(void)
{
    menuModelTerm(&_menuTopMenuModel);
}

void _menuTopLoadPokemon(void)
{
    s32 heroObjectID = heroGetObjID(1);
    menuModelLoad(&_menuTopMenuModel, heroObjectID);
    menuModelCheck(&_menuTopMenuModel, 1);

    PeopleInfo *peopleInfo = peopleInfoBiosGetPtr(0x16C90400);
    int motionID;
    u8 hasMotion;
    peopleInfoBiosGetMotion(peopleInfo, 1, &motionID, &hasMotion);
    menuModelSetMotion(&_menuTopMenuModel, motionID);
}

void _menuTopExit(void)
{
    _menuTopClose2(0x116, 2);
    scriptModelClose(_menuTopResID);
}

extern "C" {
s32 menuTopGetBattleSelectMode(void)
{
    return _menuTopWork.battleSelectMode;
}
}

extern "C" {
void menuTopSetBattleSelectMode(s32 mode)
{
    _menuTopWork.battleSelectMode = mode;
}
}

void _menuTopInit(void)
{
    _menuTopWork.cursor = _menuTopSelectCursor;
    _menuTopWork.mode = 0;
    _menuTopWork.battleSelectMode = 0;
}

extern "C" {
void menuTopSetInitCursor(s32 cursor)
{
    _menuTopSelectCursor = cursor;
}
}

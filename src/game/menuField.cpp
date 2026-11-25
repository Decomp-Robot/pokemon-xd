#include <types.h>
#include <winSprite.h>
#include <GSmsg.h>
#include <time.h>
#include <menu.h>
#include <libc/math.h>
#include <fade.h>
#include <floor.h>
#include <floorFieldCamera.h>
#include <GSflag.h>
#include <GSgfxVideo.h>
#include <GSmodel.h>
#include <GSsndPlayback.h>
#include <GSthread.h>
#include <hero.h>
#include <heroMove.h>
#include <mailMain.h>
#include <menuPokemon.h>
#include <menuPocket2.h>
#include <pokecolo.h>
#include <pokemonRelive.h>
#include <script.h>
#include <sound.h>
#include <menuOffScreen.h>

u8 menuFieldOpen;
f32 _fieldMenuTimer;

enum { FIELD_MENU_ROW_COUNT = 5 };

enum FieldMenuSpriteID {
    FIELD_MENU_BAR_0 = 21,
    FIELD_MENU_BAR_1 = 22,
    FIELD_MENU_BAR_2 = 23,
    FIELD_MENU_BAR_3 = 24,
    FIELD_MENU_BAR_4 = 25,
    FIELD_MENU_FLASH_CURSOR_0 = 26,
    FIELD_MENU_FLASH_CURSOR_1 = 27,
    FIELD_MENU_FLASH_CURSOR_2 = 28,
    FIELD_MENU_FLASH_CURSOR_3 = 29,
    FIELD_MENU_FLASH_CURSOR_4 = 30,
    FIELD_MENU_TEXT_0 = 31,
    FIELD_MENU_TEXT_1 = 32,
    FIELD_MENU_TEXT_2 = 33,
    FIELD_MENU_TEXT_3 = 34,
    FIELD_MENU_TEXT_4 = 35,
    FIELD_MENU_ALT_BAR_0 = 707,
    FIELD_MENU_ALT_BAR_1 = 708,
    FIELD_MENU_ALT_BAR_2 = 709,
    FIELD_MENU_ALT_BAR_3 = 710,
    FIELD_MENU_ALT_FLASH_CURSOR_0 = 711,
    FIELD_MENU_ALT_FLASH_CURSOR_1 = 712,
    FIELD_MENU_ALT_FLASH_CURSOR_2 = 713,
    FIELD_MENU_ALT_FLASH_CURSOR_3 = 714,
    FIELD_MENU_ALT_TEXT_0 = 715,
    FIELD_MENU_ALT_TEXT_1 = 716,
    FIELD_MENU_ALT_TEXT_2 = 717,
    FIELD_MENU_ALT_TEXT_3 = 718,
    FIELD_MENU_CURSOR_0 = 1643,
    FIELD_MENU_CURSOR_1 = 1644,
    FIELD_MENU_CURSOR_2 = 1645,
    FIELD_MENU_CURSOR_3 = 1646,
    FIELD_MENU_CURSOR_4 = 1647,
    FIELD_MENU_ALT_CURSOR_0 = 1648,
    FIELD_MENU_ALT_CURSOR_1 = 1649,
    FIELD_MENU_ALT_CURSOR_2 = 1650,
    FIELD_MENU_ALT_CURSOR_3 = 1651,
};

#define FIELD_MENU_RGBA(r, g, b, a) (((u32)(r) << 24) | ((u32)(g) << 16) | ((u32)(b) << 8) | (u32)(a))

static const u32 FIELD_MENU_TEXT_SELECTED = FIELD_MENU_RGBA(240, 240, 240, 255);
static const u32 FIELD_MENU_TEXT_UNSELECTED = FIELD_MENU_RGBA(240, 240, 240, 128);

f32 _menuFiledAlpha[FIELD_MENU_ROW_COUNT];
u32 barcolor = FIELD_MENU_RGBA(192, 192, 208, 0);
static f32 _cursorSec = 0.8f;
static f32 _max_alpha = 255.0f;
static f32 _min_alpha = 200.0f;
u32 _soundEnvID;
u32 _soundEnvVol;
f32 _menuFieldCursorAngle;

f32 _animAlpha(long row);
void _menuFieldMenuCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);
void _menuFieldFlashCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);
void _menuFieldCursorCheck(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);
void _menuFieldDrawItemSub(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);

extern "C" void menuFieldDrawItem2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);

void menuFieldDrawItem2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case FIELD_MENU_BAR_0:
        _menuFieldDrawItemSub(window, sprite, 0);
        break;
    case FIELD_MENU_BAR_1:
        _menuFieldDrawItemSub(window, sprite, 1);
        break;
    case FIELD_MENU_BAR_2:
        _menuFieldDrawItemSub(window, sprite, 2);
        break;
    case FIELD_MENU_BAR_3:
        _menuFieldDrawItemSub(window, sprite, 3);
        break;
    case FIELD_MENU_BAR_4:
        _menuFieldDrawItemSub(window, sprite, 4);
        break;
    case FIELD_MENU_FLASH_CURSOR_0:
        _menuFieldFlashCursor(window, sprite, 0);
        break;
    case FIELD_MENU_FLASH_CURSOR_1:
        _menuFieldFlashCursor(window, sprite, 1);
        break;
    case FIELD_MENU_FLASH_CURSOR_2:
        _menuFieldFlashCursor(window, sprite, 2);
        break;
    case FIELD_MENU_FLASH_CURSOR_3:
        _menuFieldFlashCursor(window, sprite, 3);
        break;
    case FIELD_MENU_FLASH_CURSOR_4:
        _menuFieldFlashCursor(window, sprite, 4);
        break;
    case FIELD_MENU_CURSOR_0:
        _menuFieldMenuCursor(window, sprite, 0);
        break;
    case FIELD_MENU_CURSOR_1:
        _menuFieldMenuCursor(window, sprite, 1);
        break;
    case FIELD_MENU_CURSOR_2:
        _menuFieldMenuCursor(window, sprite, 2);
        break;
    case FIELD_MENU_CURSOR_3:
        _menuFieldMenuCursor(window, sprite, 3);
        break;
    case FIELD_MENU_CURSOR_4:
        _menuFieldMenuCursor(window, sprite, 4);
        break;
    case FIELD_MENU_TEXT_0:
        _menuFieldCursorCheck(window, sprite, 0);
        break;
    case FIELD_MENU_TEXT_1:
        _menuFieldCursorCheck(window, sprite, 1);
        break;
    case FIELD_MENU_TEXT_2:
        _menuFieldCursorCheck(window, sprite, 2);
        break;
    case FIELD_MENU_TEXT_3:
        _menuFieldCursorCheck(window, sprite, 3);
        break;
    case FIELD_MENU_TEXT_4:
        _menuFieldCursorCheck(window, sprite, 4);
        break;
    case FIELD_MENU_ALT_BAR_0:
        _menuFieldDrawItemSub(window, sprite, 0);
        break;
    case FIELD_MENU_ALT_BAR_1:
        _menuFieldDrawItemSub(window, sprite, 1);
        break;
    case FIELD_MENU_ALT_BAR_2:
        _menuFieldDrawItemSub(window, sprite, 2);
        break;
    case FIELD_MENU_ALT_BAR_3:
        _menuFieldDrawItemSub(window, sprite, 3);
        break;
    case FIELD_MENU_ALT_FLASH_CURSOR_0:
        _menuFieldFlashCursor(window, sprite, 0);
        break;
    case FIELD_MENU_ALT_FLASH_CURSOR_1:
        _menuFieldFlashCursor(window, sprite, 1);
        break;
    case FIELD_MENU_ALT_FLASH_CURSOR_2:
        _menuFieldFlashCursor(window, sprite, 2);
        break;
    case FIELD_MENU_ALT_FLASH_CURSOR_3:
        _menuFieldFlashCursor(window, sprite, 3);
        break;
    case FIELD_MENU_ALT_CURSOR_0:
        _menuFieldMenuCursor(window, sprite, 0);
        break;
    case FIELD_MENU_ALT_CURSOR_1:
        _menuFieldMenuCursor(window, sprite, 1);
        break;
    case FIELD_MENU_ALT_CURSOR_2:
        _menuFieldMenuCursor(window, sprite, 2);
        break;
    case FIELD_MENU_ALT_CURSOR_3:
        _menuFieldMenuCursor(window, sprite, 3);
        break;
    case FIELD_MENU_ALT_TEXT_0:
        _menuFieldCursorCheck(window, sprite, 0);
        break;
    case FIELD_MENU_ALT_TEXT_1:
        _menuFieldCursorCheck(window, sprite, 1);
        break;
    case FIELD_MENU_ALT_TEXT_2:
        _menuFieldCursorCheck(window, sprite, 2);
        break;
    case FIELD_MENU_ALT_TEXT_3:
        _menuFieldCursorCheck(window, sprite, 3);
        break;
    }
}

void _menuFieldMenuCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    s16 cursorY = window->mCursor.y;
    s16 cursorTo = window->mPreviousCursor.y;

    if (cursorY == row) {
        if (cursorTo != cursorY) {
            winAnimeCursor(window, sprite);
            _fieldMenuTimer = 0.0f;
        }
        winSpriteSetDisp(sprite, true);
    } else {
        winSpriteSetDisp(sprite, false);
    }

    if (window->mState == WINDOW_STATE_CLOSING) {
        winSetSequence(&sprite->mSeq, 0);
    }
}

void _menuFieldFlashCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    if (window->mCursor.y == row) {
        sprite->mYaw = _menuFieldCursorAngle;
        winSpriteSetDisp(sprite, true);
    } else {
        winSpriteSetDisp(sprite, false);
    }
}

void _menuFieldCursorCheck(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    s16 x, y;
    u32 color;
    long cursor;

    cursor = window->mCursor.y;
    winSpriteSetDisp(sprite, false);
    if (sprite->mMsgID != 0) {
        if (cursor == row) {
            color = FIELD_MENU_TEXT_SELECTED;
        } else {
            color = FIELD_MENU_TEXT_UNSELECTED;
        }
        x = (sprite->mWidth - (s16)(GSmsgGetMessageSize(sprite->mMsgID) >> 16)) / 2;
        y = (sprite->mHeight - (s16)(GSmsgGetMessageSize(sprite->mMsgID) & 0xFFFF)) / 2 - 2;
        GSmsgPrint2(window->mLayerID, x, y, color, sprite->mMsgID);
    }
}

void _menuFieldDrawItemSub(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    long cursor = window->mCursor.y;
    f32 step = 255.0f * timeGetLastFrameSec() / 0.4f;
    if (cursor == row) {
        winSpriteSetDisp(sprite, true);
        _animAlpha(row);
    } else if (_menuFiledAlpha[row] > 0.0f) {
        _menuFiledAlpha[row] -= step;
        if (_menuFiledAlpha[row] < 0.0f) {
            _menuFiledAlpha[row] = 0.0f;
        }
    }
    sprite->mColor = barcolor | (u8)(s32)_menuFiledAlpha[row];
}

f32 _animAlpha(long row)
{
    _fieldMenuTimer += 3.1415927f * (2.0f * menuGetLastTime()) / _cursorSec;
    if (_fieldMenuTimer >= 2.0f * 3.1415927f) {
        _fieldMenuTimer -= 2.0f * 3.1415927f;
    }
    f32 range = _max_alpha - _min_alpha;
    f32 alpha = (1.0f + (f32)sin(_fieldMenuTimer)) / 2.0f;
    alpha = range * alpha;
    alpha += _min_alpha;
    if (alpha > 255.0f) {
        alpha = 255.0f;
    }
    _menuFiledAlpha[row] = alpha;
    return alpha;
}

enum FieldMenuSequence {
    FIELD_MENU_SEQUENCE_OPEN = 1,
    FIELD_MENU_SEQUENCE_CLOSE = 7,
};

enum FieldMenuWindowState {
    FIELD_MENU_WINDOW_STATE_INIT = WINDOW_STATE_INIT,
    FIELD_MENU_WINDOW_STATE_CLOSING = WINDOW_STATE_CLOSING,
};

enum FieldMenuMode {
    FIELD_MENU_MODE_NORMAL = 0,
    FIELD_MENU_MODE_DIRECT_ITEM = 1,
    FIELD_MENU_MODE_UNUSED_2 = 2,
    FIELD_MENU_MODE_UNUSED_3 = 3,
};

enum FloorEventScriptKind {
    FLOOR_EVENT_SCRIPT_KIND_PRE_MENU = 4,
};

enum FieldMenuFlag {
    FIELD_MENU_FLAG_NON_STORY_MODE = 781,
    FIELD_MENU_FLAG_PDA = 1009,
};

enum FieldMenuSelection {
    FIELD_MENU_SELECT_PARTY = 0,
    FIELD_MENU_SELECT_PDA = 1,
    FIELD_MENU_SELECT_POCKET = 2,
    FIELD_MENU_SELECT_REPORT = 3,
};

extern "C" s32 menuFieldMainCtrl(tagWINDOW_WORK *window)
{
    s32 row;
    s32 selectedRow;

    switch (window->mState) {
    case FIELD_MENU_WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, FIELD_MENU_SEQUENCE_OPEN);
            selectedRow = window->mCursor.y;
            _fieldMenuTimer = 0.0f;

            for (row = 0; row < FIELD_MENU_ROW_COUNT; row++) {
                if (selectedRow == row)
                    _menuFiledAlpha[row] = 255.0f;
                else
                    _menuFiledAlpha[row] = 0.0f;
            }

            window->mFlag = 1;
        }
        break;

    case FIELD_MENU_WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, FIELD_MENU_SEQUENCE_CLOSE);
            window->mFlag = 1;
        }
        break;
    }

    return 0;
}

s32 _menuFieldCheckCursor(s32 cursor);
bool _menuFieldCheckPDA(void);

s32 menuFieldMain(u8 mode)
{
    HeroMoveEventList eventList;
    s32 slot;
    u16 itemID;
    u32 scriptID;
    FieldMenuSelection selection;
    s32 i;
    s32 menuID;
    s32 eventCount;

    if (fadeCheck(0) != 0)
        return 0;

    eventCount = heroMoveCheckEvent(eventList);
    for (i = 0; i < eventCount; i++) {
        floorEventExecScript(FLOOR_EVENT_SCRIPT_KIND_PRE_MENU, eventList[i].eventId);
    }

    menuIntUnderTakerDispIconOff2();
    heroMoveInitEvent();
    mailMainReceiveTerminate();
    GSthreadSwitch();
    floorFieldCameraPause();
    GSmodelAllPauseAnimation();
    menuFieldOpen = 1;
    GSthreadSwitch();

    f32 offScreenFrames = 20.0f / GSgfxVideoGetVsyncRate();
    menuCreateOffScreen(offScreenFrames, 50.0f);
    while (menuDisableOffScreenCheck() == 0)
        GSthreadSwitch();

    _soundEnvID = soundGetEnvID();
    if (_soundEnvID != 0)
        _soundEnvVol = soundGetVolume(_soundEnvID);
    GSsndPushVolumeALL(85, 500, 1, 1);
    GSgfxVideoSetLockFrameRate(0);

    if (GSflagGet(FIELD_MENU_FLAG_NON_STORY_MODE) == 0) {
        if (_menuFieldCheckPDA() != 0)
            menuID = MENU_ID_FIELD_PDA;
        else
            menuID = MENU_ID_FIELD_NO_PDA;
    } else {
        menuID = MENU_ID_FIELD_NON_STORY_MODE;
    }
    preReliveExit();

    switch (mode) {
    case FIELD_MENU_MODE_NORMAL:
        if (GSflagGet(FIELD_MENU_FLAG_NON_STORY_MODE) == 0) {
            f32* alphaPtr = _menuFiledAlpha;
            f32 alpha = 255.0f;
            while (true) {
                GSgfxVideoSetLockFrameRate(0);
                for (i = 0; i < FIELD_MENU_ROW_COUNT; i++)
                    alphaPtr[i] = alpha;

                selection = (FieldMenuSelection)_menuFieldCheckCursor(menuOpen(menuID, 1));
                if (selection == FIELD_MENU_SELECT_PARTY) {
                    menuCloseCustom(menuID, 0, true);
                    menuPokemonOpen(1, 0, 0);
                    GSmodelAllPauseAnimation();
                } else if (selection == FIELD_MENU_SELECT_PDA) {
                    menuCloseCustom(menuID, 0, true);
                    menuPdaOpen();
                    GSmodelAllPauseAnimation();
                } else if (selection == FIELD_MENU_SELECT_POCKET) {
                    menuCloseCustom(menuID, 0, true);
                    itemID = menuPocket2Open(POCKET_MODE_FIELD, &slot, 0);
                    GSmodelAllPauseAnimation();
                    if (itemID != 0) {
                        scriptID = menuPocket2GetItemScript(itemID);
                        if (scriptID != -1)
                            heroMoveAddAutoEvent(scriptID, itemID, slot, 0, 0);
                        break;
                    }
                    continue;
                } else if (selection == FIELD_MENU_SELECT_REPORT) {
                    menuCloseCustom(menuID, 0, true);
                    menuReport();
                } else {
                    break;
                }
                GSthreadSwitch();
            }
        } else {
            while (true) {
                GSgfxVideoSetLockFrameRate(0);
                selection = (FieldMenuSelection)menuOpen(menuID, 1);
                if (selection == FIELD_MENU_SELECT_PARTY) {
                    menuClose(menuID);
                    menuPokemonOpen(1, 0, 0);
                } else if (selection == FIELD_MENU_SELECT_PDA) {
                    if (scriptContinueSave() != 1)
                        continue;
                    menuOffScreenRelease();
                    menuFieldOpen = 0;
                    GSthreadSwitch();
                } else {
                    break;
                }
            }
        }
        break;

    case FIELD_MENU_MODE_DIRECT_ITEM:
        itemID = menuPocket2Open(POCKET_MODE_FIELD, &slot, 0);
        if (itemID != 0) {
            scriptID = menuPocket2GetItemScript(itemID);
            if (scriptID != -1)
                heroMoveAddAutoEvent(scriptID, itemID, slot, 0, 0);
        }
        break;

    case FIELD_MENU_MODE_UNUSED_2:
    case FIELD_MENU_MODE_UNUSED_3:
        break;
    }

    menuPocket2GetItemScript(itemID);
    GSsndPopVolumeALL(500, 1, 1);
    if (_soundEnvID == soundGetEnvID())
        soundSetVolumeID(_soundEnvID, 500, _soundEnvVol);
    GSthreadSwitch();
    menuClose(menuID);
    menuReleaseOffScreen(20.0f / GSgfxVideoGetVsyncRate());
    menuCloseSync(menuID, true);
    GSgfxVideoSetLockFrameRate(1);
    heroMoveTermEvent();
    GSmodelAllUnpauseAnimation();
    floorFieldCameraUnpause();
    menuFieldOpen = 0;
    menuIntUnderTakerDispIconRestore2();
    return 0;
}

extern "C" u8 menuFieldIsOpen(void)
{
    return menuFieldOpen;
}

s32 _menuFieldCheckCursor(s32 cursor)
{
    if (_menuFieldCheckPDA()) {
        return cursor;
    }

    s32 result = cursor;
    if (cursor >= 1) {
        result = cursor + 1;
    }
    return result;
}

bool _menuFieldCheckPDA(void)
{
    if (pokecoloGetRomVersion() == 1) {
        return true;
    }
    return GSflagGet(FIELD_MENU_FLAG_PDA) != 0;
}

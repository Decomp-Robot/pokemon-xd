#include <menu.h>
#include <window.h>
#include <winSprite.h>
#include <winMsg.h>
#include <menuTitle.h>
#include <menuSub.h>
#include <menuTool.h>
#include <menuPdaSub.h>
#include <memcard.h>
#include <gamedata.h>
#include <GSinput.h>
#include <GSmsg.h>
#include <GSsnd.h>
#include <fade.h>
#include <time.h>
#include <math.h>

/*
 * Title screen options: the sound output mode and vibration switches, the
 * preview those switches animate, and the save the screen offers before it
 * hands control back to the title.
 */

/* Menu the option rows live in. */
enum { TITLE_OPTION_MENU_ID = 279 };

/* Window sequences the option menu plays when it opens and when it closes. */
enum {
    TITLE_OPTION_SEQUENCE_OPEN = 36,
    TITLE_OPTION_SEQUENCE_CLOSE = 379,
};

/* Message printed while the screen asks whether the options are saved. */
enum { TITLE_OPTION_SAVE_MESSAGE = 17495 };

/* Menu sound the rows play when the player changes one of them. */
enum { TITLE_OPTION_SOUND_CURSOR = 1 };

/* Rows the option cursor can sit on. */
enum {
    TITLE_OPTION_ROW_OUTPUT_MODE = 0,
    TITLE_OPTION_ROW_VIBRATION = 1,
    TITLE_OPTION_NORMAL_BUTTON_ROW = 2,
};

/* Steps menuTitleOption runs the screen through. */
enum {
    TITLE_OPTION_STEP_OPEN = 0,
    TITLE_OPTION_STEP_SAVE = 100,
};

/* Menu items the option rows are built out of. */
enum {
    TITLE_OPTION_ITEM_ROW_OUTPUT_MODE = 86,
    TITLE_OPTION_ITEM_ROW_VIBRATION = 87,
    TITLE_OPTION_ITEM_ROW_BUTTON = 88,
    TITLE_OPTION_ITEM_STEREO = 91,
    TITLE_OPTION_ITEM_MONO = 92,
    TITLE_OPTION_ITEM_VIBRATION_ON = 93,
    TITLE_OPTION_ITEM_VIBRATION_OFF = 94,
    TITLE_OPTION_ITEM_BUTTON_CURSOR = 95,
    TITLE_OPTION_ITEM_DESCRIPTION = 104,
};

/*
 * Work of the options screen. The preview leans back and forth while the
 * screen is up, so the block keeps the frame timing that drives it.
 */
typedef struct TAG_TITLE_OPTION_WORK {
    s32 mUnknown00;
    s32 mUnknown04;
    s32 mBounceCount;     // bounces the preview has made.
    s32 mDirection;       // flips every other bounce, picks the lean to come back with.
    f32 mRotation;        // lean the preview is drawn with.
    f32 mUnknown14;
    f32 mAmplitude;       // lean the preview reaches, 0 while it rests.
    f32 mFrameSec;        // seconds the last frame took.
    f32 mProgress;        // bounce in progress, 0 to 1.
    f32 mSeconds;         // frame seconds the screen has counted up to 1.
    f32 mBouncePerSec;    // bounces the preview makes each second.
} TAG_TITLE_OPTION_WORK;

TAG_TITLE_OPTION_WORK _menuTitleOptionWork;

/* Fade of each option row: only the selected row is drawn at full strength. */
f32 _atable[3];

/* Message printed under the option rows, one per row. */
u32 _menuOptionMsgTable[3] = {17127, 17128, 17129};

void _menuTitleOptionSetFrameSec(void);
void _menuTitleOptionCalcRot(void);
void _menuTitleOptionClose(long menuID);
void _menuTitleOptionExit(void);
void _menuTitleOptionInit(void);

extern "C" void menuTitleOptionCtrl(tagWINDOW_WORK *window)
{
    menuTitleCursorAnimeMain();

    CURSOR cursor = window->mCursor;

    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, TITLE_OPTION_SEQUENCE_OPEN);
            fadeSet(FADE_IN_BLACK, 0.5f);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_ACTIVE:
        menuPdaSubCalcAlpha(_atable, 3, cursor.y);
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, TITLE_OPTION_SEQUENCE_CLOSE);
            fadeSet(FADE_OUT_BLACK, 0.5f);
            window->mFlag = 1;
        }
        break;
    }
}

extern "C" s32 menuTitleOptionCursorControl(tagWINDOW_WORK *window)
{
    CURSOR cursor = window->mCursor;
    u16 keys;

    windowGetKeyInfo();
    keys = windowGetKeyRepeat();
    _menuTitleOptionSetFrameSec();
    _menuTitleOptionCalcRot();
    menuCursorNormal(window, 1);

    switch (cursor.y) {
    case TITLE_OPTION_ROW_OUTPUT_MODE:
        if (keys & WINDOW_KEY_LEFT) {
            menuPlaySe(window->mMenuID, TITLE_OPTION_SOUND_CURSOR);
            GSsndSetOutputMode(GSSND_OUTPUTMODE_STEREO);
        }
        if (keys & WINDOW_KEY_RIGHT) {
            menuPlaySe(window->mMenuID, TITLE_OPTION_SOUND_CURSOR);
            GSsndSetOutputMode(GSSND_OUTPUTMODE_MONO);
        }
        break;
    case TITLE_OPTION_ROW_VIBRATION:
        if (keys & WINDOW_KEY_LEFT) {
            menuPlaySe(window->mMenuID, TITLE_OPTION_SOUND_CURSOR);
            if ((s32)gamedatasaveGetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION) != 0) {
                GSinputMotorStart(GS_INPUT_PORT_1, 0, 0xFF, 30, 0);
            }
            gamedatasaveSetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION, 0);
            _menuTitleOptionWork.mBounceCount = 1;
            _menuTitleOptionWork.mProgress = 1.0f;
            _menuTitleOptionWork.mDirection = 1;
        }
        if (keys & WINDOW_KEY_RIGHT) {
            menuPlaySe(window->mMenuID, TITLE_OPTION_SOUND_CURSOR);
            gamedatasaveSetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION, 1);
            _menuTitleOptionWork.mAmplitude = 0.0f;
        }
        break;
    }
    return 0;
}

/*
 * Advances the preview's timing. While the preview runs it swings between
 * leaning and resting, and a resting preview that comes back turns the other
 * way, so the lean alternates with every bounce.
 */
void _menuTitleOptionSetFrameSec(void)
{
    _menuTitleOptionWork.mFrameSec = timeGetLastFrameSec();
    _menuTitleOptionWork.mSeconds += _menuTitleOptionWork.mFrameSec;
    if (_menuTitleOptionWork.mSeconds >= 1.0f) {
        _menuTitleOptionWork.mSeconds = 0.0f;
    }

    if ((s32)gamedatasaveGetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION) == 1) {
        if (_menuTitleOptionWork.mProgress != 0.0f) {
            _menuTitleOptionWork.mProgress += _menuTitleOptionWork.mFrameSec;
            if (_menuTitleOptionWork.mProgress >= 1.0f) {
                _menuTitleOptionWork.mProgress = 0.0f;
                _menuTitleOptionWork.mBounceCount = 0;
            }
        }
    } else {
        _menuTitleOptionWork.mProgress += _menuTitleOptionWork.mFrameSec;
        if (_menuTitleOptionWork.mProgress >= 1.0f) {
            _menuTitleOptionWork.mProgress = 0.0f;
            _menuTitleOptionWork.mBounceCount++;
            if (_menuTitleOptionWork.mBounceCount % 2 == 0) {
                _menuTitleOptionWork.mDirection ^= 1;
                if (_menuTitleOptionWork.mDirection != 0) {
                    _menuTitleOptionWork.mAmplitude = 0.0f;
                } else {
                    _menuTitleOptionWork.mAmplitude = 0.20943952f;
                }
            }
        }
    }
}

/* Turns the preview by the one bounce its progress has reached. */
void _menuTitleOptionCalcRot(void)
{
    _menuTitleOptionWork.mRotation = _menuTitleOptionWork.mAmplitude *
        (f32)sin(6.2831855f * _menuTitleOptionWork.mProgress *
                 _menuTitleOptionWork.mBouncePerSec);
}

extern "C" void menuTitleOptionHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = window->mCursor;

    if (window->mState >= WINDOW_STATE_CLOSING) {
        return;
    }

    switch (sprite->mItemID) {
    case TITLE_OPTION_ITEM_STEREO:
        if (GSsndGetOutputMode() != (u32)GSSND_OUTPUTMODE_MONO) {
            winSpriteSetDisp(sprite, true);
            if (cursor.y == TITLE_OPTION_ROW_OUTPUT_MODE) {
                menuTitleCursorAnime(window, sprite);
            } else {
                sprite->mRgba.mAlpha = 0xFF;
            }
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TITLE_OPTION_ITEM_MONO:
        if (GSsndGetOutputMode() != (u32)GSSND_OUTPUTMODE_MONO) {
            winSpriteSetDisp(sprite, false);
        } else {
            winSpriteSetDisp(sprite, true);
            if (cursor.y == TITLE_OPTION_ROW_OUTPUT_MODE) {
                menuTitleCursorAnime(window, sprite);
            } else {
                sprite->mRgba.mAlpha = 0xFF;
            }
        }
        break;
    case TITLE_OPTION_ITEM_VIBRATION_ON:
        if ((s32)gamedatasaveGetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION) != 1) {
            winSpriteSetDisp(sprite, true);
            if (cursor.y == TITLE_OPTION_ROW_VIBRATION) {
                menuTitleCursorAnime(window, sprite);
            } else {
                sprite->mRgba.mAlpha = 0xFF;
            }
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TITLE_OPTION_ITEM_VIBRATION_OFF:
        if ((s32)gamedatasaveGetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION) != 1) {
            winSpriteSetDisp(sprite, false);
        } else {
            winSpriteSetDisp(sprite, true);
            if (cursor.y == TITLE_OPTION_ROW_VIBRATION) {
                menuTitleCursorAnime(window, sprite);
            } else {
                sprite->mRgba.mAlpha = 0xFF;
            }
        }
        break;
    case TITLE_OPTION_ITEM_BUTTON_CURSOR:
        if (cursor.y == TITLE_OPTION_NORMAL_BUTTON_ROW) {
            winSpriteSetDisp(sprite, true);
            menuTitleCursorAnime(window, sprite);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case TITLE_OPTION_ITEM_ROW_OUTPUT_MODE:
        sprite->mRgba.mAlpha = _atable[0];
        break;
    case TITLE_OPTION_ITEM_ROW_VIBRATION:
        sprite->mRgba.mAlpha = _atable[1];
        break;
    case TITLE_OPTION_ITEM_ROW_BUTTON:
        sprite->mRgba.mAlpha = _atable[2];
        break;
    case TITLE_OPTION_ITEM_DESCRIPTION:
        if (cursor.y >= 0) {
            GSmsgPrint2(window->mLayerID, 0, -8,
                        0xFFFFFF00 | window->mRgba.mColorA,
                        _menuOptionMsgTable[cursor.y]);
        }
        break;
    }
}

extern "C" void menuTitleOption(void)
{
    s32 step = TITLE_OPTION_STEP_OPEN;
    s32 run = 1;
    s32 soundMode;
    s32 vibrationOff;
    s32 cursorRow;
    tagWINDOW_WORK *window;

    _menuTitleOptionInit();

    do {
        switch (step) {
        case TITLE_OPTION_STEP_OPEN:
            soundMode = GSsndGetOutputMode();
            vibrationOff = gamedatasaveGetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION);
            do {
                step = menuOpen(TITLE_OPTION_MENU_ID, 1);
                window = windowSearchID(TITLE_OPTION_MENU_ID);
                cursorRow = window->mCursor.y;
                if (step < 0) {
                    GSsndSetOutputMode(soundMode);
                    gamedatasaveSetStatus(NULL, GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION,
                                          vibrationOff);
                    break;
                }
            } while (window != NULL && cursorRow != TITLE_OPTION_NORMAL_BUTTON_ROW);
            step = TITLE_OPTION_STEP_SAVE;
            break;
        case TITLE_OPTION_STEP_SAVE:
            if (vibrationOff != (s32)gamedatasaveGetStatus(NULL,
                                                           GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION)) {
                if (memcardIsMemCardValid()) {
                    winMsgOpen(1, TITLE_OPTION_SAVE_MESSAGE, true, true);
                    if (menuSubOpenYesNo(0, 60, 220, 1) == 0) {
                        memcardTaskCall(MEMCARD_TASK_SAVE_OPTIONS, WINMSG_WINDOW_MENU, 0);
                    }
                    winMsgClose(true);
                }
            }
            run = 0;
            break;
        }
    } while (run);

    menuTitleUnloadMapModels();
    _menuTitleOptionClose(TITLE_OPTION_MENU_ID);
    fadeCheck(1);
    _menuTitleOptionExit();
}

extern "C" void menuTitleOptionButton(tagWINDOW_WORK *window)
{
    if (window->mCursor.y == TITLE_OPTION_NORMAL_BUTTON_ROW) {
        menuButtonNormal(window);
    } else if (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) {
        window->mAccept = 1;
        window->mCancel = 1;
    }
}

void _menuTitleOptionClose(long menuID)
{
    menuCloseCustom(menuID, 0, true);
}

void _menuTitleOptionExit(void)
{
}

/* Puts the option rows and the preview back at their resting state. */
void _menuTitleOptionInit(void)
{
    menuToolSetSelect(0);

    _menuTitleOptionWork.mUnknown00 = 0;
    _menuTitleOptionWork.mUnknown14 = 0.0f;
    _menuTitleOptionWork.mRotation = 0.0f;
    _menuTitleOptionWork.mUnknown04 = 0;
    _menuTitleOptionWork.mProgress = 0.0f;
    _menuTitleOptionWork.mBouncePerSec = 3.0f;
    _menuTitleOptionWork.mAmplitude = 0.20943952f;
    _menuTitleOptionWork.mBounceCount = 0;
    _menuTitleOptionWork.mDirection = 0;

    menuPdaSubInitAlpha(_atable, 3, 0);
}

#include <window.h>
#include <menu.h>
#include <menuSub.h>
#include <winSeq.h>
#include <sprite.h>
#include <GSgfx.h>
#include <GSmsg.h>
#include <GSthread.h>
#include <winMsg.h>

enum WinMsgKind {
    WINMSG_KIND_NONE = -1,
    WINMSG_KIND_UNKNOWN = 0,
    WINMSG_KIND_FIELD_MENU = 1,
    WINMSG_KIND_MENU = WINMSG_WINDOW_MENU,
    WINMSG_KIND_FIELD = WINMSG_WINDOW_FIELD,
    WINMSG_KIND_FIGHT = WINMSG_WINDOW_FIGHT,
    WINMSG_KIND_LEVEL_UP = 5,
    WINMSG_KIND_ERROR = 6,
    WINMSG_KIND_FIELD_STATUS = 7,
    WINMSG_KIND_LEVEL_UP_FIELD_STATUS = 9
};

enum WinMsgParentID { WINMSG_NO_PARENT = -1 };

enum WinMsgSequenceID {
    WINMSG_SEQUENCE_MESSAGE_OPEN = 36,
    WINMSG_SEQUENCE_MESSAGE_CLOSE = 379,
    WINMSG_SEQUENCE_DEFAULT_OPEN = 40,
    WINMSG_SEQUENCE_DEFAULT_CLOSE = 44
};

// menuOpenCustom copies WINMSG_ARG_COUNT words into the window's parameters.
// Slot 0 is a message ID for text windows and a status pointer for level-up
// windows; slot 1 packs both option bits.
enum WinMsgArg {
    WINMSG_ARG_PAYLOAD = 0,
    WINMSG_ARG_OPTIONS = 1,
    WINMSG_ARG_SOUND = 2,
    WINMSG_ARG_COUNT = 3
};

enum WinMsgOption {
    WINMSG_OPTION_KEY_WAIT = 1,
    WINMSG_OPTION_NO_WAIT = 2
};

enum WinMsgResult {
    WINMSG_RESULT_MISSING = -1,
    WINMSG_RESULT_DONE = 0,
    WINMSG_RESULT_RUNNING = 1
};

static s8 msgKind = WINMSG_KIND_NONE;
static s16 menuPos = -2;
static bool msgError;

s32 _winMsgGetMenuID(s8 kind);
s32 _winMsgOpenSub(s8 kind, void *message, bool wait, bool keyWait, bool noWait,
                  u8 sound, bool fixedPriority);

s32 winMsgDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    WINDOW_FREE_WORK *work = windowGetFreeWork(window);
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
    GSgfxLayerFindByID(layer);
    s16 width, height;
    if (window->mMenuID == MENU_ID_FIGHT_MESSAGE) {
        width = work->mMessageWidth;
        height = work->mMessageHeight;
    } else {
        width = sprite->mWidth;
        height = sprite->mHeight;
    }
    s16 offsetY = sprite->mHeight - 4 - height;
    spriteSetOffset(layer, window->mScrollX + sprite->mX,
                    window->mScrollY + sprite->mY + offsetY);
    spriteSetEnv(layer);
    switch (window->mMenuID) {
    case MENU_ID_FIELD_MESSAGE:
    case MENU_ID_FIGHT_MESSAGE:
        menuSubDrawMsgWindow2(layer, 0, menuPos, width, height, window->mRgba.mColorA, 1);
        break;
    case MENU_ID_LEVEL_UP_STATUS:
        menuSubDrawMsgWindow2(layer, 0, menuPos, width, height, window->mRgba.mColorA, 1);
        break;
    case MENU_ID_ERROR_DIALOG:
        break;
    }
    switch (window->mState) {
    case WINDOW_STATE_INIT:
    case WINDOW_STATE_WAITING:
    case WINDOW_STATE_CLOSING:
    case WINDOW_STATE_DELETE:
        break;
    case WINDOW_STATE_ACTIVE:
        if (window->mMenuID != MENU_ID_LEVEL_UP_STATUS) {
            GSmsgMain(layer, work->mMessageID, work->mKeyTrigger,
                      !(window->mStateFlags & WINDOW_RUN_UPDATE));
            work->mKeyTrigger = 0;
        }
        break;
    }
    return 0;
}

s32 winMsgCtrlYesNo(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, WINMSG_SEQUENCE_MESSAGE_OPEN);
            window->mFlag = true;
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, WINMSG_SEQUENCE_MESSAGE_CLOSE);
            window->mFlag = true;
        }
        break;
    }
    return 0;
}

s32 winMsgCtrl(tagWINDOW_WORK *window)
{
    WINDOW_FREE_WORK *work = windowGetFreeWork(window);
    bool setOpenSequence = false;
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (work->mMessageID) GSmsgKill(work->mMessageID);
        work->mMessageID = windowGetParam(window, WINMSG_ARG_PAYLOAD);
        // Both option bits share this word; any nonzero value requests key wait.
        work->mKeyWait = windowGetParam(window, WINMSG_ARG_OPTIONS) != 0;
        work->mSound = windowGetParam(window, WINMSG_ARG_SOUND);
        if (!window->mFlag) setOpenSequence = true;
        if (winSeqIsCheck(window->mMenuID, WINMSG_SEQUENCE_DEFAULT_CLOSE) == 1) setOpenSequence = true;
        if (window->mCloseRequested) setOpenSequence = true;
        if (setOpenSequence) {
            if (window->mMenuID != MENU_ID_ERROR_DIALOG) {
                if (window->mMenuID != MENU_ID_FIELD_MESSAGE) winSeqSetMenu(window->mMenuID, WINMSG_SEQUENCE_DEFAULT_OPEN);
                else winSeqSetMenu(window->mMenuID, WINMSG_SEQUENCE_MESSAGE_OPEN);
            }
            window->mFlag = true;
        }
        if (window->mMenuID == MENU_ID_FIGHT_MESSAGE) {
            u32 size = GSmsgGetMessageSize(work->mMessageID);
            work->mMessageWidth = (s16)(size >> 16);
            work->mMessageHeight = (s16)(size & 0xFFFF);
        }
        break;
    case WINDOW_STATE_WAITING:
        break;
    case WINDOW_STATE_ACTIVE:
        if (!window->mFlag) {
            GSmsgExec(work->mMessageID, (u8)work->mKeyWait, work->mSound);
            window->mFlag = true;
        }
        if (GSmsgIsCheck(work->mMessageID)) window->mAccept = 0;
        else window->mAccept = 1;
        break;
    case WINDOW_STATE_CLOSING:
        if (!window->mFlag) {
            if (window->mMenuID != MENU_ID_ERROR_DIALOG) {
                if (window->mMenuID != MENU_ID_FIELD_MESSAGE) winSeqSetMenu(window->mMenuID, WINMSG_SEQUENCE_DEFAULT_CLOSE);
                else winSeqSetMenu(window->mMenuID, WINMSG_SEQUENCE_MESSAGE_CLOSE);
            }
            window->mFlag = true;
        }
        break;
    case WINDOW_STATE_DELETE:
        GSmsgKill(work->mMessageID);
        work->mMessageID = 0;
        break;
    }
    return 0;
}

void winMsgButton(tagWINDOW_WORK *window)
{
    WINDOW_FREE_WORK *work = windowGetFreeWork(window);
    work->mKeyTrigger = windowGetKeyInfo()->mKeyTrg;
}

void winMsgCloseLevelUpStatus(bool wait) { winMsgClose(wait); }
void winMsgOpenLevelUpFiledStatus(MENU_LVUP_STATUS *status, bool wait)
{ _winMsgOpenSub(WINMSG_KIND_LEVEL_UP_FIELD_STATUS, status, wait, true, false, 0, false); }
void winMsgOpenLevelUpStatus(MENU_LVUP_STATUS *status, bool wait)
{ _winMsgOpenSub(WINMSG_KIND_LEVEL_UP, status, wait, true, false, 0, false); }
void winMsgCloseError(bool wait)
{
    s32 menu = _winMsgGetMenuID(WINMSG_KIND_ERROR);
    if (menuIsCheck(menu)) menuCloseCustom(menu, WINDOW_CLOSE_KEEP_CHILDREN, false);
    msgError = false;
}
void winMsgOpenError(u32 message, bool wait, bool keyWait)
{ _winMsgOpenSub(WINMSG_KIND_ERROR, (void *)message, wait, keyWait, false, 0, false); }
void winMsgCloseFight(bool wait) { winMsgClose(wait); }
u8 winMsgCloseCheckFight() { return winMsgCheck() >= 0; }
void winMsgOpenFightNoWait(u32 message, bool wait, bool keyWait)
{ _winMsgOpenSub(WINMSG_KIND_FIGHT, (void *)message, wait, keyWait, true, 0, false); }
void winMsgOpenFight2(u32 message, bool wait, bool keyWait, bool fixedPriority)
{ _winMsgOpenSub(WINMSG_KIND_FIGHT, (void *)message, wait, keyWait, false, 0, fixedPriority); }
s32 winMsgOpenFight(u32 message, bool wait, bool keyWait)
{ return winMsgOpen(WINMSG_KIND_FIGHT, message, wait, keyWait); }
s32 winMsgCheckField() { return winMsgCheck(); }
void winMsgCloseField(bool wait) { winMsgClose(wait); }
s32 winMsgOpenFieldWithSE(u32 message, bool wait, bool keyWait, u8 sound)
{ return _winMsgOpenSub(WINMSG_KIND_FIELD, (void *)message, wait, keyWait, false, sound, false); }
s32 winMsgOpenField(u32 message, bool wait, bool keyWait)
{ return _winMsgOpenSub(WINMSG_KIND_FIELD, (void *)message, wait, keyWait, false, 0, false); }
s8 winMsgCheck()
{
    tagWINDOW_WORK *window = windowSearchID(_winMsgGetMenuID(msgKind));
    if (!window) return WINMSG_RESULT_MISSING;
    if (window->mAccept) return WINMSG_RESULT_DONE;
    if (window->mCancel) return WINMSG_RESULT_DONE;
    return WINMSG_RESULT_RUNNING;
}
void winMsgClose(bool wait)
{
    if (msgError) {
        if (wait) {
            do { GSthreadSwitch(); } while (msgError);
        } else return;
    }
    s32 menu = _winMsgGetMenuID(msgKind);
    if (menuIsCheck(menu)) menuCloseCustom(menu, WINDOW_CLOSE_KEEP_CHILDREN, wait);
    msgKind = WINMSG_KIND_NONE;
}
void winMsgOpenWithSE(s8 kind, u32 message, bool wait, bool keyWait, u8 sound)
{ _winMsgOpenSub(kind, (void *)message, wait, keyWait, false, sound, false); }
s32 winMsgOpen(s8 kind, u32 message, bool wait, bool keyWait)
{ return _winMsgOpenSub(kind, (void *)message, wait, keyWait, false, 0, false); }

s32 _winMsgOpenSub(s8 kind, void *message, bool wait, bool keyWait, bool noWait,
                  u8 sound, bool fixedPriority)
{
    u8 flags = 0;
    s32 parent;
    s32 menu = _winMsgGetMenuID(kind);
    if (!menu) return 0;
    if (kind != WINMSG_KIND_ERROR) {
        if (kind != msgKind) winMsgClose(wait);
        msgKind = kind;
        parent = windowGetActiveID();
    } else {
        msgError = true;
        parent = WINMSG_NO_PARENT;
        wait = false;
        noWait = true;
    }
    if (keyWait) flags |= WINMSG_OPTION_KEY_WAIT;
    if (noWait) flags |= WINMSG_OPTION_NO_WAIT;
    menu = _winMsgGetMenuID(kind);
    return menuOpenCustom(menu, parent, 0, fixedPriority ? MENU_OPEN_FLAG_FIXED_PRIORITY : 0,
                          wait, WINMSG_ARG_COUNT, (u32)message, (u32)flags, (u32)sound);
}
s32 _winMsgGetMenuID(s8 kind)
{
    s32 menu = 0;
    switch (kind) {
    case WINMSG_KIND_UNKNOWN: menu = MENU_ID_FIELD_MESSAGE; break;
    case WINMSG_KIND_FIELD_MENU: menu = MENU_ID_FIELD_MENU; break;
    case WINMSG_KIND_MENU: menu = MENU_ID_MAIN_MENU; break;
    case WINMSG_KIND_FIELD: menu = MENU_ID_FIELD_MESSAGE; break;
    case WINMSG_KIND_FIGHT: menu = MENU_ID_FIGHT_MESSAGE; break;
    case WINMSG_KIND_LEVEL_UP: menu = MENU_ID_LEVEL_UP_STATUS; break;
    case WINMSG_KIND_ERROR: menu = MENU_ID_ERROR_DIALOG; break;
    case WINMSG_KIND_FIELD_STATUS: menu = MENU_ID_FIELD_STATUS; break;
    case WINMSG_KIND_LEVEL_UP_FIELD_STATUS: menu = MENU_ID_LEVEL_UP_FIELD_STATUS; break;
    }
    return menu;
}

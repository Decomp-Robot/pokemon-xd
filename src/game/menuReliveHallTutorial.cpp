#include <CMetaMenu.h>
#include <CMenuGlobalState.h>
#include <GSgfxVideo.h>

namespace {
enum TutorialStep {
    TUTORIAL_INTRO,
    TUTORIAL_PAGE_FORWARD,
    TUTORIAL_SELECT_SET,
    TUTORIAL_SET_MESSAGE,
    TUTORIAL_MOVE_RIGHT,
    TUTORIAL_MOVE_DOWN,
    TUTORIAL_CONFIRM_SET,
    TUTORIAL_POKEMON_MESSAGE,
    TUTORIAL_PAGE_BACK,
    TUTORIAL_PREVIOUS_PAGE,
    TUTORIAL_SELECT_POKEMON,
    TUTORIAL_MOVE_TO_SLOT,
    TUTORIAL_MOVE_UP,
    TUTORIAL_SELECT_ACTION,
    TUTORIAL_RETURN_TO_BOX,
    TUTORIAL_MOVE_TO_NEIGHBOR,
    TUTORIAL_MOVE_LEFT,
    TUTORIAL_CONFIRM_ACTION,
    TUTORIAL_OPEN_ACTION_MENU,
    TUTORIAL_SELECT_MENU_ACTION,
    TUTORIAL_CANCEL_ACTION,
    TUTORIAL_RETURN_UP,
    TUTORIAL_CHANGE_VIEW,
    TUTORIAL_SELECT_TARGET,
    TUTORIAL_FINAL_MESSAGE,
    TUTORIAL_CANCEL_AND_DOWN,
    TUTORIAL_COMPLETE,
};

enum TutorialMessage {
    TUTORIAL_MSG_INTRO = 53560,
    TUTORIAL_MSG_PAGE_FORWARD = 53561,
    TUTORIAL_MSG_SELECT_SET = 53562,
    TUTORIAL_MSG_SET_MESSAGE = 53563,
    TUTORIAL_MSG_MOVE_RIGHT = 53564,
    TUTORIAL_MSG_MOVE_DOWN = 53565,
    TUTORIAL_MSG_POKEMON_MESSAGE = 53566,
    TUTORIAL_MSG_PAGE_BACK = 53567,
    TUTORIAL_MSG_PREVIOUS_PAGE = 53568,
    TUTORIAL_MSG_SELECT_POKEMON = 53569,
    TUTORIAL_MSG_MOVE_TO_SLOT = 53570,
    TUTORIAL_MSG_MOVE_UP = 53571,
    TUTORIAL_MSG_SELECT_ACTION = 53572,
    TUTORIAL_MSG_MOVE_TO_NEIGHBOR = 53573,
    TUTORIAL_MSG_CONFIRM_ACTION = 53574,
    TUTORIAL_MSG_SELECT_MENU_ACTION = 53575,
    TUTORIAL_MSG_CANCEL_ACTION = 53576,
    TUTORIAL_MSG_RETURN_UP = 53577,
    TUTORIAL_MSG_CHANGE_VIEW = 53578,
    TUTORIAL_MSG_SELECT_TARGET = 53579,
    TUTORIAL_MSG_FINAL_MESSAGE = 53580,
    TUTORIAL_MSG_CANCEL_AND_DOWN = 53581,
};
}

unsigned short CMetaMenu::tutorialManagePrime()
{
    CState &state = mGlobalState->mState;
    if (mGlobalState->mExit) {
        return WINDOW_KEY_CANCEL;
    }
    if (windowGetKeyInfo()->mKeyTrg == WINDOW_KEY_CANCEL) {
        CMenuGlobalState &global = *mGlobalState;
        global.mExit = true;
        global.mCancelExit = true;
        return WINDOW_KEY_CANCEL;
    }
    state.mTutorialFrameCount++;
    if (!isAnimationStateNone()) {
        return 0;
    }
    if ((unsigned long)state.mTutorialFrameCount < (unsigned long)(0.9f * GSgfxVideoGetVsyncRate())) {
        return 0;
    }
    state.mTutorialFrameCount = 0;
    unsigned short result = 0;
    state.update();
    switch (state.mCurrent) {
    case TUTORIAL_INTRO: {
        tutorialMessage(TUTORIAL_MSG_INTRO);
        state.mNext = state.mCurrent + 1;
        memset(state.mWork, 0, sizeof(state.mWork));
        break;
    }
    case TUTORIAL_PAGE_FORWARD: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_PAGE_FORWARD);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_TRIGGER_R, WINDOW_KEY_TRIGGER_R, WINDOW_KEY_TRIGGER_R, WINDOW_KEY_TRIGGER_R };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_SELECT_SET: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_SELECT_SET);
        }
        static unsigned short tbl[] = { 0, WINDOW_KEY_ACCEPT };
        if ((unsigned long)state.mFrameCount < sizeof(tbl) / sizeof(tbl[0])) {
            result = tbl[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_SET_MESSAGE: {
        tutorialMessage(TUTORIAL_MSG_SET_MESSAGE);
        state.mNext = state.mCurrent + 1;
        memset(state.mWork, 0, sizeof(state.mWork));
        break;
    }
    case TUTORIAL_MOVE_RIGHT: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_MOVE_RIGHT);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_RIGHT, WINDOW_KEY_RIGHT, WINDOW_KEY_RIGHT, WINDOW_KEY_RIGHT, WINDOW_KEY_ACCEPT, WINDOW_KEY_ACCEPT };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_MOVE_DOWN: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_MOVE_DOWN);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_DOWN, WINDOW_KEY_DOWN, WINDOW_KEY_DOWN, WINDOW_KEY_DOWN, WINDOW_KEY_DOWN };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_CONFIRM_SET: {
        result = WINDOW_KEY_ACCEPT;
        state.mNext = state.mCurrent + 1;
        memset(state.mWork, 0, sizeof(state.mWork));
        break;
    }
    case TUTORIAL_POKEMON_MESSAGE: {
        tutorialMessage(TUTORIAL_MSG_POKEMON_MESSAGE);
        state.mNext = state.mCurrent + 1;
        memset(state.mWork, 0, sizeof(state.mWork));
        break;
    }
    case TUTORIAL_PAGE_BACK: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_PAGE_BACK);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_TRIGGER_L, WINDOW_KEY_TRIGGER_R, WINDOW_KEY_TRIGGER_R };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_PREVIOUS_PAGE: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_PREVIOUS_PAGE);
        }
        static unsigned short tbl[] = { 0, WINDOW_KEY_TRIGGER_L };
        if ((unsigned long)state.mFrameCount < sizeof(tbl) / sizeof(tbl[0])) {
            result = tbl[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_SELECT_POKEMON: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_SELECT_POKEMON);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_UP, WINDOW_KEY_ACCEPT, WINDOW_KEY_ACCEPT, WINDOW_KEY_X };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_MOVE_TO_SLOT: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_MOVE_TO_SLOT);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_Z, WINDOW_KEY_RIGHT, WINDOW_KEY_RIGHT, WINDOW_KEY_RIGHT, WINDOW_KEY_ACCEPT, WINDOW_KEY_CANCEL };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_MOVE_UP: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_MOVE_UP);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_UP, WINDOW_KEY_UP, WINDOW_KEY_ACCEPT };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_SELECT_ACTION: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_SELECT_ACTION);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_DOWN, WINDOW_KEY_ACCEPT };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_RETURN_TO_BOX: {
        static unsigned short keys[] = { 0, WINDOW_KEY_Z, WINDOW_KEY_RIGHT, WINDOW_KEY_RIGHT, WINDOW_KEY_ACCEPT, WINDOW_KEY_CANCEL };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_MOVE_TO_NEIGHBOR: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_MOVE_TO_NEIGHBOR);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_UP, WINDOW_KEY_RIGHT };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_MOVE_LEFT: {
        static unsigned short tbl[] = { 0, WINDOW_KEY_LEFT };
        if ((unsigned long)state.mFrameCount < sizeof(tbl) / sizeof(tbl[0])) {
            result = tbl[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_CONFIRM_ACTION: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_CONFIRM_ACTION);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_RIGHT, WINDOW_KEY_ACCEPT };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_OPEN_ACTION_MENU: {
        static unsigned short keys[] = { 0, WINDOW_KEY_Z, WINDOW_KEY_DOWN, WINDOW_KEY_RIGHT, WINDOW_KEY_RIGHT, WINDOW_KEY_RIGHT, WINDOW_KEY_ACCEPT, WINDOW_KEY_CANCEL };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_SELECT_MENU_ACTION: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_SELECT_MENU_ACTION);
        }
        unsigned short keys[] = { 0, WINDOW_KEY_UP, WINDOW_KEY_UP | WINDOW_KEY_RIGHT, WINDOW_KEY_UP, WINDOW_KEY_RIGHT, WINDOW_KEY_ACCEPT };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_CANCEL_ACTION: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_CANCEL_ACTION);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_Z, WINDOW_KEY_ACCEPT, WINDOW_KEY_CANCEL };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_RETURN_UP: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_RETURN_UP);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_UP, WINDOW_KEY_ACCEPT };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_CHANGE_VIEW: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_CHANGE_VIEW);
        }
        static unsigned short tbl[] = { 0, WINDOW_KEY_X };
        if ((unsigned long)state.mFrameCount < sizeof(tbl) / sizeof(tbl[0])) {
            result = tbl[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_SELECT_TARGET: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_SELECT_TARGET);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_ACCEPT, WINDOW_KEY_DOWN, WINDOW_KEY_ACCEPT, WINDOW_KEY_DOWN | WINDOW_KEY_RIGHT, WINDOW_KEY_ACCEPT };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_FINAL_MESSAGE: {
        tutorialMessage(TUTORIAL_MSG_FINAL_MESSAGE);
        state.mNext = state.mCurrent + 1;
        memset(state.mWork, 0, sizeof(state.mWork));
        break;
    }
    case TUTORIAL_CANCEL_AND_DOWN: {
        if (!state.mTime) {
            tutorialMessage(TUTORIAL_MSG_CANCEL_AND_DOWN);
        }
        static unsigned short keys[] = { 0, WINDOW_KEY_CANCEL, WINDOW_KEY_DOWN };
        if ((unsigned long)state.mFrameCount < sizeof(keys) / sizeof(keys[0])) {
            result = keys[state.mFrameCount];
        } else {
            state.mNext = state.mCurrent + 1;
            memset(state.mWork, 0, sizeof(state.mWork));
        }
        break;
    }
    case TUTORIAL_COMPLETE: {
        CMenuGlobalState &global = *mGlobalState;
        global.mExit = true;
        global.mCancelExit = false;
        result = WINDOW_KEY_ACCEPT;
        break;
    }
    }
    return result;
}

void CMetaMenu::tutorialManageUpdate()
{
    mTutorialKeysValid = false;
}

unsigned short CMetaMenu::tutorialManage()
{
    if (mTutorialKeysValid) {
        return mTutorialKeys;
    }
    mTutorialKeysValid = true;
    mTutorialKeys = tutorialManagePrime();
    return mTutorialKeys;
}

void CMetaMenu::tutorialMessage(unsigned long messageID)
{
    mTutorialInfo.exec(*this, messageID, true);
    if (mTutorialInfo.getKeyTrigger() == WINDOW_KEY_CANCEL) {
        tutorialExitKick();
        CMenuGlobalState &global = *mGlobalState;
        global.mExit = true;
        global.mCancelExit = true;
    }
}

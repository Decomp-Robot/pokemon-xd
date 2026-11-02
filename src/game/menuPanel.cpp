#include <esaba.h>
#include <menu.h>
#include <msgctrl.h>
#include <winSeq.h>
#include <winSprite.h>
#include <window.h>

// The numeric input panel asks the player for a number, such as how many of an
// item the hero hands over. Its rows print the digits of the number being
// edited, the cursor hook steps that number between the bounds the panel was
// opened with, and the sequence hook runs the window's open and close.

// Argument the numeric input panel is opened with. The window hands the record
// back to the hooks of its rows.
struct INPUT_NUMBER_ARG {
    long mCurrent; // number being edited.
    long mUpper;   // highest number the cursor can select.
    long mLower;   // lowest number the cursor can select.
    long mInitial; // number the panel was opened with.
};

// Number being edited by the open panel. The hooks of the panel's rows reach it
// through the window, so it outlives the opening call.
static INPUT_NUMBER_ARG _inputnumberarg;

// Menu data id of the numeric input panel.
enum {
    INPUT_NUMBER_MENU_ID = 185,
};

// Window sequences run when a menu window opens and closes. Several menus start
// the same pair.
enum {
    MENU_SEQ_OPEN = 36,
    MENU_SEQ_CLOSE = 379,
};

// Rows of the panel that report the number being edited: its tens digit, its
// ones digit and how many the hero holds of the item.
enum {
    INPUT_NUMBER_ITEM_TENS = 1284,
    INPUT_NUMBER_ITEM_ONES = 1283,
    INPUT_NUMBER_ITEM_REMAINING = 1297,
};

long _menuPanelInputNumber(short, short, long initial, long *, long lower,
                           long upper, bool closeOnExit);

extern "C" {

// Ask the player for a number between lower and upper.
bool menuPanelInputNumber(long initial, long *output, long lower, long upper,
                          bool closeOnExit)
{
    long result = _menuPanelInputNumber(0, 0, initial, output, lower, upper, closeOnExit);

    if (result >= 0) {
        *output = result;
        return true;
    }
    *output = initial;
    return false;
}

// Close the numeric input panel once its caller has the number.
void menuPaneInputNumberClose(void)
{
    menuClose(INPUT_NUMBER_MENU_ID);
    menuCloseSync(INPUT_NUMBER_MENU_ID, true);
}

}

// Open the panel and return the number the player chose.
long _menuPanelInputNumber(short, short, long initial, long *, long lower,
                           long upper, bool closeOnExit)
{
    long result;

    _inputnumberarg.mCurrent = initial;
    _inputnumberarg.mUpper = upper;
    _inputnumberarg.mLower = lower;
    _inputnumberarg.mInitial = initial;
    result = menuOpenCustom(INPUT_NUMBER_MENU_ID, windowGetActiveID(), 0, 0, true, 1,
                            &_inputnumberarg);
    if (result >= 0) {
        result = _inputnumberarg.mCurrent;
    }
    if (closeOnExit) {
        menuPaneInputNumberClose();
    }
    return result;
}

extern "C" {

// Print the number being edited on the row that asked for it.
void menuPanelInputNumberHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    INPUT_NUMBER_ARG *arg = (INPUT_NUMBER_ARG *)window->mArg;

    switch (sprite->mItemID) {
    case INPUT_NUMBER_ITEM_TENS:
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, arg->mCurrent / 10);
        break;
    case INPUT_NUMBER_ITEM_ONES:
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, arg->mCurrent % 10);
        break;
    case INPUT_NUMBER_ITEM_REMAINING:
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, arg->mInitial - arg->mCurrent + esabaGetEsaItemNum());
        break;
    default:
        break;
    }
}

// Step the number being edited while up or down repeats.
void menuPanelInputNumberCursor(tagWINDOW_WORK *window)
{
    INPUT_NUMBER_ARG *arg;
    u16 repeat;

    windowGetKeyInfo();
    arg = (INPUT_NUMBER_ARG *)window->mArg;
    repeat = windowGetKeyRepeat();
    if (repeat & WINDOW_KEY_UP) {
        if (arg->mCurrent < arg->mUpper) {
            arg->mCurrent++;
        }
    } else if (repeat & WINDOW_KEY_DOWN) {
        if (arg->mCurrent > arg->mLower) {
            arg->mCurrent--;
        }
    }
}

// Run the sequences that open and close the panel's window.
void menuPanelInputNumberCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQ_OPEN);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    default:
        break;
    }
}

}

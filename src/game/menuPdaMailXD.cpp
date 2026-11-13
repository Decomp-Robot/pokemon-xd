#include <menuPdaMailXD.h>
#include <menuPdaMailListSort.h>
#include <menu.h>
#include <menuPda.h>
#include <menuScript.h>
#include <window.h>
#include <winSeq.h>
#include <GSmsg.h>
#include <GSsndPlayback.h>
#include <msgctrl.h>

extern "C" {
u32 mailGetAttachFileGroup(s32 mailID);
u32 mailGetContents(s32 mailID);
s32 mailGetNbMailInMailbox(void);
u32 mailGetSenderName(s32 mailID);
u32 mailGetSubject(s32 mailID);
s32 pdaMailGetMailID(s32 index);
}

long _mailSender(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long _mailSubject(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long _mailHonbun(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);

// Menus, rows and sounds of the mail reader window.
enum {
    MENU_ID_PDA_MAIL_CONTENTS = 111, // reader window opened over the mail list.
    MENU_ID_PDA_MAIL_LIST = 119,     // mail list the reader is opened on top of.
    MAIL_PDA_SUBRES = 2,
    MAIL_CONTENTS_VISIBLE_COUNT = 5, // mail rows the reader shows at once.
    MAIL_SOUND_CURSOR = 1104,
    MAIL_SEQ_OPEN = 306,
    MAIL_SEQ_CLOSE = 310,
    MAIL_ITEM_HONBUN = 170,  // row that prints the body of the mail.
    MAIL_ITEM_SENDER = 171,  // row that prints who sent the mail.
    MAIL_ITEM_SUBJECT = 172, // row that prints the subject of the mail.
    MSG_MAIL_TEXT = 17014,   // message that prints the text set by msgctrlSetValue.
};

// Colours the sender and the subject rows print with.
enum {
    MAIL_SENDER_COLOR_R = 240,
    MAIL_SENDER_COLOR_G = 240,
    MAIL_SENDER_COLOR_B = 240,
    MAIL_SUBJECT_COLOR_R = 213,
    MAIL_SUBJECT_COLOR_G = 170,
    MAIL_SUBJECT_COLOR_B = 51,
};

// Step the reader window reports while the mail list hands the mail over.
enum { MAIL_STATE_WAITING = 1 };

// Argument the reader window is opened with.
struct MAIL_CONTENTS {
    u8 mCursorMove;  // whether the pad may move the reader cursor.
    u8 _01[3];
    u32 mValue;      // value the caller passes for the rows to read.
    CURSOR *mCursor; // cursor of the mail the reader shows.
};

// Cursor of the mail the reader shows, shared by the reader and by its rows.
static CURSOR _cursor;

long menuPdaMailCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case MAIL_ITEM_HONBUN:
        _mailHonbun(window, sprite);
        break;
    case MAIL_ITEM_SENDER:
        _mailSender(window, sprite);
        break;
    case MAIL_ITEM_SUBJECT:
        _mailSubject(window, sprite);
        break;
    }
    return 0;
}

void menuPdaMailXDScript(s32 mailIndex)
{
    MAIL_CONTENTS contents;
    s32 openResult;

    _cursor.y = 0;
    _cursor.x = mailIndex;
    contents.mCursorMove = 0;
    menuPdaSetSubres(PDATOP_MENU_RES_MAIL, MAIL_PDA_SUBRES);
    menuOpen(MENU_ID_PDA_MAIL_LIST, 0);
    do {
        contents.mCursor = &_cursor;
        openResult = menuOpenCustom(MENU_ID_PDA_MAIL_CONTENTS, windowGetActiveID(), 0, 0, true, 1, &contents);
    } while (!menuScriptCheckMailOpen() && openResult != -1);
    _menuClose(MENU_ID_PDA_MAIL_CONTENTS);
    _menuClose(MENU_ID_PDA_MAIL_LIST);
}

CURSOR *menuPdaMailXD(CURSOR *cursor, u8 cursorMove, u32 value)
{
    MAIL_CONTENTS contents;

    _cursor.y = cursor->y;
    _cursor.x = cursor->x;
    contents.mCursorMove = cursorMove;
    if (cursorMove == 0) {
        contents.mValue = value;
    }
    do {
        contents.mCursor = &_cursor;
    } while (menuOpenCustom(MENU_ID_PDA_MAIL_CONTENTS, windowGetActiveID(), 0, 0, true, 1, &contents) != -1);
    _menuClose(MENU_ID_PDA_MAIL_CONTENTS);
    return &_cursor;
}

void menuPdaMailContentsButtonXD(tagWINDOW_WORK *window)
{
    tagWINDOW_KEY *key = windowGetKeyInfo();
    CURSOR *cursor = ((MAIL_CONTENTS *)window->mArg)->mCursor;
    s32 index = cursor->y + cursor->x;

    if (window != 0) {
        if (winSeqCheckMove(window->mMenuID)) {
            return;
        }
        s32 mailID = pdaMailGetMailID(index);
        if (!menuScriptCheckMailOpen() && mailGetAttachFileGroup(mailID) == 0) {
            if (key->mKeyOn & WINDOW_KEY_ACCEPT) {
                return;
            }
        }
    }
    menuButtonNormal(window);
}

long menuPdaMailContentsCursorXD(tagWINDOW_WORK *window)
{
    MAIL_CONTENTS *contents = (MAIL_CONTENTS *)window->mArg;
    CURSOR *cursor;
    s32 count;
    s32 visibleCount;
    s16 index;
    u16 key;

    windowGetKeyInfo();
    cursor = contents->mCursor;
    if (contents->mCursorMove == 0) {
        return 0;
    }
    count = mailGetNbMailInMailbox();
    visibleCount = MAIL_CONTENTS_VISIBLE_COUNT;
    index = cursor->y + cursor->x;
    key = windowGetKeyRepeat();
    if (count < MAIL_CONTENTS_VISIBLE_COUNT) {
        visibleCount = count;
    }
    if (key & WINDOW_KEY_UP) {
        if (cursor->y + cursor->x != 0) {
            GSsndPlayNow(MAIL_SOUND_CURSOR);
        }
        cursor->y--;
    } else if (key & WINDOW_KEY_DOWN) {
        if (cursor->y + cursor->x != count - 1) {
            GSsndPlayNow(MAIL_SOUND_CURSOR);
        }
        cursor->y++;
    }
    if (cursor->y < 0) {
        cursor->x += cursor->y;
        cursor->y = 0;
        if (cursor->x < 0) {
            cursor->x = 0;
            cursor->y = 0;
        }
    } else if (cursor->y >= visibleCount) {
        cursor->x += cursor->y - (visibleCount - 1);
        cursor->y = (s8)(visibleCount - 1);
        if (cursor->x + cursor->y >= count) {
            cursor->y = (s8)(visibleCount - 1);
            cursor->x = (s8)(count - visibleCount);
        }
    }
    if (index != cursor->y + cursor->x) {
        menuPlaySe(window->mMenuID, 1);
    }
    return 0;
}

long menuPdaMailCtrlXD(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, MAIL_SEQ_OPEN);
            window->mFlag = 1;
        }
        break;
    case MAIL_STATE_WAITING:
        menuScriptCheckMailOpen();
        break;
    case WINDOW_STATE_CLOSING:
        if (!window->mFlag) {
            winSeqSetMenu(window->mMenuID, MAIL_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

long _mailSender(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR *cursor = ((MAIL_CONTENTS *)window->mArg)->mCursor;
    u32 sender = mailGetSenderName(pdaMailGetMailID(cursor->y + cursor->x));

    if (sender != 0) {
        u16 *text = GSmsgGetStringPtr(sender);
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)text);
        sprite->mMsgID = MSG_MAIL_TEXT;
    } else {
        sprite->mMsgID = 0;
    }
    sprite->mRgba.mRed = MAIL_SENDER_COLOR_R;
    sprite->mRgba.mGreen = MAIL_SENDER_COLOR_G;
    sprite->mRgba.mBlue = MAIL_SENDER_COLOR_B;
    return 0;
}

long _mailSubject(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR *cursor = ((MAIL_CONTENTS *)window->mArg)->mCursor;
    u32 subject = mailGetSubject(pdaMailGetMailID(cursor->y + cursor->x));

    if (subject != 0) {
        u16 *text = GSmsgGetStringPtr(subject);
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)text);
        sprite->mMsgID = MSG_MAIL_TEXT;
    } else {
        sprite->mMsgID = 0;
    }
    sprite->mRgba.mRed = MAIL_SUBJECT_COLOR_R;
    sprite->mRgba.mGreen = MAIL_SUBJECT_COLOR_G;
    sprite->mRgba.mBlue = MAIL_SUBJECT_COLOR_B;
    return 0;
}

long _mailHonbun(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR *cursor = ((MAIL_CONTENTS *)window->mArg)->mCursor;

    sprite->mMsgID = mailGetContents(pdaMailGetMailID(cursor->y + cursor->x));
    return 0;
}

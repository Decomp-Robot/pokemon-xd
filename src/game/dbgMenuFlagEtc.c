#include <types.h>
#include <winSprite.h>
#include <menu.h>
#include <menuIds.h>
#include <menuSub.h>
#include <dbgMenu.h>
#include <GSmsg.h>
#include <GSsndPlayback.h>
#include <GSinput.h>
#include <GSflag.h>
#include <GSthread.h>
#include <gameSetup.h>
#include <floorManager.h>
#include <floor.h>
#include <heroBios.h>
#include <hero.h>
#include <savedata.h>
#include <appScriptCall.h>


#pragma force_active on


s32 _dbgMenuFlagGetMenuNum(tagWINDOW_WORK *window, s32 *maxMsgSize);
s32 dbgMenuFlagCheck(void);
void dbgMenuFlagSetEtc(s32 entry);

enum {
    FLAG_ETC_KEY_PAGE_UP = 0x04,
    FLAG_ETC_KEY_PAGE_DOWN = 0x08,
    FLAG_ETC_PAD_TRIGGER_R = 0x20,
    FLAG_ETC_MENU_WAIT = 1
};

// Follower selector values used by heroSetFollowerWithID.
enum {
    FLAG_ETC_FOLLOWER_NONE = 0,
    FLAG_ETC_FOLLOWER_MANA = 1,
    FLAG_ETC_FOLLOWER_GIRL = 2,
    FLAG_ETC_FOLLOWER_KLEIN = 3
};

// GS flags the "set etc" application updates.
#define FLAG_ETC_PROGRESS_FLAG 964
#define FLAG_ETC_SAVE_ENABLE_FLAG 990

// Message shown when a floor change cannot be started.
#define FLAG_ETC_MSG_FLOOR_BUSY \
    "このフロアからフロアチェンジはおこなえません"

// Moves the etc-flag screen cursor. The screen shows the entries in pages
// of menuDataBios rows; x is the scroll offset (the list index of the first
// visible row), and y is the row index inside the page.
void dbgMenuFlagCursor(tagWINDOW_WORK *window)
{
    const u16 keys = windowGetKeyInfo()->mKeyRepeat;
    s8 menuNum = _dbgMenuFlagGetMenuNum(window, NULL);
    s8 visibleRows = menuDataBiosGetDispMax(window->mMenuID);
    CURSOR cursor;

    if (menuNum < visibleRows) {
        visibleRows = menuNum;
    }
    cursor = window->mCursor;

    if (keys & WINDOW_KEY_UP) {
        cursor.y--;
    } else if (keys & WINDOW_KEY_DOWN) {
        cursor.y++;
    }
    if (keys & FLAG_ETC_KEY_PAGE_UP) {
        cursor.y -= visibleRows - 1;
    } else if (keys & FLAG_ETC_KEY_PAGE_DOWN) {
        cursor.y += visibleRows - 1;
    }

    if (cursor.y < 0) {
        cursor.x += cursor.y;
        cursor.y = 0;
        if (cursor.x < 0) {
            cursor.y = (s8)(visibleRows - 1);
            cursor.x = (s8)(menuNum - visibleRows);
        }
    } else if (cursor.y >= visibleRows) {
        cursor.x += cursor.y - (visibleRows - 1);
        cursor.y = (s8)(visibleRows - 1);
        if (cursor.x + cursor.y >= menuNum) {
            cursor.x = 0;
            cursor.y = 0;
        }
    }

    window->mCursor = cursor;
    window->mCursorIndex = cursor.x + cursor.y + 1;
}

// Opens the etc-flag screen or, when R is pressed, direct flag editing.
// Reports an error while the player is on a pushed floor and applies the
// selected "set etc" entry when the menu closes.
s32 dbgMenuFlagCheck(void)
{
    s32 no;
    u32 flagNo;
    s32 digit;
    u32 i;

    if (GSfloorIsFloorPushed()) {
        GSsndPlayNow(MENU_SE_ERROR);
        for (i = 0; i < 15; i++) {
            GSprint(dbgMenuGetLayerID(), 200, 240, -1, FLAG_ETC_MSG_FLOOR_BUSY);
            GSthreadSwitch();
        }
        return -1;
    }

    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & FLAG_ETC_PAD_TRIGGER_R) {
        while (1) {
            no = menuOpen(MENU_ID_DEBUG_FLAG_INFO, FLAG_ETC_MENU_WAIT);
            if (no < 0) {
                break;
            }
            flagNo = no;
            if (menuSubOpenNumberInputDigit(GSflagGet(flagNo), &digit)) {
                GSflagSet(flagNo, digit);
            }
        }
        if (no == -1) {
            menuClose(MENU_ID_DEBUG_FLAG_INFO);
            return -1;
        }
    } else {
        no = menuOpen(MENU_ID_DEBUG_FLAG_ETC, FLAG_ETC_MENU_WAIT);
        menuClose(MENU_ID_DEBUG_FLAG_ETC);
        if (no > 0) {
            dbgMenuFlagSetEtc(no);
        } else if (no == -1) {
            return -1;
        }
    }
    return 0;
}

// Applies the selected "set etc" entry: refreshes the progress GS flag,
// spawns the follower, sets the Pokedoru balance, runs the entry's common
// script and teleports to the entry's floor and landing position.
void dbgMenuFlagSetEtc(s32 entry)
{
    u32 floorID;

    GSflagSet(FLAG_ETC_PROGRESS_FLAG, flag_etc[entry].progressValue);
    GSflagSet(FLAG_ETC_SAVE_ENABLE_FLAG, 1);

    switch (flag_etc[entry].followerID) {
    case FLAG_ETC_FOLLOWER_NONE:
        heroSetFollowerWithID(FLAG_ETC_FOLLOWER_NONE, 0);
        break;
    case FLAG_ETC_FOLLOWER_MANA:
        heroSetFollowerWithID(FLAG_ETC_FOLLOWER_MANA, 0);
        break;
    case FLAG_ETC_FOLLOWER_GIRL:
        heroSetFollowerWithID(FLAG_ETC_FOLLOWER_GIRL, 0);
        break;
    case FLAG_ETC_FOLLOWER_KLEIN:
        heroSetFollowerWithID(FLAG_ETC_FOLLOWER_KLEIN, 0);
        break;
    }

    heroBiosSetPokedoru(savedataGetStatus(NULL, SAVE_DATA_HERO), flag_etc[entry].pokedoru);

    if (flag_etc[entry].scriptID != 0) {
        appScriptCommonCall(flag_etc[entry].scriptID);
    }

    floorID = flag_etc[entry].floorID;
    if (floorID == 0) {
        floorID = GSfloorGetCurrent();
    }
    floorChangePos(floorID, 0, flag_etc[entry].posX, flag_etc[entry].posY,
        flag_etc[entry].posZ);
}

// Counts the etc-flag entries that appear in the debug menu (those with a
// message id) and records the widest message among them.
s32 _dbgMenuFlagGetMenuNum(tagWINDOW_WORK *window, s32 *maxMsgSize)
{
    s32 i;
    s32 count;

    if (maxMsgSize != NULL) {
        *maxMsgSize = 0;
    }
    count = 0;
    for (i = 0; i < *flag_etc_number; i++) {
        u32 menuMsgID = flag_etc[i].menuMsgID;

        if (menuMsgID != 0 && menuMsgID != 0) {
            count++;
            if (maxMsgSize != NULL) {
                s32 size = GSmsgGetMessageSize(menuMsgID) >> 16;

                if (*maxMsgSize < size) {
                    *maxMsgSize = size;
                }
            }
        }
    }
    return count;
}
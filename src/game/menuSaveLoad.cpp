#include <gamedata.h>
#include <menu.h>
#include <fade.h>
#include <libc/memory.h>
#include <gsFlag.h>
#include "menuSaveLoad.h"
#include "floorRoom.h"
#include "menuSub.h"
#include "pokecolo.h"
#include "darkPokemon.h"
#include "heroStatus.h"
#include "hero.h"
#include "ProgressionData.h"
#include "savedata.h"
#include "memcard.h"
#include "msgctrl.h"

enum { SAVE_AVAILABILITY_FLAG = 0x3DE };

// Save/report flow. menuReport builds the report object the save-load window
// works on, runs the save flow on it and destroys it again. menuSaveLoadHook
// drives the sprites of the save report and feeds the message printer the
// values the report is showing.

// Report object of the running save flow; 0 when no report is open.
extern menuSaveload *menuSaveLoadReport;

// Optional playtime override for a report-less save.
extern unsigned long menuSaveLoadPlaytimeOverride;

extern "C" {
s32 memcardTaskCall(s32 task, s32 flag, s32 arg);
void winMsgOpen(s32 p1, s32 msgID, s32 p2, s32 p3);
void winMsgClose(s32 p1);
void winSeqSetMenu(long menuID, s32 seq);
}

// Entry points of this object: the save-flow driver ("run the save flow on
// this report"), the memory-card write behind it, and the save availability
// check.
extern "C" {
void menuSaveLoadSave(menuSaveload *report);
s32 menuSaveLoadWrite(menuSaveload *report);
s32 menuSaveLoadCanSave(void);
}

void _SaveParameterSet(void);

// The save/load window step control. The report object takes over the window
// once it exists; without it the window runs its own fade-in steps.
extern "C" void menuSaveLoadCtrl(tagWINDOW_WORK *window)
{
    if (menuSaveLoadReport != 0) {
        menuSaveLoadReport->Ctrl(window);
    } else {
        s8 state = (s8)window->mState;
        switch (state) {
        case WINDOW_STATE_INIT:
            if ((s8)window->mFlag == 0) {
                fadeSet(2, 0.5f);
                window->mFlag = 1;
            }
            break;
        case WINDOW_STATE_CLOSING:
            if ((s8)window->mFlag == 0) {
                fadeSet(3, 0.5f);
                window->mFlag = 1;
            }
            break;
        }
    }
}

// Run the save flow once on a fresh report object and release it.
extern "C" void menuReport(void)
{
    menuSaveLoadReport = new menuSaveload;
    menuSaveLoadSave(menuSaveLoadReport);
    delete menuSaveLoadReport;
    menuSaveLoadReport = 0;
}

// Sprite hook of the save report. The pokemon sprites only show once enough
// dark pokemon were met, the counter sprites hide with the flag progress and
// the write sprite runs the actual save.
extern "C" s32 menuSaveLoadHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u8 show;
    s16 item;

    if (GSflagGet(0x3C4) < worldMapScooterUnlockData[10].mRequiredProgress) {
        show = 0;
    } else {
        show = 1;
    }
    item = sprite->mItemID;
    switch (item - 0x266) {
    case 1:
    case 9:
        if (show == 0) {
            winSpriteSetDisp(sprite, 0);
        } else {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    case 5:
        if (show == 0) {
            winSpriteSetDisp(sprite, 0);
        } else {
            winSpriteSetDisp(sprite, 1);
        }
        if (menuSaveLoadReport != 0) {
            menuSaveLoadReport->SaveParameterSet();
        } else {
            _SaveParameterSet();
        }
        break;
    case 0:
    case 4:
    case 8:
        if (darkPokemonGetMeetNum() != 0) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    }
    return 0;
}

// Release the save report.
menuSaveload::~menuSaveload()
{
}

// No saved playtime until the save prompt opens.
menuSaveload::menuSaveload()
{
    mPlaytime = 0;
}

// Run the save flow on the report: ask whether to save, write the save block
// and close the report menu again.
extern "C" void menuSaveLoadSave(menuSaveload *report)
{
    u8 ok = menuSaveLoadCanSave();
    if (ok != 0) {
        menuOpen(0x64, 0);
        winMsgOpen(2, 0x3BF2, 1, 1);
        if (menuSubOpenYesNo(0, 0x1C2, 0xC0, 0) == 0) {
            u8 saved = menuSaveLoadWrite(report);
            if (saved != 0) {
                u32 savedMap = floorGetPreviousMap();
                gamedatasaveSetStatus(0, SAVE_METADATA_MAP, savedMap);
                memcardTaskCall(4, 2, 0);
            }
        }
        winMsgClose(1);
        report->_menuClose(0x64);
    } else {
        winMsgOpen(2, 0x3BE9, 1, 1);
        winMsgClose(1);
    }
}

// Write the current save block to the memory card slot and restore it, so the
// report can ask whether the write went through. Returns whether the player
// kept the save.
extern "C" s32 menuSaveLoadWrite(menuSaveload *report)
{
    s32 kept = 1;
    void *buffer = memcardMemAllocForSavedata();

    memcpy(buffer, savedataGetStatus(0, TRAINER_DATA_SAVE_BLOCK), 0x27FD0);
    if (memcardTaskCall(3, 2, 0) == 3) {
        if (gamedatasaveGetStatus(0, SAVE_METADATA_SAVE_COUNT) != 0) {
            unsigned long backupTrainerId = heroGetStatus((Hero *)savedataGetStatus(buffer, TRAINER_DATA_HERO), HERO_STATUS_TRAINER_ID, 0);
            if (backupTrainerId == heroGetStatus(0, HERO_STATUS_TRAINER_ID, 0) || gamedatasaveGetStatus(buffer, SAVE_METADATA_SAVE_COUNT) == 0) {
                report->mPlaytime = pokecoloGetPlayTimeUnsigned();
                report->SaveParameterSet();
                winMsgOpen(2, 0x3BF3, 1, 1);
                if (menuSubOpenYesNo(0, 0x1C2, 0xC0, 1) != 0) {
                    kept = 0;
                } else {
                    kept = 1;
                }
                report->mPlaytime = pokecoloGetPlayTimeUnsigned() - report->mPlaytime;
            }
        }
    }
    memcpy(savedataGetStatus(0, TRAINER_DATA_SAVE_BLOCK), buffer, 0x27FD0);
    pokecoloSetPlayTime((f64)(report->mPlaytime + pokecoloGetPlayTimeUnsigned()));
    report->mPlaytime = 0;
    memcardMemFreeForSavedata(buffer);
    return kept != 0;
}

// Fill the message printer values for a report-less save write.
void _SaveParameterSet(void)
{
    unsigned long playtime = pokecoloGetPlayTimeUnsigned();

    if (menuSaveLoadPlaytimeOverride != 0) {
        playtime = menuSaveLoadPlaytimeOverride;
    }
    msgctrlSetValue(MSG_VALUE_REPORT_PLAYTIME, playtime);
    msgctrlSetValue(MSG_VALUE_REPORT_SNAGGED_POKEMON_COUNT, darkPokemonGetSnaggedCount());
    msgctrlSetValue(MSG_VALUE_REPORT_MET_POKEMON_COUNT, darkPokemonGetMeetNum());
}

// Fill the message printer values for the report's save write.
void menuSaveload::SaveParameterSet()
{
    unsigned long playtime = pokecoloGetPlayTimeUnsigned();

    if (mPlaytime != 0) {
        playtime = mPlaytime;
    }
    msgctrlSetValue(MSG_VALUE_REPORT_PLAYTIME, playtime);
    msgctrlSetValue(MSG_VALUE_REPORT_SNAGGED_POKEMON_COUNT, darkPokemonGetSnaggedCount());
    msgctrlSetValue(MSG_VALUE_REPORT_MET_POKEMON_COUNT, darkPokemonGetMeetNum());
}

// Execution mode gates the report save-availability flag.
extern "C" s32 menuSaveLoadCanSave(void)
{
    if (pokecoloGetRomVersion() == 1) {
        return 0;
    }
    return flagGetBooleanValue(SAVE_AVAILABILITY_FLAG);
}

// Window step control of the report menu: step 0 loads the save message,
// step 3 the write message.
void menuSaveload::Ctrl(tagWINDOW_WORK *window)
{
    s8 state = (s8)window->mState;
    switch (state) {
    case WINDOW_STATE_INIT:
        if ((s8)window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x17F);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_CLOSING:
        if ((s8)window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x185);
            window->mFlag = 1;
        }
        break;
    }
}

void menuSaveload::_menuClose(long menu)
{
    menuClose(menu);
    menuCloseSync(menu, 1);
}

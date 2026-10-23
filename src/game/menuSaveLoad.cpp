#include <winMsg.h>
#include <darkPokemon.h>
#include <gamedata.h>
#include <menu.h>
#include <memcard.h>
#include <winSprite.h>
#include <fade.h>
#include <msgctrl.h>
#include <savedata.h>
#include <hero.h>
#include <floor.h>
#include <GSflag.h>
#include <pokecolo.h>
#include <string.h>
#include "menuSaveLoad.h"
#include "floorRoom.h"
#include "menuSub.h"

// Save/report flow. menuReport builds the report object the save-load window
// works on, runs the save flow on it and destroys it again. menuSaveLoadHook
// drives the sprites of the save report and feeds the message printer the
// values the report is showing.

// Report object of the running save flow; 0 when no report is open.
extern menuSaveload *menuSaveLoadReport;

// Play-time override used when no save report is open.
extern unsigned long menuSaveLoadSaveTarget;

// Save data used to check whether the scooter has been unlocked.
#include <gameSetup.h>

struct Pokemon;

extern "C" {
s32 memcardTaskCall(s32 task, s32 flag, s32 arg);
}

extern "C" s32 menuSaveLoadCanSave(void);

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
    menuSaveLoadReport->Main();
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

    if (GSflagGet(0x3C4) < worldMapScooterUnlockData[GAME_SETUP_ENTRY_REPORT_POKEMON].progressValue) {
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
        if (darkPokemonGetReliveNum() != 0) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    }
    return 0;
}

menuSaveload::~menuSaveload()
{
}

menuSaveload::menuSaveload()
{
    mPlayTime = 0;
}

// Run the save flow on the report: ask whether to save, write the save block
// and close the report menu again.
void menuSaveload::Main()
{
    u8 ok = menuSaveLoadCanSave();
    if (ok != 0) {
        menuOpen(0x64, 0);
        winMsgOpen(2, 0x3BF2, 1, 1);
        if (menuSubOpenYesNo(0, 0x1C2, 0xC0, 0) == 0) {
            if (ComfirmOverwriting()) {
                s32 floorID = floorGetNextFloorID();
                gamedatasaveSetStatus(0, 5, floorID);
                memcardTaskCall(4, 2, 0);
            }
        }
        winMsgClose(1);
        _menuClose(0x64);
    } else {
        winMsgOpen(2, 0x3BE9, 1, 1);
        winMsgClose(1);
    }
}

// Check for an existing save, then restore the live save data and play time.
bool menuSaveload::ComfirmOverwriting()
{
    s32 kept = 1;
    void *buffer = memcardMemAllocForSavedata();

    memcpy(buffer, savedataGetStatus(0, 0), 0x27FD0);
    if (memcardTaskCall(3, 2, 0) == 3) {
        if (gamedatasaveGetStatus(0, 4) != 0) {
            u32 savedTrainerRnd = heroGetStatus((Hero *)savedataGetStatus(buffer, 2), HERO_STATUS_TRAINER_RND, 0);
            if (savedTrainerRnd == heroGetStatus(0, HERO_STATUS_TRAINER_RND, 0) || gamedatasaveGetStatus(buffer, 4) == 0) {
                mPlayTime = pokecoloGetPlayTimeValue();
                SaveParameterSet();
                winMsgOpen(2, 0x3BF3, 1, 1);
                if (menuSubOpenYesNo(0, 0x1C2, 0xC0, 1) != 0) {
                    kept = 0;
                } else {
                    kept = 1;
                }
                mPlayTime = pokecoloGetPlayTimeValue() - mPlayTime;
            }
        }
    }
    memcpy(savedataGetStatus(0, 0), buffer, 0x27FD0);
    pokecoloSetPlayTime((f64)(mPlayTime + pokecoloGetPlayTimeValue()));
    mPlayTime = 0;
    memcardMemFreeForSavedata(buffer);
    return kept != 0;
}

// Fill the message printer values for a report-less save write.
void _SaveParameterSet(void)
{
    unsigned long playTime = pokecoloGetPlayTimeValue();

    if (menuSaveLoadSaveTarget != 0) {
        playTime = menuSaveLoadSaveTarget;
    }
    msgctrlSetValue(0x4C, playTime);
    msgctrlSetValue(0x34, darkPokemonGetSnatchSuccessNum());
    msgctrlSetValue(0x35, darkPokemonGetReliveNum());
}

// Fill the message printer values for the report's save write.
void menuSaveload::SaveParameterSet()
{
    unsigned long playTime = pokecoloGetPlayTimeValue();

    if (mPlayTime != 0) {
        playTime = mPlayTime;
    }
    msgctrlSetValue(0x4C, playTime);
    msgctrlSetValue(0x34, darkPokemonGetSnatchSuccessNum());
    msgctrlSetValue(0x35, darkPokemonGetReliveNum());
}

// Saving is disabled for this ROM version and until the save flag is set.
extern "C" s32 menuSaveLoadCanSave(void)
{
    if (pokecoloGetRomVersion() == 1) {
        return 0;
    }
    return GSflagTest(0x3DE);
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

#include "menuSaveLoad.h"
#include "menuSub.h"
#include "pdaMemoList.h"

// Save/report flow. menuReport builds the report object the save-load window
// works on, runs the save flow on it and destroys it again. menuSaveLoadHook
// drives the sprites of the save report and feeds the message printer the
// values the report is showing.

// Report object of the running save flow; 0 when no report is open.
extern FlashMenu *menuSaveLoadReport;

// Save target override read by the report-less write. No writer for it remains
// in this binary; it is only read here, so a nonzero value replaces the live
// target below.
extern unsigned long menuSaveLoadSaveTarget;

// Save-block pointer the world map's scooter unlock also reads. The report's
// display gate compares flag 0x3C4 against the block's field at +0x148.
extern void *worldMapScooterUnlockData;

struct Pokemon;

// Callees shared with the unrecovered save/memory-card code: the zz_ names are
// the symbols those objects still carry, called here with the roles their call
// sites show.
extern "C" {
unsigned long zz_005c0c8_(void);
s32 zz_005c228_(void);
s32 zz_01204c0_(void);
void *zz_014bb50_(void);
unsigned long zz_0157d74_(void *save, s32 slot);
void zz_0157c18_(s32 msgID, s32 flag, void *text);
void zz_01ce1b0_(void *buffer);
s32 zz_flag_get_boolean_value(s32 flag);
void *memcardMemAllocForSavedata(void);
s32 memcardTaskCall(s32 task, s32 flag, s32 arg);
void *memcpy(void *dst, const void *src, unsigned long len);
void *getTrainerDataStartOffset(void *save, s32 slot);
Pokemon *heroGetStatus(void *save, s32 flag, u16 slot);
s32 darkPokemonGetMeetNum(void);
void peopleTalkMessageBgmSetVolumeRate(f64 rate);
unsigned long GSflagGet(long flag);
s32 menuOpen(s32 menuId, s32 mode);
void winMsgOpen(s32 p1, s32 msgID, s32 p2, s32 p3);
void winMsgClose(s32 p1);
void msgctrlSetValue(long win, long value);
void winSeqSetMenu(long menuID, s32 seq);
void fadeSet(s32 p1, f32 p2);
void winSpriteSetDisp(tagSPRITE_WORK *sprite, s32 on);
}

// Entry points of this object: the save-flow driver ("run the save flow on
// this report"), the memory-card write behind it, and the save availability
// check.
extern "C" {
void menuSaveLoadSave(FlashMenu *report);
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
        case 0:
            if ((s8)window->mFlag == 0) {
                fadeSet(2, 0.5f);
                window->mFlag = 1;
            }
            break;
        case 3:
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
    menuSaveLoadReport = new GSstreamFileOutput;
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

    if (GSflagGet(0x3C4) < *(unsigned long *)((unsigned char *)worldMapScooterUnlockData + 0x148)) {
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

// FlashMenu objects own their memory; releasing the object frees it.
FlashMenu::~FlashMenu()
{
}

// The report object starts out without a save target.
GSstreamFileOutput::GSstreamFileOutput()
{
    mTarget = 0;
}

// Run the save flow on the report: ask whether to save, write the save block
// and close the report menu again.
extern "C" void menuSaveLoadSave(FlashMenu *report)
{
    u8 ok = menuSaveLoadCanSave();
    if (ok != 0) {
        menuOpen(0x64, 0);
        winMsgOpen(2, 0x3BF2, 1, 1);
        if (menuSubOpenYesNo(0, 0x1C2, 0xC0, 0) == 0) {
            u8 saved = menuSaveLoadWrite(report);
            if (saved != 0) {
                s32 text = zz_01204c0_();
                zz_0157c18_(0, 5, (void *)text);
                memcardTaskCall(4, 2, 0);
            }
        }
        winMsgClose(1);
        ((pdaMemoList *)report)->_menuClose(0x64);
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

    memcpy(buffer, getTrainerDataStartOffset(0, 0), 0x27FD0);
    if (memcardTaskCall(3, 2, 0) == 3) {
        if (zz_0157d74_(0, 4) != 0) {
            Pokemon *bufferPokemon = heroGetStatus(getTrainerDataStartOffset(buffer, 2), 2, 0);
            if (bufferPokemon == heroGetStatus(0, 2, 0) || zz_0157d74_(buffer, 4) == 0) {
                report->mTarget = zz_005c0c8_();
                report->SaveParameterSet();
                winMsgOpen(2, 0x3BF3, 1, 1);
                if (menuSubOpenYesNo(0, 0x1C2, 0xC0, 1) != 0) {
                    kept = 0;
                } else {
                    kept = 1;
                }
                report->mTarget = zz_005c0c8_() - report->mTarget;
            }
        }
    }
    memcpy(getTrainerDataStartOffset(0, 0), buffer, 0x27FD0);
    peopleTalkMessageBgmSetVolumeRate((f64)(report->mTarget + zz_005c0c8_()));
    report->mTarget = 0;
    zz_01ce1b0_(buffer);
    return kept != 0;
}

// Fill the message printer values for a report-less save write.
void _SaveParameterSet(void)
{
    unsigned long target = zz_005c0c8_();

    if (menuSaveLoadSaveTarget != 0) {
        target = menuSaveLoadSaveTarget;
    }
    msgctrlSetValue(0x4C, target);
    msgctrlSetValue(0x34, (long)zz_014bb50_());
    msgctrlSetValue(0x35, darkPokemonGetMeetNum());
}

// Fill the message printer values for the report's save write.
void menuSaveload::SaveParameterSet()
{
    unsigned long target = zz_005c0c8_();

    if (mTarget != 0) {
        target = mTarget;
    }
    msgctrlSetValue(0x4C, target);
    msgctrlSetValue(0x34, (long)zz_014bb50_());
    msgctrlSetValue(0x35, darkPokemonGetMeetNum());
}

// Save availability check behind the report: the daily check gates the flag.
extern "C" s32 menuSaveLoadCanSave(void)
{
    if (zz_005c228_() == 1) {
        return 0;
    }
    return zz_flag_get_boolean_value(0x3DE);
}

// Window step control of the report menu: step 0 loads the save message,
// step 3 the write message.
void menuSaveload::Ctrl(tagWINDOW_WORK *window)
{
    s8 state = (s8)window->mState;
    switch (state) {
    case 0:
        if ((s8)window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x17F);
            window->mFlag = 1;
        }
        break;
    case 3:
        if ((s8)window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, 0x185);
            window->mFlag = 1;
        }
        break;
    }
}

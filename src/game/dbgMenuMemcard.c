#include <types.h>
#include <GSdvd.h>
#include <GSmem.h>
#include <gamedata.h>
#include <hsd/assert.h>
#include <memcard.h>
#include <pokecolo.h>
#include <savedata.h>
#include <string.h>
#include <winMsg.h>

/* Save data swapped through the memcard task while a save is written. */
u8 *bk_savedataPtr;
/* Scratch buffer a debug save is built in. */
u8 *dbgMenuSaveData;

/* Names of the ten debug save slots: five ".DSV1" and five ".DSV2" images. */
const char *saveFileName[10] = {
    "HDSaveData1.DSV1",
    "HDSaveData2.DSV1",
    "HDSaveData3.DSV1",
    "HDSaveData4.DSV1",
    "HDSaveData5.DSV1",
    "HDSaveData_1.DSV2",
    "HDSaveData_2.DSV2",
    "HDSaveData_3.DSV2",
    "HDSaveData_4.DSV2",
    "HDSaveData_5.DSV2",
};

/* The regional builds the debug loader stamps a save for, as reported by
   pokecoloGetCountryCode(). */
enum {
    COUNTRY_JAPAN = 0,
    COUNTRY_USA = 1,
    COUNTRY_EUROPE = 2
};

/* Messages the debug memcard menu shows. */
enum {
    MSG_MEMCARD_LOAD_COMPLETED = 242
};

/* Report a failed condition together with the caller's message. */
#define ASSERTMSGLINE(line, cond, msg) \
    ((cond) || (__assert(__FILE__, line, #cond " &&\"" msg "\""), 0))

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

u8 _memcard_debug_type_check(s32 type) {
	switch (type) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return 1;
	default:
		return 0;
	}
}

#pragma peephole off

s32 dbgMenuMemcardDebugLoad(s32 menuID, s32 type)
{
    GSdvdFile *file;
    void *gamedata;

    if (GSdvdOpenTest(saveFileName[type]) != 0) {
        file = GSdvdOpen(saveFileName[type]);
        if (file == 0) {
            return 0;
        }
    } else {
        return 0;
    }
    if (_memcard_debug_type_check(type)) {
        dbgMenuSaveData = GSmemAllocAlign(163840, 32);
        GSdvdRead(file, dbgMenuSaveData, 163840, 0);
        GSdvdClose(file);
        /* The slot file carries an eight byte header before the image. */
        memcard_debug_Load(dbgMenuSaveData + 8);
        GSmemFree(dbgMenuSaveData);
        dbgMenuSaveData = 0;
    } else {
        GSdvdRead(file, now_savedata_ptr, 163776, 0);
        GSdvdClose(file);
        memcard_debug_Load2();
    }
    /* Stamp the loaded save with this build's version and area. */
    gamedata = savedataGetStatus(0, SAVE_DATA_GAME);
    switch (pokecoloGetCountryCode()) {
    case COUNTRY_JAPAN:
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_ORIGIN, GAMEDATA_ATTEST_VERSION);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_GENERATION, GAMEDATA_ATTEST_GENERATION);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_REGION, GAMEDATA_ATTEST_AREA_JAPAN);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_LANGUAGE, GAMEDATA_ATTEST_LANGAREA_JAPANESE);
        break;
    case COUNTRY_USA:
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_ORIGIN, GAMEDATA_ATTEST_VERSION);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_GENERATION, GAMEDATA_ATTEST_GENERATION);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_REGION, GAMEDATA_ATTEST_AREA_USA);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_LANGUAGE, GAMEDATA_ATTEST_LANGAREA_ENGLISH);
        break;
    case COUNTRY_EUROPE:
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_ORIGIN, GAMEDATA_ATTEST_VERSION);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_GENERATION, GAMEDATA_ATTEST_GENERATION);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_REGION, GAMEDATA_ATTEST_AREA_EUROPE);
        gamedataSetStatus(gamedata, GAMEDATA_STATUS_LANGUAGE, GAMEDATA_ATTEST_LANGAREA_EUROPE);
        break;
    }
    return 0;
}

s32 dbgMenuMemcardDebugSave(s32 menuID, s32 type)
{
    GSdvdFile *file;

    if (_memcard_debug_type_check(type)) {
        dbgMenuSaveData = GSmemAllocAlign(163840, 32);
        memcard_debug_Save(dbgMenuSaveData + 8);
        if (GSdvdOpenTest(saveFileName[type])) {
            file = GSdvdOpen(saveFileName[type]);
        } else {
            file = GSdvdCreate(saveFileName[type]);
        }
        if (file != 0) {
            GSdvdWrite(file, dbgMenuSaveData, 163840, 0);
            GSdvdClose(file);
        }
        GSmemFree(dbgMenuSaveData);
        dbgMenuSaveData = 0;
    } else {
        bk_savedataPtr = memcardMemAllocForSavedata();
        /* "out of memory" */
        ASSERTMSGLINE(501, bk_savedataPtr, "\x83\x81\x83\x82\x83\x8a\x81[\x95s\x91\xab");
        memcpy(bk_savedataPtr, savedataGetStatus(0, SAVE_DATA_IMAGE), 163792);
        if (memcardTaskCall(MEMCARD_TASK_LOAD, WINMSG_WINDOW_MENU, 0) == MEMCARD_RESULT_LOADED) {
            if (GSdvdOpenTest(saveFileName[type])) {
                file = GSdvdOpen(saveFileName[type]);
            } else {
                file = GSdvdCreate(saveFileName[type]);
            }
            if (file != 0) {
                GSdvdWrite(file, now_savedata_ptr, 163776, 0);
                GSdvdClose(file);
            }
        }
        memcpy(savedataGetStatus(0, SAVE_DATA_IMAGE), bk_savedataPtr, 163792);
        bk_savedataPtr = memcardMemFreeForSavedata(bk_savedataPtr);
        /* "free failed" */
        ASSERTMSGLINE(534, !bk_savedataPtr, "\x83\x81\x83\x82\x83\x8a\x81[\x89\xf0\x95\xfa\x8e\xb8\x94s");
    }
    return 0;
}

s32 dbgMenuMemcardDelete(void)
{
    memcardTaskCall(MEMCARD_TASK_DELETE, WINMSG_WINDOW_MENU, 0);
    return 0;
}

s32 dbgMenuMemcardLoad(void)
{
    memcardTaskCall(MEMCARD_TASK_LOAD, WINMSG_WINDOW_MENU, 0);
    winMsgOpen(WINMSG_WINDOW_MENU, MSG_MEMCARD_LOAD_COMPLETED, 1, 1);
    winMsgClose(1);
    return 0;
}

s32 dbgMenuMemcardSave(void)
{
    memcardTaskCall(MEMCARD_TASK_SAVE, WINMSG_WINDOW_MENU, 0);
    return 0;
}

#pragma peephole reset

#pragma force_active reset

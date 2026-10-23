#include <GSdvd.h>
#include <winMsg.h>
#include <pokecolo.h>
#include <memcard.h>
#include <savedata.h>
#include <GSmem.h>
#include <string.h>
#include <types.h>
#include <memcard.h>
#include <savedata.h>

/* Save data handed to the memcard task; allocated and freed per use. */
u8 *bk_savedataPtr;
/* Scratch buffer the DVD save is read into when the debug type is off. */
extern u8 *memcard_dvd_buf;
/* Scratch buffer the debug save is built in. */
u8 *dbgMenuSaveData;

/* Save-file names of the ten memcard debug slots. */
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

s32 memcardTaskCall(s32 a, s32 b, s32 c);
u8 _memcard_debug_type_check(s32 type);
void memcard_debug_Load(u8 *buf);
void memcard_debug_Load2(void);
void memcard_debug_Save(u8 *buf);
void gamedataSetStatus(void *gamedata, s32 index, s32 status);
void __assert(const char *file, s32 line, const char *cond);

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
        dbgMenuSaveData = (u8 *)GSmemAllocAlign(0x28000, 0x20);
        GSdvdRead(file, dbgMenuSaveData, 0x28000, 0);
        GSdvdClose(file);
        memcard_debug_Load(dbgMenuSaveData + 8);
        GSmemFree(dbgMenuSaveData);
        dbgMenuSaveData = 0;
    } else {
        GSdvdRead(file, memcard_dvd_buf, 0x27FC0, 0);
        GSdvdClose(file);
        memcard_debug_Load2();
    }
    gamedata = savedataGetStatus(0, 1);
    switch (pokecoloGetCountryCode()) {
    case 0:
        gamedataSetStatus(gamedata, 2, 0xB);
        gamedataSetStatus(gamedata, 3, 3);
        gamedataSetStatus(gamedata, 4, 1);
        gamedataSetStatus(gamedata, 5, 1);
        break;
    case 1:
        gamedataSetStatus(gamedata, 2, 0xB);
        gamedataSetStatus(gamedata, 3, 3);
        gamedataSetStatus(gamedata, 4, 2);
        gamedataSetStatus(gamedata, 5, 2);
        break;
    case 2:
        gamedataSetStatus(gamedata, 2, 0xB);
        gamedataSetStatus(gamedata, 3, 3);
        gamedataSetStatus(gamedata, 4, 3);
        gamedataSetStatus(gamedata, 5, 8);
        break;
    }
    return 0;
}

s32 dbgMenuMemcardDebugSave(s32 menuID, s32 type)
{
    GSdvdFile *file;

    if (_memcard_debug_type_check(type)) {
        dbgMenuSaveData = (u8 *)GSmemAllocAlign(0x28000, 0x20);
        memcard_debug_Save(dbgMenuSaveData + 8);
        if (GSdvdOpenTest(saveFileName[type])) {
            file = GSdvdOpen(saveFileName[type]);
        } else {
            file = GSdvdCreate(saveFileName[type]);
        }
        if (file != 0) {
            GSdvdWrite(file, dbgMenuSaveData, 0x28000, 0);
            GSdvdClose(file);
        }
        GSmemFree(dbgMenuSaveData);
        dbgMenuSaveData = 0;
    } else {
        bk_savedataPtr = (u8 *)memcardMemAllocForSavedata();
        if (bk_savedataPtr == 0) {
            /* CP932 message: "out of memory". */
            __assert("dbgMenuMemcard.c", 501, "bk_savedataPtr &&\"\x83\x81\x83\x82\x83\x8a\x81[\x95s\x91\xab\"");
        }
        memcpy(bk_savedataPtr, savedataGetStatus(0, 0), 0x27FD0);
        if (memcardTaskCall(2, 2, 0) == 3) {
            if (GSdvdOpenTest(saveFileName[type])) {
                file = GSdvdOpen(saveFileName[type]);
            } else {
                file = GSdvdCreate(saveFileName[type]);
            }
            if (file != 0) {
                GSdvdWrite(file, memcard_dvd_buf, 0x27FC0, 0);
                GSdvdClose(file);
            }
        }
        memcpy(savedataGetStatus(0, 0), bk_savedataPtr, 0x27FD0);
        bk_savedataPtr = (u8 *)memcardMemFreeForSavedata(bk_savedataPtr);
        if (bk_savedataPtr != 0) {
            /* CP932 message: "free failed". */
            __assert("dbgMenuMemcard.c", 534, "!bk_savedataPtr &&\"\x83\x81\x83\x82\x83\x8a\x81[\x89\xf0\x95\xfa\x8e\xb8\x94s\"");
        }
    }
    return 0;
}

s32 dbgMenuMemcardDelete(void)
{
    memcardTaskCall(0xC, 2, 0);
    return 0;
}

s32 dbgMenuMemcardLoad(void)
{
    memcardTaskCall(2, 2, 0);
    winMsgOpen(2, 0xF2, 1, 1);
    winMsgClose(1);
    return 0;
}

s32 dbgMenuMemcardSave(void)
{
    memcardTaskCall(0xB, 2, 0);
    return 0;
}

#pragma peephole reset

#pragma force_active reset

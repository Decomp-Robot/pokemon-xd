#include <types.h>

/* Save data handed to the memcard task; allocated and freed per use. */
extern u8 *bk_savedataPtr;
/* Scratch buffer the DVD save is read into when the debug type is off. */
extern u8 *memcard_dvd_buf;
/* Scratch buffer the debug save is built in. */
extern u8 *memcard_save_buf;

/* Save-file names of the ten memcard debug slots. */
const char *memcard_file_name_table[10] = {
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

u8 GSdvdOpenTest(const char *fileName);
void *GSdvdOpen(const char *fileName);
void GSdvdRead(void *file, void *buf, u32 size, s32 offset);
void GSdvdClose(void *file);
void *GSfsysStreamHeaderPostFunc(const char *fileName);
void zz_0187c74_(void *file, void *buf, u32 size, s32 offset);
void *zz_00a77b0_(u32 size, u32 align);
void GSmemFree(void *ptr);
void *memcardMemAllocForSavedata(void);
s32 memcardTaskCall(s32 a, s32 b, s32 c);
void *zz_01ce1b0_(void *ptr);
u8 _memcard_debug_type_check(s32 type);
void memcard_debug_Load(u8 *buf);
void memcard_debug_Load2(void);
void memcard_debug_Save(u8 *buf);
void *getTrainerDataStartOffset(s32 a, s32 b);
s32 zz_game_get_region(void);
void gamedataSetStatus(void *gamedata, s32 index, s32 status);
void __assert(const char *file, s32 line, const char *cond);
void *memcpy(void *dest, const void *src, u32 count);
void winMsgOpen(s32 a, s32 b, s32 c, s32 d);
void winMsgClose(s32 id);

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

#pragma peephole off

s32 zz_000b644_(s32 menuID, s32 type)
{
    void *file;
    void *gamedata;

    if (GSdvdOpenTest(memcard_file_name_table[type]) != 0) {
        file = GSdvdOpen(memcard_file_name_table[type]);
        if (file == 0) {
            return 0;
        }
    } else {
        return 0;
    }
    if (_memcard_debug_type_check(type)) {
        memcard_save_buf = (u8 *)zz_00a77b0_(0x28000, 0x20);
        GSdvdRead(file, memcard_save_buf, 0x28000, 0);
        GSdvdClose(file);
        memcard_debug_Load(memcard_save_buf + 8);
        GSmemFree(memcard_save_buf);
        memcard_save_buf = 0;
    } else {
        GSdvdRead(file, memcard_dvd_buf, 0x27FC0, 0);
        GSdvdClose(file);
        memcard_debug_Load2();
    }
    gamedata = getTrainerDataStartOffset(0, 1);
    switch (zz_game_get_region()) {
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

s32 zz_000b850_(s32 menuID, s32 type)
{
    void *file;

    if (_memcard_debug_type_check(type)) {
        memcard_save_buf = (u8 *)zz_00a77b0_(0x28000, 0x20);
        memcard_debug_Save(memcard_save_buf + 8);
        if (GSdvdOpenTest(memcard_file_name_table[type])) {
            file = GSdvdOpen(memcard_file_name_table[type]);
        } else {
            file = GSfsysStreamHeaderPostFunc(memcard_file_name_table[type]);
        }
        if (file != 0) {
            zz_0187c74_(file, memcard_save_buf, 0x28000, 0);
            GSdvdClose(file);
        }
        GSmemFree(memcard_save_buf);
        memcard_save_buf = 0;
    } else {
        bk_savedataPtr = (u8 *)memcardMemAllocForSavedata();
        if (bk_savedataPtr == 0) {
            /* CP932 message: "out of memory". */
            __assert("dbgMenuMemcard.c", 501, "bk_savedataPtr &&\"\x83\x81\x83\x82\x83\x8a\x81[\x95s\x91\xab\"");
        }
        memcpy(bk_savedataPtr, getTrainerDataStartOffset(0, 0), 0x27FD0);
        if (memcardTaskCall(2, 2, 0) == 3) {
            if (GSdvdOpenTest(memcard_file_name_table[type])) {
                file = GSdvdOpen(memcard_file_name_table[type]);
            } else {
                file = GSfsysStreamHeaderPostFunc(memcard_file_name_table[type]);
            }
            if (file != 0) {
                zz_0187c74_(file, memcard_dvd_buf, 0x27FC0, 0);
                GSdvdClose(file);
            }
        }
        memcpy(getTrainerDataStartOffset(0, 0), bk_savedataPtr, 0x27FD0);
        bk_savedataPtr = (u8 *)zz_01ce1b0_(bk_savedataPtr);
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

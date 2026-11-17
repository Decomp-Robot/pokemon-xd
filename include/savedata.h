#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <types.h>

/* Sections selected from a trainer save block. */
enum TrainerDataSection {
    TRAINER_DATA_SAVE_BLOCK = 0,
    TRAINER_DATA_GAME_DATA = 1,
    TRAINER_DATA_HERO = 2,
    TRAINER_DATA_FIGHT_RULES = 14
};

// Regions of the current save image used by menus.
enum SaveDataRegion {
    SAVE_DATA_IMAGE = 0,
    SAVE_DATA_GAME = 1,
    SAVE_DATA_HERO = 2,
    SAVE_DATA_PCBOX = 3,
    SAVE_DATA_MAIL = 10,
    SAVEDATA_BLOCK_SODATEYA = 11,
    SAVEDATA_BLOCK_MEMO = 12,
    SAVE_DATA_FIGHT_RULE = 14,
    SAVE_DATA_DARK_POKEMON = 15,
    SAVE_DATA_EXRIBBON = 16,
    SAVE_DATA_RELIVE_HALL = 21,
    SAVE_DATA_ANYWAY = 22
};

typedef struct AnywaySaveHeader {
    char signature[4];
    u32 size;
    u32 scriptStateSize;
    u32 moduleStateSize;
    u32 unknown_10;
    u32 maxSize;
    u16 unknown_18;
    u8 reserved[6];
} AnywaySaveHeader;

#ifdef __cplusplus
extern "C" {
#endif
// Save image the game is currently using.
extern u8 *now_savedata_ptr;
extern u8 pokecolo_savedata[0x287D0];
void *savedataBiosGetNowSavedataPtr(void);
void *savedataBiosGetGamedataPtr(void *save);
void *savedataBiosGetHeroPtr(void *save);
void *savedataBiosGetPcboxPtr(void *save);
void *savedataBiosGetFlagLevel0Ptr(void *save);
void *savedataBiosGetFlagLevel1Ptr(void *save);
void *savedataBiosGetFlagLevel2Ptr(void *save);
void *savedataBiosGetMailboxPtr(void *save);
void *savedataBiosGetSodateyaPtr(void *save);
void *savedataBiosGetMemoDataPtr(void *save);
void *savedataBiosGetToolPtr(void *save);
void *savedataBiosGetDarkpokemonPtr(void *save);
void *savedataBiosGetExRibbonPtr(void *save);
void *savedataBiosGetFlagSavedataPtr(void *save);
void *savedataBiosGetYobi1Ptr(void *save);
void *savedataBiosGetYobi2Ptr(void *save);
void *savedataBiosGetYobi3Ptr(void *save);
void *savedataBiosGetYobi4Ptr(void *save);
void *savedataBiosGetRelivePtr(void *save);
void *savedataBiosGetAnywaySavePtr(void *save);
// A null image selects the live save data.
void *savedataGetStatus(void *saveData, u16 region);
// Marks the battle disk at index as sold out, so it cannot be sold again.
void savedataSetBattleDiskSellOut(u32 index);
// Whether the battle disk at index was already sold out.
u32 savedataGetBattleDiskSellOut(u32 index);
// Reinitializes an existing save image in place.
void savedataInit(void *saveData);
// Builds a new save image from the current game data.
void savedataCreate(void *saveData, u16 *name);
#ifdef __cplusplus
}
#endif

#endif

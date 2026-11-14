#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <types.h>

typedef struct GamedataAttest {
    u8 versionId;
    u8 generationId;
    u8 areaId;
    u8 langareaId;
} GamedataAttest;

typedef struct GamedataSaveMetadata {
    u64 memcardID;
    u32 savecount;
    u32 savernd;
    u32 optionAudioWord;
    u32 floorid;
    f32 unknownFloat;
    u32 prevfloorid;
    u8 floorposindex;
    u8 optionNoVibration;
    u8 optionAudio;
    u8 parChecksum;
    f64 playtime;
    u32 sysHeadChecksum;
} GamedataSaveMetadata;

typedef struct GamedataPlayData {
    u16 zukanFlags;
} GamedataPlayData;

typedef struct GamedataSave {
    GamedataAttest attest;
    GamedataSaveMetadata metadata;
    GamedataPlayData playdata;
} GamedataSave;

enum {
    GAMEDATA_ZUKAN_FLAG_BITS = 16
};

enum GAMEDATA_STATUS_FIELD {
    GAMEDATA_STATUS_ATTEST = 1,
    GAMEDATA_STATUS_ORIGIN = 2,
    GAMEDATA_STATUS_GENERATION = 3,
    GAMEDATA_STATUS_REGION = 4,
    GAMEDATA_STATUS_LANGUAGE = 5
};
/* Version and generation of the game that wrote a save's attest block. */
enum {
    GAMEDATA_ATTEST_VERSION = 11,
    GAMEDATA_ATTEST_GENERATION = 3
};

/* Area a save was written in. */
enum GamedataAttestArea {
    GAMEDATA_ATTEST_AREA_JAPAN = 1,
    GAMEDATA_ATTEST_AREA_USA = 2,
    GAMEDATA_ATTEST_AREA_EUROPE = 3
};

/* Language a save was written in. */
enum GamedataAttestLangarea {
    GAMEDATA_ATTEST_LANGAREA_JAPANESE = 1,
    GAMEDATA_ATTEST_LANGAREA_ENGLISH = 2,
    GAMEDATA_ATTEST_LANGAREA_EUROPE = 8
};

enum GAMEDATASAVE_FIELD {
    GAMEDATASAVE_FIELD_WORLD_MAP_FLOOR = 5,
    GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION = 9
};

/* Word-valued fields in the save block's system metadata. */
enum SaveMetadataField {
    SAVE_METADATA_SAVE_COUNT = 4,
    SAVE_METADATA_MAP = 5
};

#ifdef __cplusplus
extern "C" {
#endif
GamedataAttest *gamedataBiosGetGamedataAtttestPtr(void *gamedata);
void gamedataBiosSetZukanFLag(GamedataPlayData *playdata, u16 index, u16 value);
u8 gamedataBiosGetZukanFlag(GamedataPlayData *playdata, u16 index);
GamedataPlayData *gamedatasaveBiosGetplaydata(GamedataSave *save);
void gamedatasaveBiosSetSysHeadChecksum(GamedataSaveMetadata *metadata, u32 checksum);
u32 gamedatasaveBiosGetSysHeadChecksum(GamedataSaveMetadata *metadata);
void gamedatasaveBiosSetParChecksum(GamedataSaveMetadata *metadata, u8 checksum);
u8 gamedatasaveBiosGetParChecksum(GamedataSaveMetadata *metadata);
void gamedatasaveBiosSetOptionAudio(GamedataSaveMetadata *metadata, u32 value);
u32 gamedatasaveBiosGetOptionAudioWord(GamedataSaveMetadata *metadata);
void gamedatasaveBiosSetOptionAudioByte(GamedataSaveMetadata *metadata, u8 value);
void gamedatasaveBiosSetOptionNoVibration(GamedataSaveMetadata *metadata, u8 value);
void gamedatasaveBiosSetFloorposindex(GamedataSaveMetadata *metadata, u8 index);
void gamedatasaveBiosSetPrevfloorid(GamedataSaveMetadata *metadata, u32 floorid);
void gamedatasaveBiosSetPlaytime(GamedataSaveMetadata *metadata, f64 playtime);
void gamedatasaveBiosSetFloorid(GamedataSaveMetadata *metadata, u32 floorid);
void gamedatasaveBiosSetSavecount(GamedataSaveMetadata *metadata, u32 count);
void gamedatasaveBiosSetMemcardID(GamedataSaveMetadata *metadata, u64 memcardID);
void gamedatasaveBiosSetSavernd(GamedataSaveMetadata *metadata, u32 value);
u8 gamedatasaveBiosGetOptionAudio(GamedataSaveMetadata *metadata);
u8 gamedatasaveBiosGetOptionNoVibration(GamedataSaveMetadata *metadata);
u8 gamedatasaveBiosGetFloorposindex(GamedataSaveMetadata *metadata);
u32 gamedatasaveBiosGetPrevfloorid(GamedataSaveMetadata *metadata);
f64 gamedatasaveBiosGetPlaytime(GamedataSaveMetadata *metadata);
u32 gamedatasaveBiosGetFloorid(GamedataSaveMetadata *metadata);
u32 gamedatasaveBiosGetSavecount(GamedataSaveMetadata *metadata);
u64 gamedatasaveBiosGetMemcardID(GamedataSaveMetadata *metadata);
u32 gamedatasaveBiosGetSavernd(GamedataSaveMetadata *metadata);
void gamedatasaveBiosSetPtr(GamedataSave *save, GamedataSaveMetadata *metadata);
GamedataSaveMetadata *gamedatasaveBiosGetPtr(GamedataSave *save);
void gamedatasaveInit(GamedataSave *save);
// A null save selects the current game data.
unsigned long gamedatasaveGetStatus(void *save, int field);
void gamedatasaveSetStatus(void *save, int field, unsigned long value);
void gamedatasaveSetDataByIndex(void *save, s32 field, u32 value);
void *gamedataGetStatus(void *gamedata, u16 field);
// Fields 2 to 5 replace one byte of the attest block, field 1 the whole block.
void gamedataSetStatus(void *gamedata, u16 field, u32 status);
void gamedataAttestCreate(GamedataAttest *attest, u8 versionId, u8 generationId, u8 areaId, u8 langareaId);
u8 gamedataAttestCheckValid(GamedataAttest *attest);
u8 gamedataAttestBiosGetLangareaId(GamedataAttest *attest);
u8 gamedataAttestBiosGetVerId(GamedataAttest *attest);
#ifdef __cplusplus
}
#endif

#endif

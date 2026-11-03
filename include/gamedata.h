#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <types.h>

typedef struct GamedataAttest {
    u8 versionId;
    u8 generationId;
    u8 areaId;
    u8 langareaId;
} GamedataAttest;

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
#ifdef __cplusplus
}
#endif

#endif

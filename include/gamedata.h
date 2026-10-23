#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <types.h>

enum GAMEDATA_STATUS_FIELD { GAMEDATA_STATUS_ATTEST = 1, GAMEDATA_STATUS_ORIGIN = 2 };
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
void *gamedataGetStatus(void *gamedata, int field);
#ifdef __cplusplus
}
#endif

#endif

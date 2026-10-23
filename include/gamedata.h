#ifndef GAMEDATA_H
#define GAMEDATA_H

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
#ifdef __cplusplus
}
#endif

#endif

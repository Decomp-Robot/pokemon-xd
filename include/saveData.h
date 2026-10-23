#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include <types.h>

/* Sections selected from a trainer save block. */
enum TrainerDataSection {
    TRAINER_DATA_SAVE_BLOCK = 0,
    TRAINER_DATA_GAME_DATA = 1,
    TRAINER_DATA_HERO = 2,
    TRAINER_DATA_FIGHT_RULES = 14
};

/* Word-valued fields in the save block's system metadata. */
enum SaveMetadataField {
    SAVE_METADATA_SAVE_COUNT = 4,
    SAVE_METADATA_MAP = 5
};
#ifdef __cplusplus
extern "C" {
#endif
/* A null base selects the active save block. */
void *getTrainerDataStartOffset(void *base, u16 selector);
unsigned long saveDataGetMetadata(void *save, int field);
void saveDataSetMetadata(void *save, int field, unsigned long value);
#ifdef __cplusplus
}
#endif
#endif

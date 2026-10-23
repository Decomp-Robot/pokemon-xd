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
    SAVE_DATA_FIGHT_RULE = 0xE,
    SAVEDATA_BLOCK_MEMO = 0xC
};

#ifdef __cplusplus
extern "C" {
#endif
// A null image selects the live save data.
void *savedataGetStatus(void *saveData, u16 region);
#ifdef __cplusplus
}
#endif

#endif

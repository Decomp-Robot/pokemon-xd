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
    SAVEDATA_BLOCK_SODATEYA = 11,
    SAVEDATA_BLOCK_MEMO = 12,
    SAVE_DATA_FIGHT_RULE = 14
};

#ifdef __cplusplus
extern "C" {
#endif
// A null image selects the live save data.
void *savedataGetStatus(void *saveData, u16 region);
// Builds a new save image from the current game data.
void savedataCreate(s32 mode, u16 *name);
#ifdef __cplusplus
}
#endif

#endif

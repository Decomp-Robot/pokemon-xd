#ifndef GAME_SETUP_H
#define GAME_SETUP_H

#include <types.h>

// Setup entries used for progress, follower, money, script and room selection.
// Every entry keeps the progress requirement it gates a GS flag against.
typedef struct GameSetupEntry {
    u8 followerID; // Follower to spawn (heroSetFollowerWithID); 0-3.
    u8 _01;
    u16 floorID; // Floor the entry teleports to; 0 keeps the current floor.
    u16 pokedoru; // Pokedoru the entry grants the hero.
    u8 _06[2];
    u32 progressValue; // Progress requirement of the entry.
    u32 scriptID; // Common script the entry calls; 0 for none.
    f32 posX; // Landing position passed to floorChangePos.
    f32 posY;
    f32 posZ;
    u32 menuMsgID; // Debug-menu message id; 0 hides the entry.
} GameSetupEntry;

// Known entries of the setup table, named by what their progress gates.
enum GameSetupEntryID {
    GAME_SETUP_ENTRY_REPORT_POKEMON = 10, // dark-pokemon progress the save report needs
    GAME_SETUP_ENTRY_SCOOTER_UNLOCK = 68, // progress the special scooters need
};

// Number of entries in the setup table.
extern u32 *flag_etc_number;

extern GameSetupEntry *flag_etc;

#endif

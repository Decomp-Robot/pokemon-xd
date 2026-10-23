#ifndef GAME_SETUP_H
#define GAME_SETUP_H

#include <types.h>

// Setup entries used for progress, follower, money, script and room selection.
// Every entry keeps the progress requirement it gates a GS flag against at
// +0x08 (dbgMenuFlagSetEtc reads entry index << 5 +8 to update its flags).
struct GameSetupEntry {
    u8 _00[0x08]; // 0x00-0x07: unknown.
    u32 progressValue; // 0x08: progress requirement of the entry.
    u8 _0C[0x14]; // 0x0C-0x1F: unknown.
};

// Known entries of the setup table, named by what their progress gates.
enum GameSetupEntryID {
    GAME_SETUP_ENTRY_REPORT_POKEMON = 10, // dark-pokemon progress the save report needs
    GAME_SETUP_ENTRY_SCOOTER_UNLOCK = 68, // progress the special scooters need
};

extern GameSetupEntry *flag_etc;

#endif

#ifndef FIGHT_TRAINER_H
#define FIGHT_TRAINER_H

#include <hero.h>
#include <types.h>

typedef struct FIGHT_TRAINER FIGHT_TRAINER;

// FIGHT_TRAINER -- one battle trainer record of the MenuCBTaisenWork work
// area: the fight trainer data IDs it plays, the home place the battle-start
// screen shows, the versus battle ID the record was booked under, and the two
// hero bios snapshots (the entered hero and the hero the battle is fought
// with). One record is 0x1320 bytes.
//
// The record head doubles as a Pokemon record: its held-item slot at 0x02
// keeps the trainer data ID the record resets to, and the bios module reads
// and writes that slot through the Pokemon accessors. The fight-side half of
// the record belongs to the fight module and stays undeclared here.
struct FIGHT_TRAINER {
    u16 mDataId; // 0x00: fight trainer data ID the record is set to.
    u16 mResetDataId; // 0x02: reset data ID, kept in the head's Pokemon item slot.
    s32 mHomePlace; // 0x04: home place of the record's hero.
    s32 mEntryList[6]; // 0x08-0x1F: party slot list of the entry screen.
    s32 mEntryCount; // 0x20: slots of the list that are in use.
    s32 mCatchTrainerRnd; // 0x24: controller slot the record's hero plays on,
                          // kept in the head's Pokemon catch-trainer slot.
    s32 mVersusBattleId; // 0x28: versus battle ID the record is booked under
                         // (menuCBBios_GetTrainerFromVersusBattleID).
    Hero mHero; // 0x2C: hero the player entered with.
    Hero mBattleHero; // 0x9A4: hero the battle is fought with.
    u8 _131C[0x1320 - 0x131C]; // 0x131C-0x131F: unknown.
};

#endif

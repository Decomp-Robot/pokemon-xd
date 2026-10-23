#ifndef MENU_CB_TRAINER_H
#define MENU_CB_TRAINER_H

#include <hero.h>
#include <types.h>

enum MenuCBPlayerTrainerID {
    MENU_CB_TRAINER_DEFAULT_PLAYER = 0x1388,
    MENU_CB_TRAINER_HOME_1_MALE = 0x1389,
    MENU_CB_TRAINER_HOME_1_FEMALE = 0x138A,
    MENU_CB_TRAINER_HOME_2_MALE = 0x138B,
    MENU_CB_TRAINER_HOME_2_FEMALE = 0x138C,
    MENU_CB_TRAINER_HOME_3_MALE = 0x138D,
    MENU_CB_TRAINER_HOME_3_FEMALE = 0x138E,
};

// Connect-battle trainer entry: the fight trainer data IDs it plays, the home place the battle-start
// screen shows, the versus battle ID the record was booked under, and the two
// hero bios snapshots (the entered hero and the hero the battle is fought
// with). One record is 0x1320 bytes.
struct _MENUCBTRAINER {
#ifdef __cplusplus
    _MENUCBTRAINER() {}
#endif
    u16 mDataId; // 0x00: fight trainer data ID the record is set to.
    u16 mResetDataId; // 0x02: trainer data ID used when resetting the record.
    s32 mHomePlace; // 0x04: home place of the record's hero.
    s32 mEntryList[6]; // 0x08-0x1F: party slot list of the entry screen.
    s32 mEntryCount; // 0x20: slots of the list that are in use.
    s32 mControllerId; // 0x24: controller slot the record's hero plays on.
    s32 mVersusBattleId; // 0x28: versus battle ID the record is booked under
                         // (menuCBBios_GetTrainerFromVersusBattleID).
    Hero mHero; // 0x2C: hero the player entered with.
    Hero mBattleHero; // 0x9A4: hero the battle is fought with.
    u8 _131C[0x1320 - 0x131C]; // 0x131C-0x131F: unknown.
};

#endif

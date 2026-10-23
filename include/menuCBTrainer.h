#ifndef MENU_CB_TRAINER_H
#define MENU_CB_TRAINER_H

#include <menuCBBios.h>

// Player ID a connect-battle trainer record is booked under. The IDs of the
// home-town trainers follow the default record.
enum MenuCBPlayerTrainerID {
    MENU_CB_TRAINER_DEFAULT_PLAYER = 0x1388,
    MENU_CB_TRAINER_HOME_1_MALE = 0x1389,
    MENU_CB_TRAINER_HOME_1_FEMALE = 0x138A,
    MENU_CB_TRAINER_HOME_2_MALE = 0x138B,
    MENU_CB_TRAINER_HOME_2_FEMALE = 0x138C,
    MENU_CB_TRAINER_HOME_3_MALE = 0x138D,
    MENU_CB_TRAINER_HOME_3_FEMALE = 0x138E,
};

#endif

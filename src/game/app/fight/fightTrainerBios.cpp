#include "app/fight/fightTrainerBios.h"

// BGM sound id of the trainer kind.
u32 fightTrainerKindDataBiosGetBgmSndId(void *kindData) {
    FIGHT_TRAINER_KIND_DATA *kindDataRecord = static_cast<FIGHT_TRAINER_KIND_DATA *>(kindData);

    if (kindDataRecord == 0) {
        return 0;
    }
    return kindDataRecord->mBgmSndId;
}

// Message id of the name the trainer class is displayed with.
u32 fightTrainerKindDataBiosGetPrefixName(void *kindData) {
    FIGHT_TRAINER_KIND_DATA *kindDataRecord = static_cast<FIGHT_TRAINER_KIND_DATA *>(kindData);

    if (kindDataRecord == 0) {
        return 0;
    }
    return kindDataRecord->mPrefixName;
}

// Prize-money multiplier of the trainer kind.
u16 fightTrainerKindDataBiosGetSyoukinBairitu(void *kindData) {
    FIGHT_TRAINER_KIND_DATA *kindDataRecord = static_cast<FIGHT_TRAINER_KIND_DATA *>(kindData);

    if (kindDataRecord == 0) {
        return 0;
    }
    return kindDataRecord->mSyoukinBairitu;
}

// Record of the given trainer kind; null when the loaded table holds no record
// for that kind.
void *fightTrainerKindDataBiosGetPtr(u16 kindId) {
    if (kindId >= *fight_trainer_kind_data_number) {
        return 0;
    }
    return &fight_trainer_kind_data[kindId];
}

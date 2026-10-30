#include <app/fight/fightType.h>

// One entry of the fight type table: the number of trainers a side fields, the
// number of Pokemon it enters the fight with, the number of Pokemon it keeps
// out at once and the name the fight type is displayed with.
typedef struct FIGHT_TYPE_DATA {
    u8 mTrainerNum;
    u8 mEntryPokemonNum;
    u8 mFightoutPokemonNum;
    u32 mName;
} FIGHT_TYPE_DATA;

extern "C" {
// The fight type table loaded for the running fight and the number of records
// it holds.
extern u32 *fight_type_data_number;
extern FIGHT_TYPE_DATA *fight_type_data;

u16 fightTypeGetFightSideFightOutPokemonMax(long fightType);
u32 fightTypeDataBiosGetName(void *fightTypeData);
}

// How many Pokemon one side of the fight keeps out at once: every trainer of
// that side keeps its own Pokemon out.
u16 fightTypeGetFightSideFightOutPokemonMax(long fightType) {
    u8 trainerNum = fightTypeDataBiosGetTrainerNum(fightTypeDataBiosGetPtr(fightType));
    u8 fightoutPokemonNum = fightTypeDataBiosGetFightoutPokemonNum(fightTypeDataBiosGetPtr(fightType));
    return trainerNum * fightoutPokemonNum;
}

// Number of Pokemon the fight type keeps out at once.
u8 fightTypeDataBiosGetFightoutPokemonNum(void *fightTypeData) {
    FIGHT_TYPE_DATA *fightTypeDataRecord = static_cast<FIGHT_TYPE_DATA *>(fightTypeData);

    if (fightTypeDataRecord == 0) {
        return 0;
    }
    return fightTypeDataRecord->mFightoutPokemonNum;
}

// Number of Pokemon a side enters the fight with.
u8 fightTypeDataBiosGetEntryPokemonNum(void *fightTypeData) {
    FIGHT_TYPE_DATA *fightTypeDataRecord = static_cast<FIGHT_TYPE_DATA *>(fightTypeData);

    if (fightTypeDataRecord == 0) {
        return 0;
    }
    return fightTypeDataRecord->mEntryPokemonNum;
}

// Number of trainers one side of the fight fields.
u8 fightTypeDataBiosGetTrainerNum(void *fightTypeData) {
    FIGHT_TYPE_DATA *fightTypeDataRecord = static_cast<FIGHT_TYPE_DATA *>(fightTypeData);

    if (fightTypeDataRecord == 0) {
        return 0;
    }
    switch (fightTypeDataRecord->mTrainerNum) {
    case 0:
        return 1;
    case 1:
        return 1;
    case 2:
        return 2;
    }
    return 0;
}

// Name the fight type is displayed with.
u32 fightTypeDataBiosGetName(void *fightTypeData) {
    FIGHT_TYPE_DATA *fightTypeDataRecord = static_cast<FIGHT_TYPE_DATA *>(fightTypeData);

    if (fightTypeDataRecord == 0) {
        return 0;
    }
    return fightTypeDataRecord->mName;
}

// Record of a fight type, or null when the fight has no such type.
void *fightTypeDataBiosGetPtr(long type) {
    if ((u16)type > *fight_type_data_number) {
        return 0;
    }
    return &fight_type_data[(u16)type];
}

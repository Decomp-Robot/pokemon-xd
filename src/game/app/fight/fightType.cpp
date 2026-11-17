#include <app/fight/fightType.h>

extern "C" {
// The fight type table loaded for the running fight and the number of records
// it holds.
extern u32 *fight_type_data_number;
extern FIGHT_TYPE_DATA *fight_type_data;

u16 fightTypeGetFightSideFightOutPokemonMax(u16 fightType);
u32 fightTypeDataBiosGetName(FIGHT_TYPE_DATA *fightTypeData);
}

// How many Pokemon one side of the fight keeps out at once: every trainer of
// that side keeps its own Pokemon out.
u16 fightTypeGetFightSideFightOutPokemonMax(u16 fightType) {
    u8 trainerNum = fightTypeDataBiosGetTrainerNum(fightTypeDataBiosGetPtr(fightType));
    u8 fightoutPokemonNum = fightTypeDataBiosGetFightoutPokemonNum(fightTypeDataBiosGetPtr(fightType));
    return trainerNum * fightoutPokemonNum;
}

// Number of Pokemon the fight type keeps out at once.
u8 fightTypeDataBiosGetFightoutPokemonNum(FIGHT_TYPE_DATA *fightTypeData) {
    FIGHT_TYPE_DATA *fightTypeDataRecord = fightTypeData;

    if (fightTypeDataRecord == 0) {
        return 0;
    }
    return fightTypeDataRecord->mFightoutPokemonNum;
}

// Number of Pokemon a side enters the fight with.
u8 fightTypeDataBiosGetEntryPokemonNum(FIGHT_TYPE_DATA *fightTypeData) {
    FIGHT_TYPE_DATA *fightTypeDataRecord = fightTypeData;

    if (fightTypeDataRecord == 0) {
        return 0;
    }
    return fightTypeDataRecord->mEntryPokemonNum;
}

// Number of trainers one side of the fight fields.
u8 fightTypeDataBiosGetTrainerNum(FIGHT_TYPE_DATA *fightTypeData) {
    FIGHT_TYPE_DATA *fightTypeDataRecord = fightTypeData;

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
u32 fightTypeDataBiosGetName(FIGHT_TYPE_DATA *fightTypeData) {
    FIGHT_TYPE_DATA *fightTypeDataRecord = fightTypeData;

    if (fightTypeDataRecord == 0) {
        return 0;
    }
    return fightTypeDataRecord->mName;
}

// Record of a fight type, or null when the fight has no such type.
FIGHT_TYPE_DATA *fightTypeDataBiosGetPtr(u16 type) {
    if (type > *fight_type_data_number) {
        return 0;
    }
    return &fight_type_data[type];
}

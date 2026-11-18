#include "app/fight/fightTrainerBios.h"
#include "deck.h"
#include "fightTrainer.h"

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

void fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag) {
    if (pokemon == 0) {
        return;
    }
    pokemon->param1bantakaiFlag = flag;
}

void fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(FightTrainerEnemyPokemon *pokemon, u8 flag) {
    if (pokemon == 0) {
        return;
    }
    pokemon->badwazaHaveFlag = flag;
}

void fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag) {
    if (pokemon == 0) {
        return;
    }
    pokemon->defense1banhikuiFlag = flag;
}

void fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag) {
    if (pokemon == 0) {
        return;
    }
    pokemon->lv1banhikuiFlag = flag;
}

void fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag) {
    if (pokemon == 0) {
        return;
    }
    pokemon->nowhp1banhikuiFlag = flag;
}

void fightTrainerEnemyPokemonBiosSetStoreTokuseiData(FightTrainerEnemyPokemon *pokemon, u16 data) {
    if (pokemon == 0) {
        return;
    }
    pokemon->storeTokuseiData = data;
}

void fightTrainerEnemyPokemonBiosSetTokuseiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag) {
    if (pokemon == 0) {
        return;
    }
    pokemon->tokuseiFlag = flag;
}

void fightTrainerEnemyPokemonBiosSetExpectWazaDataId(FightTrainerEnemyPokemon *pokemon, u8 index,
                                                     u16 wazaDataId) {
    if (pokemon == 0) {
        return;
    }
    if (index > sizeof(pokemon->expectWazaDataId) / sizeof(pokemon->expectWazaDataId[0])) {
        return;
    }
    pokemon->expectWazaDataId[index] = wazaDataId;
}

void fightTrainerEnemyPokemonBiosSetStoreWazaDataId(FightTrainerEnemyPokemon *pokemon, u8 index,
                                                    u16 wazaDataId) {
    if (pokemon == 0) {
        return;
    }
    if (index > sizeof(pokemon->storeWazaDataId) / sizeof(pokemon->storeWazaDataId[0])) {
        return;
    }
    pokemon->storeWazaDataId[index] = wazaDataId;
}

void fightTrainerEnemyPokemonBiosSetExpectWazaNum(FightTrainerEnemyPokemon *pokemon, u8 wazaNum) {
    if (pokemon == 0) {
        return;
    }
    pokemon->expectWazaNum = wazaNum;
}

void fightTrainerEnemyPokemonBiosSetStoreWazaNum(FightTrainerEnemyPokemon *pokemon, u8 wazaNum) {
    if (pokemon == 0) {
        return;
    }
    pokemon->storeWazaNum = wazaNum;
}

void fightTrainerEnemyPokemonBiosSetFightEntryeId(FightTrainerEnemyPokemon *pokemon,
                                                  s16 fightEntryeId) {
    if (pokemon == 0) {
        return;
    }
    pokemon->fightEntryeId = fightEntryeId;
}

s16 fightTrainerEnemyPokemonBiosGetFightEntryeId(FightTrainerEnemyPokemon *pokemon) {
    if (pokemon == 0) {
        return 0;
    }
    return pokemon->fightEntryeId;
}

FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *fightTrainerAiValueAddsubDataBiosGetPtr(u16 index) {
    if (index >= *fight_trainer_ai_value_addsub_data_number) {
        return 0;
    }
    return &fight_trainer_ai_value_addsub_data[index];
}

FIGHT_TRAINER_POKEMON_PART_DATA *fightTrainerPokemonPartDataBiosGetPtr(u16 index) {
    if (index >= *fight_trainer_pokemon_part_data_number) {
        return 0;
    }
    return &fight_trainer_pokemon_part_data[index];
}

FIGHT_TRAINER_AI_DATA *fightTrainerAiDataBiosGetPtr(u16 index) {
    return reinterpret_cast<FIGHT_TRAINER_AI_DATA *>(DeckTrainerAI::getDeckTrainerAIObj(index));
}

DeckTrainer *fightTrainerDataBiosGetPtr(u16 trainerDataId) {
    enum VersusTrainerDataId {
        VS_PLAYER_TRAINER_DATA_ID = 5000,
        VS_TRAINER_DATA_ID_1 = 5001,
        VS_TRAINER_DATA_ID_2 = 5002,
        VS_TRAINER_DATA_ID_3 = 5003,
        VS_TRAINER_DATA_ID_4 = 5004,
        VS_TRAINER_DATA_ID_5 = 5005,
        VS_TRAINER_DATA_ID_6 = 5006
    };
    enum VersusTrainerDeckIndex { VS_DECK_INDEX_PLAYER = 1, VS_DECK_INDEX_MALE = 2, VS_DECK_INDEX_FEMALE = 3 };
    u16 deckIndex = trainerDataId;
    switch (trainerDataId) {
    case VS_PLAYER_TRAINER_DATA_ID:
        deckIndex = VS_DECK_INDEX_PLAYER;
        break;
    case VS_TRAINER_DATA_ID_1:
    case VS_TRAINER_DATA_ID_3:
    case VS_TRAINER_DATA_ID_5:
        deckIndex = VS_DECK_INDEX_MALE;
        break;
    case VS_TRAINER_DATA_ID_2:
    case VS_TRAINER_DATA_ID_4:
    case VS_TRAINER_DATA_ID_6:
        deckIndex = VS_DECK_INDEX_FEMALE;
        break;
    }
    return DeckTrainer::getDeckTrainerObj(deckIndex);
}

extern "C" u8 fightTrainerBiosGetControllerId(FIGHT_TRAINER *trainer) {
    if (trainer == 0) {
        return 0;
    }
    return trainer->mControllerId;
}

FightPokemon *fightTrainerBiosGetFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot) {
    if (trainer == 0) {
        return 0;
    }
    if (slot >= 6) {
        return 0;
    }
    return &trainer->mFightPokemon[slot];
}

Hero *fightTrainerBiosGetHeroPtr(FIGHT_TRAINER *trainer) {
    if (trainer == 0) {
        return 0;
    }
    return &trainer->mHero;
}

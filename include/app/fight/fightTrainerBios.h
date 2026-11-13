#ifndef APP_FIGHT_FIGHTTRAINERBIOS_H
#define APP_FIGHT_FIGHTTRAINERBIOS_H

#include <types.h>

// One enemy Pokemon of a trainer. The fight status block describes the moves it
// has and expects to use. The fight-out flags are per-battle comparisons against
// the rest of the trainer's team.
typedef struct FightTrainerEnemyPokemon {
    s16 fightEntryeId;  // negative while the slot is unused
    u8 storeWazaNum;    // stored move slots in use
    u8 expectWazaNum;   // expected move slots in use
    u16 storeWazaDataId[4];
    u16 expectWazaDataId[8];
    u8 tokuseiFlag;
    u16 storeTokuseiData;
    u8 nowhp1banhikuiFlag;      // has the lowest current HP
    u8 lv1banhikuiFlag;         // has the lowest level
    u8 defense1banhikuiFlag;    // has the lowest defense
    u8 badwazaHaveFlag;         // holds a bad move
    u8 param1bantakaiFlag;      // has the highest parameter
} FightTrainerEnemyPokemon;

// One entry of the trainer kind table: the prize-money multiplier of the kind,
// the message id the trainer class is named with and the BGM sound id played
// for the kind.
typedef struct FIGHT_TRAINER_KIND_DATA {
    u16 mSyoukinBairitu;
    u8 mUnknown[2];
    u32 mPrefixName;
    u32 mBgmSndId;
} FIGHT_TRAINER_KIND_DATA;

#ifdef __cplusplus
extern "C" {
#endif

// The trainer kind table loaded for the running fight and the number of records
// it holds.
extern u32 *fight_trainer_kind_data_number;
extern FIGHT_TRAINER_KIND_DATA *fight_trainer_kind_data;

void *fightTrainerDataBiosGetPtr(u32 trainerDataId);

u32 fightTrainerKindDataBiosGetBgmSndId(void *kindData);
u32 fightTrainerKindDataBiosGetPrefixName(void *kindData);
u16 fightTrainerKindDataBiosGetSyoukinBairitu(void *kindData);
void *fightTrainerKindDataBiosGetPtr(u16 kindId);

void fightTrainerEnemyPokemonBiosSetFightEntryeId(FightTrainerEnemyPokemon *pokemon,
                                                 s16 fightEntryeId);
s16 fightTrainerEnemyPokemonBiosGetFightEntryeId(FightTrainerEnemyPokemon *pokemon);
void fightTrainerEnemyPokemonBiosSetStoreWazaNum(FightTrainerEnemyPokemon *pokemon, u8 wazaNum);
void fightTrainerEnemyPokemonBiosSetExpectWazaNum(FightTrainerEnemyPokemon *pokemon, u8 wazaNum);
void fightTrainerEnemyPokemonBiosSetStoreWazaDataId(FightTrainerEnemyPokemon *pokemon, u8 index,
                                                   u16 wazaDataId);
void fightTrainerEnemyPokemonBiosSetExpectWazaDataId(FightTrainerEnemyPokemon *pokemon, u8 index,
                                                    u16 wazaDataId);
void fightTrainerEnemyPokemonBiosSetTokuseiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag);
void fightTrainerEnemyPokemonBiosSetStoreTokuseiData(FightTrainerEnemyPokemon *pokemon, u16 data);
void fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag);
void fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag);
void fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag);
void fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(FightTrainerEnemyPokemon *pokemon, u8 flag);
void fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(FightTrainerEnemyPokemon *pokemon, u8 flag);

#ifdef __cplusplus
}
#endif

#endif

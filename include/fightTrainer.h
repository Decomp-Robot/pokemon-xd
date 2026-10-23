#ifndef FIGHT_TRAINER_H
#define FIGHT_TRAINER_H

enum { FIGHT_TRAINER_STATUS_TYPE = 0x39 };

#include <hero.h>
#include <types.h>

typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;
typedef struct FIGHT_TRAINER FIGHT_TRAINER;

// Battle-side trainer record: the data IDs the trainer plays, the hero record
// the battle is fought with, and the party and action work that follows it.
#ifdef __cplusplus
struct FIGHT_TRAINER {
    FIGHT_TRAINER() {}

    u16 mDataId;
    u16 mReserved;
    Hero mHero;
    u8 mWork[0x2DC8];
};
#else
struct FIGHT_TRAINER {
    u16 mDataId;
    u16 mReserved;
    Hero mHero;
    u8 mWork[0x2DC8];
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
void fightTrainer_SetOkaneBai(FIGHT_TRAINER *trainer, s32 multiplier);
s32 fightTrainerGetStatus(FIGHT_TRAINER *trainer, u16 id, u16 field, u16 sub);
void fightTrainerSetStatus(FIGHT_TRAINER *trainer, u16 id, u16 field, u16 sub, s32 value);

struct FightPokemon *fightTrainerBiosGetFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
FightPokemon *fightTrainerGetValidFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
u8 fightTrainerCheckCanIrekaeFightPokemon(FIGHT_TRAINER *trainer, FightPokemon *pokemon);
FightOutPokemon *fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(FIGHT_TRAINER *trainer, FightPokemon *pokemon);
Hero *fightTrainerBiosGetHeroPtr(FIGHT_TRAINER *trainer);
void fightTrainerCreateFightTrainerDataIdToHero(u16 trainerDataId, s32 controllerId, Hero *hero, u32 battleId);
u8 fightTrainerIsSelectedItemBall(FIGHT_TRAINER *trainer);
extern u32 *fight_trainer_pokemon_part_data_number;
extern u32 *fight_trainer_ai_value_addsub_data_number;

#ifdef __cplusplus
}
#endif

#endif

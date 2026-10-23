#ifndef FIGHT_TRAINER_H
#define FIGHT_TRAINER_H

#include <hero.h>
#include <fightPokemon.h>
#include <types.h>

enum { FIGHT_TRAINER_STATUS_TYPE = 0x39 };

typedef struct FIGHT_TRAINER FIGHT_TRAINER;

#ifdef __cplusplus
extern "C" {
#endif

FightPokemon *fightTrainerBiosGetFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
Hero *fightTrainerBiosGetHeroPtr(FIGHT_TRAINER *trainer);
/* Reads one field of a battle trainer record. The field selects the record
 * area (trainer data, pokemon part data, deck pokemon, AI value data) and sub
 * selects the entry within it. */
s32 fightTrainerGetStatus(void *trainer, u16 id, u16 field, u16 sub);
void fightTrainerSetStatus(void *trainer, u16 id, u16 field, u16 sub, s32 value);
FightPokemon *fightTrainerGetFightPokemonPtrValid(FIGHT_TRAINER *trainer, u16 slot);
u8 fightTrainerCheckCanIrekaeFightPokemon(FIGHT_TRAINER *trainer, FightPokemon *pokemon);
FightOutPokemon *fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(
    FIGHT_TRAINER *trainer, FightPokemon *pokemon);

extern u32 *fight_trainer_pokemon_part_data_number;
extern u32 *fight_trainer_ai_value_addsub_data_number;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
u8 fightTrainerIsSelectedItemBall(FIGHT_TRAINER *trainer);
#ifdef __cplusplus
}
#endif
#endif

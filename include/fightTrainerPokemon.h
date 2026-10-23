#ifndef FIGHT_TRAINER_POKEMON_H
#define FIGHT_TRAINER_POKEMON_H

#include <types.h>

struct FIGHT_TRAINER;
struct FightPokemon;

#ifdef __cplusplus
extern "C" {
#endif

struct FightPokemon *fightTrainerGetValidFightPokemonPtr(struct FIGHT_TRAINER *trainer, u16 index);

#ifdef __cplusplus
}
#endif

#endif

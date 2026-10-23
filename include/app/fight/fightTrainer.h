#ifndef APP_FIGHT_FIGHTTRAINER_H
#define APP_FIGHT_FIGHTTRAINER_H

#include <types.h>

typedef struct BattleTrainer BattleTrainer;
typedef struct FightOutPokemon FightOutPokemon;
typedef struct FIGHT_TRAINER FIGHT_TRAINER;
typedef struct FightPokemon FightPokemon;

#ifdef __cplusplus
extern "C" {
#endif

// Pokemon the trainer has out in the given slot, and the Pokemon record it
// keeps in the given slot; null when the slot holds none.
FightOutPokemon *fightTrainerGetValidFightOutPokemonPtr(void *trainerData, u16 index);
FightPokemon *fightTrainerGetValidFightPokemonPtr(FIGHT_TRAINER *trainerData, u16 index);


#ifdef __cplusplus
}
#endif

#endif

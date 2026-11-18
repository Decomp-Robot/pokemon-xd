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
u8 fightTrainerBiosGetControllerId(FIGHT_TRAINER *trainer);

// Valid Pokemon the trainer has sent out in the given slot, or null when the
// record is absent or invalid. The party-record accessor returns a valid
// record, which may still be a Pokemon that cannot fight.
FightOutPokemon *fightTrainerGetValidFightOutPokemonPtr(FIGHT_TRAINER *trainerData, u16 index);
FightPokemon *fightTrainerGetValidFightPokemonPtr(FIGHT_TRAINER *trainerData, u16 index);


#ifdef __cplusplus
}
#endif

#endif

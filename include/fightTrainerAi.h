#ifndef FIGHT_TRAINER_AI_H
#define FIGHT_TRAINER_AI_H

#include <types.h>
#include <fightTrainer.h>

#ifdef __cplusplus
extern "C" {
#endif

u16 fightTrainerAiCheckHorobinouta(FIGHT_TRAINER *trainer);
u8 fightTrainerAiIsFightOutPokemonJoutaiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 joutaiDataId);
u8 fightTrainerAiIsFightOutPokemonTokuseiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 tokuseiDataId);

#ifdef __cplusplus
}
#endif

#endif

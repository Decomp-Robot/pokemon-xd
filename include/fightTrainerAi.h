#ifndef FIGHT_TRAINER_AI_H
#define FIGHT_TRAINER_AI_H

#include <types.h>
#include <fightTrainer.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId(FIGHT_TRAINER *trainer, u16 joutaiDataId, u16 wazaDataId);
u16 fightTrainerAiCheckHorobinouta(FIGHT_TRAINER *trainer);
u8 fightTrainerAiCheckTextureZokusei(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiCheckAbiCnt(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, FightOutPokemon *defender,
                             u16 wazaDataId, u8 paraId, u8 actType, u16 flags);
u8 fightTrainerAiCheckGuard(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 wazaDataId);
u16 fightTrainerAiCheckOumu(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiCheckSimerike(FIGHT_TRAINER *trainer);
u8 fightTrainerAiCheckSawagu(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiIsFightOutPokemonJoutaiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 joutaiDataId);
u8 fightTrainerAiIsFightOutPokemonTokuseiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 tokuseiDataId);

#ifdef __cplusplus
}
#endif

#endif

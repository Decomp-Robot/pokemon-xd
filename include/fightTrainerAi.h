#ifndef FIGHT_TRAINER_AI_H
#define FIGHT_TRAINER_AI_H

#include <types.h>
#include <fightTrainer.h>

typedef struct FIGHT_FLOOR FIGHT_FLOOR;

#ifdef __cplusplus
extern "C" {
#endif

u8 fightTrainerAiGetFightFloorNowTenkouDataId(FIGHT_TRAINER *trainer, FIGHT_FLOOR *floor, u8 checkAbilities);
u8 fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiGetFightOutPokemonAbiCntPhyDef(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiGetFightOutPokemonAbiCntSpeAtk(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiGetFightOutPokemonAbiCntSpeDef(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiGetFightOutPokemonAbiCntSubayasa(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiIsFightOutPokemonZokuseiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 typeId);
s16 fightTrainerAiGetWazaIryoku(FIGHT_TRAINER *trainer, u16 wazaDataId);
u16 fightTrainerAiGetFightOutPokemonZokuseiDataIdWazaJoutaiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 typeId, s16 power);

u8 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId(FIGHT_TRAINER *trainer, u16 joutaiDataId, u16 wazaDataId);
u16 fightTrainerAiCheckHorobinouta(FIGHT_TRAINER *trainer);
u8 fightTrainerAiCheckTextureZokusei(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
// Flags that select how the trainer AI checks a stat-stage change.
enum FIGHT_TRAINER_AI_CHG_FLAGS {
    FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT = 1,
    FIGHT_TRAINER_AI_CHG_KIND_MASK = 0x1F,
    FIGHT_TRAINER_AI_CHG_SKIP_GUARD = 0x20,
    FIGHT_TRAINER_AI_CHG_ON_ATTACKER = 0x40,
    FIGHT_TRAINER_AI_CHG_IGNORE_MIST_AND_STAT_DOWN_PREVENTION = 0x80
};

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

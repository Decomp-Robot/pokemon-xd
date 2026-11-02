#ifndef APP_FIGHT_FIGHTTRAINERENEMY_H
#define APP_FIGHT_FIGHTTRAINERENEMY_H

#include <app/fight/fightTrainerBios.h>
#include <bool.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

bool fightTrainerEnemyPokemonEraseAry(FightTrainerEnemyPokemon *ary, u16 num, s16 fightEntryeId);
bool fightTrainerEnemyPokemonRegistAry(FightTrainerEnemyPokemon *ary, u16 num, s16 fightEntryeId);
FightTrainerEnemyPokemon *fightTrainerEnemyPokemonSearchAry(FightTrainerEnemyPokemon *ary, u16 num,
                                                           s16 fightEntryeId);
bool fightTrainerEnemyPokemonCheckValid(FightTrainerEnemyPokemon *pokemon);
void fightTrainerEnemyPokemonCreate(FightTrainerEnemyPokemon *pokemon, s16 fightEntryeId);
void fightTrainerEnemyPokemonInitAry(FightTrainerEnemyPokemon *ary, u16 num);
void fightTrainerEnemyPokemonInit(FightTrainerEnemyPokemon *pokemon);
void fightTrainerEnemyPokemonInitFightOutStatus(FightTrainerEnemyPokemon *pokemon);
void fightTrainerEnemyPokemonInitFightStatus(FightTrainerEnemyPokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif

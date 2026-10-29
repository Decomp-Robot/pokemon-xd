#ifndef FIGHTPOKEMONENEMY_H
#define FIGHTPOKEMONENEMY_H

#include <bool.h>
#include <fightOutPokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

FightOutPokemonEnemy *fightOutPokemonEnemySearchAry(FightOutPokemonEnemy *enemyAry, u16 count,
                                                    FightOutPokemon *target);
bool fightOutPokemonEnemyCheckValid(FightOutPokemonEnemy *enemy);
void fightOutPokemonEnemyCreate(FightOutPokemonEnemy *enemy, FightOutPokemon *target);
void fightOutPokemonEnemyInitAry(FightOutPokemonEnemy *enemyAry, u16 count);
void fightOutPokemonEnemyInit(FightOutPokemonEnemy *enemy);

#ifdef __cplusplus
}
#endif

#endif

#ifndef FIGHTPOKEMONBIOS_H
#define FIGHTPOKEMONBIOS_H

#include <fightPokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u16 fightOutPokemonBiosGetZokuseiDataId(FightOutPokemon *pokemon, u8 index);

void fightOutPokemonBiosCopy(FightOutPokemon *dst, const FightOutPokemon *src);

void fightOutPokemonEnemyBiosSetDamage(FightOutPokemonEnemy *enemy, u16 damage);
void fightOutPokemonEnemyBiosSetInitHp(FightOutPokemonEnemy *enemy, u16 initHp);
void fightOutPokemonEnemyBiosSetOumuWazaDataId(FightOutPokemonEnemy *enemy, u16 wazaDataId);
void fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(FightOutPokemonEnemy *enemy,
                                                         FightOutPokemon *target);
u16 fightOutPokemonEnemyBiosGetDamage(FightOutPokemonEnemy *enemy);
u16 fightOutPokemonEnemyBiosGetInitHp(FightOutPokemonEnemy *enemy);
u16 fightOutPokemonEnemyBiosGetOumuWazaDataId(FightOutPokemonEnemy *enemy);
FightOutPokemon *fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(FightOutPokemonEnemy *enemy);
FightOutPokemonEnemy *fightOutPokemonBiosGetFightOutPokemonEnemyPtr(FightOutPokemon *pokemon,
                                                                   u16 index);

#ifdef __cplusplus
}
#endif

#endif

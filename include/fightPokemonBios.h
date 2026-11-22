#ifndef FIGHTPOKEMONBIOS_H
#define FIGHTPOKEMONBIOS_H

#include <fightPokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

s16 fightPokemonBiosGetEntryId(FightPokemon *pokemon);

void fightOutPokemonBiosSetFightActionFlag(FightOutPokemon *pokemon, u8 flag);
u16 fightOutPokemonBiosGetZokuseiDataId(FightOutPokemon *pokemon, u8 index);
u8 fightPokemonBiosGetHokakuFlag(FightPokemon *pokemon);

void fightOutPokemonBiosCopy(FightOutPokemon *dst, const FightOutPokemon *src);

// Deployed move memory: the remembered move, Bide damage and the last moves
// a Pokemon selected, used and received. A missing record has no stored move.
void fightOutPokemonBiosSetOumuWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemonBiosSetGamanDamageTargetId(FightOutPokemon *pokemon, u16 targetDataId);
void fightOutPokemonBiosSetGamanDamageValue(FightOutPokemon *pokemon, s16 damage);
void fightOutPokemonBiosSetHitWazaZokuseiDataId(FightOutPokemon *pokemon, u16 zokuseiDataId);
void fightOutPokemonBiosSetHitWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemonBiosSetLastReceiveWazaTargetDataId(FightOutPokemon *pokemon, u16 targetDataId);
void fightOutPokemonBiosSetLastUseWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemonBiosSetLastSelectWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemonBiosSetSketchWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);

FIGHT_WAZA *fightOutPokemonBiosGetKeepFightWazaPtr(FightOutPokemon *pokemon);
u16 fightOutPokemonBiosGetOumuWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonBiosGetGamanDamageTargetId(FightOutPokemon *pokemon);
s16 fightOutPokemonBiosGetGamanDamageValue(FightOutPokemon *pokemon);
u16 fightOutPokemonBiosGetHitWazaZokuseiDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonBiosGetHitWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonBiosGetLastReceiveWazaTargetDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonBiosGetLastUseWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonBiosGetLastSelectWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonBiosGetSketchWazaDataId(FightOutPokemon *pokemon);

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

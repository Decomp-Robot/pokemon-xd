#ifndef FIGHTPOKEMON_H
#define FIGHTPOKEMON_H

#include <Pokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;

Pokemon *fightPokemonGetPokemonPtr(FightPokemon *pokemon);
Pokemon *fightOutPokemonGetPokemonPtr(FightOutPokemon *pokemon);
u8 figthPokemonGetLevel(FightPokemon *pokemon);
u16 fightPokemonGetItemDataId(FightPokemon *pokemon);
u8 fightOutPokemonIsUseHensinBuff(FightOutPokemon *pokemon);
u16 fightOutPokemonGetTokuseiDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonMaxHpWaruValue(FightOutPokemon *pokemon, u16 divisor);
u8 fightOutPokemonIsJoutaiDataId(FightOutPokemon *pokemon, u16 status);
u8 fightOutPokemonIsZokuseiDataId(FightOutPokemon *pokemon, u16 type);
u8 fightOutPokemonIsHpMantan(FightOutPokemon *pokemon);
u8 fightOutPokemonCheckFightOut(FightOutPokemon *pokemon);
u8 fightOutPokemonCheckWriteJoutaiDataId(FightOutPokemon *pokemon, u32 status);
void fightOutPokemonWriteJoutaiDataId(FightOutPokemon *pokemon, u32 status, u32 value);
u16 fightOutPokemonGetSoubiItemDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonGetSoubiItemSoubiDataId(FightOutPokemon *pokemon);
s32 figthOutPokemonGetSoubiItemBuff(FightOutPokemon *pokemon);
u8 fightOutPokemonIsJoutaiNormal(FightOutPokemon *pokemon);
void fightOutPokemonResetSeqStatus(FightOutPokemon *pokemon, u32 status);
u8 fightOutPokemonIsNokoriHpFollowing(FightOutPokemon *pokemon, u16 divisor);
s8 fightOutPokemonGetTasteLike(FightOutPokemon *pokemon, u16 taste);
void fightOutPokemonSetKaisuuJoutaiDataId(FightOutPokemon *pokemon, u32 status, u32 count);
void fightOutPokemonLoadWazaEffect(FightOutPokemon *pokemon, u32 effect, u32 slot, u32 mode);
void fightOutPokemonWazaEffect(FightOutPokemon *pokemon, u32 effect, u32 slot, u32 mode, void *buffer);
void fightOutPokemonFreeWazaEffect(FightOutPokemon *pokemon, u32 effect, u32 slot);
u8 fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry(FightOutPokemon *pokemon, u16 *moves);

void fightOutPokemonSetTokuseiDataId(FightOutPokemon *pokemon, u16 abilityId);
u16 fightOutPokemonGetZokuseiDataId(FightOutPokemon *pokemon, u8 index);
void fightOutPokemonSetZokuseiDataId(FightOutPokemon *pokemon, u8 index, u16 typeId);
void fightOutPokemonInitJoutaiDataId(FightOutPokemon *pokemon, u16 statusId);

#ifdef __cplusplus
}
#endif

#endif

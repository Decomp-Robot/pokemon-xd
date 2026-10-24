#ifndef FIGHTPOKEMONBIOS_H
#define FIGHTPOKEMONBIOS_H

#include <fightPokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u16 fightOutPokemonBiosGetZokuseiDataId(FightOutPokemon *pokemon, u8 index);

void fightOutPokemonBiosCopy(FightOutPokemon *dst, const FightOutPokemon *src);

#ifdef __cplusplus
}
#endif

#endif

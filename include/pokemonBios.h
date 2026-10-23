#ifndef POKEMON_BIOS_H
#define POKEMON_BIOS_H

#include <Pokemon.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 pokemonBiosGetLevel(Pokemon *pokemon);
u16 pokemonGetSoubiItemDataId(Pokemon *pokemon);
u16 pokemonDoItemSoubi(Pokemon *pokemon, u16 itemID, u8 equip);
typedef struct PokemonTokuseiData PokemonTokuseiData;
u16 pokemonBiosGetPokemonWazaDataId(Pokemon *pokemon, u16 slot);
u8 pokemonBiosGetPokemonWazaPp(Pokemon *pokemon, u16 slot);
PokemonTokuseiData *pokemonTokuseiDataBiosGetPtr(u16 abilityID);
u32 pokemonTokuseiDataBiosGetName(PokemonTokuseiData *data);
u32 pokemonTokuseiDataBiosGetDoc(PokemonTokuseiData *data);

#ifdef __cplusplus
}
#endif

#endif

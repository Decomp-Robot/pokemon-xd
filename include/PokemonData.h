#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#include <types.h>

typedef struct PokemonData PokemonData;
typedef PokemonData PokemonSpeciesStats;

// Tutor selection, progression gate, and learned-state flag.
typedef struct PokemonTutorMove {
    u16 mMoveId;
    u8 _02[2];
    u32 mProgressionGateIndex;
    u32 mLearnedFlagId;
} PokemonTutorMove;

#ifdef __cplusplus
extern "C" {
#endif
extern const PokemonTutorMove *pokemonTutorMoveTable;
PokemonSpeciesStats *pokemonDataBiosGetPtr(u16 species);
s32 pokemonDataBiosGetVoice(PokemonSpeciesStats *stats);
u32 pokemonDataBiosGetZokuseiDataId(PokemonSpeciesStats *data, s32 index);
u32 pokemonDB_GetName(u16 species);
u8 pokemonDataBiosGetWazaMcn(PokemonSpeciesStats *stats, u16 machineNo);
u8 pokemonDataCanLearnTutorMove(u16 species, u16 itemId);

void *pokemonSeikakuDataBiosGetPtr(u8 id);

u32 pokemonSeikakuDataGetMessageId(void *ptr);
#ifdef __cplusplus
}
#endif

#endif

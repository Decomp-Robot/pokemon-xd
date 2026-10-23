#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#include <types.h>

typedef struct PokemonSpeciesStats PokemonSpeciesStats;

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
PokemonSpeciesStats *pokemonDataGetSpeciesStats(u16 species);
u32 pokemonDataBiosGetZokuseiDataId(PokemonSpeciesStats *data, s32 index);
u32 pokemonDataGetNameId(u16 species);
u8 pokemonDataBiosGetWazaMcn(PokemonSpeciesStats *stats, u16 machineNo);
u8 pokemonDataCanLearnTutorMove(u16 species, u16 itemId);
#ifdef __cplusplus
}
#endif

#endif

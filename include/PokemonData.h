#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H

#include <types.h>

typedef struct PokemonData PokemonData;
typedef PokemonData PokemonSpeciesStats;

struct PokemonData {
    u8 _00[36];
    s32 mMitaFlagId; // id of the zukan seen flag.
    s32 mTukamaetaFlagId; // id of the zukan caught flag.
    u8 _2C[248];
};

// Voice id reported for a species that has no voice data to play.
#define POKEMON_VOICE_ID_INVALID 0xFFFF

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
s8 *pokemonNakigoeDataBiosGetDataAddress(PokemonSpeciesStats *stats);
u32 pokemonDataBiosGetZokuseiDataId(PokemonSpeciesStats *data, s32 index);
u32 pokemonDB_GetName(u16 species);
u8 pokemonDataBiosGetWazaMcn(PokemonSpeciesStats *stats, u16 machineNo);

void *pokemonSeikakuDataBiosGetPtr(u8 id);

u32 pokemonSeikakuDataGetMessageId(void *ptr);
#ifdef __cplusplus
}
#endif

#endif

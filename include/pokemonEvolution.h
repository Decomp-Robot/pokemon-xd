#ifndef POKEMONEVOLUTION_H
#define POKEMONEVOLUTION_H

#include <types.h>

struct Pokemon;
class Hero;

// What an evolution check found: the kind of evolution that fired and the
// value it fired on (the item used, the level reached, and so on).
struct POKEMON_EVOLUTION_INFO {
    u8 method;
    u16 condition;
};

enum EVOLUTION_RESULT {
    EVOLUTION_RESULT_COMPLETE = 0,
    EVOLUTION_RESULT_CANCEL = 1,
    EVOLUTION_RESULT_ERROR = 2
};

// Evolution check kinds for pokemonEvolution.
enum EVOLUTION_CHECK_KIND {
    EVOLUTION_CHECK_LEVEL_UP = 0,
    EVOLUTION_CHECK_ITEM = 1,
    EVOLUTION_CHECK_COMMUNICATION = 2,
};

#ifdef __cplusplus
extern "C" {
#endif

// Checks whether pokemon can evolve under kind with itemID.
// Reports the evolution target species, with the additional species for the
// additional-pokemon operation through outAdditionalSpecies and the firing kind
// through info.
u16 pokemonEvolutionCheck(Pokemon *pokemon, s32 kind, u16 itemID, u16 *outAdditionalSpecies, POKEMON_EVOLUTION_INFO *info);

// Runs the evolution scene for species. hero is the trainer the additional
// pokemon is added to (null at the item handlers). registerMemo records the
// evolved form, enableCancel enables the demo's delayed cancel-input check
// (button mask 0x200), and fade lets the routine fade the screen itself
// (the item handlers fade themselves). The scene opening is unconditional.
// Reports the scene result.
enum EVOLUTION_RESULT pokemonEvolutionAll(Pokemon *pokemon, u16 species, u16 additionalSpecies, POKEMON_EVOLUTION_INFO *info, Hero *hero, s32 registerMemo, s32 enableCancel, s32 fade);

s32 pokemonEvolutionCreateAddPokemon(Pokemon *destination, Pokemon *source, u16 species);
s32 pokemonEvolution(Pokemon *destination, Pokemon *source, u16 species, POKEMON_EVOLUTION_INFO *info, u16 *outWazaIDs);

#ifdef __cplusplus
}
#endif

#endif

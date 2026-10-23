#ifndef EVOLUTION_H
#define EVOLUTION_H

#include <types.h>
#include <Pokemon.h>

class Hero;

// What an evolution check found: the kind of evolution that fired and the
// value it fired on (the item used, the level reached, and so on).
struct POKEMON_EVOLUTION_INFO {
    u8 method;
    u16 condition;
};

// Evolution check kinds for pokemonEvolution.
enum {
    EVOLUTION_CHECK_LEVEL_UP = 0,
    EVOLUTION_CHECK_ITEM = 1,
    EVOLUTION_CHECK_COMMUNICATION = 2,
};

// Message window selected by the evolution scene. EVOLUTION_WINDOW_FIELD
// opens through the field path, EVOLUTION_WINDOW_GENERAL through the general
// path (see openMsgWin).
enum EVOLUTION_WINDOW_TYPE {
    EVOLUTION_WINDOW_FIELD = 0,
    EVOLUTION_WINDOW_GENERAL = 1
};

#ifdef __cplusplus
extern "C" {
#endif

// Checks whether pokemon can evolve under evoMode with itemID.
// Reports the evolution target species, with the additional species for the
// additional-pokemon operation through outAddedSpecies and the firing kind
// through info.
s32 pokemonEvolutionCheck(Pokemon *pokemon, s32 evoMode, u16 itemID, u16 *outAddedSpecies, POKEMON_EVOLUTION_INFO *info);

// Runs the evolution scene for evoSpecies. hero is the trainer the additional
// pokemon is added to (null at the item handlers). registerMemo records the
// evolved form, enableCancel enables the demo's delayed cancel-input check
// (button mask 0x200), and fade lets the routine fade the screen itself
// (the item handlers fade themselves). The scene opening is unconditional.
// Reports the scene result.
s32 pokemonEvolutionAll(Pokemon *pokemon, s32 evoSpecies, u16 addedSpecies, POKEMON_EVOLUTION_INFO *info, Hero *hero, s32 registerMemo, s32 enableCancel, s32 fade);

// Teaches wazaID through the move-learning scene. windowType selects the
// window type; when every move slot is taken, forgetWazaSelect runs the
// forget-and-learn prompt with context and reports the slot to learn into.
// outIndex receives the slot that was learned into.
s32 evolutionWazaLearn(Pokemon *pokemon, u16 wazaID, u8 *outIndex, s32 windowType, s32 (*forgetWazaSelect)(Pokemon *pokemon, u16 wazaID, long context), long context);

#ifdef __cplusplus
}
#endif

#endif

#ifndef EVOLUTION_H
#define EVOLUTION_H

#include <types.h>
#include <Pokemon.h>
#include <pokemonEvolution.h>

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

void evolution(void);

EVOLUTION_RESULT evolutionOpen(Pokemon *before, Pokemon *after, s32 enableCancel, u16 *wazaIDs, s32 wazaCount, u8 *outLearnedSlots);

// Teaches wazaID through the move-learning scene. windowType selects the
// window type; when every move slot is taken, forgetWazaSelect runs the
// forget-and-learn prompt with context and reports the slot to learn into.
// outIndex receives the slot that was learned into.
s32 evolutionWazaLearn(Pokemon *pokemon, u16 wazaID, u8 *outIndex, s32 windowType, s32 (*forgetWazaSelect)(Pokemon *pokemon, u16 wazaID, long context), long context);

#ifdef __cplusplus
}
#endif

#endif

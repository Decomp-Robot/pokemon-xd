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

// Value written to a learned-slot output when its move was not learned.
enum { EVOLUTION_WAZA_NOT_LEARNED = 0xFF };

#ifdef __cplusplus
extern "C" {
#endif

void evolution(void);

// Runs the evolution scene and writes one learned-slot result per requested
// move into outLearnedSlots: the slot the move was learned into, or
// EVOLUTION_WAZA_NOT_LEARNED when the player declined it.
EVOLUTION_RESULT evolutionOpen(Pokemon *before, Pokemon *after, int enableCancel, u16 *wazaIDs, s32 wazaCount, u8 *outLearnedSlots);

// Runs the move-learning prompts for wazaID. When every move slot is taken,
// forgetWazaSelect selects the slot to replace with context. On success the
// selected slot is written to outIndex and the caller installs the move; on
// failure the routine returns 0 without writing outIndex.
s32 evolutionWazaLearn(Pokemon *pokemon, u16 wazaID, u8 *outIndex, EVOLUTION_WINDOW_TYPE windowType, s32 (*forgetWazaSelect)(Pokemon *pokemon, u16 wazaID, s32 context), s32 context);

#ifdef __cplusplus
}
#endif

#endif

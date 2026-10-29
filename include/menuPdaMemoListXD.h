#ifndef MENU_PDA_MEMO_LIST_XD_H
#define MENU_PDA_MEMO_LIST_XD_H

#include <types.h>
#include <PokemonData.h>

#ifdef __cplusplus
extern "C" {
#endif
// Species data of the memo list entry at the given index.
PokemonSpeciesStats *menuPdaMemoListGetPokemonDataPointer(long index);
#ifdef __cplusplus
}
#endif

#endif

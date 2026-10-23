#ifndef MENU_MOVE_SELECT_H
#define MENU_MOVE_SELECT_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 menuMoveSelect(s32 pokemonIndex);
s32 menuTutorMoveSelect(u8 *selectedMoveFlag);

// Move-selection window offsets shared with the party menu.
extern s32 menuPokemonMoveSelectXAdjustment;
extern s32 menuPokemonMoveSelectYAdjustment;

#ifdef __cplusplus
}
#endif

#endif

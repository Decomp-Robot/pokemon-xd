#ifndef MENU_POKEMON_H
#define MENU_POKEMON_H

#include <types.h>

// Party-menu work (bss 0x804280C0, 0x28 bytes). Partial declaration: only
// the full-block clear in menuPokemonInitWork is recovered so far; the
// per-field layout belongs to the sibling menuPokemon helpers sharing
// this block and stays padding here.
struct MenuPokemonWork {
    u8 _00[0x28];
};

extern MenuPokemonWork menuPokemonWork;

#ifdef __cplusplus
extern "C" {
#endif

void menuPokemonInitWork(void);

#ifdef __cplusplus
}
#endif

#endif

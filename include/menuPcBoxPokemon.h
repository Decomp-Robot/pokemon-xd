#ifndef MENU_PC_BOX_POKEMON_H
#define MENU_PC_BOX_POKEMON_H

class CMenuGlobalState;

void menuPcBoxPokemon(CMenuGlobalState &state);

extern "C" {
long menuPcBoxPokemonGetCurrentBox();
void menuPcBoxPokemonSetCurrentBox(long box);
}

#endif

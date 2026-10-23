#include <menuPokemon.h>

// MSL memory routine used to clear the work block.
extern "C" void *memset(void *dest, int ch, unsigned long count);

// menuPokemonInitWork -- reset the party-menu work block.
void menuPokemonInitWork(void)
{
    memset(&menuPokemonWork, 0, sizeof(menuPokemonWork));
}

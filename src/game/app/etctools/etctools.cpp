#include <types.h>
#include <pokemonDataBios.h>
#include <GSsndPlayback.h>

extern "C" {

// Play a Pokemon's cry from its species ID. Species with no data have no cry.
void etctoolSetPokemonNakigoe(u16 species)
{
    PokemonData *data = pokemonDataBiosGetPtr(species);
    if (data == 0) {
        return;
    }

    u16 voice = pokemonDataBiosGetVoice(data);
    GSsndPlay(voice, 0, 0);
}

}

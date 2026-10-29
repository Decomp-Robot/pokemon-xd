#include <savedata.h>
#include <hero.h>
#include <heroBios.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <types.h>

extern "C" {

// Ball the Pokemon in a party slot was caught in, or zero while that slot
// holds no usable Pokemon.
u8 revoceryCatchPokemonBall(u16 slot)
{
    Pokemon *pokemon;

    pokemon = heroBiosGetPokemonPtr((Hero *)savedataGetStatus(0, SAVE_DATA_HERO), slot);
    if (pokemonCheckValid(pokemon) == 0) {
        return 0;
    }
    return pokemonBiosGetCatchBallId(pokemon);
}

// Restores the condition of every Pokemon in the party.
void recoveryPokemon(void)
{
    Hero *hero;
    unsigned long count;
    u32 i;

    hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    count = heroGetPokeomCount(hero);
    for (i = 0; i <= count; i++) {
        pokemonAllKaihuku(heroBiosGetPokemonPtr(hero, (u16)i));
    }
}

} // extern "C"

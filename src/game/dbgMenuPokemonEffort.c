#include <types.h>
#include <Pokemon.h>

/* Total of the six effort values of a pokemon. The editor keeps their sum
 * within the 510 points a pokemon may earn. */
u32 dbgMenuPokemonEditSubGetTotalEffort(Pokemon *pokemon)
{
    u32 total;

    total = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_EFFORT, 0);
    total += pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK_EFFORT, 0);
    total += pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE_EFFORT, 0);
    total += pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK_EFFORT, 0);
    total += pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE_EFFORT, 0);
    total += pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED_EFFORT, 0);
    return total;
}

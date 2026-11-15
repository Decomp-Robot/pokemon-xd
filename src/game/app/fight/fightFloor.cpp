#include <fightFloor.h>

u32 encount_floor_id;
FIGHT_FLOOR fight_floor;

s16 _fightFloorSetFightOutPokemonPtrAry(FightOutPokemon **pokemonArray, FightOutPokemon *pokemon)
{
    for (u16 index = 0; index < FIGHT_OUT_POKEMON_COUNT; index++) {
        if (pokemonArray[index] == 0) {
            pokemonArray[index] = pokemon;
            return index;
        }
    }
    return -1;
}

void _fightFloorInitFightOutPokemonPtrAry(FightOutPokemon **pokemonArray)
{
    for (u16 index = 0; index < FIGHT_OUT_POKEMON_COUNT; index++) {
        pokemonArray[index] = 0;
    }
}

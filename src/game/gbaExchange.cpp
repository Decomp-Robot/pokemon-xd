#include "Pokemon.h"
#include <libc/memory.h>

enum GbaPokemonConditionFlag {
    GBA_POKEMON_CONDITION_BAD_POISON = 0x80,
    GBA_POKEMON_CONDITION_PARALYSIS = 0x40,
    GBA_POKEMON_CONDITION_FREEZE = 0x20,
    GBA_POKEMON_CONDITION_BURN = 0x10,
    GBA_POKEMON_CONDITION_POISON = 0x08,
    GBA_POKEMON_CONDITION_SLEEP_MASK = 7,
    GBA_POKEMON_CONDITION_BAD_POISON_COUNT_MASK = 0xF00,
};

extern "C" u16 gbaPokemonConditonFromGC(Pokemon *pokemon)
{
    u16 condition = 0;
    if (pokemonIsJoutaiDataId(pokemon, POKEMON_CONDITION_BAD_POISON))
        condition = (pokemonGetCountAsJoutaiDataId(pokemon, POKEMON_CONDITION_BAD_POISON) * 256) | GBA_POKEMON_CONDITION_BAD_POISON;
    else if (pokemonIsJoutaiDataId(pokemon, POKEMON_CONDITION_PARALYSIS)) condition |= GBA_POKEMON_CONDITION_PARALYSIS;
    else if (pokemonIsJoutaiDataId(pokemon, POKEMON_CONDITION_FREEZE)) condition |= GBA_POKEMON_CONDITION_FREEZE;
    else if (pokemonIsJoutaiDataId(pokemon, POKEMON_CONDITION_BURN)) condition |= GBA_POKEMON_CONDITION_BURN;
    else if (pokemonIsJoutaiDataId(pokemon, POKEMON_CONDITION_POISON)) condition |= GBA_POKEMON_CONDITION_POISON;
    else if (pokemonIsJoutaiDataId(pokemon, POKEMON_CONDITION_SLEEP)) condition = (s16)pokemonGetKaisuuJoutaiDataId(pokemon, POKEMON_CONDITION_SLEEP);
    return condition;
}

extern "C" void gbaPokemonConditonToGC(Pokemon *pokemon, u16 condition)
{
    if (condition & GBA_POKEMON_CONDITION_BAD_POISON) {
        pokemonWriteJoutaiDataId(pokemon, POKEMON_CONDITION_BAD_POISON, 0);
        pokemonSetCountAsJoutaiDataId(pokemon, POKEMON_CONDITION_BAD_POISON, (condition & GBA_POKEMON_CONDITION_BAD_POISON_COUNT_MASK) >> 8);
    } else if (condition & GBA_POKEMON_CONDITION_PARALYSIS) pokemonWriteJoutaiDataId(pokemon, POKEMON_CONDITION_PARALYSIS, 0);
    else if (condition & GBA_POKEMON_CONDITION_FREEZE) pokemonWriteJoutaiDataId(pokemon, POKEMON_CONDITION_FREEZE, 0);
    else if (condition & GBA_POKEMON_CONDITION_BURN) pokemonWriteJoutaiDataId(pokemon, POKEMON_CONDITION_BURN, 0);
    else if (condition & GBA_POKEMON_CONDITION_POISON) pokemonWriteJoutaiDataId(pokemon, POKEMON_CONDITION_POISON, 0);
    else if (condition & GBA_POKEMON_CONDITION_SLEEP_MASK) {
        pokemonWriteJoutaiDataId(pokemon, POKEMON_CONDITION_SLEEP, 0);
        pokemonSetKaisuuJoutaiDataId(pokemon, POKEMON_CONDITION_SLEEP, condition & GBA_POKEMON_CONDITION_SLEEP_MASK);
    }
}

void _rotatepp(unsigned char *data, unsigned long selector)
{
    unsigned char block[12];
    unsigned int offset;
    unsigned int rank = selector % 24;
    if (rank / 6) {
        offset = rank / 6 * 12;
        memcpy(block, data + offset, 12);
        memmove(data + 12, data, offset);
        memcpy(data, block, 12);
    }
    unsigned int group = rank % 6;
    if (group / 2) {
        offset = group / 2 * 12;
        memcpy(block, data + offset + 12, 12);
        memmove(data + 24, data + 12, offset);
        memcpy(data + 12, block, 12);
    }
    if (group & 1) {
        memcpy(block, data + 36, 12);
        memcpy(data + 36, data + 24, 12);
        memcpy(data + 24, block, 12);
    }
}

void _revrotatepp(unsigned char *data, unsigned long selector)
{
    unsigned char block[12];
    if (selector & 1) {
        memcpy(block, data + 24, 12);
        memcpy(data + 24, data + 36, 12);
        memcpy(data + 36, block, 12);
    }
    if ((selector / 2) % 3) {
        memcpy(block, data + 12, 12);
        memmove(data + 12, data + 24, (selector / 2) % 3 * 12);
        memcpy(data + 12 + (selector / 2) % 3 * 12, block, 12);
    }
    if ((selector / 6) & 3) {
        memcpy(block, data, 12);
        memmove(data, data + 12, ((selector / 6) & 3) * 12);
        memcpy(data + ((selector / 6) & 3) * 12, block, 12);
    }
}

unsigned long _swap32(unsigned long value)
{
    return (value << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | (value >> 24);
}

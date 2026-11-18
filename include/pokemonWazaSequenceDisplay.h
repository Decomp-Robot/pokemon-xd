#ifndef POKEMON_WAZA_SEQUENCE_DISPLAY_H
#define POKEMON_WAZA_SEQUENCE_DISPLAY_H

#include <types.h>
#include <ModelSequence.h>

struct Pokemon;

struct LoadSequenceEntry {
    u16 sequence; // id of the sequence data of the move
    u16 category; // sequence resource category
};

extern "C" {
void waitSequenceTime(f32 seconds);
s32 loadSequence(ModelSequence **model, Pokemon *pokemon, LoadSequenceEntry *list, s32 count);
}

#endif

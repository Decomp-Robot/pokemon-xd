#include <GSthread.h>
#include <types.h>

#include <Pokemon.h>
#include <pokemonBios.h>
#include <pokecolo.h>
#include <wazaSequence.h>

extern "C" {

/*
 * The sequence display of a Pokemon: the move (waza) sequences a Pokemon carries
 * are handed to the model of the Pokemon, which plays them back when the relive
 * ceremony or the evolution demo shows one of its moves.
 *
 * loadSequence builds the model of the Pokemon out of the species and the random
 * number of the record it is built for, and hands it the sequences the caller
 * lists; the entries of the list are the sequence (the id of the sequence data)
 * and the move that is played back with it.
 */

// Entry of the list a caller hands loadSequence: the sequence-data id and
// the resource category used to select its sequence resources.
typedef struct LoadSequenceEntry {
    u16 sequence; // id of the sequence data of the move
    u16 category; // sequence resource category
} LoadSequenceEntry;

// Status words of a Pokemon record the model of the sequence display is built
// from: the species the record holds, the model of the species and the word the
// shiny check of the record is read with.
#define POKEMON_STATUS_SPECIES 0x6E
#define POKEMON_STATUS_MODEL 0x66
#define POKEMON_STATUS_SHINY 0xC1


// Wait until the frame time the caller has waited so far has reached the time it
// asks for: the ceremony runs in its own thread and hands it back to the game
// before every frame it waits out.
void waitSequenceTime(f32 seconds) {
    f32 waited = 0.0f;

    while (waited < seconds) {
        GSthreadSwitch();
        waited += pokecoloGetFrameTime();
    }
}

// Build the model of a Pokemon and hand it the sequences of the list. The model
// is built for the species of the Pokemon and the random number it was created
// with, so two Pokemon of the same species show the sequences their own record
// asks for. The function reports whether the Pokemon and all of its sequences
// could be loaded.
s32 loadSequence(ModelSequence **model, Pokemon *pokemon, LoadSequenceEntry *list, s32 count) {
    u32 pid;
    u16 modelId;
    u16 species;
    bool shiny;
    s32 i;

    species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIES, 0);
    if (species == 0) {
        return 0;
    }

    modelId = (u16)pokemonGetStatus(0, species, POKEMON_STATUS_MODEL, 0);
    if (modelId == 0) {
        return 0;
    }

    pid = pokemonBiosGetRnd(pokemon);
    shiny = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SHINY, 0) != 0;

    if ((*model = Sequence_LoadPokemon(modelId, pid, shiny)) == 0) {
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (Sequence_LoadWaza(*model, list[i].sequence, (u8)list[i].category, 0) == 0) {
            return 0;
        }
    }
    return 1;
}

} // extern "C"

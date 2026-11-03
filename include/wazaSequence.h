#ifndef WAZASEQUENCE_H
#define WAZASEQUENCE_H

#include <types.h>
#include <NullSequence.h>
#include <ModelSequence.h>

class SequenceEntry;

/*
 * A waza sequence plays back the move sequence that was loaded for it on the
 * stage of a Pokemon. The move data of the sequence is read into the resource
 * named by its resource id and handed back to the systems that own its parts
 * through Unload once that resource is released; the camera the sequence is
 * rendered with is a resource of its own.
 */
class WazaSequence {
public:
    WazaSequence(NullSequence *stage, u16 sequence, u8 category, u8 variation);
    ~WazaSequence();

    static void Unload(u8 *data);

    f32 time;                 // playback position within the sequence.
    f32 startTime;            // position the timing of the sequence is measured from.
    u32 flags;                // playback and render flags read from the move data.
    u16 sequence;             // index of the sequence on the move.
    u8 category;              // category the sequence was loaded for.
    u8 state;                 // zero while the sequence is usable, set on failure.
    u16 resourceID;           // resource the move data was read into.
    u8 lastEntry;             // index of the last entry found in the move data.
    u8 motionType;            // motion type the timing tables are selected by.
    u8 playing;               // set while the sequence is playing back.
    u8 unk15;
    u8 unk16;
    u8 stopRequested;         // set when the sequence has been asked to stop.
    f32 duration;             // length of the sequence, derived from the move data.
    u32 cameraID;             // id of the camera resource the sequence is rendered
                              // with, in the camera resource group.
    SequenceEntry *entries;   // entries the sequence is made of.
    NullSequence *stage;      // stage the sequence plays back on.
    WazaSequence *next;       // next sequence of the stage.
    WazaSequence *prev;       // previous sequence of the stage.
};

extern "C" {
ModelSequence *Sequence_LoadPokemon(u16 species, u32 rnd, bool shiny);
ModelSequence *Sequence_LoadTrainer(u16 trainerDataId);
u16 Sequence_GetID(NullSequence *sequence);
u8 Sequence_GetFlags(NullSequence *sequence);
void Sequence_SetFlags(NullSequence *sequence, u8 flags);
bool Sequence_IsFlipped(NullSequence *sequence);
void Sequence_SetFlipped(NullSequence *sequence, bool flipped);
bool Sequence_IsRare(NullSequence *sequence);
u32 Sequence_GetPokemonRndVal(NullSequence *sequence);
WazaSequence *Sequence_LoadWaza(NullSequence *model, u16 sequence, u8 category, u8 variation);
NullSequence *Sequence_CreateNull(void);
void Sequence_Free(NullSequence *sequence);
void Sequence_SetVisibility(NullSequence *sequence, bool visible);
void Sequence_SetPosition(NullSequence *sequence, GSvec *position);
void Sequence_SetRotation(NullSequence *sequence, GSvec *rotation);
void Sequence_SetScale(NullSequence *sequence, GSvec *scale);
void WazaSequence_Start(WazaSequence *sequence, int mode);
bool WazaSequence_IsActive(WazaSequence *sequence);
void WazaSequence_Stop(WazaSequence *sequence);
void WazaSequence_Free(WazaSequence *sequence);
}

#endif

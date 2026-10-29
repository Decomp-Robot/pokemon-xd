#ifndef WAZASEQUENCE_H
#define WAZASEQUENCE_H

#include <types.h>
#include <NullSequence.h>
#include <ModelSequence.h>

class WazaSequence;

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

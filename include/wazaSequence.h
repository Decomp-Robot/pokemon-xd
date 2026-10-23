#ifndef WAZASEQUENCE_H
#define WAZASEQUENCE_H

#include <types.h>
#include <NullSequence.h>
#include <ModelSequence.h>

class WazaSequence;

extern "C" {
ModelSequence *Sequence_LoadPokemon(u16 species, u32 rnd, bool shiny);
WazaSequence *Sequence_LoadWaza(NullSequence *model, u16 sequence, u8 category, u8 variation);
NullSequence *Sequence_CreateNull(void);
void Sequence_Free(NullSequence *sequence);
void Sequence_SetVisibility(NullSequence *sequence, u8 visible);
void Sequence_SetPosition(NullSequence *sequence, GSvec *position);
void Sequence_SetRotation(NullSequence *sequence, GSvec *rotation);
void Sequence_SetScale(NullSequence *sequence, GSvec *scale);
void WazaSequence_Start(WazaSequence *sequence, int mode);
bool WazaSequence_IsActive(WazaSequence *sequence);
void WazaSequence_Stop(WazaSequence *sequence);
void WazaSequence_Free(WazaSequence *sequence);
}

#endif

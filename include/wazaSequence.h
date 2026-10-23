#ifndef WAZA_SEQUENCE_H
#define WAZA_SEQUENCE_H

#include <types.h>
#include <sequence.h>
struct GSvec;
struct Sequence;
struct WazaSequence;
#ifdef __cplusplus
extern "C" {
#endif
Sequence* Sequence_CreateNull(void);
WazaSequence* Sequence_LoadWaza(Sequence*, u16, u8, u8);
void WazaSequence_Start(WazaSequence*, int);
bool WazaSequence_IsActive(WazaSequence*);
void Sequence_SetPosition(Sequence*, GSvec*);
void Sequence_SetRotation(Sequence*, GSvec*);
void Sequence_SetScale(Sequence*, GSvec*);
void WazaSequence_Stop(WazaSequence*);
void WazaSequence_Free(WazaSequence*);

#ifdef __cplusplus
}
#endif
#endif

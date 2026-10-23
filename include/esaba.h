#ifndef ESABA_H
#define ESABA_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
int esabaSet(int place);
void esabaSetPokemonRnd(s32 place, u32 random);

u16 esabaGetPokemonID(int place);
void esabaSetEsaNum(int place, int number);
int esabaGetEsaNum(int place);
bool esabaGetPokemonIsRare(int place);
#ifdef __cplusplus
}
#endif
#endif

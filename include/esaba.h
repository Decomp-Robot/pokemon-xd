#ifndef ESABA_H
#define ESABA_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
extern u32 *esabadata_number;
void esabaSetPokemon(u16 place, u16 species);
u16 esabaGetPokemonMax(u16 place);
int esabaGetDigestRate(u16 place);
long esabaGet(int place);
long esabaPut(int place);
void esabaHeroCallBackInit(void);
u32 esabaGetPokemonRnd(int place);
int esabaSet(int place);
void esabaSetPokemonRnd(int place, u32 random);

u16 esabaGetPokemonID(int place);
void esabaSetEsaNum(int place, int number);
int esabaGetEsaNum(int place);
int esabaGetEsaItemNum(void);
bool esabaGetPokemonIsRare(int place);
#ifdef __cplusplus
}
#endif
#endif

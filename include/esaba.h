#ifndef ESABA_H
#define ESABA_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Slots of the esaba table the Poke Spots take. */
enum EsabaSpotIndex {
    ESABA_FIRST_SPOT_INDEX = 0,
    ESABA_SECOND_SPOT_INDEX = 1,
    ESABA_THIRD_SPOT_INDEX = 2
};

/* Floors the three Poke Spots stand on. */
enum EsabaSpotArea {
    ESABA_FIRST_SPOT_AREA = 90,
    ESABA_SECOND_SPOT_AREA = 91,
    ESABA_THIRD_SPOT_AREA = 92
};

#define ESABA_PROGRESS_FLAG 964 /* story progress GS flag */
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

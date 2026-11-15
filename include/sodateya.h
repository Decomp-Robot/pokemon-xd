#ifndef SODATEYA_H
#define SODATEYA_H
#include <Pokemon.h>
typedef struct SodateyaSave SodateyaSave;
#ifdef __cplusplus
extern "C" {
#endif
void sodateyaInit(SodateyaSave *save);
void sodateyaHeroCallBackInit(void);
int sodateyaGetPokemonLevelUpValue(void);
int sodateyaGetPokemonDPValue(void);
void sodateyaRaise(int);
void sodateyaReclaim(int);
int sodateyaCalcPrice(int);
int sodateyaeChekcRais(void);
Pokemon* sodateyaGetPokemonPtr(int);
#ifdef __cplusplus
}
#endif
#endif

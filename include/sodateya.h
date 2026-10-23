#ifndef SODATEYA_H
#define SODATEYA_H
#include <Pokemon.h>
#ifdef __cplusplus
extern "C" {
#endif
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

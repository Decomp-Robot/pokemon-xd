#ifndef POKEMON_RELIVE_H
#define POKEMON_RELIVE_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
void preReliveSetParameter(int mode, int parameter, bool wait);
#else
void preReliveSetParameter(int mode, int parameter, int wait);
#endif

void reliveMain(void);
void preReliveDisplayHokoraParticle(void);
#ifdef __cplusplus
}
#endif

#endif

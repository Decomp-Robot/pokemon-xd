#ifndef POKEMON_RELIVE_H
#define POKEMON_RELIVE_H

#include <types.h>
#include <bool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum PreReliveMode {
    PRE_RELIVE_MODE_START_PURIFICATION = 0,
};

void preReliveSetParameter(int mode, int parameter, bool wait);

void reliveMain(void);
void preReliveDisplayHokoraParticle(void);
#ifdef __cplusplus
}
#endif

#endif

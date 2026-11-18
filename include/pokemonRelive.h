#ifndef POKEMON_RELIVE_H
#define POKEMON_RELIVE_H

#include <types.h>
#include <bool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum PreReliveMode {
    PRE_RELIVE_MODE_START_PURIFICATION = 0, // the ceremony runs on the Relive Hall floor
    PRE_RELIVE_MODE_START_HOLOGRAM = 1, // the ceremony runs on the hologram stage
};

void preReliveSetParameter(int mode, int parameter, bool wait);

void reliveMain(void);
void preReliveDisplayHokoraParticle(void);
void preReliveExit(void);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
struct Pokemon;
class ModelSequence;
ModelSequence *reliveCeremonyFromHologram(Pokemon *pokemon, unsigned long sourceSequence);
#endif

#endif

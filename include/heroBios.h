#ifndef HERO_BIOS_H
#define HERO_BIOS_H

#include <heroStatus.h>
#include <Pokemon.h>

#ifdef __cplusplus
extern "C" {
#endif

void *heroBiosGetNamePtr(void *hero);
Pokemon *heroBiosGetPokemonPtr(void *hero, u16 slot);

#ifdef __cplusplus
}
#endif

#endif

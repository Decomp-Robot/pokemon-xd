#ifndef HERO_BIOS_H
#define HERO_BIOS_H

#include <heroStatus.h>
#include <Pokemon.h>

#ifdef __cplusplus
extern "C" {
#endif


void heroBiosSetPokedoru(Hero *hero, s32 pokedoru);
void heroBiosSetHomePlace(Hero *hero, u8 homePlace);
void heroBiosSetNamePtr(Hero *hero, u16 *name);
void heroBiosSetRnd(Hero *hero, u32 rnd);
void heroBiosSetSexDataId(Hero *hero, u8 sexDataId);
u16 *heroBiosGetNamePtr(Hero *hero);
u32 heroBiosGetRnd(Hero *hero);
Pokemon *heroBiosGetPokemonPtr(Hero *hero, u16 slot);

#ifdef __cplusplus
}
#endif

#endif

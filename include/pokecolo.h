#ifndef POKECOLO_H
#define POKECOLO_H

#include <types.h>

/* Regional builds reported by pokecoloGetCountryCode(). */
enum PokeColoCountryCode {
    POKECOLO_COUNTRY_CODE_JAPAN = 0,
    POKECOLO_COUNTRY_CODE_USA = 1,
    POKECOLO_COUNTRY_CODE_EUROPE = 2
};

#ifdef __cplusplus
extern "C" {
#endif
f32 pokecoloGetFrameTime(void);
s32 pokecoloGetCountryCode(void);
void pokecoloInitRomVersion(void);
s32 pokecoloGetLanguage(void);
s32 pokecoloGetRomVersion(void);
unsigned long pokecoloGetPlayTimeValue(void);
void pokecoloSetPlayTime(f64 playTime);
void pokecoloStartCountPlayTime(u8 started);
f64 pokecoloGetPlayTime(void);
void pokecoloCountPlayTime(void);
#ifdef __cplusplus
}
#endif

#endif

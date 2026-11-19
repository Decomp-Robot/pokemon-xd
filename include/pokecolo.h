#ifndef POKECOLO_H
#define POKECOLO_H

#include <types.h>

/* Regional builds reported by pokecoloGetCountryCode(). */
enum PokeColoCountryCode {
    POKECOLO_COUNTRY_CODE_JAPAN = 0,
    POKECOLO_COUNTRY_CODE_USA = 1,
    POKECOLO_COUNTRY_CODE_EUROPE = 2
};

/* Text languages reported by pokecoloGetLanguage(). */
enum PokeColoLanguage {
    POKECOLO_LANGUAGE_ENGLISH = 2,
    POKECOLO_LANGUAGE_GERMAN = 3,
    POKECOLO_LANGUAGE_FRENCH = 4,
    POKECOLO_LANGUAGE_ITALIAN = 5,
    POKECOLO_LANGUAGE_SPANISH = 6
};

/* ROM versions reported by pokecoloGetRomVersion(). Version 2 is the one
   whose encounter-less fight rooms use the alternate fight kind and whose
   floors select the effect viewer. */
enum PokeColoRomVersion {
    POKECOLO_ROM_VERSION_DEFAULT = 0,
    POKECOLO_ROM_VERSION_ENCOUNTERLESS_FIGHT = 2,
    POKECOLO_ROM_VERSION_EX = 4
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

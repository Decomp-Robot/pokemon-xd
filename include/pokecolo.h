#ifndef POKECOLO_H
#define POKECOLO_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
s32 pokecoloGetRomVersion(void);
unsigned long pokecoloGetPlayTimeUnsigned(void);
void pokecoloSetPlayTime(f64 playtime);
#ifdef __cplusplus
}
#endif

#endif

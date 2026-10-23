#ifndef HERO_STATUS_H
#define HERO_STATUS_H

#include <hero.h>

enum { HERO_STATUS_MONEY = HERO_STATUS_POKEDORU };

#ifdef __cplusplus
extern "C" {
#endif

// A null hero selects the current Hero. Values are words or pointers by selector.
void heroSetStatus(void *hero, unsigned short selector, unsigned long value);

#ifdef __cplusplus
}
#endif

#endif

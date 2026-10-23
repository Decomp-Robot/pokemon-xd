#ifndef HERO_STATUS_H
#define HERO_STATUS_H

enum HeroStatusSelector {
    HERO_STATUS_TRAINER_ID = 2,
    HERO_STATUS_POKEMON = 3,
    HERO_STATUS_MONEY = 12,
    HERO_STATUS_HIZUKI_NAME = 0x17
};

#ifdef __cplusplus
extern "C" {
#endif

// A null hero selects the current Hero. Values are words or pointers by selector.
void heroSetStatus(void *hero, unsigned short selector, unsigned long value);
unsigned long heroGetStatus(void *hero, unsigned short selector, int slot);

#ifdef __cplusplus
}
#endif

#endif

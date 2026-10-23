#ifndef HERO_STATUS_H
#define HERO_STATUS_H

enum {
    HERO_STATUS_TRAINER_ID = 2,
    HERO_STATUS_POKEMON = 3,
    HERO_STATUS_MONEY = 12,
    HERO_STATUS_HIZUKI_NAME = 0x17
};

#ifdef __cplusplus
extern "C" {
#endif

// Values are scalar words or pointers, according to the selector.
void heroSetStatus(void *hero, unsigned long selector, unsigned long value);

unsigned long heroGetStatus(void *hero, int selector, unsigned short slot);

#ifdef __cplusplus
}
#endif

#endif

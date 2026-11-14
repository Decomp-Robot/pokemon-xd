#ifndef EXRIBBON_H
#define EXRIBBON_H

#include <types.h>
#include <Pokemon.h>

enum ExRibbonSlot {
    EXRIBBON_NATIONAL = 4,
    EXRIBBON_EARTH = 5,
    EXRIBBON_COUNT = 11
};

enum SpecialRibbonNo {
    RIBBON_NO_NATIONAL = 44,
    RIBBON_NO_EARTH = 45
};

#ifdef __cplusplus
extern "C" {
#endif

s32 exribbonSetEarthRibbon(Pokemon *pokemon);
s32 exribbonSetNarionalRibbon(Pokemon *pokemon);
u8 exribbonGetNo(s32 ribbon);
void exribbonSetNo(s32 ribbon, u8 no);
void exribbonInit(u8 *numbers);

#ifdef __cplusplus
}
#endif

#endif

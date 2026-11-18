#ifndef MENU_MEW_WAZA_H
#define MENU_MEW_WAZA_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Indexed moves of the pokemon with per-entry generated-move flags, and the
// script sequence that opens their window.
u16 mewWazaGet(long index);
long mewWazaSequence(void);

#ifdef __cplusplus
}
#endif

#endif

#ifndef BATTLEGRID_H
#define BATTLEGRID_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void battleGridReplacePokemon(void *oldSequence, void *newSequence);
void battleGridUpdate(void);

#ifdef __cplusplus
}
#endif

#endif

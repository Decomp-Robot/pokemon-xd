#ifndef TRK_CRITICAL_SECTION_H
#define TRK_CRITICAL_SECTION_H

#include <dolphin/types.h>

void MWEnterCriticalSection(u32* lock);
void MWExitCriticalSection(u32* lock);
void MWInitializeCriticalSection(u32* lock);

#endif

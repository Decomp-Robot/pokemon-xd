#include "nub.h"
#include "critical_section.h"

void MWTRACE(int level, const char* fmt, ...) {
    (void)level;
    (void)fmt;
}

void MWExitCriticalSection(u32* lock) {
    OSRestoreInterrupts(*lock);
}

void MWEnterCriticalSection(u32* lock) {
    *lock = OSDisableInterrupts();
}

void MWInitializeCriticalSection(u32* lock) {
    (void)lock;
}

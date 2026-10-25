#ifndef WAZA_VIEWER_H
#define WAZA_VIEWER_H

#include <types.h>

struct WazaViewerWork {
    u8 unknownBeforeSelections[22];
    u8 pokemonSelectionState;
    u8 moveSelectionState;
    u8 unknownBeforeRunning[24];
    s32 debugRunning;
    u8 unknownAfterRunning[4];
};

extern struct WazaViewerWork wazaViewerData;

#endif

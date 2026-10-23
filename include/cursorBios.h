#ifndef CURSOR_BIOS_H
#define CURSOR_BIOS_H

#include <types.h>

typedef struct CURSOR {
    s16 x;
    s16 y;
} CURSOR;

#ifdef __cplusplus
extern "C" {
#endif

CURSOR cursorBiosGetPos(int cursorID);
CURSOR cursorBiosSetPos(int cursorID, CURSOR pos);

#ifdef __cplusplus
}
#endif

#endif

#ifndef MENU_TOP_H
#define MENU_TOP_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void menuTopSetInitCursor(s32 cursor);
s32 menuTopSelectMode(void);
u8 menuTopIsRetainingResources(void);

#ifdef __cplusplus
}
#endif

#endif

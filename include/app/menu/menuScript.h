#ifndef APP_MENU_SCRIPT_H
#define APP_MENU_SCRIPT_H

#include <types.h>

extern "C" {
void menuScriptSetOpenWorldMapFlag(long enabled);
u8 menuScriptGetOpenWorldMapFlag(void);
u16 menuScriptOpenWorldMapGetMapNumber(void);
}

#endif

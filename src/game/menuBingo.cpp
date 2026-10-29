#include <types.h>
#include <app/menu/menuBingo.h>

extern "C" u8 menuBattleDeBingoGetMode(void) { return gBingo.mMode; }

extern "C" u8 menuBattleDeBingoGetMball(void) { return gBingo.mMball; }

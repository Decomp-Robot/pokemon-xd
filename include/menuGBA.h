#ifndef MENUGBA_H
#define MENUGBA_H
#include <types.h>

class Hero;
extern "C" long openMenuGBA(long menu, long mode, Hero** heroes);
extern "C" void menuGBACSetRomVer(long port, unsigned short version);
#endif

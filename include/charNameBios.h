#ifndef CHAR_NAME_BIOS_H
#define CHAR_NAME_BIOS_H
#include <types.h>
extern "C" {
u16 charNameBiosGetHearFlag(u32 name);
u32 charNameBiosGetNameID(u32 name);
}
#endif

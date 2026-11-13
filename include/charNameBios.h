#ifndef CHAR_NAME_BIOS_H
#define CHAR_NAME_BIOS_H
#include <types.h>
extern "C" {
u16 charNameBiosGetHearFlag(u16 index);
u16 charNameBiosSearchIndex(u32 nameID);
u32 charNameBiosGetNameID(u16 index);
}
#endif

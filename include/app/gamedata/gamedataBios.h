#ifndef APP_GAMEDATA_BIOS_H
#define APP_GAMEDATA_BIOS_H

#include <types.h>

typedef struct GamedataAttest GamedataAttest;

#ifdef __cplusplus
extern "C" {
#endif

u8 gamedataAttestBiosGetLangareaId(GamedataAttest* attest);

#ifdef __cplusplus
}
#endif

#endif

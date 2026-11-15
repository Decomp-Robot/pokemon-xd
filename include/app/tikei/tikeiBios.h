#ifndef APP_TIKEI_TIKEIBIOS_H
#define APP_TIKEI_TIKEIBIOS_H

#include <types.h>

struct TikeiData {
    u8 zokuseiDataId;
    u16 fightKoukaId;
    u16 wazaId;
    u8 reserved[6];
};

extern "C" {
extern TikeiData tikei_data[];
extern u32 tikei_data_number;

u16 tikeiDataBiosGetWazaId(u16 tikeiId);
u8 tikeiDataBiosGetZokuseiDataId(u16 tikeiId);
u16 tikeiDataBiosGetFightKoukaId(u16 tikeiId);
}

#endif

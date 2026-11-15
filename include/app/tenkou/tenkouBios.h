#ifndef APP_TENKOU_TENKOUBIOS_H
#define APP_TENKOU_TENKOUBIOS_H

#include <types.h>

struct TenkouData {
    u8 solarFlag;
    u16 joutaiDataId;
    u32 reserved;
    u32 fightInitMsgId;
    u32 startMsgId;
    u32 continueMsgId;
    u32 endMsgId;
};

extern "C" {
extern TenkouData tenkou_data[];
extern u32 tenkou_data_number;

u32 tenkouDataBiosGetFightInitMsgId(u16 tenkouId);
u8 tenkouDataBiosGetSolarFlag(u16 tenkouId);
}

#endif

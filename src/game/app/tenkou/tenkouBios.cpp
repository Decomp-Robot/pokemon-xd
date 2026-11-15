#include "app/tenkou/tenkouBios.h"

static inline TenkouData *getTenkouData(u16 tenkouId)
{
    if (tenkouId > tenkou_data_number) {
        return 0;
    }
    return &tenkou_data[tenkouId];
}

u32 tenkouDataBiosGetFightInitMsgId(u16 tenkouId)
{
    TenkouData *data = getTenkouData(tenkouId);
    if (data == 0) {
        return 0;
    }
    return data->fightInitMsgId;
}

u8 tenkouDataBiosGetSolarFlag(u16 tenkouId)
{
    TenkouData *data = getTenkouData(tenkouId);
    if (data == 0) {
        return 0;
    }
    return data->solarFlag != 0;
}

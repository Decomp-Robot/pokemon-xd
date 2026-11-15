#include "app/tikei/tikeiBios.h"

static inline TikeiData *getTikeiData(u16 tikeiId)
{
    if (tikeiId > tikei_data_number) {
        return 0;
    }
    return &tikei_data[tikeiId];
}

u16 tikeiDataBiosGetWazaId(u16 tikeiId)
{
    TikeiData *data = getTikeiData(tikeiId);
    if (data == 0) {
        return 0;
    }
    return data->wazaId;
}

u8 tikeiDataBiosGetZokuseiDataId(u16 tikeiId)
{
    TikeiData *data = getTikeiData(tikeiId);
    if (data == 0) {
        return 0;
    }
    return data->zokuseiDataId;
}

u16 tikeiDataBiosGetFightKoukaId(u16 tikeiId)
{
    TikeiData *data = getTikeiData(tikeiId);
    if (data == 0) {
        return 0;
    }
    return data->fightKoukaId;
}

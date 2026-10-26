#include "app/fight/fightKoukaBios.h"

extern "C" {

unsigned short fightKoukaDataBiosGetKoukaDataId(const FightKoukaData* data) {
    if (data == 0) return 0;
    return data->koukaDataId;
}

unsigned short fightKoukaDataBiosGetFightTargetDataId(const FightKoukaData* data) {
    if (data == 0) return 0;
    return data->fightTargetDataId;
}

unsigned short fightKoukaDataBiosGetFightJoukenDataId(const FightKoukaData* data) {
    if (data == 0) return 0;
    return data->joukenDataId;
}

FightKoukaData* fightKoukaDataBiosGetPtr(unsigned short id) {
    unsigned int index = id;
    if (index >= fight_kouka_data_number) return 0;
    return fight_kouka_data + index;
}
}

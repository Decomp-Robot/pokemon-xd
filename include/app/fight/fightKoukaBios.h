#ifndef APP_FIGHT_FIGHTKOUKABIOS_H
#define APP_FIGHT_FIGHTKOUKABIOS_H

struct FightKoukaData {
    unsigned short joukenDataId;
    unsigned short fightTargetDataId;
    unsigned short koukaDataId;
};

#ifdef __cplusplus
extern "C" {
#endif
extern FightKoukaData fight_kouka_data[];
extern unsigned int fight_kouka_data_number;

unsigned short fightKoukaDataBiosGetKoukaDataId(const FightKoukaData* data);
unsigned short fightKoukaDataBiosGetFightTargetDataId(const FightKoukaData* data);
unsigned short fightKoukaDataBiosGetFightJoukenDataId(const FightKoukaData* data);
FightKoukaData* fightKoukaDataBiosGetPtr(unsigned short id);
#ifdef __cplusplus
}
#endif

#endif

#ifndef APP_FIGHT_FIGHTKOUKA_H
#define APP_FIGHT_FIGHTKOUKA_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned short fightKoukaDataGetKoukaDataId(unsigned short fightKoukaId);
unsigned short fightKoukaDataGetFightTargetDataId(unsigned short fightKoukaId);
unsigned short fightKoukaDataGetFightJoukenDataId(unsigned short fightKoukaId);
void fightKoukaDoFightKoukaJoukenAndKouka(void *target, unsigned short fightKoukaId);
int fightKoukaDoJoukenAndKouka(void *target, unsigned short joukenDataId,
                              unsigned short fightTargetDataId, unsigned short koukaDataId);

#ifdef __cplusplus
}
#endif

#endif

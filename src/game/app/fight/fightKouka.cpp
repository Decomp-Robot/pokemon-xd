#include "app/fight/fightKouka.h"

#include "app/fight/fightKoukaBios.h"
#include <fightFloor.h>
#include <fightTarget.h>

extern "C" {
unsigned char fightJoukenExec(unsigned short joukenDataId, void *target);
void koukaExec(unsigned long koukaDataId, void *target, void *param, long *result);
}

unsigned short fightKoukaDataGetKoukaDataId(unsigned short fightKoukaId) {
    return fightKoukaDataBiosGetKoukaDataId(fightKoukaDataBiosGetPtr(fightKoukaId));
}

unsigned short fightKoukaDataGetFightTargetDataId(unsigned short fightKoukaId) {
    return fightKoukaDataBiosGetFightTargetDataId(fightKoukaDataBiosGetPtr(fightKoukaId));
}

unsigned short fightKoukaDataGetFightJoukenDataId(unsigned short fightKoukaId) {
    return fightKoukaDataBiosGetFightJoukenDataId(fightKoukaDataBiosGetPtr(fightKoukaId));
}

void fightKoukaDoFightKoukaJoukenAndKouka(void *target, unsigned short fightKoukaId) {
    unsigned short joukenDataId = fightKoukaDataGetFightJoukenDataId(fightKoukaId);
    unsigned short fightTargetDataId = fightKoukaDataGetFightTargetDataId(fightKoukaId);
    unsigned short koukaDataId = fightKoukaDataGetKoukaDataId(fightKoukaId);
    fightKoukaDoJoukenAndKouka(target, joukenDataId, fightTargetDataId, koukaDataId);
}

int fightKoukaDoJoukenAndKouka(void *target, unsigned short joukenDataId,
                              unsigned short fightTargetDataId, unsigned short koukaDataId) {
    int result = 0;
    void *koukaTarget = fightTargetGetPtr(fightTargetDataId, target, fightFloor_GetFightType(0));
    if (fightJoukenExec(joukenDataId, target) == 1) {
        koukaExec(koukaDataId, koukaTarget, target, 0);
        result = 1;
    }
    return result;
}

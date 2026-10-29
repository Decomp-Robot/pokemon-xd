#include "app/fight/fightAbicnt.h"

extern "C" {

int fightAbicntFitMinMax(int abicnt) {
    if (abicnt < 0) abicnt = 0;
    if (abicnt > 12) abicnt = 12;
    return abicnt;
}

unsigned int fightAbicntDoKakeWaru(unsigned short abicnt, unsigned int value) {
    unsigned char waru = fightAbicntGetWaru(abicnt);
    unsigned char kake = fightAbicntGetKake(abicnt);
    return value * kake / waru;
}

unsigned int fightAbicntGetWaru(unsigned short abicnt) {
    FightAbicntData* data = fightAbicntDataBiosGetPtr(abicnt);
    if (data == 0) return 1;
    return fightAbicntDataBiosGetWaru(data);
}

unsigned int fightAbicntGetKake(unsigned short abicnt) {
    FightAbicntData* data = fightAbicntDataBiosGetPtr(abicnt);
    if (data == 0) return 0;
    return fightAbicntDataBiosGetKake(data);
}

unsigned int fightAbicntDataBiosGetWaru(const FightAbicntData* data) {
    if (data == 0) return 1;
    return data->waru;
}

unsigned int fightAbicntDataBiosGetKake(const FightAbicntData* data) {
    if (data == 0) return 0;
    return data->kake;
}

FightAbicntData* fightAbicntDataBiosGetPtr(unsigned short abicntId) {
    FightAbicntData* data = fight_abicnt_data + abicntId;
    if (abicntId >= fight_abicnt_data_number) return 0;
    return data;
}
}

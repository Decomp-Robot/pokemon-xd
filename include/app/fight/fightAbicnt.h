#ifndef APP_FIGHT_FIGHTABICNT_H
#define APP_FIGHT_FIGHTABICNT_H

struct FightAbicntData {
    unsigned char kake;
    unsigned char waru;
};

#ifdef __cplusplus
extern "C" {
#endif
extern unsigned int fight_abicnt_data_number;
extern FightAbicntData fight_abicnt_data[];

int fightAbicntFitMinMax(int abicnt);
unsigned int fightAbicntDoKakeWaru(unsigned short abicnt, unsigned int value);
unsigned int fightAbicntGetWaru(unsigned short abicnt);
unsigned int fightAbicntGetKake(unsigned short abicnt);
unsigned int fightAbicntDataBiosGetWaru(const FightAbicntData* data);
unsigned int fightAbicntDataBiosGetKake(const FightAbicntData* data);
FightAbicntData* fightAbicntDataBiosGetPtr(unsigned short abicntId);
#ifdef __cplusplus
}
#endif

#endif

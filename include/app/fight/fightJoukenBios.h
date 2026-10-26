#ifndef APP_FIGHT_FIGHTJOUKENBIOS_H
#define APP_FIGHT_FIGHTJOUKENBIOS_H

struct FightJoukenLinkData {
    unsigned char joint;
    unsigned char reserved;
    unsigned short joukenDataId;
    unsigned short nextLinkDataId;
};

struct FightJoukenValue {
    unsigned char kind;
    unsigned char hoseiFlag;
    short kake;
    short waru;
    unsigned short target;
    unsigned short buff;
};

struct FightJoukenData {
    unsigned char form;
    unsigned char reserved;
    unsigned short link;
    FightJoukenValue values[2];
};

#ifdef __cplusplus
extern "C" {
#endif
extern unsigned int fight_jouken_data_number;
extern FightJoukenData fight_jouken_data[];
extern unsigned int fight_jouken_link_data_number;
extern FightJoukenLinkData fight_jouken_link_data[];

int fightJoukenLinkDataBiosGetNextLinkDataId(unsigned short id);
int fightJoukenLinkDataBiosGetJoukenDataId(unsigned short id);
int fightJoukenLinkDataBiosGetJoint(unsigned short id);
FightJoukenLinkData* fightJoukenLinkDataBiosGetDataPtr(unsigned short id);
int fightJoukenDataBiosGetValueWaru(unsigned short id, unsigned short valueIndex);
int fightJoukenDataBiosGetValueKake(unsigned short id, unsigned short valueIndex);
int fightJoukenDataBiosGetValueHoseiFlag(unsigned short id, unsigned short valueIndex);
int fightJoukenDataBiosGetValueBuff(unsigned short id, unsigned short valueIndex);
int fightJoukenDataBiosGetValueTarget(unsigned short id, unsigned short valueIndex);
int fightJoukenDataBiosGetValueKind(unsigned short id, unsigned short valueIndex);
int fightJoukenDataBiosGetLink(unsigned short id);
int fightJoukenDataBiosGetForm(unsigned short id);
FightJoukenValue* fightJoukenDataBiosGetValuePtr(unsigned short id, unsigned short valueIndex);
FightJoukenData* fightJoukenDataBiosGetDataPtr(unsigned short id);
#ifdef __cplusplus
}
#endif

#endif

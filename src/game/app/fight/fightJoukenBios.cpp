#include "app/fight/fightJoukenBios.h"

extern "C" {
FightJoukenLinkData* fightJoukenLinkDataBiosGetDataPtr(unsigned short id);
FightJoukenData* fightJoukenDataBiosGetDataPtr(unsigned short id);

unsigned short fightJoukenLinkDataBiosGetNextLinkDataId(unsigned short id) {
    FightJoukenLinkData* data = fightJoukenLinkDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->nextLinkDataId;
}

unsigned short fightJoukenLinkDataBiosGetJoukenDataId(unsigned short id) {
    FightJoukenLinkData* data = fightJoukenLinkDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->joukenDataId;
}

unsigned char fightJoukenLinkDataBiosGetJoint(unsigned short id) {
    FightJoukenLinkData* data = fightJoukenLinkDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->joint;
}

FightJoukenLinkData* fightJoukenLinkDataBiosGetDataPtr(unsigned short id) {
    if (id >= fight_jouken_link_data_number) return 0;
    return fight_jouken_link_data + id;
}

short fightJoukenDataBiosGetValueWaru(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->waru;
}

short fightJoukenDataBiosGetValueKake(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->kake;
}

unsigned char fightJoukenDataBiosGetValueHoseiFlag(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->hoseiFlag > 0;
}

unsigned short fightJoukenDataBiosGetValueBuff(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->buff;
}

unsigned short fightJoukenDataBiosGetValueTarget(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->target;
}

unsigned char fightJoukenDataBiosGetValueKind(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->kind;
}

unsigned short fightJoukenDataBiosGetLink(unsigned short id) {
    FightJoukenData* data = fightJoukenDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->link;
}

unsigned char fightJoukenDataBiosGetForm(unsigned short id) {
    FightJoukenData* data = fightJoukenDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->form;
}

FightJoukenValue* fightJoukenDataBiosGetValuePtr(unsigned short id, unsigned short valueIndex) {
    FightJoukenData* data = fightJoukenDataBiosGetDataPtr(id);
    if (!data) return 0;
    if (valueIndex >= 2) return 0;
    return &data->values[valueIndex];
}

FightJoukenData* fightJoukenDataBiosGetDataPtr(unsigned short id) {
    if (id >= fight_jouken_data_number) return 0;
    return fight_jouken_data + id;
}
}

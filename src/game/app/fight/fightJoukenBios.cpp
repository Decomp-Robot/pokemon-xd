#include "app/fight/fightJoukenBios.h"

extern "C" {
FightJoukenLinkData* fightJoukenLinkDataBiosGetDataPtr(unsigned short id);
FightJoukenData* fightJoukenDataBiosGetDataPtr(unsigned short id);

int fightJoukenLinkDataBiosGetNextLinkDataId(unsigned short id) {
    FightJoukenLinkData* data = fightJoukenLinkDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->nextLinkDataId;
}

int fightJoukenLinkDataBiosGetJoukenDataId(unsigned short id) {
    FightJoukenLinkData* data = fightJoukenLinkDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->joukenDataId;
}

int fightJoukenLinkDataBiosGetJoint(unsigned short id) {
    FightJoukenLinkData* data = fightJoukenLinkDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->joint;
}

FightJoukenLinkData* fightJoukenLinkDataBiosGetDataPtr(unsigned short id) {
    if (id >= fight_jouken_link_data_number) return 0;
    return fight_jouken_link_data + id;
}

int fightJoukenDataBiosGetValueWaru(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->waru;
}

int fightJoukenDataBiosGetValueKake(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->kake;
}

int fightJoukenDataBiosGetValueHoseiFlag(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->hoseiFlag > 0;
}

int fightJoukenDataBiosGetValueBuff(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->buff;
}

int fightJoukenDataBiosGetValueTarget(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->target;
}

int fightJoukenDataBiosGetValueKind(unsigned short id, unsigned short valueIndex) {
    FightJoukenValue* value = fightJoukenDataBiosGetValuePtr(id, valueIndex);
    if (value == 0) return 0;
    return value->kind;
}

int fightJoukenDataBiosGetLink(unsigned short id) {
    FightJoukenData* data = fightJoukenDataBiosGetDataPtr(id);
    if (data == 0) return 0;
    return data->link;
}

int fightJoukenDataBiosGetForm(unsigned short id) {
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

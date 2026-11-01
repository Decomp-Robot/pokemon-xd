#include "app/fight/fightJoukenBios.h"
#include <fightFloor.h>
#include <fightTarget.h>
#include <hsdRandom.h>
#include <status.h>

enum FightJoukenJoint { FIGHT_JOUKEN_JOINT_OR = 1, FIGHT_JOUKEN_JOINT_AND = 2 };
enum FightJoukenForm { FIGHT_JOUKEN_FORM_ALWAYS = 0, FIGHT_JOUKEN_FORM_EQUAL = 1, FIGHT_JOUKEN_FORM_NOT_EQUAL = 2, FIGHT_JOUKEN_FORM_GREATER_EQUAL = 3, FIGHT_JOUKEN_FORM_LESS_EQUAL = 4, FIGHT_JOUKEN_FORM_LESS = 5, FIGHT_JOUKEN_FORM_GREATER = 6 };
enum FightJoukenValueKind { FIGHT_JOUKEN_VALUE_NONE = 0, FIGHT_JOUKEN_VALUE_IMMEDIATE = 1, FIGHT_JOUKEN_VALUE_RANDOM_RANGE = 2, FIGHT_JOUKEN_VALUE_TARGET_STATUS = 3 };
enum FightJoukenCorrectionMode { FIGHT_JOUKEN_CORRECTION_STATUS = 0, FIGHT_JOUKEN_CORRECTION_MULTIPLY_DIVIDE = 1 };

unsigned char _fightJoukenExecOne(unsigned short joukenDataId, void *target);
s32 _fightJoukenGetValue(unsigned short joukenDataId, unsigned short valueIndex, void *target);

// Evaluates a jouken (battle condition) and every condition chained onto it.
extern "C" unsigned char fightJoukenExec(unsigned short joukenDataId, void *target) {
    u8 result = _fightJoukenExecOne(joukenDataId, target);
    u16 linkJoukenDataId;
    u16 link = fightJoukenDataBiosGetLink(joukenDataId);

    if (link == 0) return result;

    do {
        linkJoukenDataId = fightJoukenLinkDataBiosGetJoukenDataId(link);
        u8 linkResult = _fightJoukenExecOne(linkJoukenDataId, target);
        u8 joint = fightJoukenLinkDataBiosGetJoint(linkJoukenDataId);

        switch (joint) {
        case FIGHT_JOUKEN_JOINT_OR:
            if (result || linkResult) result = 1;
            else result = 0;
            break;
        case FIGHT_JOUKEN_JOINT_AND:
            if (result && linkResult) result = 1;
            else result = 0;
            break;
        }

        link = fightJoukenLinkDataBiosGetNextLinkDataId(link);
    } while (link != 0);

    return result;
}

// Evaluates a single jouken by comparing the two values it resolves as its form requests.
unsigned char _fightJoukenExecOne(unsigned short joukenDataId, void *target) {
    s32 values[2];
    u8 i;

    for (i = 0; i < 2; i++) {
        values[i] = _fightJoukenGetValue(joukenDataId, i, target);
    }

    u8 result = 0;
    u8 form = fightJoukenDataBiosGetForm(joukenDataId);

    switch (form) {
    case FIGHT_JOUKEN_FORM_ALWAYS:
        result = 1;
        break;
    case FIGHT_JOUKEN_FORM_EQUAL:
        if (values[0] == values[1]) result = 1;
        break;
    case FIGHT_JOUKEN_FORM_NOT_EQUAL:
        if (values[0] != values[1]) result = 1;
        break;
    case FIGHT_JOUKEN_FORM_GREATER_EQUAL:
        if (values[0] >= values[1]) result = 1;
        break;
    case FIGHT_JOUKEN_FORM_LESS_EQUAL:
        if (values[0] <= values[1]) result = 1;
        break;
    case FIGHT_JOUKEN_FORM_LESS:
        if (values[0] < values[1]) result = 1;
        break;
    case FIGHT_JOUKEN_FORM_GREATER:
        if (values[0] > values[1]) result = 1;
        break;
    }
    return result;
}

// Resolves one operand of a jouken: an immediate value, a random value or a fight status value.
s32 _fightJoukenGetValue(unsigned short joukenDataId, unsigned short valueIndex, void *target) {
    s32 value = 0;
    u8 kind = fightJoukenDataBiosGetValueKind(joukenDataId, valueIndex);
    u16 valueTarget = fightJoukenDataBiosGetValueTarget(joukenDataId, valueIndex);
    u16 valueBuff = fightJoukenDataBiosGetValueBuff(joukenDataId, valueIndex);
    s16 kake = fightJoukenDataBiosGetValueKake(joukenDataId, valueIndex);
    s16 waru = fightJoukenDataBiosGetValueWaru(joukenDataId, valueIndex);
    u8 hoseiFlag = fightJoukenDataBiosGetValueHoseiFlag(joukenDataId, valueIndex);

    switch (kind) {
    case FIGHT_JOUKEN_VALUE_NONE:
        break;
    case FIGHT_JOUKEN_VALUE_IMMEDIATE:
        value = valueTarget;
        break;
    case FIGHT_JOUKEN_VALUE_RANDOM_RANGE: {
        u32 random = (HSD_Rand() << 16) | HSD_Rand();
        u32 base = valueTarget;
        value = base + random % (valueBuff - base);
        break;
    }
    case FIGHT_JOUKEN_VALUE_TARGET_STATUS: {
        void *fightTarget = fightTargetGetPtr(valueTarget, target, fightFloor_GetFightType(0));
        if (fightTarget == 0) return 0;
        u8 statusKind = fightTargetDataBiosGetStatusKid(fightTargetDataBiosGetPtr(valueTarget));
        if (hoseiFlag == FIGHT_JOUKEN_CORRECTION_STATUS)
            value = statusGetStatus(statusKind, fightTarget, kake, valueBuff, waru);
        else
            value = statusGetStatus(statusKind, fightTarget, 0, valueBuff, 0);
        break;
    }
    }
    if (hoseiFlag == FIGHT_JOUKEN_CORRECTION_MULTIPLY_DIVIDE) {
        value *= kake;
        if (waru != 0) value /= waru;
    }
    return value;
}

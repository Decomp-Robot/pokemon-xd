#include "app/kouka/koukaBios.h"
#include <fightFloor.h>
#include <fightTarget.h>
#include <status.h>

enum KoukaOperation {
    KOUKA_SET = 0,
    KOUKA_SCALE = 1,
    KOUKA_ADD = 2,
    KOUKA_SUBTRACT = 3,
    KOUKA_ADD_RATIO = 4,
    KOUKA_SUBTRACT_RATIO = 5
};

enum KoukaValueSource {
    KOUKA_VALUE_HALF_STATUS = -1,
    KOUKA_VALUE_STATUS = -2
};

long _koukaOneExec(unsigned long koukaId, void* target, void* source, long* result);

extern "C" void koukaExec(unsigned long koukaId, void* target, void* source, long* result) {
    if (result) {
        _koukaOneExec(koukaId, target, source, result);
    } else {
        _koukaOneExec(koukaId, target, source, 0);
    }
    unsigned short linkId = koukaDataBiosGetLink(koukaId);
    if (linkId) {
        for (unsigned short i = 0; i < KOUKA_LINK_COUNT; i++) {
            if (koukaLinkDataBiosGetKouka(linkId, i)) {
                if (result) {
                    _koukaOneExec(koukaId, target, source, &result[i + 1]);
                } else {
                    _koukaOneExec(koukaId, target, source, 0);
                }
            }
        }
    }
}

long _koukaOneExec(unsigned long koukaId, void* target, void* source, long* result) {
    if (koukaId == 0) return 0;
    unsigned short status;
    unsigned char statusKind;
    statusKind = koukaDataBiosGetStatusKind(koukaId);
    status = koukaDataBiosGetStatus(koukaId);
    short values[KOUKA_VALUE_COUNT];
    values[0] = koukaDataBiosGetValue(koukaId, 0);
    values[1] = koukaDataBiosGetValue(koukaId, 1);
    unsigned long operation = koukaDataBiosGetVar(koukaId);
    long oldValue = statusGetStatus(statusKind, target, 0, status, values[0]);
    if (operation == KOUKA_SET || operation == KOUKA_ADD || operation == KOUKA_SUBTRACT) {
        if (values[0] == KOUKA_VALUE_HALF_STATUS) {
            values[0] = statusGetStatus(statusKind, target, 0, values[1], 0) / 2;
        } else if (values[0] == KOUKA_VALUE_STATUS) {
            values[0] = statusGetStatus(statusKind, target, 0, values[1], 0);
        } else if (values[0] < 0 || values[1] < 0) {
            return 0;
        }
    }
    long newValue;
    switch (operation) {
    case KOUKA_SET: newValue = values[0]; break;
    case KOUKA_SCALE: newValue = oldValue * values[0] / values[1]; break;
    case KOUKA_ADD: newValue = oldValue + values[0]; break;
    case KOUKA_SUBTRACT: newValue = oldValue - values[0]; break;
    case KOUKA_ADD_RATIO: newValue = oldValue + oldValue * values[0] / values[1]; break;
    case KOUKA_SUBTRACT_RATIO: newValue = oldValue - oldValue * values[0] / values[1]; break;
    default: return 0;
    }
    unsigned short sourceId;
    if (source) {
        sourceId = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(source,
            fightFloorGetStatus(0, 0, FIGHT_FLOOR_STATUS_FIGHT_TYPE, 0));
    } else {
        sourceId = 0;
    }
    long statusResult = statusSetStatus(statusKind, target, 0, status, sourceId, newValue);
    if (result) *result = newValue;
    return statusResult;
}

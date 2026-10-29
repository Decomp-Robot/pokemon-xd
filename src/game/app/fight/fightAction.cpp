#include <fightAction.h>

FIGHT_ACTION _fight_action_fifo[FIGHT_ACTION_FIFO_NUM];
u32 _fight_action_fifo_head;
u32 _fight_action_fifo_tail;

FIGHT_ACTION_DATA_dd_ *_fightActionDataSearchDataPtr(FIGHT_ACTION_DATA_dd_ *data, u16 kind, s32 buff) {
    s32 curBuff;
    u16 curKind;
    u16 i;
    FIGHT_ACTION_DATA_dd_ *cur;

    if (data == 0) {
        return 0;
    }
    for (i = 0;; i++) {
        cur = &data[i];
        curKind = fightActionDataBiosGetKind(cur);
        if (curKind == 0) {
            break;
        }
        curBuff = fightActionDataBiosGetBuff(cur);
        if (kind == curKind && buff == curBuff) {
            return cur;
        }
    }
    return 0;
}

u8 _fightActionFlowSub(FIGHT_ACTION *action) {
    FIGHT_ACTION_FUNC func;

    if (fightActionCheckValid(action) == 0) {
        return 0;
    }
    func = fightActionKindDataBiosGetFlowFuncPtr(
        fightActionKindDataBiosGetPtr(fightActionDataBiosGetKind(fightActionBiosGetFightActionDataPtr(action))));
    if (func != 0) {
        return func(action);
    } else {
        return 1;
    }
}

FIGHT_ACTION_DATA_dd_ *fightActionSearchDataPtr(FIGHT_ACTION *action) {
    FIGHT_ACTION_DATA_dd_ *data;
    u16 kind;
    s32 buff;

    if (fightActionCheckValid(action) == 0) {
        return 0;
    }
    kind = fightActionBiosGetKind(action);
    buff = fightActionBiosGetBuff(action);
    data = _fightActionDataSearchDataPtr(fightActionBiosGetFightActionDataPtr(action), kind, buff);
    if (data == 0) {
        return 0;
    }
    fightActionBiosSetFightActionDataPtr(action, data);
    return data;
}

void fightActionInit(FIGHT_ACTION *action) {
    u16 i;

    fightActionBiosSetKind(action, 0);
    fightActionBiosSetBuff(action, 0);
    fightActionBiosSetFightActionDataPtr(action, 0);
    for (i = 0; i < 4; i++) {
        fightActionBiosSetDispBuff(action, i, 0);
    }
    fightActionBiosSetBuffDataPtr(action, 0);
    fightActionBiosSetBuffDataId(action, 0);
    fightActionBiosSetActorFightTargetPtr(action, 0);
    fightActionBiosSetMotoFightActionDataPtr(action, 0);
    fightActionBiosSetFifoBanme(action, -1);
}

u8 fightActionCreate(FIGHT_ACTION *action, FIGHT_ACTION_DATA_dd_ *motoActionData, void *pokemon, u16 kind,
                     s32 bufferSelector, void *actionData) {
    fightActionInit(action);
    fightActionBiosSetKind(action, kind);
    fightActionBiosSetBuff(action, bufferSelector);
    fightActionBiosSetFightActionDataPtr(action, actionData);
    fightActionBiosSetActorFightTargetPtr(action, pokemon);
    if (fightActionSearchDataPtr(action) == 0) {
        fightActionInit(action);
        return 4;
    }
    fightActionBiosSetBuffDataId(action, bufferSelector);
    fightActionBiosSetMotoFightActionDataPtr(action, motoActionData);
    return 1;
}

u8 fightActionCheckValid(void *action) {
    FIGHT_ACTION *actionData = (FIGHT_ACTION *)action;

    if (actionData == 0) {
        return 0;
    }
    if (fightActionBiosGetKind(actionData) == 0) {
        return 0;
    }
    if (fightActionBiosGetFightActionDataPtr(actionData) == 0) {
        return 0;
    }
    return 1;
}

void fightActionFifoInit(void) {
    u16 i;

    _fight_action_fifo_head = 0;
    _fight_action_fifo_tail = 0;
    for (i = 0; i < FIGHT_ACTION_FIFO_NUM; i++) {
        fightActionInit(&_fight_action_fifo[i]);
    }
}

FIGHT_ACTION *fightActionFifoIn(FIGHT_ACTION *action) {
    u32 tail = _fight_action_fifo_tail;
    FIGHT_ACTION *entry;

    if (((tail + 1) & (FIGHT_ACTION_FIFO_NUM - 1)) != _fight_action_fifo_head) {
        _fight_action_fifo[_fight_action_fifo_tail] = *action;
        entry = &_fight_action_fifo[_fight_action_fifo_tail];
        _fight_action_fifo_tail++;
        _fight_action_fifo_tail &= FIGHT_ACTION_FIFO_NUM - 1;
    } else {
        return 0;
    }
    fightActionBiosSetFifoBanme(action, tail);
    fightActionBiosSetFifoBanme(entry, tail);
    return entry;
}

u8 fightActionFifoOut(FIGHT_ACTION *out) {
    FIGHT_ACTION *entry;

    if (_fight_action_fifo_tail != _fight_action_fifo_head) {
        entry = &_fight_action_fifo[_fight_action_fifo_head];
        *out = *entry;
        _fight_action_fifo_head++;
        _fight_action_fifo_head &= FIGHT_ACTION_FIFO_NUM - 1;
    } else {
        return 0;
    }
    return 1;
}

u8 fightActionDisp(FIGHT_ACTION *action) {
    FIGHT_ACTION_FUNC func;

    if (fightActionCheckValid(action) == 0) {
        return 0;
    }
    func = fightActionKindDataBiosGetDispFuncPtr(
        fightActionKindDataBiosGetPtr(fightActionDataBiosGetKind(fightActionBiosGetFightActionDataPtr(action))));
    if (func != 0) {
        return func(action);
    } else {
        return 1;
    }
}

u8 fightActionFlowFifo(FIGHT_ACTION *action) {
    FIGHT_ACTION removed;
    u8 result;
    FIGHT_ACTION *entry;

    entry = fightActionFifoIn(action);
    if (entry == 0) {
        return 2;
    }
    result = _fightActionFlowSub(action);
    if (result != 1) {
        fightActionFifoOut(&removed);
    } else {
        fightActionBiosCopy(entry, action);
    }
    return result;
}

u8 fightActionDispFifo(void) {
    FIGHT_ACTION action;

    if (fightActionFifoOut(&action) == 0) {
        return 3;
    }
    return fightActionDisp(&action);
}

u8 fightActionCreateAndFlowFifo(FIGHT_ACTION *action, FIGHT_ACTION_DATA_dd_ *motoActionData, void *pokemon, u16 kind,
                                s32 bufferSelector, void *actionData) {
    u8 result;

    result = fightActionCreate(action, motoActionData, pokemon, kind, bufferSelector, actionData);
    if (result != 1) {
        return result;
    }
    return fightActionFlowFifo(action);
}

void fightActionDispFifoAll(void) {
    u8 result;

    for (;;) {
        result = fightActionDispFifo();
        if (result == 3) {
            break;
        }
        if (result != 1) {
            break;
        }
    }
}

s8 fightActionGetPri(FIGHT_ACTION *action) {
    FIGHT_ACTION_DATA_dd_ *data;
    FIGHT_KIND_DATA *kindData;

    if (fightActionCheckValid(action) == 0) {
        return -128;
    }
    data = fightActionBiosGetFightActionDataPtr(action);
    if (data == 0) {
        return -128;
    }
    kindData = fightActionKindDataBiosGetPtr(fightActionDataBiosGetKind(data));
    if (kindData == 0) {
        return -128;
    }
    return fightActionKindDataBiosGetPri(kindData);
}

u16 fightActionGetKindDataId(FIGHT_ACTION *action) {
    FIGHT_ACTION_DATA_dd_ *data;

    if (fightActionCheckValid(action) == 0) {
        return 0;
    }
    data = fightActionBiosGetFightActionDataPtr(action);
    if (data == 0) {
        return 0;
    }
    return fightActionDataBiosGetKind(data);
}

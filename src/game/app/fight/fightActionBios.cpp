#include <fightAction.h>

extern "C" {
extern u32 fight_action_kind_data_number;
extern FIGHT_KIND_DATA fight_action_kind_data[];
}

void fightActionBiosSetFifoBanme(FIGHT_ACTION *action, s32 banme) {
    if (action == 0) {
        return;
    }
    action->mFifoBanme = banme;
}

FIGHT_ACTION_FUNC fightActionKindDataBiosGetDispFuncPtr(FIGHT_KIND_DATA *kindData) {
    if (kindData == 0) {
        return 0;
    }
    return kindData->mDispFunc;
}

FIGHT_ACTION_FUNC fightActionKindDataBiosGetFlowFuncPtr(FIGHT_KIND_DATA *kindData) {
    if (kindData == 0) {
        return 0;
    }
    return kindData->mFlowFunc;
}

s8 fightActionKindDataBiosGetPri(FIGHT_KIND_DATA *kindData) {
    if (kindData == 0) {
        return -128;
    }
    return kindData->mPri;
}

FIGHT_KIND_DATA *fightActionKindDataBiosGetPtr(u16 kind) {
    if (kind >= fight_action_kind_data_number) {
        return 0;
    }
    return &fight_action_kind_data[kind];
}

u32 fightActionDataBiosGetBuff(FIGHT_ACTION_DATA_dd_ *data) {
    if (data == 0) {
        return 0;
    }
    return data->mBuff;
}

u16 fightActionDataBiosGetKind(FIGHT_ACTION_DATA_dd_ *data) {
    if (data == 0) {
        return 0;
    }
    return data->mKind;
}

void fightActionBiosSetDispBuff(FIGHT_ACTION *action, u16 id, void *ptr) {
    if (action == 0) {
        return;
    }
    if (id >= 4) {
        return;
    }
    action->mDispBuff[id] = ptr;
}

void fightActionBiosSetMotoFightActionDataPtr(FIGHT_ACTION *action, FIGHT_ACTION_DATA_dd_ *ptr) {
    if (action == 0) {
        return;
    }
    action->mMotoFightActionDataPtr = ptr;
}

void fightActionBiosSetBuffDataId(FIGHT_ACTION *action, u32 buffDataId) {
    if (action == 0) {
        return;
    }
    action->mBuffDataId = buffDataId;
}

void fightActionBiosSetBuffDataPtr(FIGHT_ACTION *action, void *arg) {
    if (action == 0) {
        return;
    }
    action->mBuffDataPtr = arg;
}

void fightActionBiosSetActorFightTargetPtr(FIGHT_ACTION *action, void *ptr) {
    if (action == 0) {
        return;
    }
    action->mActorFightTargetPtr = ptr;
}

void fightActionBiosSetFightActionDataPtr(FIGHT_ACTION *action, void *ptr) {
    if (action == 0) {
        return;
    }
    action->mActionData = (FIGHT_ACTION_DATA_dd_ *)ptr;
}

void fightActionBiosSetBuff(FIGHT_ACTION *action, u32 buff) {
    if (action == 0) {
        return;
    }
    action->mBuff = buff;
}

void fightActionBiosSetKind(FIGHT_ACTION *action, u16 kind) {
    if (action == 0) {
        return;
    }
    action->mKind = kind;
}

u32 fightActionBiosGetBuffDataId(FIGHT_ACTION *action) {
    if (action == 0) {
        return 0;
    }
    return action->mBuffDataId;
}

void *fightActionBiosGetBuffDataPtr(FIGHT_ACTION *action) {
    if (action == 0) {
        return 0;
    }
    return action->mBuffDataPtr;
}

void *fightActionBiosGetActorFightTargetPtr(FIGHT_ACTION *action) {
    if (action == 0) {
        return 0;
    }
    return action->mActorFightTargetPtr;
}

FIGHT_ACTION_DATA_dd_ *fightActionBiosGetFightActionDataPtr(FIGHT_ACTION *action) {
    if (action == 0) {
        return 0;
    }
    return action->mActionData;
}

u32 fightActionBiosGetBuff(FIGHT_ACTION *action) {
    if (action == 0) {
        return 0;
    }
    return action->mBuff;
}

u16 fightActionBiosGetKind(FIGHT_ACTION *action) {
    if (action == 0) {
        return 0;
    }
    return action->mKind;
}

void fightActionBiosCopy(FIGHT_ACTION *dst, FIGHT_ACTION *src) {
    if (dst == 0) {
        return;
    }
    if (src == 0) {
        return;
    }
    *dst = *src;
}

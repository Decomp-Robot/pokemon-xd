#include <fightFloor.h>
#include <fightSide.h>

extern "C" {

void fightSide_SetMakibisiCheckFlag(FIGHT_SIDE *side, u8 flag) {
    if (side != 0) {
        side->mMakibisiCheckFlag = flag;
    }
}

void fightSide_SetFightSideDataId(FIGHT_SIDE *side, u16 dataId) {
    if (side != 0) {
        side->mFightSideDataId = dataId;
    }
}

u8 fightSide_GetMakibisiCheckFlag(FIGHT_SIDE *side) {
    if (side == 0) {
        return 0;
    }
    return side->mMakibisiCheckFlag;
}

FIGHT_TRAINER *fightSide_GetFightTrainerPtr(FIGHT_SIDE *side, u16 index) {
    if (side == 0) {
        return 0;
    }
    if (index >= FIGHT_SIDE_TRAINER_COUNT) {
        return 0;
    }
    return &side->mTrainerAry[index];
}

_JOUTAI *fightSide_GetJoutaiPtr(FIGHT_SIDE *side, u16 index) {
    if (side == 0) {
        return 0;
    }
    if (index >= FIGHT_SIDE_JOUTAI_COUNT) {
        return 0;
    }
    return &side->mJoutaiAry[index];
}

u16 fightSide_GetFightSideDataId(FIGHT_SIDE *side) {
    if (side == 0) {
        return 0;
    }
    return side->mFightSideDataId;
}

u16 fightSideDB_GetYrot(u16 dataId) {
    FIGHT_SIDE_DATA *data = fightSideDataBiosGetPtr(dataId);
    if (data == 0) {
        return 0;
    }
    return data->mYrot;
}

}

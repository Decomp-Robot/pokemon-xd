#include <fightTarget.h>

u8 fightTargetDataBiosGetRelativeFlag(FIGHT_TARGET_DATA *targetData) {
    if (targetData == 0) {
        return 0;
    }
    return targetData->mRelativeFlag;
}

s32 fightTargetDataBiosGetBuff(FIGHT_TARGET_DATA *targetData) {
    if (targetData == 0) {
        return 0;
    }
    return targetData->mBuff;
}

u8 fightTargetDataBiosGetStatusKid(FIGHT_TARGET_DATA *targetData) {
    if (targetData == 0) {
        return 0;
    }
    return targetData->mStatusKid;
}

FIGHT_TARGET_DATA *fightTargetDataBiosGetPtr(u16 target) {
    if (target >= fight_target_data_number) {
        return 0;
    }
    return &fight_target_data[target];
}

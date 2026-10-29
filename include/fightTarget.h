#ifndef FIGHTTARGET_H
#define FIGHTTARGET_H

#include <types.h>

// One entry of the fight target table.
typedef struct FIGHT_TARGET_DATA {
    u8 mStatusKid;
    u8 mRelativeFlag;
    s32 mBuff;
} FIGHT_TARGET_DATA;

#ifdef __cplusplus
extern "C" {
#endif

extern u32 fight_target_data_number;
extern FIGHT_TARGET_DATA fight_target_data[];

u8 fightTargetDataBiosGetRelativeFlag(FIGHT_TARGET_DATA *targetData);
s32 fightTargetDataBiosGetBuff(FIGHT_TARGET_DATA *targetData);
u8 fightTargetDataBiosGetStatusKid(FIGHT_TARGET_DATA *targetData);
FIGHT_TARGET_DATA *fightTargetDataBiosGetPtr(u16 target);
u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(void *target, u16 fightType);
void *fightTargetGetPtrAsNowFightType(u16 kind, void *target);
void *fightTargetGetPtr(u16 kind, void *target, u16 fightType);

#ifdef __cplusplus
}
#endif

#endif

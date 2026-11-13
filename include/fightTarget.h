#ifndef FIGHTTARGET_H
#define FIGHTTARGET_H

#include <types.h>

// One entry of the fight target table.
typedef struct FIGHT_TARGET_DATA {
    u8 mStatusKid;
    u8 mRelativeFlag;
    s32 mBuff;
} FIGHT_TARGET_DATA;

// Values stored in target records for the status-menu selector argument
// passed to Pokemon effects.
enum FightTargetStatusMenuSelector {
    FIGHT_TARGET_STATUS_MENU_SELECTOR_ATTACKER = 269,
    FIGHT_TARGET_STATUS_MENU_SELECTOR_DEFENDER = 270,
};

#ifdef __cplusplus
extern "C" {
#endif

extern u32 fight_target_data_number;
extern FIGHT_TARGET_DATA fight_target_data[];

u8 fightTargetDataBiosGetRelativeFlag(FIGHT_TARGET_DATA *targetData);
s32 fightTargetDataBiosGetBuff(FIGHT_TARGET_DATA *targetData);
u8 fightTargetDataBiosGetStatusKid(FIGHT_TARGET_DATA *targetData);
FIGHT_TARGET_DATA *fightTargetDataBiosGetPtr(u16 target);
bool fightTargetIsHostSide(void *target, u16 fightType);
void *fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(u16 targetId, u16 fightType);
u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(void *target, u16 fightType);
u16 fightTargetGetTragetPtrToRelativeFightTargetId(void *base, void *target, u16 fightType);
void *fightTargetGetPtrAsNowFightType(u16 kind, void *target);
void *fightTargetGetPtr(u16 kind, void *target, u16 fightType);

#ifdef __cplusplus
}
#endif

#endif

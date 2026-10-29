#include <dbgMenuFight.h>
#include <fightMenu.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <menu.h>
#include <menuFight.h>

// Selectors that locate the status panel of a fight trainer.
enum {
    FIGHT_SIDE_STATUS_SIDE_DATA_ID = 5,
    FIGHT_SIDE_STATUS_FIGHT_TRAINER_STATUS_MENU_DATA_ID = 2
};

// Ball display state the trainer status panel is opened with, one entry per
// party slot.
struct MenuBallStatus {
    u8 mBallStatus[6];
};

void fightMenuFightTrainerCloseStatusMenu(FIGHT_TRAINER *trainer, u16 fightType,
    bool waitForClose)
{
    s32 statusMenuId = fightMenuGetFightTrainerPtrToStatusMenuId(trainer, fightType);

    if (menuIsCheck(statusMenuId) != 0) {
        menuCloseCustom(statusMenuId, 0, waitForClose);
    }
}

void fightMenuFightTrainerRenewStatusMenu(FIGHT_TRAINER *trainer, u16 fightType)
{
    if (menuIsCheck(fightMenuGetFightTrainerPtrToStatusMenuId(trainer, fightType)) != 0) {
        fightMenuFightTrainerOpenStatusMenu(trainer, fightType);
    }
}

void fightMenuFightTrainerOpenStatusMenu(FIGHT_TRAINER *trainer, u16 fightType)
{
    MenuBallStatus ballStatus;
    s32 statusMenuId = fightMenuGetFightTrainerPtrToStatusMenuId(trainer, fightType);

    fightTrainerToMenuBallStatus(trainer, ballStatus.mBallStatus);
    if (dbgMenuFightGetFightPokeStMenuOpenFlag() == 1 && menuFightChkOpenBall(statusMenuId) != 0) {
        menuOpenCustom(statusMenuId, 0, NULL, 0, 0, 1, ballStatus);
    }
}

u32 fightMenuGetFightTrainerPtrToStatusMenuId(FIGHT_TRAINER *trainer, u16 fightType)
{
    FIGHT_SIDE *side;
    u16 sideDataId;
    u16 targetId;
    s32 buffNo;

    side = (FIGHT_SIDE *)fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE, trainer, fightType);
    if (side == NULL) {
        return 0;
    }
    sideDataId = fightSideGetStatus(side, 0, FIGHT_SIDE_STATUS_SIDE_DATA_ID, 0);
    targetId = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(trainer, fightType);
    if (targetId == 0) {
        return 0;
    }
    buffNo = fightTargetDataBiosGetBuff(fightTargetDataBiosGetPtr(targetId));
    if (buffNo < 0) {
        return 0;
    }
    return fightSideGetStatus(NULL, sideDataId,
        FIGHT_SIDE_STATUS_FIGHT_TRAINER_STATUS_MENU_DATA_ID, buffNo);
}

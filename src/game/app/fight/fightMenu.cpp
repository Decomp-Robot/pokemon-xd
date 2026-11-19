#include <dbgMenuFight.h>
#include <fightFloor.h>
#include <fightMenu.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <menu.h>
#include <menuFight.h>
#include <menuFightStatus.h>
#include <menuPokemon.h>
#include <window.h>

void _fightMenuFightOutPokemonCloseStatusMenuSub(FightOutPokemon *pokemon,
    u16 fightType, bool unused, bool unusedLookupFlag);
void _fightMenuFightOutPokemonOpenStatusMenuSub(FightOutPokemon *pokemon,
    u16 fightType, bool showDark, bool unusedLookupFlag, bool hideLevelInfo);

namespace {
// Parent ID for a top-level panel.
const s32 MENU_PARENT_NONE = -1;
}

void fightMenuFightOutPokemonCloseStatusMenuAnotherMine(u32 statusMenuId,
    FightOutPokemon *pokemon, u16 fightType, bool waitForClose,
    bool unusedLookupFlag)
{
    // The four stick-direction panels, in the order the close-other sweep checks them.
    const u8 ids[] = { FIGHT_STATUS_MENU_LEFT, FIGHT_STATUS_MENU_UP,
                       FIGHT_STATUS_MENU_DOWN, FIGHT_STATUS_MENU_RIGHT };
    const s32 keepOpenMenuId = statusMenuId != FIGHT_STATUS_MENU_NONE ? statusMenuId :
        fightMenuGetFightOutPokemonPtrToStatusMenuId(pokemon, fightType, unusedLookupFlag);
    for (u8 i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
        if (keepOpenMenuId != ids[i] && menuIsCheck(ids[i])) {
            if (fightFloor_GetStatusMenuCnt(ids[i]) == FIGHT_STATUS_MENU_INACTIVE) {
                fightFloor_SetStatusMenuCnt(ids[i], FIGHT_STATUS_MENU_CLOSE_DELAY);
            }
            menuCloseCustom(ids[i], WINDOW_CLOSE_KEEP_CHILDREN, waitForClose);
        }
    }
}

void fightMenuFightOutPokemonCloseStatusMenu(FightOutPokemon *pokemon,
    u16 fightType, bool unused)
{
    _fightMenuFightOutPokemonCloseStatusMenuSub(pokemon, fightType, unused, true);
}

void _fightMenuFightOutPokemonCloseStatusMenuSub(FightOutPokemon *pokemon,
    u16 fightType, bool unused, bool unusedLookupFlag)
{
    s32 statusMenuId = fightMenuGetFightOutPokemonPtrToStatusMenuId(pokemon,
                                                                   fightType,
                                                                   unusedLookupFlag);

    if (menuIsCheck(statusMenuId) == 0) {
        return;
    }
    if (fightFloor_GetStatusMenuCnt(statusMenuId) == FIGHT_STATUS_MENU_INACTIVE) {
        fightFloor_SetStatusMenuCnt(statusMenuId, FIGHT_STATUS_MENU_CLOSE_DELAY);
    }
}

void fightMenuFightOutPokemonRenewStatusMenu(FightOutPokemon *pokemon,
    u16 fightType, bool showDarkStatus)
{
    s32 statusMenuId = fightMenuGetFightOutPokemonPtrToStatusMenuId(pokemon,
                                                                   fightType, true);

    if (menuIsCheck(statusMenuId) == 0) {
        return;
    }
    fightMenuFightOutPokemonOpenStatusMenu(pokemon, fightType, showDarkStatus);
    fightFloor_SetStatusMenuCnt(statusMenuId, FIGHT_STATUS_MENU_INACTIVE);
}

void fightMenuFightStatusStartAnimHP(u32 menuId, s16 hp)
{
    if (dbgMenuFightGetFightPokeStMenuOpenFlag() == 1) {
        menuFightStatusStartAnimHP(menuId, hp);
    }
}

void fightMenuFightOutPokemonOpenStatusMenuStart(FightOutPokemon *pokemon,
    u16 fightType, bool showDark)
{
    _fightMenuFightOutPokemonOpenStatusMenuSub(pokemon, fightType, showDark,
                                              true, true);
}

void fightMenuFightOutPokemonOpenStatusMenu(FightOutPokemon *pokemon,
    u16 fightType, bool showDark)
{
    _fightMenuFightOutPokemonOpenStatusMenuSub(pokemon, fightType, showDark,
                                              true, false);
}

void _fightMenuFightOutPokemonOpenStatusMenuSub(FightOutPokemon *pokemon,
    u16 fightType, bool showDark, bool unusedLookupFlag, bool hideLevelInfo)
{
    s32 statusMenuId = fightMenuGetFightOutPokemonPtrToStatusMenuId(pokemon,
                                                                   fightType,
                                                                   unusedLookupFlag);
    MENU_POKEMON_STATUS status;

    fightOutPokemonToMenuPokemonStatus(pokemon, &status);
    if (!showDark) {
        status.mBallType = POKEMON_BALL_TYPE_NORMAL;
    }
    if (hideLevelInfo) {
        status.mHideLevelInfo = 1;
    } else {
        status.mHideLevelInfo = 0;
    }

    if (dbgMenuFightGetFightPokeStMenuOpenFlag() == 1) {
        menuOpenCustom(statusMenuId, MENU_PARENT_NONE, NULL, 0, false, 1, status);
        fightFloor_SetStatusMenuCnt(statusMenuId, FIGHT_STATUS_MENU_INACTIVE);
    }
}

void fightMenuFightOutPokemonSetStatusMenuActiveFlag(FightOutPokemon *pokemon,
    u16 fightType, bool active)
{
    if (fightFloorIsNowEncountTuusinTaisen(NULL) == 1) {
        return;
    }
    menuFightStatusSetActive(fightMenuGetFightOutPokemonPtrToStatusMenuId(
        pokemon, fightType, true), active);
}

u32 fightMenuGetFightOutPokemonPtrToStatusMenuId(FightOutPokemon *pokemon,
    u16 fightType, bool /* unused */)
{
    FIGHT_SIDE *side = (FIGHT_SIDE *)fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE,
                                                       pokemon, fightType);
    if (side == NULL) {
        return FIGHT_STATUS_MENU_NONE;
    }
    u16 sideDataId = fightSideGetStatus(side, 0, FIGHT_SIDE_STATUS_SIDE_DATA_ID, 0);
    u16 targetId = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(pokemon,
                                                                        fightType);
    if (targetId == FIGHT_TARGET_ID_NONE) {
        return FIGHT_STATUS_MENU_NONE;
    }
    s32 buffNo = fightTargetDataBiosGetBuff(fightTargetDataBiosGetPtr(targetId));
    if (buffNo < 0) {
        return FIGHT_STATUS_MENU_NONE;
    }
    return fightSideGetStatus(NULL, sideDataId,
        FIGHT_SIDE_STATUS_FIGHTOUT_POKEMON_STATUS_MENU_DATA_ID, buffNo);
}

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

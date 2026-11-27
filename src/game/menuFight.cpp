#include <types.h>
#include <window.h>
#include <winSprite.h>
#include <menu.h>
#include <menuIds.h>
#include <menuFight.h>
#include <menuItemBios.h>
#include <menuSub.h>
#include <menuSeBios.h>
#include <menuWazaStatus.h>
#include <menuPokemon.h>
#include <msgctrl.h>
#include <GSmsg.h>
#include <Pokemon.h>
#include <PokemonData.h>
#include <PokemonStatus.h>
#include <pokemonBios.h>
#include <pokemonDB.h>
#include <waza.h>
#include <zokusei.h>
#include <itemData.h>
#include <fightFloor.h>
#include <fightTrainer.h>
#include <fightSide.h>
#include <fightPokemon.h>
#include <fightOutPokemon.h>
#include <app/fight/fightTrainer.h>
#include <menuPokemonMessages.h>
#include <pokemonDataBios.h>
#include <fightTarget.h>
#include <gbaCommand.h>
#include <GSsndPlayback.h>
#include <app/fight/fightTimer/fightTimer.h>
#include <app/fight/fightKind.h>
#include <app/fight/fightTrainerBios.h>
#include <fightTrainerDB.h>
#include <fightPokemonBios.h>
#include <fightSeq.h>
#include <hero.h>
#include <app/bingo/bingoBios.h>
#include <math.h>
#include <menuCBBios.h>
#include <winMsg.h>
#include <winSeq.h>
#include <fight.h>
#include <app/fight/fightType.h>
#include <fightMenu.h>
#include <time.h>
#include <GSthread.h>
#include <GSinput.h>
#include <GSlog.h>
#include <dolphin/pad.h>
#include <menuOffScreen.h>
#include <floor.h>
#include <floorManager.h>
#include <GSgfxVideo.h>
#include <fightEncountBios.h>
#include <fightEncountData.h>
#include <menuBattleDisk.h>

enum WazaBanKind {
    WAZA_BAN_KIND_NONE,
    WAZA_BAN_KIND_NO_PP,
    WAZA_BAN_KIND_SHADOW_TAG,
    WAZA_BAN_KIND_MAGNET_PULL,
    WAZA_BAN_KIND_ARENA_TRAP,
    WAZA_BAN_KIND_FORCED_WAZA,
    WAZA_BAN_KIND_SOUUBI_ITEM,
};

enum MenuFightWindowSequence {
    MENU_SEQ_WINDOW_OPEN = 32,
    MENU_SEQ_WINDOW_CLOSE = 34,
};

enum MenuFightSoundEffect {
    MENU_SE_COUNTDOWN_TICK = 1089,
};

enum MenuFightKindIcon {
    SPRITE_KIND_ICON = 490,
    SPRITE_KIND_ICON_QUICK_BATTLE_GBA = 1015,
};

u32 menuFightStatus;

enum MenuFightWazaCursor {
    WAZA_CURSOR_KOUSAN = 5,
};

static const s32 MENU_FIGHT_CURSOR_NONE = 127;

s32 menuPokemonMoveSelectXAdjustment = 60;
s32 menuPokemonMoveSelectYAdjustment = 13;

u16 oldKeyInfo[2] = { 0, 0 };

enum MenuFightBallSprite {
    MENU_FIGHT_BALL_SPRITE_NONE = 140,
    MENU_FIGHT_BALL_SPRITE_NORMAL,
    MENU_FIGHT_BALL_SPRITE_ABNORMAL,
    MENU_FIGHT_BALL_SPRITE_FIGHT_OUT,
};

static u16 menuFightWazaDetailIDs[4] = {
    BT_ITEM_WAZA_DETAIL_0,
    BT_ITEM_WAZA_DETAIL_1,
    BT_ITEM_WAZA_DETAIL_2,
    BT_ITEM_WAZA_DETAIL_3,
};

s32 menuFightWazaDetailSlot[3] = { -1, -1, 0 };

static const f32 menuFightAxisAngle = M_PI / 4.0f;
static const f32 menuFightDiagonalAngle = 3.0f * M_PI / 4.0f;
static const f32 menuFightKeyWaitSeconds = 2.5f;
static const f32 menuFightOffScreenFadeFrames = 50.0f;

enum MenuFightCursorBackAlpha {
    MENU_FIGHT_CURSOR_BACK_ALPHA_OPAQUE = 255
};

#define menuFightAbs(x) ((x) > 0.0f ? (x) : -(x))
extern "C" double atan2(double y, double x);

enum FightWazaButtonSeq {
    FIGHT_WAZA_BUTTON_SEQ_SELECT,
    FIGHT_WAZA_BUTTON_SEQ_REPLACE
};

struct FightWazaButtonWork {
    u8 mSeq;
    u8 mUnused;
    s8 mMoveSlot;
};

extern "C" s32 menuFightChkOpenBall(s32 menuId)
{
    if ((menuId == MENU_ID_FIGHT_STATUS_3 || menuId == MENU_ID_FIGHT_STATUS_4) &&
        fightKind_CheckFightKind(FIGHT_BATTLE_KIND_WILD_POKEMON))
        return 0;

    return 1;
}

extern "C" s32 menuFightGetStatus(void)
{
    return menuFightStatus;
}

extern "C" void menuFightSetStatus(u32 status)
{
    menuFightStatus = status;
}

extern "C" s32 menuFightChkSeq(void)
{
    if (winSeqCheckMove(MENU_ID_FIGHT_STATUS_UPPER_LEFT))
        return 1;
    if (winSeqCheckMove(MENU_ID_FIGHT_STATUS_LOWER_LEFT))
        return 1;
    if (winSeqCheckMove(MENU_ID_FIGHT_STATUS_UPPER_RIGHT))
        return 1;

    return winSeqCheckMove(MENU_ID_FIGHT_STATUS_LOWER_RIGHT) != 0;
}

extern "C" void menuFightSetSeq(void)
{
    winSeqSetMenu(MENU_ID_FIGHT_STATUS_UPPER_LEFT, MENU_SEQ_FIGHT_STATUS_LEFT_DOWN_IN);
    winSeqSetMenu(MENU_ID_FIGHT_STATUS_LOWER_LEFT, MENU_SEQ_FIGHT_STATUS_LEFT_DOWN_IN);
    winSeqSetMenu(MENU_ID_FIGHT_STATUS_UPPER_RIGHT, MENU_SEQ_FIGHT_STATUS_UP_RIGHT_IN);
    winSeqSetMenu(MENU_ID_FIGHT_STATUS_LOWER_RIGHT, MENU_SEQ_FIGHT_STATUS_UP_RIGHT_IN);
}

void menuFightSetSeqOut(void)
{
    winSeqSetMenu(MENU_ID_FIGHT_STATUS_UPPER_LEFT, MENU_SEQ_FIGHT_STATUS_LEFT_DOWN_OUT);
    winSeqSetMenu(MENU_ID_FIGHT_STATUS_LOWER_LEFT, MENU_SEQ_FIGHT_STATUS_LEFT_DOWN_OUT);
    winSeqSetMenu(MENU_ID_FIGHT_STATUS_UPPER_RIGHT, MENU_SEQ_FIGHT_STATUS_UP_RIGHT_OUT);
    winSeqSetMenu(MENU_ID_FIGHT_STATUS_LOWER_RIGHT, MENU_SEQ_FIGHT_STATUS_UP_RIGHT_OUT);
    menuFightSetStatus(1);
}

extern "C" void menuFightCloseAll(void)
{
    u32 menuIds[28] = {
        MENU_ID_FIGHT_STATUS_UPPER_LEFT, MENU_ID_FIGHT_STATUS_LOWER_LEFT, MENU_ID_FIGHT_STATUS_UPPER_RIGHT, MENU_ID_FIGHT_STATUS_LOWER_RIGHT,
        MENU_ID_FIGHT_BALL, MENU_ID_FIGHT_BALL_SUB, MENU_ID_FIGHT_WAZA,
        MENU_ID_FIGHT_COMMAND, MENU_ID_FIGHT_STATUS_1, MENU_ID_FIGHT_STATUS_2,
        MENU_ID_FIGHT_STATUS_3, MENU_ID_FIGHT_STATUS_4, MENU_ID_FIGHT_STATUS_WAIT_1,
        MENU_ID_FIGHT_STATUS_WAIT_2, MENU_ID_FIGHT_STATUS_WAIT_3,
        MENU_ID_FIGHT_STATUS_WAIT_4, MENU_ID_FIGHT_COUNTDOWN, MENU_ID_FIGHT_STATUS_BAR,
        MENU_ID_FIGHT_TOTAL_TIMER, MENU_ID_FIGHT_TIMEUP, MENU_ID_FIGHT_WAZA_1P,
        MENU_ID_FIGHT_WAZA_DETAIL_1P, MENU_ID_FIGHT_TARGET, MENU_ID_FIGHT_WAZA_2P,
        MENU_ID_FIGHT_WAZA_DETAIL_2P, MENU_ID_FIGHT_TARGET_2P,
        MENU_ID_FIGHT_CONFIRM_1P, MENU_ID_FIGHT_CONFIRM_2P
    };
    u8 i;

    for (i = 0; i < 28; i++) {
        if (menuIsCheck(menuIds[i]) == 1)
            menuCloseCustom(menuIds[i], 0, 0);
    }
}

extern "C" s32 menuFightGetKeyWaitMenuId(FIGHT_TRAINER *trainer, u16 fightType)
{
    u32 result = MENU_ID_FIGHT_STATUS_WAIT_1;

    switch (fightMenuGetFightTrainerPtrToStatusMenuId(trainer, fightType)) {
    case MENU_ID_FIGHT_STATUS_1:
        result = MENU_ID_FIGHT_STATUS_WAIT_1;
        break;
    case MENU_ID_FIGHT_STATUS_2:
        result = MENU_ID_FIGHT_STATUS_WAIT_2;
        break;
    case MENU_ID_FIGHT_STATUS_3:
        result = MENU_ID_FIGHT_STATUS_WAIT_3;
        break;
    case MENU_ID_FIGHT_STATUS_4:
        result = MENU_ID_FIGHT_STATUS_WAIT_4;
        break;
    }
    return result;
}

bool isImasugu(void)
{
    if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_QUICK_BATTLE))
        return true;

    return fightKind_CheckFightKind(FIGHT_BATTLE_KIND_QUICK_BATTLE_GBA) != 0;
}

bool _menuFightCheckTimeOut(void)
{
    if (fightFloorIsNowEncountTuusinTaisen(NULL)) {
        if (fightFloorIsUseFightTimerCommand(NULL) == 1 && fightTimerCommandIsOver() == 1)
            return true;
        if (fightGetFightResultId() == 1)
            return true;
    }
    return false;
}

void _menuFightKeyWait(GSinputDevice device)
{
    f32 elapsed = 0.0f;

    if (isImasugu()) {
        while (elapsed < menuFightKeyWaitSeconds) {
            elapsed += timeGetLastFrameSec();
            GSthreadSwitch();
        }
    }
    else {
        while ((GSinputButtonsPressed(device) & GSinputButtonsChanged(device) &
                (PAD_BUTTON_A | PAD_BUTTON_B)) == 0) {
            if (_menuFightCheckTimeOut())
                break;
            GSthreadSwitch();
        }
    }
}

enum MenuFightCommandResult {
    MENU_FIGHT_COMMAND_RESULT_FIGHT,
    MENU_FIGHT_COMMAND_RESULT_POKEMON,
    MENU_FIGHT_COMMAND_RESULT_BAG,
    MENU_FIGHT_COMMAND_RESULT_BALL,
    MENU_FIGHT_COMMAND_RESULT_RETURN_FROM_INFO = 127,
};

static const u32 FIGHT_MENU_FLOOR_ID = 907;

extern "C" s32 menuFightOpenTop(FIGHT_TRAINER *, FightMenuTopParam *param, void *result, bool wait)
{
    s32 ret;
    s32 docResult;

    menuFightSetStatus(0);
    if (isImasugu())
        return 0;

    if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK))
        menuOpen(MENU_ID_FIGHT_STATUS_BAR, 0);

    while (true) {
        switch (param->mMenuKind) {
        case MENU_FIGHT_COMMAND_KIND_NORMAL:
            ret = menuOpenCustom(MENU_ID_FIGHT_COMMAND, 0, result, 0, wait, 1, param);
            break;
        case MENU_FIGHT_COMMAND_KIND_GBA:
            ret = menuOpenCustom(MENU_ID_FIGHT_COMMAND_GBA, 0, result, 0, wait, 1, param);
            break;
        }

        if (param->mCanPlaySeq == 0 || fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO) ||
            fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK) ||
            fightKind_CheckFightKind(FIGHT_BATTLE_KIND_VIRTUAL_TUTORIAL) ||
            fightKind_CheckFightKind(FIGHT_BATTLE_KIND_GBA_LINK) || fightKind_CheckFightKind(FIGHT_BATTLE_KIND_GBA_WIRELESS)) {
            if (ret == MENU_FIGHT_COMMAND_RESULT_BALL) {
                s32 docParam = 0;

                menuFightSetSeqOut();
                menuFightCloseTop(1);
                if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK))
                    menuClose(MENU_ID_FIGHT_STATUS_BAR);
                fightMenuCloseInfoMenu(1);

                if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK))
                    docResult = menuBattleDiskOpenDocPush((u8)fightEncountDataBiosGetDiskId(
                        fightEncountDataBiosGetPtr(FIGHT_ENCOUNT_ID_BATTLE_DISK)));
                else if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_VIRTUAL_TUTORIAL))
                    docResult = menuBattleDiskOpenDocPush((u8)fightEncountDataBiosGetDiskId(
                        fightEncountDataBiosGetPtr(FIGHT_ENCOUNT_ID_TUTORIAL)));
                else {
                    docResult = menuOpenCustom(MENU_ID_FIGHT_WAZA_DOC_GBA, 0, &docParam, 0, 1, 0);
                    menuCloseCustom(MENU_ID_FIGHT_WAZA_DOC_GBA, 0, 1);
                }

                if (docResult != BT_ITEM_DOC_BATTLE) {
                    fightMenuOpenInfoMenu(FIGHT_MENU_INFO_MENU_COUNTDOWN);
                    if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK))
                        menuOpen(MENU_ID_FIGHT_STATUS_BAR, 0);
                    continue;
                }
            }
        }

        else {
            if (param->mCanPlaySeq != 0 &&
                (ret == MENU_FIGHT_COMMAND_RESULT_POKEMON || ret == MENU_FIGHT_COMMAND_RESULT_BAG))
                menuFightSetSeqOut();
        }

        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK)) {
            if (ret == MENU_FIGHT_COMMAND_RESULT_POKEMON || ret == MENU_FIGHT_COMMAND_RESULT_BAG)
                menuClose(MENU_ID_FIGHT_STATUS_BAR);
        }

        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO)) {
            if (bingo_bios::getStaticBall() == 0 && ret == MENU_FIGHT_COMMAND_RESULT_POKEMON) {
                menuFightCloseTop(1);
                winMsgOpen(WINMSG_WINDOW_FIGHT, MSG_FIGHT_BINGO_BALL_EMPTY, 1, 0);
                winMsgClose(1);
                continue;
            }
            if (ret == MENU_FIGHT_COMMAND_RESULT_RETURN_FROM_INFO) {
                menuFightSetSeqOut();
                menuFightCloseTop(1);
                fightMenuCloseInfoMenu(1);
                f32 fadeFrames = 8.0f / (f32)GSgfxVideoGetVsyncRate();
                menuCreateOffScreen(fadeFrames, menuFightOffScreenFadeFrames);
                menuOffScreenSetPriority(0);
                menuOffScreenFadeSync(1);
                GSfloorPush(FIGHT_MENU_FLOOR_ID);
                floorSetFadeScript(0, 0);
                floorWaitForChange();
                menuReleaseOffScreen(8.0f / (f32)GSgfxVideoGetVsyncRate());
                fightMenuOpenInfoMenu(FIGHT_MENU_INFO_MENU_COUNTDOWN);
            }
        }

        if (fightFloorIsNowEncountSnatchOnly(0)) {
            if (ret == MENU_FIGHT_COMMAND_RESULT_FIGHT || ret == MENU_FIGHT_COMMAND_RESULT_BALL) {
                menuFightCloseTop(1);
                fightSeqEventKurein(FIGHT_SEQ_KUREIN_HOLD);
                winMsgOpen(WINMSG_WINDOW_MENU, MSG_FIGHT_SNATCH_USED, 1, 1);
                winMsgClose(1);
                fightSeqEventKurein(FIGHT_SEQ_KUREIN_RELEASE);
                continue;
            }
        }
        break;
    }
    return ret;
}

extern "C" s32 menuFightCloseTop(bool wait)
{
    if (menuIsCheck(MENU_ID_FIGHT_COMMAND))
        menuCloseCustom(MENU_ID_FIGHT_COMMAND, 0, wait);
    if (menuIsCheck(MENU_ID_FIGHT_COMMAND_GBA))
        menuCloseCustom(MENU_ID_FIGHT_COMMAND_GBA, 0, wait);

    return 0;
}

s32 chkWazaNew(MENU_WAZA_STATUS *status, u8 wazaIndex)
{
    Pokemon *pokemon = fightOutPokemonGetPokemonPtr(status->fightOutPokemon);
    DarkPokemon *dark;

    if (wazaIndex == 0)
        return -1;
    if (pokemon != NULL && pokemon->isDarkPokemon() != 0) {
        dark = pokemon->getDarkPokemon();
        if (dark != NULL) {
            s32 waza = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_MOVE, wazaIndex);

            switch (waza) {
            case WAZA_DATA_ID_NONE:
            case WAZA_DATA_ID_DARK_BLITZ:
            case WAZA_DATA_ID_DARK_RUSH:
            case WAZA_DATA_ID_DARK_BREAK:
            case WAZA_DATA_ID_DARK_END:
            case WAZA_DATA_ID_DARK_WAVE:
            case WAZA_DATA_ID_DARK_RAVE:
            case WAZA_DATA_ID_DARK_STORM:
            case WAZA_DATA_ID_DARK_FIRE:
            case WAZA_DATA_ID_DARK_BOLT:
            case WAZA_DATA_ID_DARK_CHILL:
            case WAZA_DATA_ID_DARK_BLAST:
            case WAZA_DATA_ID_DARK_SKY:
            case WAZA_DATA_ID_DARK_HOLD:
            case WAZA_DATA_ID_DARK_MIST:
            case WAZA_DATA_ID_DARK_PANIC:
            case WAZA_DATA_ID_DARK_DOWN:
            case WAZA_DATA_ID_DARK_SHED:
            case WAZA_DATA_ID_DARK_HALF:
            case WAZA_SHADOW_SPECIAL:
                break;
            default:
                return dark->isWazaNew(wazaIndex - 1);
            }
        }
    }
    return -1;
}

extern "C" s32 menuFightOpenWaza(void *target, MENU_WAZA_STATUS *status, void *result, bool wait)
{
    bool retry = true;
    s32 ret;
    s32 menuId;
    s32 msgId = 0;
    s32 subMenuId = 0;
    u32 waza;
    s32 i;

    switch (status->reserved2[0]) {
    case MENU_FIGHT_COMMAND_KIND_NORMAL:
        if (isImasugu()) {
            ret = menuCB_GetBattleController2P();
            if (fightTargetIsHostSide(target, (u16)fightFloor_GetFightType(0))) {
                menuId = MENU_ID_FIGHT_WAZA_1P;
                msgId = MENU_ID_FIGHT_CONFIRM_1P;
                subMenuId = MENU_ID_FIGHT_WAZA_DETAIL_2P;
            }
            else {
                menuId = MENU_ID_FIGHT_WAZA_2P;
                msgId = MENU_ID_FIGHT_CONFIRM_2P;
                subMenuId = MENU_ID_FIGHT_WAZA_DETAIL_1P;
            }
        }
        else {
            menuId = MENU_ID_FIGHT_WAZA;
        }
        break;
    case MENU_FIGHT_COMMAND_KIND_GBA:
        menuId = MENU_ID_FIGHT_WAZA_GBA;
        break;
    }

    while (true) {
        winSeqSetMenu(menuId, MENU_SEQ_WINDOW_OPEN);
        if (isImasugu()) {
            if (subMenuId != 0)
                menuOpen(subMenuId, 0);
            while (retry) {
                ret = menuOpenCustom(menuId, 0, result, 0, wait, 1, status);
                if (ret != -1)
                    menuCloseCustom(menuId, MENU_CLOSE_IMMEDIATE, wait);
                if (ret == WAZA_CURSOR_KOUSAN) {
                    if (menuOpen(msgId, 1) == 1)
                        retry = false;
                    menuCloseCustom(msgId, MENU_CLOSE_IMMEDIATE, 1);
                }
                else {
                    retry = false;
                }
            }
            if (ret == WAZA_CURSOR_KOUSAN)
                break;
        }
        else {
            ret = menuOpenCustom(menuId, 0, result, 0, wait, 1, status);
            if (menuId == MENU_ID_FIGHT_WAZA_GBA)
                menuClose(menuId);
        }
        if (ret < 0)
            break;
        waza = _menuFightIsUse(status, (u16)ret);
        if (waza == 0)
            break;
        menuFightCloseWaza(1);
        if (isImasugu() && subMenuId != 0)
            menuClose(subMenuId);
        winMsgOpenFight(waza, 1, 1);
        _menuFightKeyWait(GS_INPUT_PORT_1);
        winMsgCloseFight(1);
        retry = true;
    }

    winSeqSetMenu(menuId, MENU_SEQ_WINDOW_CLOSE);
    subMenuId = 0;
    for (i = 0; i < 4; i++) {
        if (chkWazaNew(status, (u8)i) != -1) {
            DarkPokemon *dark = fightOutPokemonGetPokemonPtr(status->fightOutPokemon)->getDarkPokemon();
            dark->setWazaNew((u8)(i - 1), 1);
        }
    }
    return ret;
}

extern "C" s32 menuFightCloseWaza(bool wait)
{
    if (menuIsCheck(MENU_ID_FIGHT_WAZA))
        menuCloseCustom(MENU_ID_FIGHT_WAZA, 0, wait);
    if (menuIsCheck(MENU_ID_FIGHT_SECRET_WAZA_LIST))
        menuCloseCustom(MENU_ID_FIGHT_SECRET_WAZA_LIST, 0, wait);
    if (menuIsCheck(MENU_ID_FIGHT_SECRET_WAZA_DETAIL))
        menuCloseCustom(MENU_ID_FIGHT_SECRET_WAZA_DETAIL, 0, wait);
    if (menuIsCheck(MENU_ID_FIGHT_WAZA_GBA))
        menuCloseCustom(MENU_ID_FIGHT_WAZA_GBA, 0, wait);

    return 0;
}

extern "C" s32 menuFightOpenPokemon(FIGHT_TRAINER *trainer, FightOutPokemon *outgoing, u16 fightType, u8 battleSelection, u8 flag)
{
    u8 num;
    s32 ret;
    FightTargetSelectParam param;

    num = fightTypeDataBiosGetFightoutPokemonNum(fightTypeDataBiosGetPtr(fightType));

    if (flag == 0) {
        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK))
            menuClose(MENU_ID_FIGHT_STATUS_BAR);
        ret = menuPokemonOpenFight(num, battleSelection, trainer, outgoing);
        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK))
            menuOpen(MENU_ID_FIGHT_STATUS_BAR, 0);
    }
    else {
        while (true) {
            winSeqSetMenu(MENU_ID_FIGHT_SECRET_SELECT, MENU_SEQ_WINDOW_OPEN);
            ret = menuOpenCustom(MENU_ID_FIGHT_SECRET_SELECT, 0, 0, 0, 1, 3, trainer, outgoing, battleSelection);
            winSeqSetMenu(MENU_ID_FIGHT_SECRET_SELECT, MENU_SEQ_WINDOW_CLOSE);
            if (ret == -1) {
                menuCloseCustom(MENU_ID_FIGHT_SECRET_SELECT, 0, 1);
                GSthreadSwitch();
                GSthreadSwitch();
                GSthreadSwitch();
                GSthreadSwitch();
                GSthreadSwitch();
                GSthreadSwitch();
                GSthreadSwitch();
                GSthreadSwitch();
                return -1;
            }
            if (!menuPokemonCheckPokemonChange(outgoing, trainer, ret))
                continue;

            if (battleSelection != 0 && (num >= 2 || fightTypeDataBiosGetTrainerNum(fightTypeDataBiosGetPtr(fightType)) == 2)) {
                FightOutPokemon *p = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OPPOSING_SIDE_FIRST_POKEMON, outgoing, fightType);
                param.mTargets[0].mMenuId =
                    fightOutPokemonCheckFightOut(p) == 1 ? fightMenuGetFightOutPokemonPtrToStatusMenuId(p, fightType, 0) : 0;
                p = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OPPOSING_SIDE_SECOND_POKEMON, outgoing, fightType);
                param.mTargets[1].mMenuId =
                    fightOutPokemonCheckFightOut(p) == 1 ? fightMenuGetFightOutPokemonPtrToStatusMenuId(p, fightType, 0) : 0;
                p = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE, outgoing, fightType);
                param.mTargets[2].mMenuId =
                    fightOutPokemonCheckFightOut(p) == 1 ? fightMenuGetFightOutPokemonPtrToStatusMenuId(p, fightType, 0) : 0;
                param.mTargets[3].mMenuId = 0;
                param.mMode = flag;
                s32 sel = menuFightOpenTarget(&param, 0, 1);
                menuFightCloseTarget(1);
                if (sel == -1)
                    continue;
            }
            menuCloseCustom(MENU_ID_FIGHT_SECRET_SELECT, 0, 1);
            break;
        }
    }
    return ret;
}

extern "C" s32 menuFightOpenTarget(FightTargetSelectParam *param, void *result, bool wait)
{
    s32 ret;
    s32 value;
    s32 cursor;
    s32 i;
    s32 j;

    if (param->mMode == MENU_FIGHT_TARGET_MODE_NORMAL) {
        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO)) {
            ret = 0;
        }
        else if (isImasugu()) {
            FIGHT_SIDE *side = (FIGHT_SIDE *)fightFloorGetStatus(0, 0, FIGHT_FLOOR_STATUS_FIGHT_SIDE_PTR, 0);
            FIGHT_TRAINER *trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr((FIGHT_FLOOR *)fightFloorGetNowPtr(), param->mOutgoing);
            s32 menuId;
            s32 subMenuId;

            if (trainer == &side->mTrainerAry[0]) {
                menuId = MENU_ID_FIGHT_TARGET;
                subMenuId = MENU_ID_FIGHT_WAZA_DETAIL_2P;
            }
            else {
                menuId = MENU_ID_FIGHT_TARGET_2P;
                subMenuId = MENU_ID_FIGHT_WAZA_DETAIL_1P;
            }
            ret = menuOpenCustom(menuId, 0, result, 0, wait, 1, param);
            menuCloseCustom(menuId, MENU_CLOSE_IMMEDIATE, wait);
            if (menuIsCheck(subMenuId))
                menuClose(subMenuId);
        }
        else {
            menuItemBiosSetSelectFlag(MENU_ID_FIGHT_TARGET_SELECT, BT_ITEM_TARGET_CURSOR_LEFT, 0);
            menuItemBiosSetSelectFlag(MENU_ID_FIGHT_TARGET_SELECT, BT_ITEM_TARGET_CURSOR_DOWN, 0);
            menuItemBiosSetSelectFlag(MENU_ID_FIGHT_TARGET_SELECT, BT_ITEM_TARGET_CURSOR_UP, 0);
            menuItemBiosSetSelectFlag(MENU_ID_FIGHT_TARGET_SELECT, BT_ITEM_TARGET_CURSOR_RIGHT, 0);

            for (i = 0; i < 4; i++) {
                switch (param->mTargets[i].mMenuId) {
                case MENU_ID_FIGHT_STATUS_UPPER_LEFT: value = BT_ITEM_TARGET_CURSOR_LEFT; break;
                case MENU_ID_FIGHT_STATUS_LOWER_LEFT: value = BT_ITEM_TARGET_CURSOR_DOWN; break;
                case MENU_ID_FIGHT_STATUS_UPPER_RIGHT: value = BT_ITEM_TARGET_CURSOR_UP; break;
                case MENU_ID_FIGHT_STATUS_LOWER_RIGHT: value = BT_ITEM_TARGET_CURSOR_RIGHT; break;
                default: value = 0; break;
                }
                if (value != 0)
                    menuItemBiosSetSelectFlag(MENU_ID_FIGHT_TARGET_SELECT, value, 1);
            }

            cursor = menuGetCursorFromItemID(MENU_ID_FIGHT_TARGET_SELECT, BT_ITEM_TARGET_CURSOR_UP);
            if (cursor == -1) {
                cursor = menuGetCursorFromItemID(MENU_ID_FIGHT_TARGET_SELECT, BT_ITEM_TARGET_CURSOR_RIGHT);
                if (cursor == -1)
                    cursor = 0;
            }

            ret = menuOpenCustom(MENU_ID_FIGHT_TARGET_SELECT, 0, &cursor, 0, wait, 1, param);
            switch (ret) {
            case BT_ITEM_TARGET_CURSOR_UP: value = MENU_ID_FIGHT_STATUS_UPPER_RIGHT; break;
            case BT_ITEM_TARGET_CURSOR_RIGHT: value = MENU_ID_FIGHT_STATUS_LOWER_RIGHT; break;
            case BT_ITEM_TARGET_CURSOR_LEFT: value = MENU_ID_FIGHT_STATUS_UPPER_LEFT; break;
            case BT_ITEM_TARGET_CURSOR_DOWN: value = MENU_ID_FIGHT_STATUS_LOWER_LEFT; break;
            default: value = -1; break;
            }
            ret = -1;
            for (j = 0; j < 4; j++) {
                if (value == param->mTargets[j].mMenuId) {
                    ret = j;
                    if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK))
                        menuClose(MENU_ID_FIGHT_STATUS_BAR);
                    break;
                }
            }
        }
    }
    else if (param->mMode == MENU_FIGHT_TARGET_MODE_SUB) {
        if (menuIsCheck(MENU_ID_FIGHT_SECRET_SELECT))
            menuCloseCustom(MENU_ID_FIGHT_SECRET_SELECT, 0, 1);
        ret = menuOpenCustom(MENU_ID_FIGHT_TARGET_SUB, 0, result, 0, wait, 1, param);
    }
    return ret;
}

extern "C" s32 menuFightCloseTarget(bool wait)
{
    if (menuIsCheck(MENU_ID_FIGHT_TARGET_SELECT))
        menuCloseCustom(MENU_ID_FIGHT_TARGET_SELECT, 0, wait);
    if (menuIsCheck(MENU_ID_FIGHT_TARGET_SUB))
        menuCloseCustom(MENU_ID_FIGHT_TARGET_SUB, 0, wait);

    return 0;
}

extern "C" long menuFightOpenGBAIrekae(u32 channel, FIGHT_TRAINER *trainer, u32 slot, u16 fightType)
{
    return gbaCommandRequestAction(channel, trainer, slot, 1);
}

extern "C" long menuFightOpenGBAMain(u32 channel, FIGHT_TRAINER *trainer, u32 slot, u16 fightType)
{
    return gbaCommandRequestAction(channel, trainer, slot, 0);
}

extern "C" u8 menuPokemonCheckPokemonChange(FightOutPokemon *outgoing, FIGHT_TRAINER *trainer, s32 index)
{
    static const char sPokemonChangeErrorMessage[40] =
        "%s: 予定外の条件が帰ってきています\n";

    FightOutPokemon *replacement;
    u8 irekaeCheck = fightFloorCheckFightActionFightOutPokemonIrekaeSelect(NULL, outgoing, &replacement);

    if (irekaeCheck == FIGHT_FLOOR_SWITCH_BLOCKED_BY_STATUS) {
        msgctrlSetValue(MSGCTRL_FIGHT_CHANGE_NICKNAME, (u32)fightOutPokemonGetNicknamePtr(outgoing));
        winMsgOpen(1, MSG_FIGHT_CHANGE_OUT_BLOCKED, 1, 0);
        _menuFightKeyWait(GS_INPUT_PORT_1);
        winMsgClose(1);
        return 0;
    }
    if (irekaeCheck == FIGHT_FLOOR_SWITCH_BLOCKED_BY_ABILITY) {
        fightFloorSetStatus(NULL, 0, FIGHT_FLOOR_STATUS_APPOINT_TOKUSEI_DATA_ID, 0,
            fightOutPokemonGetTokuseiDataId(replacement));
        msgctrlSetValue(MSGCTRL_FIGHT_CHANGE_NICKNAME, (u32)fightOutPokemonGetNicknamePtr(replacement));
        msgctrlSetValue(MSGCTRL_FIGHT_CHANGE_OUTGOING_NICKNAME, (u32)fightOutPokemonGetNicknamePtr(outgoing));
        winMsgOpen(1, MSG_FIGHT_CHANGE_REPLACE_SET, 1, 0);
        _menuFightKeyWait(GS_INPUT_PORT_1);
        winMsgClose(1);
        return 0;
    }

    if (trainer == NULL)
        trainer = fightFloorGetGcHeroFightTrainerPtr(NULL);
    if (trainer == NULL)
        return 0;

    FightPokemon *pokemon = fightTrainerGetValidFightPokemonPtr(trainer, (u16)index);
    if (pokemon == NULL)
        return 0;

    u8 canIrekae = fightTrainerCheckCanIrekaeFightPokemon(trainer, pokemon);
    if (canIrekae == FIGHT_TRAINER_IREKAE_CANNOT_FIGHT) {
        msgctrlSetValue(MSGCTRL_FIGHT_CHANGE_NICKNAME, (u32)fightPokemonGetNicknamePtr(pokemon));
        winMsgOpen(1, MSG_FIGHT_CHANGE_CANNOT_FIGHT, 1, 0);
        _menuFightKeyWait(GS_INPUT_PORT_1);
        winMsgClose(1);
        return 0;
    }
    if (canIrekae == FIGHT_TRAINER_IREKAE_FIGHT_OUT) {
        msgctrlSetValue(MSGCTRL_FIGHT_CHANGE_NICKNAME, (u32)fightPokemonGetNicknamePtr(pokemon));
        winMsgOpen(1, MSG_FIGHT_CHANGE_FIGHT_OUT, 1, 0);
        _menuFightKeyWait(GS_INPUT_PORT_1);
        winMsgClose(1);
        return 0;
    }
    if (canIrekae == FIGHT_TRAINER_IREKAE_REFUSED) {
        msgctrlSetValue(MSGCTRL_FIGHT_CHANGE_NICKNAME, (u32)fightPokemonGetNicknamePtr(pokemon));
        winMsgOpen(1, MSG_FIGHT_CHANGE_REFUSED, 1, 0);
        _menuFightKeyWait(GS_INPUT_PORT_1);
        winMsgClose(1);
        return 0;
    }
    if (canIrekae == FIGHT_TRAINER_IREKAE_SWAPPABLE)
        return 1;

    GSlogWrite(sPokemonChangeErrorMessage, __FUNCTION__);
    return 0;
}

u16 _menuFightGetLeftKeyInfo(long stickNo)
{
    u16 keyInfo = 0;
    u32 buttons;

    stickNo = stickNo ? 1 : 0;
    buttons = stickNo ? GSinputButtonsPressed(GS_INPUT_PORT_2) : GSinputButtonsPressed(GS_INPUT_PORT_1);

    if (buttons & PAD_BUTTON_UP)
        keyInfo |= WINDOW_KEY_UP;
    if (buttons & PAD_BUTTON_DOWN)
        keyInfo |= WINDOW_KEY_DOWN;
    if (buttons & PAD_BUTTON_LEFT)
        keyInfo |= WINDOW_KEY_LEFT;
    if (buttons & PAD_BUTTON_RIGHT)
        keyInfo |= WINDOW_KEY_RIGHT;

    if (keyInfo == oldKeyInfo[stickNo])
        keyInfo = 0;
    else
        oldKeyInfo[stickNo] = keyInfo;

    return keyInfo;
}

u16 _menuFightGetRightStickInfo(long stickNo, bool trigger)
{
    u16 keyInfo = 0;
    s8 x;
    s8 y;

    stickNo = stickNo ? 1 : 0;

    if (stickNo == 0) {
        y = GSinputGetRightStickYData(GS_INPUT_PORT_1, 0);
        x = GSinputGetRightStickXData(GS_INPUT_PORT_1, 0);
    }
    else {
        y = GSinputGetRightStickYData(GS_INPUT_PORT_2, 0);
        x = GSinputGetRightStickXData(GS_INPUT_PORT_2, 0);
    }

    if ((x < 0 ? -x : x) > 32 || (y < 0 ? -y : y) > 32) {
        f32 angle = (f32)atan2(x, y);

        if (menuFightAbs(angle) < menuFightAxisAngle) {
            keyInfo |= WINDOW_KEY_DOWN;
        }
        else if (menuFightAbs(angle) > menuFightDiagonalAngle) {
            keyInfo |= WINDOW_KEY_UP;
        }

        if (menuFightAxisAngle < menuFightAbs(angle) && menuFightAbs(angle) < menuFightDiagonalAngle) {
            if (angle < 0.0f)
                keyInfo |= WINDOW_KEY_LEFT;
            else
                keyInfo |= WINDOW_KEY_RIGHT;
        }

        if (trigger) {
            if (keyInfo == oldKeyInfo[stickNo])
                keyInfo = 0;
            else
                oldKeyInfo[stickNo] = keyInfo;
        }
    }
    else {
        oldKeyInfo[stickNo] = 0;
    }

    return keyInfo;
}

extern "C" s32 menuFightMainCtrl(tagWINDOW_WORK *window)
{
    if (window->mState == WINDOW_STATE_INIT) {
        FightMenuTopParam *work = (FightMenuTopParam *)windowAllocMemory(window, sizeof(FightMenuTopParam));
        if (work != NULL)
            memcpy(work, (void *)window->mArgs[0], sizeof(FightMenuTopParam));

        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK) ||
            fightKind_CheckFightKind(FIGHT_BATTLE_KIND_VIRTUAL_TUTORIAL)) {
            tagSPRITE_WORK *sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_B);
            sprite->mMsgID = MSG_FIGHT_COMMAND;
            menuItemBiosSetSelectFlag(window->mMenuID, BT_ITEM_COMMAND_BALL, true);
        }
        else if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO)) {
            tagSPRITE_WORK *sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_A);
            sprite->mMsgID = MSG_FIGHT_COMMAND;
            sprite = windowSearchItemID(window, BT_ITEM_COMMAND_GBA);
            sprite->mMsgID = MSG_FIGHT_COMMAND_BINGO;
            sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_B);
            sprite->mMsgID = MSG_FIGHT_COMMAND_BATTLE_DISK;
            menuItemBiosSetSelectFlag(window->mMenuID, BT_ITEM_COMMAND_BALL, true);
        }
        else if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_GBA_LINK) ||
                 fightKind_CheckFightKind(FIGHT_BATTLE_KIND_GBA_WIRELESS)) {
            tagSPRITE_WORK *sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_A);
            sprite->mMsgID = MSG_FIGHT_COMMAND;
            sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_B);
            sprite->mMsgID = MSG_NONE;
            menuItemBiosSetSelectFlag(window->mMenuID, BT_ITEM_COMMAND_BALL, false);
        }
        else if (work->mCanPlaySeq != 0) {
            tagSPRITE_WORK *sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_A);
            sprite->mMsgID = MSG_FIGHT_COMMAND_TRAINER;
            sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_B);
            sprite->mMsgID = MSG_FIGHT_COMMAND_LINK;
            menuItemBiosSetSelectFlag(window->mMenuID, BT_ITEM_COMMAND_BALL, true);
        }
        else {
            tagSPRITE_WORK *sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_A);
            sprite->mMsgID = MSG_FIGHT_COMMAND;
            sprite = windowSearchItemID(window, BT_ITEM_COMMAND_LABEL_B);
            sprite->mMsgID = MSG_NONE;
            menuItemBiosSetSelectFlag(window->mMenuID, BT_ITEM_COMMAND_BALL, false);
        }
    }

    FightMenuTopParam *work = (FightMenuTopParam *)windowGetAllocPtr(window);
    switch (menuGetCursorItemID(window->mMenuID)) {
    case BT_ITEM_COMMAND_FIGHT: window->mCursorIndex = MENU_FIGHT_COMMAND_RESULT_FIGHT; break;
    case BT_ITEM_COMMAND_POKEMON:
        if ((fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO) || work->mCanPlaySeq == 0) &&
            !fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK) &&
            !fightKind_CheckFightKind(FIGHT_BATTLE_KIND_VIRTUAL_TUTORIAL))
            window->mCursorIndex = MENU_FIGHT_COMMAND_RESULT_BALL;
        else
            window->mCursorIndex = MENU_FIGHT_COMMAND_RESULT_POKEMON;
        break;
    case BT_ITEM_COMMAND_BAG:
        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO))
            window->mCursorIndex = MENU_FIGHT_COMMAND_RESULT_POKEMON;
        else
            window->mCursorIndex = MENU_FIGHT_COMMAND_RESULT_BAG;
        break;
    case BT_ITEM_COMMAND_BALL:
        window->mCursorIndex = MENU_FIGHT_COMMAND_RESULT_BALL;
        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO))
            window->mCursorIndex = MENU_FIGHT_CURSOR_NONE;
        break;
    default: window->mCursorIndex = -1; break;
    }
    return 0;
}

extern "C" s32 menuFightWazaCtrl(tagWINDOW_WORK *window)
{
    MENU_WAZA_STATUS *status;
    void *copy;
    s32 i;

    if (window->mState == WINDOW_STATE_INIT) {
        copy = windowAllocMemory(window, sizeof(MENU_WAZA_STATUS));
        if (copy != NULL)
            memcpy(copy, (void *)windowGetParam(window, 0), sizeof(MENU_WAZA_STATUS));

        FightWazaButtonWork *work = (FightWazaButtonWork *)windowGetFreeWork(window);
        work->mSeq = FIGHT_WAZA_BUTTON_SEQ_SELECT;
        work->mUnused = 0;
        work->mMoveSlot = -1;
    }
    else if (window->mState == WINDOW_STATE_CLOSING) {
        copy = windowGetAllocPtr(window);
        if (copy != NULL)
            memcpy((void *)windowGetParam(window, 0), copy, sizeof(MENU_WAZA_STATUS));
    }

    status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
    if (window->mState == WINDOW_STATE_DELETE) {
        for (i = 0; i < 4; i++)
            menuItemBiosSetSelectFlag(window->mMenuID, menuFightWazaDetailIDs[i], true);
    }
    else {
        for (i = 0; i < 4; i++) {
            if (status->moves[i].moveName != 0)
                menuItemBiosSetSelectFlag(window->mMenuID, menuFightWazaDetailIDs[i], true);
            else
                menuItemBiosSetSelectFlag(window->mMenuID, menuFightWazaDetailIDs[i], false);
        }
    }

    switch (menuGetCursorItemID(window->mMenuID)) {
    case BT_ITEM_WAZA_DETAIL_0: window->mCursorIndex = 0; break;
    case BT_ITEM_WAZA_DETAIL_1: window->mCursorIndex = 1; break;
    case BT_ITEM_WAZA_DETAIL_2: window->mCursorIndex = 2; break;
    case BT_ITEM_WAZA_DETAIL_3: window->mCursorIndex = 3; break;
    default:    window->mCursorIndex = -1; break;
    }
    return 0;
}

extern "C" s32 menuFightWazaButton(tagWINDOW_WORK *window)
{
    MENU_WAZA_STATUS *status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
    FightWazaButtonWork *work = (FightWazaButtonWork *)windowGetFreeWork(window);
    Pokemon *pokemon = fightOutPokemonGetPokemonPtr(status->fightOutPokemon);
    tagWINDOW_KEY *key = windowGetKeyInfo();
    u16 keyTrg = key->mKeyTrg;

    switch (work->mSeq) {
    case FIGHT_WAZA_BUTTON_SEQ_SELECT:
        if (keyTrg & (WINDOW_KEY_X | WINDOW_KEY_Y)) {
            if (pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_SEALED, 0) != 0 ||
                fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO))
                GSsndPlayNow(MENU_SE_ERROR);
            else if ((s32)fightFloorGetStatus(NULL, 0, FIGHT_FLOOR_STATUS_POKEMON_STATUS_MENU_SUBBAR_FLAG, 0) != 0 ||
                     fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK)) {
                work->mMoveSlot = (s8)window->mCursor.y;
                if (work->mMoveSlot == 0)
                    window->mCursor.y = 1;
                else
                    window->mCursor.y = 0;
                if (menuGetSelectItemNum(window->mMenuID) <= 1)
                    window->mCursor.y = 0;
                windowCreateCursorSprite(window);
                work->mSeq = FIGHT_WAZA_BUTTON_SEQ_REPLACE;
            }
        }
        else if (pokemon != NULL &&
                 (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, (u16)window->mCursor.y) == WAZA_SHADOW_SPECIAL) {
            if (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
            }
        }
        else {
            menuButtonNormal(window);
        }
        break;
    case FIGHT_WAZA_BUTTON_SEQ_REPLACE:
        if (keyTrg & (WINDOW_KEY_ACCEPT | WINDOW_KEY_X | WINDOW_KEY_Y)) {
            if (pokemon != NULL) {
                pokemonWazaReplace(pokemon, (s8)work->mMoveSlot, (u16)window->mCursor.y);
                FightOutPokemon *outPokemon = status->fightOutPokemon;
                pokemonToMenuWazaStatus(pokemon, status);
                status->fightOutPokemon = outPokemon;
            }
            work->mMoveSlot = -1;
            work->mSeq = FIGHT_WAZA_BUTTON_SEQ_SELECT;
        }
        else if (keyTrg & WINDOW_KEY_CANCEL) {
            work->mMoveSlot = -1;
            work->mSeq = FIGHT_WAZA_BUTTON_SEQ_SELECT;
        }
        break;
    }
    return 0;
}

extern "C" void menuFightDrawCmdMsg(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u32 messageID = 0;

    switch (sprite->mItemID) {
    case BT_ITEM_COMMAND_NAME: {
        MENU_WAZA_STATUS *status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
        msgctrlSetValue(MSGCTRL_FIGHT_STATUS, (u32)status);
        messageID = MSG_FIGHT_STATUS_NAME;
        break;
    }
    case BT_ITEM_COMMAND_STATUS: {
        MENU_WAZA_STATUS *status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
        FightWazaButtonWork *work = (FightWazaButtonWork *)windowGetFreeWork(window);
        switch (work->mSeq) {
        case FIGHT_WAZA_BUTTON_SEQ_SELECT:
            msgctrlSetValue(MSGCTRL_FIGHT_STATUS, (u32)status->pokemonName);
            messageID = MSG_FIGHT_STATUS_NAME;
            break;
        case FIGHT_WAZA_BUTTON_SEQ_REPLACE:
            messageID = MSG_FIGHT_STATUS_NAME_2;
            break;
        }
        break;
    }
    case BT_ITEM_COMMAND_CURSOR_0:
        if (menuGetCursor(MENU_ID_FIGHT_COMMAND) == 0)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        break;
    case BT_ITEM_COMMAND_CURSOR_1:
        if (menuGetCursor(MENU_ID_FIGHT_COMMAND) == 1)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        break;
    case BT_ITEM_COMMAND_CURSOR_2:
        if (menuGetCursor(MENU_ID_FIGHT_COMMAND) == 2)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        break;
    case BT_ITEM_COMMAND_CURSOR_3:
        if (menuGetCursor(MENU_ID_FIGHT_COMMAND) == 3)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        break;
    }

    if (messageID != 0) {
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, -2,
            menuSubCalcColor(window, sprite), messageID);
    }
}

extern "C" void menuFightDrawNew(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_WAZA_STATUS *status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
    winSpriteSetDisp(sprite, 0);
    u8 index = 0;
    switch (sprite->mItemID) {
    case BT_ITEM_WAZA_NEW_SLOT_3:
        index = 1;
    case BT_ITEM_WAZA_NEW_SLOT_2:
        index++;
    case BT_ITEM_WAZA_NEW_SLOT_1:
        index++;
    case BT_ITEM_WAZA_NEW_SLOT_0:
        break;
    }
    s32 isNew = chkWazaNew(status, index);
    if (isNew != -1 && isNew == 0)
        winSpriteSetDisp(sprite, 1);
}

extern "C" void menuFightDrawWaza(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_WAZA_STATUS *status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
    FightWazaButtonWork *work = (FightWazaButtonWork *)windowGetFreeWork(window);
    s32 index = 0;

    switch (sprite->mItemID) {
    case BT_ITEM_WAZA_DETAIL_0:
        index = 0;
        break;
    case BT_ITEM_WAZA_DETAIL_1:
        index = 1;
        break;
    case BT_ITEM_WAZA_DETAIL_2:
        index = 2;
        break;
    case BT_ITEM_WAZA_DETAIL_3:
        index = 3;
        break;
    case BT_ITEM_WAZA_NAME_0:
        index = 4;
        break;
    case BT_ITEM_WAZA_NAME_1:
        index = 5;
        break;
    case BT_ITEM_WAZA_NAME_2:
        index = 6;
        break;
    case BT_ITEM_WAZA_NAME_3:
        index = 7;
        break;
    }

    if (index < 4) {
        if (status->moves[index].moveName == NULL)
            return;
        msgctrlSetValue(MSGCTRL_TEXT, (u32)status->moves[index].moveName);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 23, 0,
            menuSubCalcColor(window, sprite), MSG_POKEMON_NAME);
    }
    else {
        s32 row = index - 4;

        if (window->mCursor.y == row) {
            winSpriteSetDisp(sprite, 1);
            menuSubDrawCursorBack(winSpriteGetLayerID(window, sprite), sprite->mX,
                sprite->mY, sprite->mWidth, sprite->mHeight, MENU_FIGHT_CURSOR_BACK_ALPHA_OPAQUE, 1);
        }
        else if (work->mMoveSlot == row) {
            winSpriteSetDisp(sprite, 0);
            tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
            menuSubDrawCursorBack2(winSpriteGetLayerID(window, sprite), 0, 0,
                item->mWidth, item->mHeight, MENU_FIGHT_CURSOR_BACK_ALPHA_OPAQUE, 1, 1);
        }
        else {
            winSpriteSetDisp(sprite, 0);
        }
    }
}

extern "C" void menuFightDrawType(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_WAZA_STATUS *status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
    CURSOR cursor = windowGetCursor(window->mMenuID);
    s32 row = cursor.y;

    if (row < 0)
        return;
    if (row >= 4)
        return;
    if (status->moves[row].moveName == NULL)
        return;

    Pokemon *pokemon = fightOutPokemonGetPokemonPtr(status->fightOutPokemon);
    u16 move = pokemon != NULL
        ? (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_MOVE, (u16)window->mCursor.y)
        : 0;

    switch (move) {
    case WAZA_DATA_ID_NONE:
    case WAZA_DATA_ID_DARK_BLITZ:
    case WAZA_DATA_ID_DARK_RUSH:
    case WAZA_DATA_ID_DARK_BREAK:
    case WAZA_DATA_ID_DARK_END:
    case WAZA_DATA_ID_DARK_WAVE:
    case WAZA_DATA_ID_DARK_RAVE:
    case WAZA_DATA_ID_DARK_STORM:
    case WAZA_DATA_ID_DARK_FIRE:
    case WAZA_DATA_ID_DARK_BOLT:
    case WAZA_DATA_ID_DARK_CHILL:
    case WAZA_DATA_ID_DARK_BLAST:
    case WAZA_DATA_ID_DARK_SKY:
    case WAZA_DATA_ID_DARK_HOLD:
    case WAZA_DATA_ID_DARK_MIST:
    case WAZA_DATA_ID_DARK_PANIC:
    case WAZA_DATA_ID_DARK_DOWN:
    case WAZA_DATA_ID_DARK_SHED:
    case WAZA_DATA_ID_DARK_HALF:
    case WAZA_SHADOW_SPECIAL:
        return;
    default:
        break;
    }

    u16 type = status->moves[row].type;
    windowDrawSprite(0, 2, window, zokuseiBiosGetSpriteID(type), 0,
        winSpriteGetLayerID(window, sprite));
}

extern "C" void menuFightDrawPP(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_WAZA_STATUS *status;
    CURSOR cursor;
    s16 row;
    s16 slashWidth;
    s16 x;
    u32 color;
    s32 waza;

    status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
    color = menuSubCalcColor(window, sprite);
    cursor = windowGetCursor(window->mMenuID);
    row = cursor.y;
    slashWidth = (s16)(GSmsgGetMessageSize(MSG_SLASH_TEXT) >> 16);
    x = (s16)(sprite->mWidth - slashWidth * 5);
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);

    GSmsgPrint2(layer, x, 0, color, MSG_SLASH_TEXT);

    if (row < 0)
        return;
    if (row >= 4)
        return;
    if (status->moves[row].moveName == NULL)
        return;

    Pokemon *pokemon = fightOutPokemonGetPokemonPtr(status->fightOutPokemon);
    waza = pokemon != NULL ? (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_MOVE, (u16)window->mCursor.y) : (u16)0;

    switch (waza) {
    case WAZA_DATA_ID_NONE:
    case WAZA_DATA_ID_DARK_BLITZ:
    case WAZA_DATA_ID_DARK_RUSH:
    case WAZA_DATA_ID_DARK_BREAK:
    case WAZA_DATA_ID_DARK_END:
    case WAZA_DATA_ID_DARK_WAVE:
    case WAZA_DATA_ID_DARK_RAVE:
    case WAZA_DATA_ID_DARK_STORM:
    case WAZA_DATA_ID_DARK_FIRE:
    case WAZA_DATA_ID_DARK_BOLT:
    case WAZA_DATA_ID_DARK_CHILL:
    case WAZA_DATA_ID_DARK_BLAST:
    case WAZA_DATA_ID_DARK_SKY:
    case WAZA_DATA_ID_DARK_HOLD:
    case WAZA_DATA_ID_DARK_MIST:
    case WAZA_DATA_ID_DARK_PANIC:
    case WAZA_DATA_ID_DARK_DOWN:
    case WAZA_DATA_ID_DARK_SHED:
    case WAZA_DATA_ID_DARK_HALF:
        GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight, color, MSG_PP_NO_VALUE);
        GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PP_NO_VALUE);
        break;
    case WAZA_SHADOW_SPECIAL:
        GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight, color, MSG_PP_SPECIAL);
        GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PP_SPECIAL);
        break;
    default:
        msgctrlSetValue(MSGCTRL_NUM_VALUE, status->moves[row].currentPP);
        GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight, color, MSG_PRINT_NUMBER);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, status->moves[row].maxPP);
        GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PRINT_NUMBER);
        break;
    }
}

s32 _menuFightIsUse(MENU_WAZA_STATUS *status, u16 wazaIndex)
{
    FightOutPokemon *fightOutPokemon = status->fightOutPokemon;
    Pokemon *pokemon = fightOutPokemonGetPokemonPtr(fightOutPokemon);
    u16 out;
    u8 kind = fightOutPokemonCheckCanOutOkWazaBanme(fightOutPokemon, wazaIndex, 1, &out);
    u16 wazaID = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_MOVE, wazaIndex);
    s32 messageID;

    if (kind != WAZA_BAN_KIND_NONE) {
        msgctrlSetValue(MSGCTRL_FIGHT_OUT_POKEMON, (u32)fightOutPokemon);
        messageID = wazaGetStatus(0, wazaID, WAZA_FIELD_NAME, 0);
        msgctrlSetValue(FIGHT_MSG_SLOT_MOVE_NAME, (u32)GSmsgGetStringPtr(messageID));
        u16 itemId;
        itemId = fightOutPokemonGetSoubiItemDataId(fightOutPokemon);
        fightFloorSetStatus(0, 0, FIGHT_FLOOR_STATUS_APPOINT_ITEM_DATA_ID, 0, itemId);
    }

    if (kind == WAZA_BAN_KIND_SOUUBI_ITEM) {
        messageID = MSG_FIGHT_WAZA_BAN_SOUUBI_ITEM;
    }
    else if (kind == WAZA_BAN_KIND_FORCED_WAZA) {
        msgctrlSetValue(FIGHT_MSG_SLOT_MOVE_NAME, (u32)GSmsgGetStringPtr(wazaGetStatus(0, out, WAZA_FIELD_NAME, 0)));
        messageID = MSG_FIGHT_WAZA_BAN_FORCED_WAZA;
    }
    else if (kind == WAZA_BAN_KIND_ARENA_TRAP) {
        messageID = MSG_FIGHT_WAZA_BAN_ARENA_TRAP;
    }
    else if (kind == WAZA_BAN_KIND_MAGNET_PULL) {
        messageID = MSG_FIGHT_WAZA_BAN_MAGNET_PULL;
    }
    else if (kind == WAZA_BAN_KIND_SHADOW_TAG) {
        messageID = MSG_FIGHT_WAZA_BAN_SHADOW_TAG;
    }
    else if (kind == WAZA_BAN_KIND_NO_PP) {
        messageID = MSG_FIGHT_WAZA_BAN_NO_PP;
    }

    if (kind != WAZA_BAN_KIND_NONE)
        return messageID;
    return 0;
}

extern "C" s32 menuFightCtrlBall(tagWINDOW_WORK *window)
{
    void *work = windowGetFreeWork(window);

    if (window->mState == WINDOW_STATE_INIT)
        memcpy(work, (void *)windowGetParam(window, 0), 6);
    return 0;
}

extern "C" void menuFightDrawBall(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u16 spriteID = 0;
    s32 index = -1;
    u8 *ballStatus = (u8 *)windowGetFreeWork(window);

    switch (sprite->mItemID) {
    case BT_ITEM_BALL_SLOT_0:
    case BT_ITEM_BALL_SLOT_0_ALT_1:
    case BT_ITEM_BALL_SLOT_0_ALT_2:
    case BT_ITEM_BALL_SLOT_0_ALT_3:
        index = 0;
        break;
    case BT_ITEM_BALL_SLOT_1:
    case BT_ITEM_BALL_SLOT_1_ALT_1:
    case BT_ITEM_BALL_SLOT_1_ALT_2:
    case BT_ITEM_BALL_SLOT_1_ALT_3:
        index = 1;
        break;
    case BT_ITEM_BALL_SLOT_2:
    case BT_ITEM_BALL_SLOT_2_ALT_1:
    case BT_ITEM_BALL_SLOT_2_ALT_2:
    case BT_ITEM_BALL_SLOT_2_ALT_3:
        index = 2;
        break;
    case BT_ITEM_BALL_SLOT_3:
    case BT_ITEM_BALL_SLOT_3_ALT_1:
    case BT_ITEM_BALL_SLOT_3_ALT_2:
    case BT_ITEM_BALL_SLOT_3_ALT_3:
        index = 3;
        break;
    case BT_ITEM_BALL_SLOT_4:
    case BT_ITEM_BALL_SLOT_4_ALT_1:
    case BT_ITEM_BALL_SLOT_4_ALT_2:
    case BT_ITEM_BALL_SLOT_4_ALT_3:
        index = 4;
        break;
    case BT_ITEM_BALL_SLOT_5:
    case BT_ITEM_BALL_SLOT_5_ALT_1:
    case BT_ITEM_BALL_SLOT_5_ALT_2:
    case BT_ITEM_BALL_SLOT_5_ALT_3:
        index = 5;
        break;
    case BT_ITEM_BALL_STOCK_0:
        winSpriteSetDisp(sprite, 0);
        if ((u8)fightFloor_GetStockPokemonNum(0) != 0)
            winSpriteSetDisp(sprite, 1);
        return;
    case BT_ITEM_BALL_ALL:
        index = 0;
    case BT_ITEM_BALL_STOCK_1:
        index++;
    case BT_ITEM_BALL_STOCK_2:
        index++;
    case BT_ITEM_BALL_STOCK_3:
        index++;
    case BT_ITEM_BALL_STOCK_4:
        index++;
    case BT_ITEM_BALL_STOCK_5:
        winSpriteSetDisp(sprite, 0);
        if (index + 2 <= (u8)fightFloor_GetStockPokemonNum(0))
            winSpriteSetDisp(sprite, 1);
        return;
    }

    if (index < 0)
        return;

    switch (ballStatus[index]) {
    case FIGHT_TRAINER_MENU_BALL_STATUS_NONE:
        spriteID = MENU_FIGHT_BALL_SPRITE_NONE;
        break;
    case FIGHT_TRAINER_MENU_BALL_STATUS_NORMAL:
        spriteID = MENU_FIGHT_BALL_SPRITE_NORMAL;
        break;
    case FIGHT_TRAINER_MENU_BALL_STATUS_ABNORMAL:
        spriteID = MENU_FIGHT_BALL_SPRITE_ABNORMAL;
        break;
    case FIGHT_TRAINER_MENU_BALL_STATUS_FIGHT_OUT:
        spriteID = MENU_FIGHT_BALL_SPRITE_FIGHT_OUT;
        break;
    }

    if (spriteID != 0)
        windowDrawSprite(0, 0, window, spriteID, 0, winSpriteGetLayerID(window, sprite));
}

extern "C" s32 menuFightCtrlSecretMain(tagWINDOW_WORK *window)
{
    if (window->mState == WINDOW_STATE_INIT) {
        void *work = windowAllocMemory(window, 24);
        if (work != NULL)
            memcpy(work, (void *)window->mArgs[0], 24);
    }
    return 0;
}

extern "C" void menuFightButtonSecretMain(tagWINDOW_WORK *window)
{
    u16 key = windowGetKeyInfo()->mKeyTrg;
    s32 cursorIndex = -1;

    if (key & WINDOW_KEY_ACCEPT)
        cursorIndex = 0;
    else if (key & WINDOW_KEY_CANCEL)
        cursorIndex = 2;
    else if (key & WINDOW_KEY_START)
        cursorIndex = 3;
    else if (key & WINDOW_KEY_TRIGGER_L) {
        window->mAccept = 1;
        window->mCancel = 1;
        window->mCursorIndex = -1;
    }

    if (cursorIndex >= 0) {
        window->mAccept = 1;
        window->mCursorIndex = cursorIndex;
    }
    menuFightButtonTimeOutCheck(window);
}

extern "C" s32 menuFightCtrlSecretWazaTop(tagWINDOW_WORK *window)
{
    MENU_WAZA_STATUS *status;

    if (window->mState == WINDOW_STATE_INIT) {
        status = (MENU_WAZA_STATUS *)windowAllocMemory(window, sizeof(MENU_WAZA_STATUS));
        if (status != 0)
            memcpy(status, (void *)window->mArgs[0], sizeof(MENU_WAZA_STATUS));
    }

    status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);

    return 0;
}

extern "C" void menuFightCurIkousan(tagWINDOW_WORK *window)
{
    if (menuIsCheck(MENU_ID_FIGHT_CONFIRM_2P)) {
        window = windowSearchID(MENU_ID_FIGHT_CONFIRM_2P);
        tagWINDOW_KEY *key;
        if (menuCB_GetBattleController2P() == 1)
            key = windowGetPortKeyInfo(1);
        else
            key = windowGetPortKeyInfo(2);
        if (key->mKeyTrg & WINDOW_KEY_UP) {
            if (window->mCursor.y != 0) {
                window->mCursor.y--;
                menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
            }
        } else if (key->mKeyTrg & WINDOW_KEY_DOWN) {
            if (window->mCursor.y == 0) {
                window->mCursor.y++;
                menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
            }
        }
        if (menuCursorCmp(window->mPreviousCursor, window->mCursor)) {
            windowCreateCursorSprite(window);
            menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
        }
        window->mPreviousCursor = window->mCursor;
    }
    if (menuIsCheck(MENU_ID_FIGHT_CONFIRM_1P)) {
        window = windowSearchID(MENU_ID_FIGHT_CONFIRM_1P);
        tagWINDOW_KEY *key = windowGetPortKeyInfo(1);
        if (key->mKeyTrg & WINDOW_KEY_UP) {
            if (window->mCursor.y != 0) {
                window->mCursor.y--;
                menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
            }
        } else if (key->mKeyTrg & WINDOW_KEY_DOWN) {
            if (window->mCursor.y == 0) {
                window->mCursor.y++;
                menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
            }
        }
        if (menuCursorCmp(window->mPreviousCursor, window->mCursor)) {
            windowCreateCursorSprite(window);
            menuPlaySe(window->mMenuID, MENU_SOUND_CURSOR);
        }
        window->mPreviousCursor = window->mCursor;
    }
}

extern "C" void menuFightButtonIwaza1p(tagWINDOW_WORK *window)
{
    tagWINDOW_KEY *key;
    MENU_WAZA_STATUS *status;
    s32 selectedIndex;
    Pokemon *pokemon;
    u16 stickTrigger, stickOn, leftKey;
    MENU_FIGHT_TARGET_SLOT *entries;

    if (menuIsCheck(MENU_ID_FIGHT_CONFIRM_2P)) {
        if (menuIsCheck(MENU_ID_FIGHT_WAZA_DOC_2P))
            menuClose(MENU_ID_FIGHT_WAZA_DOC_2P);
        if (menuIsCheck(MENU_ID_FIGHT_WAZA_DETAIL_2P))
            menuClose(MENU_ID_FIGHT_WAZA_DETAIL_2P);
        window = windowSearchID(MENU_ID_FIGHT_CONFIRM_2P);
        if (menuCB_GetBattleController2P() == 1)
            key = windowGetPortKeyInfo(1);
        else
            key = windowGetPortKeyInfo(2);
        if (key->mKeyTrg & WINDOW_KEY_ACCEPT) {
            window->mAccept = 1;
            window->mCursorIndex = window->mCursor.y;
            menuPlaySe(window->mMenuID, MENU_SOUND_ACCEPT);
            return;
        } else if (key->mKeyTrg & WINDOW_KEY_CANCEL) {
            window->mCancel = 1;
            window->mCursorIndex = -1;
            menuPlaySe(window->mMenuID, MENU_SOUND_CANCEL);
            return;
        }
        menuFightButtonTimeOutCheck(window);
    }
    if (menuIsCheck(MENU_ID_FIGHT_CONFIRM_1P)) {
        if (menuIsCheck(MENU_ID_FIGHT_WAZA_DOC_1P))
            menuClose(MENU_ID_FIGHT_WAZA_DOC_1P);
        if (menuIsCheck(MENU_ID_FIGHT_WAZA_DETAIL_1P))
            menuClose(MENU_ID_FIGHT_WAZA_DETAIL_1P);
        window = windowSearchID(MENU_ID_FIGHT_CONFIRM_1P);
        key = windowGetPortKeyInfo(1);
        if (key->mKeyTrg & WINDOW_KEY_ACCEPT) {
            window->mAccept = 1;
            window->mCursorIndex = window->mCursor.y;
            menuPlaySe(window->mMenuID, MENU_SOUND_ACCEPT);
        } else if (key->mKeyTrg & WINDOW_KEY_CANCEL) {
            window->mCancel = 1;
            window->mCursorIndex = -1;
            menuPlaySe(window->mMenuID, MENU_SOUND_CANCEL);
        }
        menuFightButtonTimeOutCheck(window);
    }
    selectedIndex = -1;
    if (menuIsCheck(MENU_ID_FIGHT_WAZA_2P)) {
        window = windowSearchID(MENU_ID_FIGHT_WAZA_2P);
        status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
        pokemon = fightOutPokemonGetPokemonPtr(status->fightOutPokemon);
        if (menuCB_GetBattleController2P() == 1) {
            key = windowGetPortKeyInfo(1);
            stickTrigger = _menuFightGetRightStickInfo(0, true);
            stickOn = _menuFightGetRightStickInfo(0, false);
        } else {
            key = windowGetPortKeyInfo(2);
            stickTrigger = _menuFightGetRightStickInfo(1, true);
            stickOn = _menuFightGetRightStickInfo(1, false);
        }
        if (menuIsCheck(MENU_ID_FIGHT_WAZA_DETAIL_2P)) {
            if (!(key->mKeyOn & WINDOW_KEY_X)) {
                menuClose(MENU_ID_FIGHT_WAZA_DETAIL_2P);
                menuSetDisp(MENU_ID_FIGHT_WAZA_2P, true);
            }
        } else if (stickTrigger) {
            if (stickTrigger & WINDOW_KEY_UP) selectedIndex = 0;
            else if (stickTrigger & WINDOW_KEY_RIGHT) selectedIndex = 1;
            else if (stickTrigger & WINDOW_KEY_DOWN) selectedIndex = 2;
            else if (stickTrigger & WINDOW_KEY_LEFT) selectedIndex = 3;
            else if (stickTrigger & WINDOW_KEY_TRIGGER_L) selectedIndex = -1;
            if (key->mKeyOn & WINDOW_KEY_TRIGGER_R) {
                menuFightWazaDetailSlot[1] = selectedIndex;
                if (menuIsCheck(MENU_ID_FIGHT_WAZA_DOC_2P))
                    menuClose(MENU_ID_FIGHT_WAZA_DOC_2P);
                if (status->moves[menuFightWazaDetailSlot[1]].moveName != 0)
                    menuOpenCustom(MENU_ID_FIGHT_WAZA_DOC_2P, window->mMenuID, 0, 0, false, 2,
                                   window->mMenuID, &menuFightWazaDetailSlot[1]);
            } else {
                if (menuIsCheck(MENU_ID_FIGHT_WAZA_DOC_2P))
                    menuClose(MENU_ID_FIGHT_WAZA_DOC_2P);
                if (!pokemonWazaCheckValid(pokemon, (u16)selectedIndex))
                    selectedIndex = -1;
                if (selectedIndex >= 0) {
                    window->mAccept = 1;
                    window->mCursorIndex = selectedIndex;
                }
            }
        } else if (menuIsCheck(MENU_ID_FIGHT_WAZA_DOC_2P)) {
            if (!(key->mKeyOn & WINDOW_KEY_TRIGGER_R) || !stickOn)
                menuClose(MENU_ID_FIGHT_WAZA_DOC_2P);
        } else if (key->mKeyOn & WINDOW_KEY_X) {
            menuSetDisp(MENU_ID_FIGHT_WAZA_2P, false);
            menuOpen(MENU_ID_FIGHT_WAZA_DETAIL_2P, 0);
        } else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
            window->mCancel = 1;
            window->mCursorIndex = -1;
        } else if (key->mKeyTrg & WINDOW_KEY_START) {
            window->mAccept = 1;
            window->mCursorIndex = WAZA_CURSOR_KOUSAN;
        }
        menuFightButtonTimeOutCheck(window);
    }
    selectedIndex = -1;
    if (menuIsCheck(MENU_ID_FIGHT_WAZA_1P)) {
        window = windowSearchID(MENU_ID_FIGHT_WAZA_1P);
        status = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
        pokemon = fightOutPokemonGetPokemonPtr(status->fightOutPokemon);
        key = windowGetPortKeyInfo(1);
        stickTrigger = _menuFightGetRightStickInfo(0, true);
        stickOn = _menuFightGetRightStickInfo(0, false);
        if (menuIsCheck(MENU_ID_FIGHT_WAZA_DETAIL_1P)) {
            if (!(key->mKeyOn & WINDOW_KEY_X)) {
                menuClose(MENU_ID_FIGHT_WAZA_DETAIL_1P);
                menuSetDisp(MENU_ID_FIGHT_WAZA_1P, true);
            }
        } else if (stickTrigger) {
            if (stickTrigger & WINDOW_KEY_UP) selectedIndex = 0;
            else if (stickTrigger & WINDOW_KEY_RIGHT) selectedIndex = 1;
            else if (stickTrigger & WINDOW_KEY_DOWN) selectedIndex = 2;
            else if (stickTrigger & WINDOW_KEY_LEFT) selectedIndex = 3;
            if (key->mKeyOn & WINDOW_KEY_TRIGGER_R) {
                menuFightWazaDetailSlot[0] = selectedIndex;
                if (menuIsCheck(MENU_ID_FIGHT_WAZA_DOC_1P))
                    menuClose(MENU_ID_FIGHT_WAZA_DOC_1P);
                if (status->moves[menuFightWazaDetailSlot[0]].moveName != 0)
                    menuOpenCustom(MENU_ID_FIGHT_WAZA_DOC_1P, window->mMenuID, 0, 0, false, 2,
                                   window->mMenuID, &menuFightWazaDetailSlot[0]);
            } else {
                if (menuIsCheck(MENU_ID_FIGHT_WAZA_DOC_1P))
                    menuClose(MENU_ID_FIGHT_WAZA_DOC_1P);
                if (!pokemonWazaCheckValid(pokemon, (u16)selectedIndex))
                    selectedIndex = -1;
                if (selectedIndex >= 0) {
                    window->mAccept = 1;
                    window->mCursorIndex = selectedIndex;
                }
            }
        } else if (menuIsCheck(MENU_ID_FIGHT_WAZA_DOC_1P)) {
            if (!(key->mKeyOn & WINDOW_KEY_TRIGGER_R) || !stickOn)
                menuClose(MENU_ID_FIGHT_WAZA_DOC_1P);
        } else if (key->mKeyOn & WINDOW_KEY_X) {
            menuSetDisp(MENU_ID_FIGHT_WAZA_1P, false);
            menuOpen(MENU_ID_FIGHT_WAZA_DETAIL_1P, 0);
        } else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
            window->mCancel = 1;
            window->mCursorIndex = -1;
        } else if (key->mKeyTrg & WINDOW_KEY_START) {
            window->mAccept = 1;
            window->mCursorIndex = WAZA_CURSOR_KOUSAN;
        }
        menuFightButtonTimeOutCheck(window);
    }
    selectedIndex = -1;
    if (menuIsCheck(MENU_ID_FIGHT_TARGET_2P)) {
        window = windowSearchID(MENU_ID_FIGHT_TARGET_2P);
        entries = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
        if (menuCB_GetBattleController2P() == 1) {
            key = windowGetPortKeyInfo(1);
            leftKey = _menuFightGetLeftKeyInfo(0);
        } else {
            key = windowGetPortKeyInfo(2);
            leftKey = _menuFightGetLeftKeyInfo(1);
        }
        entries = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
        if (key->mKeyTrg) {
            if (leftKey & WINDOW_KEY_UP) selectedIndex = 2;
            else if (leftKey & WINDOW_KEY_RIGHT) selectedIndex = 3;
            else if (leftKey & WINDOW_KEY_LEFT) selectedIndex = 0;
            else if (leftKey & WINDOW_KEY_DOWN) selectedIndex = 1;
            else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
                window->mAccept = 1;
                window->mCancel = 1;
                window->mCursorIndex = -1;
            }
        }
        if (selectedIndex >= 0) {
            for (s32 i = 0; i < 4; i++) {
                s32 direction;
                switch (entries[i].mMenuId) {
                case MENU_ID_FIGHT_STATUS_UPPER_LEFT: direction = 0; break;
                case MENU_ID_FIGHT_STATUS_LOWER_LEFT: direction = 1; break;
                case MENU_ID_FIGHT_STATUS_UPPER_RIGHT: direction = 2; break;
                case MENU_ID_FIGHT_STATUS_LOWER_RIGHT: direction = 3; break;
                default: direction = -1; break;
                }
                if (selectedIndex == direction) {
                    if (selectedIndex > 2) selectedIndex = 2;
                    window->mAccept = 1;
                    window->mCursorIndex = selectedIndex;
                    return;
                }
            }
        }
        menuFightButtonTimeOutCheck(window);
    }
    selectedIndex = -1;
    if (menuIsCheck(MENU_ID_FIGHT_TARGET)) {
        window = windowSearchID(MENU_ID_FIGHT_TARGET);
        entries = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
        stickTrigger = _menuFightGetRightStickInfo(0, true);
        leftKey = _menuFightGetLeftKeyInfo(0);
        key = windowGetPortKeyInfo(1);
        if (key->mKeyTrg) {
            if (leftKey & WINDOW_KEY_UP) selectedIndex = 0;
            else if (leftKey & WINDOW_KEY_RIGHT) selectedIndex = 1;
            else if (leftKey & WINDOW_KEY_DOWN) selectedIndex = 2;
            else if (leftKey & WINDOW_KEY_LEFT) selectedIndex = 3;
            else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
                window->mAccept = 1;
                window->mCancel = 1;
                window->mCursorIndex = -1;
            }
        }
        if (selectedIndex >= 0) {
            for (s32 i = 0; i < 4; i++) {
                s32 direction;
                switch (entries[i].mMenuId) {
                case MENU_ID_FIGHT_STATUS_UPPER_LEFT: direction = 3; break;
                case MENU_ID_FIGHT_STATUS_LOWER_LEFT: direction = 2; break;
                case MENU_ID_FIGHT_STATUS_UPPER_RIGHT: direction = 0; break;
                case MENU_ID_FIGHT_STATUS_LOWER_RIGHT: direction = 1; break;
                default: direction = -1; break;
                }
                if (selectedIndex == direction) {
                    if (selectedIndex > 2) selectedIndex = 2;
                    window->mAccept = 1;
                    window->mCursorIndex = selectedIndex;
                    return;
                }
            }
        }
        menuFightButtonTimeOutCheck(window);
    }
}

extern "C" void menuFightButtonSecretWazaTop(tagWINDOW_WORK *window)
{
    static long waza_no = -1;

    long cursorIndex;
    long openMenuID;
    long nextMenuID;
    tagWINDOW_KEY *key;
    MENU_WAZA_STATUS *work;

    key = windowGetKeyInfo();
    work = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
    cursorIndex = -1;
    openMenuID = 0;
    nextMenuID = 0;
    menuSetDisp(window->mMenuID, true);

    if (menuIsCheck(MENU_ID_FIGHT_SECRET_WAZA_LIST))
        openMenuID = MENU_ID_FIGHT_SECRET_WAZA_LIST;
    else if (menuIsCheck(MENU_ID_FIGHT_SECRET_WAZA_DETAIL))
        openMenuID = MENU_ID_FIGHT_SECRET_WAZA_DETAIL;

    u16 stick = _menuFightGetRightStickInfo(0, true);
    if (stick != 0) {
        if (stick & WINDOW_KEY_UP)
            cursorIndex = 0;
        else if (stick & WINDOW_KEY_RIGHT)
            cursorIndex = 1;
        else if (stick & WINDOW_KEY_DOWN)
            cursorIndex = 2;
        else if (stick & WINDOW_KEY_LEFT)
            cursorIndex = 3;

        if (work->moves[cursorIndex].moveName == 0)
            cursorIndex = -1;

        if (cursorIndex >= 0) {
            window->mAccept = 1;
            window->mCursorIndex = cursorIndex;
        }
    } else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
        window->mAccept = 1;
        window->mCancel = 1;
        window->mCursorIndex = -1;
    } else if (key->mKeyOn & WINDOW_KEY_TRIGGER_R) {
        menuSetDisp(window->mMenuID, false);

        if (key->mKeyOn & WINDOW_KEY_UP)
            waza_no = 0;
        else if (key->mKeyOn & WINDOW_KEY_RIGHT)
            waza_no = 1;
        else if (key->mKeyOn & WINDOW_KEY_DOWN)
            waza_no = 2;
        else if (key->mKeyOn & WINDOW_KEY_LEFT)
            waza_no = 3;
        else
            waza_no = -1;

        if (work->moves[waza_no].moveName == 0)
            waza_no = -1;

        nextMenuID = (waza_no < 0) ? MENU_ID_FIGHT_SECRET_WAZA_LIST
                                          : MENU_ID_FIGHT_SECRET_WAZA_DETAIL;
    }

    menuFightButtonTimeOutCheck(window);
    if (openMenuID != nextMenuID) {
        if (openMenuID != 0)
            menuClose(openMenuID);
        if (!window->mAccept) {
            if (nextMenuID == MENU_ID_FIGHT_SECRET_WAZA_LIST) {
                menuOpenCustom(MENU_ID_FIGHT_SECRET_WAZA_LIST, window->mMenuID, 0, 0, false, 1,
                    window->mMenuID);
                menuSetDisp(MENU_ID_FIGHT_SECRET_WAZA_LIST, true);
                menuSetDisp(MENU_ID_FIGHT_SECRET_WAZA_DETAIL, false);
            } else if (nextMenuID == MENU_ID_FIGHT_SECRET_WAZA_DETAIL) {
                menuOpenCustom(MENU_ID_FIGHT_SECRET_WAZA_DETAIL, window->mMenuID, 0, 0, false, 2,
                    window->mMenuID, &waza_no);
                menuSetDisp(MENU_ID_FIGHT_SECRET_WAZA_DETAIL, true);
                menuSetDisp(MENU_ID_FIGHT_SECRET_WAZA_LIST, false);
            }
        }
    }
}

extern "C" void menuFightDrawSecretWazaSelect(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 index = -1;
    tagWINDOW_WORK *moveWindow;
    MENU_WAZA_STATUS *status;
    u32 color;

    moveWindow = windowSearchID(windowGetParam(window, 0));
    if (moveWindow == 0) {
        return;
    }

    status = (MENU_WAZA_STATUS *)windowGetAllocPtr(moveWindow);
    if (status == 0) {
        return;
    }

    color = menuSubCalcColor(window, sprite);

    switch (sprite->mItemID) {
    case BT_ITEM_SECRET_WAZA_NAME_0:
    case BT_ITEM_SECRET_WAZA_TYPE_0:
    case BT_ITEM_SECRET_WAZA_PP_0:
    case BT_ITEM_SECRET_WAZA_SPARE_0:
        index = 0;
        break;
    case BT_ITEM_SECRET_WAZA_NAME_1:
    case BT_ITEM_SECRET_WAZA_TYPE_1:
    case BT_ITEM_SECRET_WAZA_PP_1:
    case BT_ITEM_SECRET_WAZA_SPARE_1:
        index = 1;
        break;
    case BT_ITEM_SECRET_WAZA_NAME_2:
    case BT_ITEM_SECRET_WAZA_TYPE_2:
    case BT_ITEM_SECRET_WAZA_PP_2:
    case BT_ITEM_SECRET_WAZA_SPARE_2:
        index = 2;
        break;
    case BT_ITEM_SECRET_WAZA_NAME_3:
    case BT_ITEM_SECRET_WAZA_TYPE_3:
    case BT_ITEM_SECRET_WAZA_PP_3:
    case BT_ITEM_SECRET_WAZA_SPARE_3:
        index = 3;
        break;
    }

    if (index < 0) {
        return;
    }

    if (status->moves[index].moveName != 0) {
        winSpriteSetDisp(sprite, true);
    } else {
        winSpriteSetDisp(sprite, false);
        return;
    }

    switch (sprite->mItemID) {
    case BT_ITEM_SECRET_WAZA_TYPE_0:
    case BT_ITEM_SECRET_WAZA_TYPE_1:
    case BT_ITEM_SECRET_WAZA_TYPE_2:
    case BT_ITEM_SECRET_WAZA_TYPE_3: {
        u16 type = status->moves[index].type;

        windowDrawSprite(0, 0, window, zokuseiBiosGetSpriteID(type), 0,
            winSpriteGetLayerID(window, sprite));
        break;
    }
    case BT_ITEM_SECRET_WAZA_PP_0:
    case BT_ITEM_SECRET_WAZA_PP_1:
    case BT_ITEM_SECRET_WAZA_PP_2:
    case BT_ITEM_SECRET_WAZA_PP_3: {
        s16 width = GSmsgGetMessageSize(MSG_SLASH_TEXT) >> 16;
        s16 x = (sprite->mWidth - width) / 2;

        GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 0, color,
            MSG_SLASH_TEXT);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, status->moves[index].currentPP);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, x,
            sprite->mHeight, color, MSG_PRINT_NUMBER_WIDE);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, status->moves[index].maxPP);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0,
            sprite->mWidth, sprite->mHeight, color, MSG_PRINT_NUMBER_WIDE);
        break;
    }
    case BT_ITEM_SECRET_WAZA_NAME_0:
    case BT_ITEM_SECRET_WAZA_NAME_1:
    case BT_ITEM_SECRET_WAZA_NAME_2:
    case BT_ITEM_SECRET_WAZA_NAME_3:
        msgctrlSetValue(MSGCTRL_TEXT, (u32)status->moves[index].moveName);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color,
            MSG_POKEMON_NAME);
        break;
    case BT_ITEM_SECRET_WAZA_SPARE_0:
    case BT_ITEM_SECRET_WAZA_SPARE_1:
    case BT_ITEM_SECRET_WAZA_SPARE_2:
    case BT_ITEM_SECRET_WAZA_SPARE_3:
        break;
    }
}

extern "C" void menuFightDrawSecretWazaDoc(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    s16 remaining1;
    s16 half;
    u16 waza;
    s32 sel = -1;
    tagWINDOW_WORK *parent = windowSearchID(windowGetParam(window, 0));
    if (parent == NULL)
        return;
    MENU_WAZA_STATUS *work = (MENU_WAZA_STATUS *)windowGetAllocPtr(parent);
    if (work == NULL)
        return;
    s32 *pSelect = (s32 *)windowGetParam(window, 1);
    if (*pSelect < 0)
        return;
    Pokemon *pokemon = fightOutPokemonGetPokemonPtr(work->fightOutPokemon);
    if (pokemon == NULL)
        return;
    waza = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, (u16)*pSelect);
    u32 color = menuSubCalcColor(window, sprite);

    switch (sprite->mItemID) {
    case BT_ITEM_WAZA_DOC_MOVE_0: case BT_ITEM_WAZA_DOC_MOVE_1: case BT_ITEM_WAZA_DOC_MOVE_3: case BT_ITEM_WAZA_DOC_MOVE_2:
        switch (sprite->mItemID) {
        case BT_ITEM_WAZA_DOC_MOVE_0: sel = 0; break;
        case BT_ITEM_WAZA_DOC_MOVE_1: sel = 1; break;
        case BT_ITEM_WAZA_DOC_MOVE_2: sel = 2; break;
        case BT_ITEM_WAZA_DOC_MOVE_3: sel = 3; break;
        }
        if (sel == *pSelect)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case BT_ITEM_WAZA_DOC_MOVE_0_B: case BT_ITEM_WAZA_DOC_MOVE_1_B: case BT_ITEM_WAZA_DOC_MOVE_3_B: case BT_ITEM_WAZA_DOC_MOVE_2_B:
        switch (sprite->mItemID) {
        case BT_ITEM_WAZA_DOC_MOVE_0_B: sel = 0; break;
        case BT_ITEM_WAZA_DOC_MOVE_1_B: sel = 1; break;
        case BT_ITEM_WAZA_DOC_MOVE_2_B: sel = 2; break;
        case BT_ITEM_WAZA_DOC_MOVE_3_B: sel = 3; break;
        }
        if (sel == *pSelect)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case BT_ITEM_WAZA_DOC_MOVE_0_C: case BT_ITEM_WAZA_DOC_MOVE_1_C: case BT_ITEM_WAZA_DOC_MOVE_3_C: case BT_ITEM_WAZA_DOC_MOVE_2_C:
        switch (sprite->mItemID) {
        case BT_ITEM_WAZA_DOC_MOVE_0_C: sel = 0; break;
        case BT_ITEM_WAZA_DOC_MOVE_1_C: sel = 1; break;
        case BT_ITEM_WAZA_DOC_MOVE_2_C: sel = 2; break;
        case BT_ITEM_WAZA_DOC_MOVE_3_C: sel = 3; break;
        }
        if (sel == *pSelect)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case BT_ITEM_WAZA_DOC_TYPE: case BT_ITEM_WAZA_DOC_TYPE_B: case BT_ITEM_WAZA_DOC_TYPE_C: {
        u16 zokuseiId = work->moves[*pSelect].type;
        windowDrawSprite(0, 0, window, zokuseiBiosGetSpriteID(zokuseiId), 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case BT_ITEM_WAZA_DOC_DESCRIPTION: case BT_ITEM_WAZA_DOC_DESCRIPTION_B: case BT_ITEM_WAZA_DOC_DESCRIPTION_C: {
        s32 descID = wazaGetStatus(0, waza, WAZA_FIELD_DESCRIPTION, 0);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, descID);
        return;
    }

    case BT_ITEM_WAZA_DOC_ACCURACY: case BT_ITEM_WAZA_DOC_ACCURACY_B: case BT_ITEM_WAZA_DOC_ACCURACY_C: {
        u32 accuracy = wazaGetStatus(0, waza, WAZA_FIELD_ACCURACY, 0);
        if (accuracy <= 1) {
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_NO_VALUE_TEXT);
        } else {
            msgctrlSetValue(MSGCTRL_NUM_VALUE, accuracy);
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PRINT_NUMBER);
        }
        return;
    }

    case BT_ITEM_WAZA_DOC_POWER: case BT_ITEM_WAZA_DOC_POWER_B: case BT_ITEM_WAZA_DOC_POWER_C: {
        u32 power = wazaGetStatus(0, waza, WAZA_FIELD_POWER, 0);
        if (power <= 1) {
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_NO_VALUE_TEXT);
        } else {
            msgctrlSetValue(MSGCTRL_NUM_VALUE, power);
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PRINT_NUMBER);
        }
        return;
    }

    case BT_ITEM_WAZA_DOC_NAME: case BT_ITEM_WAZA_DOC_NAME_B: case BT_ITEM_WAZA_DOC_NAME_C:
        msgctrlSetValue(MSGCTRL_TEXT, (u32)work->moves[*pSelect].moveName);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_PRINT_TEXT);
        return;

    case BT_ITEM_WAZA_DOC_PP: case BT_ITEM_WAZA_DOC_PP_B: case BT_ITEM_WAZA_DOC_PP_C: {
        s16 width1 = (s16)(GSmsgGetMessageSize(MSG_PP_LABEL) >> 16);
        remaining1 = (s16)(sprite->mWidth - width1);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_PP_LABEL);
        s16 width2 = (s16)(GSmsgGetMessageSize(MSG_SLASH_TEXT) >> 16);
        half = (s16)((remaining1 - width2) / 2);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), width1 + half, 0, color, MSG_SLASH_TEXT);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, work->moves[*pSelect].currentPP);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), width1, 0, half, sprite->mHeight, color, MSG_PRINT_NUMBER_WIDE);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, work->moves[*pSelect].maxPP);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), width1, 0, remaining1, sprite->mHeight, color, MSG_PRINT_NUMBER_WIDE);
        return;
    }
    }
}

Pokemon *_menuFightGetPokemonPtr(FIGHT_TRAINER *trainer, u32 index)
{
    FightPokemon *fightPokemon;
    Pokemon *pokemon;

    if (index >= 6) {
        return 0;
    }

    if (trainer == 0) {
        trainer = fightFloorGetGcHeroFightTrainerPtr(0);
    }
    if (trainer == 0) {
        return 0;
    }

    fightPokemon = fightTrainerGetValidFightPokemonPtr(trainer, index);
    if (fightPokemon == 0) {
        return 0;
    }

    pokemon = (Pokemon *)pokemonGetStatus(fightPokemon, 0, POKEMON_QUERY_PARTY_RECORD, 0);
    if (pokemonCheckValid(pokemon) == 0) {
        return 0;
    }

    return pokemon;
}

extern "C" void menuFightDrawSecretSelect(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool visible = true;

    if (window->mMenuID == MENU_ID_FIGHT_SECRET_SELECT) {
        bool showLabels = windowGetParam(window, 2);
        if (!showLabels) {
            visible = false;
        }
    }
    switch (sprite->mItemID) {
    case BT_ITEM_SECRET_SELECT_LABEL_0:
    case BT_ITEM_SECRET_SELECT_LABEL_1:
        winSpriteSetDisp(sprite, visible);
        break;
    }
}

extern "C" s32 menuFightCtrlSecretPokemonTop(tagWINDOW_WORK *window)
{
    FIGHT_TRAINER *trainer = (FIGHT_TRAINER *)windowGetParam(window, 0);
    s32 *pSelect = (s32 *)windowGetParam(window, 1);
    u8 waitFlag = (windowGetParam(window, 2) != 0);

    return 0;
}

extern "C" void menuFightButtonSecretPokemonTop(tagWINDOW_WORK *window)
{
    static long pokemon_no = -1;

    long cursorIndex;
    long openMenuID;
    long nextMenuID;
    FIGHT_TRAINER *trainer;
    s32 *pSelect;
    tagWINDOW_KEY *key;
    u8 waitFlag;

    key = windowGetKeyInfo();
    cursorIndex = -1;
    openMenuID = 0;
    nextMenuID = 0;
    trainer = (FIGHT_TRAINER *)windowGetParam(window, 0);
    pSelect = (s32 *)windowGetParam(window, 1);
    waitFlag = (windowGetParam(window, 2) != 0);
    menuSetDisp(window->mMenuID, true);

    if (menuIsCheck(MENU_ID_FIGHT_SECRET_POKEMON_LIST))
        openMenuID = MENU_ID_FIGHT_SECRET_POKEMON_LIST;
    else if (menuIsCheck(MENU_ID_FIGHT_SECRET_POKEMON_DETAIL))
        openMenuID = MENU_ID_FIGHT_SECRET_POKEMON_DETAIL;

    u16 stick = _menuFightGetRightStickInfo(0, true);
    if (stick & WINDOW_KEY_UP)
        cursorIndex = 0;
    else if (stick & WINDOW_KEY_RIGHT)
        cursorIndex = 1;
    else if (stick & WINDOW_KEY_DOWN)
        cursorIndex = 2;
    else if (stick & WINDOW_KEY_LEFT)
        cursorIndex = 3;
    else if (key->mKeyTrg & WINDOW_KEY_Y)
        cursorIndex = 4;
    else if (key->mKeyTrg & WINDOW_KEY_X)
        cursorIndex = 5;

    if (cursorIndex >= 0) {
        if (!_menuFightGetPokemonPtr(trainer, (u16)cursorIndex))
            cursorIndex = -1;
        if (cursorIndex >= 0) {
            window->mAccept = 1;
            window->mCursorIndex = cursorIndex;
        }
    }

    if (cursorIndex < 0) {
        if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
            if (waitFlag) {
                window->mAccept = 1;
                window->mCancel = 1;
                window->mCursorIndex = -1;
            }
        } else if (key->mKeyOn & WINDOW_KEY_TRIGGER_R) {
            menuSetDisp(window->mMenuID, false);
            pokemon_no = -1;
            u16 keyOn = key->mKeyOn;
            if (keyOn & WINDOW_KEY_UP) {
                if (keyOn & WINDOW_KEY_RIGHT)
                    pokemon_no = 5;
                else if (keyOn & WINDOW_KEY_LEFT)
                    pokemon_no = 4;
                else
                    pokemon_no = 0;
            } else if (keyOn & WINDOW_KEY_DOWN) {
                pokemon_no = 2;
            } else if (keyOn & WINDOW_KEY_RIGHT) {
                pokemon_no = 1;
            } else if (keyOn & WINDOW_KEY_LEFT) {
                pokemon_no = 3;
            }

            if (pokemon_no >= 0) {
                if (!_menuFightGetPokemonPtr(trainer, (u16)pokemon_no))
                    pokemon_no = -1;
            }
            nextMenuID = (pokemon_no < 0) ? MENU_ID_FIGHT_SECRET_POKEMON_LIST
                                                : MENU_ID_FIGHT_SECRET_POKEMON_DETAIL;
        }
    }

    menuFightButtonTimeOutCheck(window);
    if (openMenuID != nextMenuID) {
        if (openMenuID != 0)
            menuClose(openMenuID);
        if (!window->mAccept) {
            if (nextMenuID == MENU_ID_FIGHT_SECRET_POKEMON_LIST) {
                menuOpenCustom(MENU_ID_FIGHT_SECRET_POKEMON_LIST, window->mMenuID, 0, 0, false, 1,
                    trainer);
                menuSetDisp(MENU_ID_FIGHT_SECRET_POKEMON_LIST, true);
                menuSetDisp(MENU_ID_FIGHT_SECRET_POKEMON_DETAIL, false);
            } else if (nextMenuID == MENU_ID_FIGHT_SECRET_POKEMON_DETAIL) {
                menuOpenCustom(MENU_ID_FIGHT_SECRET_POKEMON_DETAIL, window->mMenuID, 0, 0, false, 2,
                    trainer, &pokemon_no);
                menuSetDisp(MENU_ID_FIGHT_SECRET_POKEMON_DETAIL, true);
                menuSetDisp(MENU_ID_FIGHT_SECRET_POKEMON_LIST, false);
            }
        }
    }
}

extern "C" void menuFightDrawSecretPokemon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 partyIndex = -1;
    FIGHT_TRAINER *trainer = (FIGHT_TRAINER *)windowGetParam(window, 0);

    switch (sprite->mItemID) {
    case BT_ITEM_SECRET_POKEMON_NICKNAME_0: case BT_ITEM_SECRET_POKEMON_HP_0: case BT_ITEM_SECRET_POKEMON_STATE_0: partyIndex = 0; break;
    case BT_ITEM_SECRET_POKEMON_NICKNAME_1: case BT_ITEM_SECRET_POKEMON_HP_1: case BT_ITEM_SECRET_POKEMON_STATE_1: partyIndex = 1; break;
    case BT_ITEM_SECRET_POKEMON_NICKNAME_2: case BT_ITEM_SECRET_POKEMON_HP_2: case BT_ITEM_SECRET_POKEMON_STATE_2: partyIndex = 2; break;
    case BT_ITEM_SECRET_POKEMON_NICKNAME_3: case BT_ITEM_SECRET_POKEMON_HP_3: case BT_ITEM_SECRET_POKEMON_STATE_3: partyIndex = 3; break;
    case BT_ITEM_SECRET_POKEMON_NICKNAME_4: case BT_ITEM_SECRET_POKEMON_HP_4: case BT_ITEM_SECRET_POKEMON_STATE_4: partyIndex = 4; break;
    case BT_ITEM_SECRET_POKEMON_NICKNAME_5: case BT_ITEM_SECRET_POKEMON_HP_5: case BT_ITEM_SECRET_POKEMON_STATE_5: partyIndex = 5; break;
    }
    if (partyIndex < 0) return;

    Pokemon *pokemon = _menuFightGetPokemonPtr(trainer, (u16)partyIndex);
    if (!pokemon) return;

    unsigned long color = menuSubCalcColor(window, sprite);

    switch (sprite->mItemID) {
    case BT_ITEM_SECRET_POKEMON_SPARE:
        break;

    case BT_ITEM_SECRET_POKEMON_STATE_0: case BT_ITEM_SECRET_POKEMON_STATE_1: case BT_ITEM_SECRET_POKEMON_STATE_2: case BT_ITEM_SECRET_POKEMON_STATE_3: case BT_ITEM_SECRET_POKEMON_STATE_4: case BT_ITEM_SECRET_POKEMON_STATE_5: {
        u16 spriteID = menuPokemonGetJoutaiSpriteID(pokemon, true);
        windowDrawSprite(0, 0, window, spriteID, 0, winSpriteGetLayerID(window, sprite));
        break;
    }

    case BT_ITEM_SECRET_POKEMON_HP_0: case BT_ITEM_SECRET_POKEMON_HP_1: case BT_ITEM_SECRET_POKEMON_HP_2: case BT_ITEM_SECRET_POKEMON_HP_3: case BT_ITEM_SECRET_POKEMON_HP_4: case BT_ITEM_SECRET_POKEMON_HP_5: {
        s16 gapWidth, labelWidth, slashWidth;
        labelWidth = (GSmsgGetMessageSize(MSG_HP_LABEL) >> 16) + 2;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_HP_LABEL);

        slashWidth = GSmsgGetMessageSize(MSG_SLASH_TEXT) >> 16;
        gapWidth = (sprite->mWidth - labelWidth - slashWidth) / 2;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), labelWidth + gapWidth, 0, color, MSG_SLASH_TEXT);

        msgctrlSetValue(MSGCTRL_NUM_VALUE, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, labelWidth + gapWidth, sprite->mHeight,
                       color, MSG_PRINT_NUMBER_WIDE);

        msgctrlSetValue(MSGCTRL_NUM_VALUE, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                       color, MSG_PRINT_NUMBER_WIDE);
        break;
    }

    case BT_ITEM_SECRET_POKEMON_NICKNAME_0: case BT_ITEM_SECRET_POKEMON_NICKNAME_1: case BT_ITEM_SECRET_POKEMON_NICKNAME_2: case BT_ITEM_SECRET_POKEMON_NICKNAME_3: case BT_ITEM_SECRET_POKEMON_NICKNAME_4: case BT_ITEM_SECRET_POKEMON_NICKNAME_5: {
        u16 *nickname = (u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NICKNAME, 0);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)nickname);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                       color, MSG_POKEMON_NAME);

        u32 message;
        switch (menuSubGetPokemonSexForFightDisp(pokemon)) {
        case POKEMON_SEX_MALE: message = MSG_SEX_MALE; break;
        case POKEMON_SEX_FEMALE: message = MSG_SEX_FEMALE; break;
        case POKEMON_SEX_NONE: default: message = 0; break;
        }
        if (message) {
            msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(message));
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 126, 0, color, MSG_PRINT_TEXT);
        }
        break;
    }
    }
}

extern "C" void menuFightDrawSecretPokemonStatus(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u32 color;
    s32 *pSelect;
    Pokemon *pokemon;
    s32 slot;
    FIGHT_TRAINER *trainer;
    slot = -1;
    trainer = (FIGHT_TRAINER *)windowGetParam(window, 0);
    pSelect = (s32 *)windowGetParam(window, 1);
    if (*pSelect < 0) {
        return;
    }
    pokemon = _menuFightGetPokemonPtr(trainer, (u16)*pSelect);
    if (pokemon == NULL) {
        return;
    }
    color = menuSubCalcColor(window, sprite);
    switch (sprite->mItemID) {
    case BT_ITEM_SECRET_POKEMON_STATUS_HELD_ITEM_NAME: {
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(
                itemDataBiosGetName(itemDataBiosGetPtr(pokemonGetSoubiItemDataId(pokemon)))));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                color, MSG_PRINT_TEXT);
        }
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_HELD_ITEM_ICON: {
        winSpriteSetDisp(sprite, false);
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            winSpriteSetDisp(sprite, true);
        }
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_ABILITY_NAME: {
        u16 *label = GSmsgGetStringPtr(MSG_ABILITY_LABEL);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)label);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_PRINT_TEXT);
        u16 x = (u16)(GSmsgGetMessageSize(MSG_PRINT_TEXT) >> 16);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(
            pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr((u16)pokemonGetTokuseiDataId(pokemon)))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), x, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_PRINT_TEXT);
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_0:
    case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_1:
    case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_3:
    case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_2:
    case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_5:
    case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_4: {
        switch (sprite->mItemID) {
        case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_0: slot = 0; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_1: slot = 1; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_3: slot = 3; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_2: slot = 2; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_4: slot = 4; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_CURSOR_SLOT_5: slot = 5; break;
        }
        if (slot == *pSelect) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_TYPE_FIRST: {
        u16 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        PokemonSpeciesStats *stats = pokemonDataBiosGetPtr(species);
        u8 type = (u8)pokemonDataBiosGetZokuseiDataId(stats, 0);
        u16 spriteID = (u16)zokuseiBiosGetSpriteID(type);
        windowDrawSprite(0, 0, window, spriteID, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_TYPE_SECOND: {
        u16 species;
        u8 type0;
        PokemonSpeciesStats *stats;
        u8 type1;
        species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        stats = pokemonDataBiosGetPtr(species);
        type0 = (u8)pokemonDataBiosGetZokuseiDataId(stats, 0);
        type1 = (u8)pokemonDataBiosGetZokuseiDataId(stats, 1);
        if (type0 != type1) {
            u16 spriteID = (u16)zokuseiBiosGetSpriteID(type1);
            windowDrawSprite(0, 0, window, spriteID, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_0_TYPE:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_1_TYPE:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_2_TYPE:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_3_TYPE: {
        switch (sprite->mItemID) {
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_3_TYPE: slot = 3; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_2_TYPE: slot = 2; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_1_TYPE: slot = 1; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_0_TYPE: slot = 0; break;
        }
        u16 moveID = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, slot);
        if (moveID != 0) {
            u16 type = (u16)wazaGetStatus(NULL, moveID, WAZA_FIELD_TYPE, 0);
            u16 spriteID = (u16)zokuseiBiosGetSpriteID(type);
            windowDrawSprite(0, 0, window, spriteID, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_0_KNOWN:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_1_KNOWN:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_2_KNOWN:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_3_KNOWN: {
        switch (sprite->mItemID) {
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_3_KNOWN: slot = 3; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_2_KNOWN: slot = 2; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_1_KNOWN: slot = 1; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_0_KNOWN: slot = 0; break;
        }
        winSpriteSetDisp(sprite, false);
        if ((u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, slot) != 0) {
            winSpriteSetDisp(sprite, true);
        }
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_0_PP:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_1_PP:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_2_PP:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_3_PP: {
        switch (sprite->mItemID) {
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_3_PP: slot = 3; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_2_PP: slot = 2; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_1_PP: slot = 1; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_0_PP: slot = 0; break;
        }
        u16 moveID = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, slot);
        if (moveID == 0) {
            break;
        }
        s16 x = ((s16)sprite->mWidth - (s16)(GSmsgGetMessageSize(MSG_SLASH_TEXT) >> 16)) / 2;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 0, color, MSG_SLASH_TEXT);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, slot));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight,
            color, MSG_PRINT_NUMBER_WIDE);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, pokemonWazaGetMaxPP(pokemon, slot));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_PRINT_NUMBER_WIDE);
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_0_NAME:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_1_NAME:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_2_NAME:
    case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_3_NAME: {
        switch (sprite->mItemID) {
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_3_NAME: slot = 3; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_2_NAME: slot = 2; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_1_NAME: slot = 1; break;
        case BT_ITEM_SECRET_POKEMON_STATUS_MOVE_0_NAME: slot = 0; break;
        }
        u16 moveID = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, slot);
        if (moveID == 0) {
            break;
        }
        u32 name = wazaGetStatus(NULL, moveID, WAZA_FIELD_NAME, 0);
        if (name == 0) {
            break;
        }
        msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(name));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_POKEMON_NAME);
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_HP: {
        s16 half;
        s16 x = (s16)((GSmsgGetMessageSize(MSG_HP_LABEL) >> 16) + 2);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_HP_LABEL);
        s16 slashWidth = (s16)(GSmsgGetMessageSize(MSG_SLASH_TEXT) >> 16);
        half = (((s16)sprite->mWidth - x) - slashWidth) / 2;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), x + half, 0, color, MSG_SLASH_TEXT);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, x + half, sprite->mHeight, color, MSG_PRINT_NUMBER_WIDE);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_PRINT_NUMBER_WIDE);
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_LEVEL: {
        s16 x = (s16)(GSmsgGetMessageSize(MSG_LEVEL_LABEL) >> 16);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_LEVEL_LABEL);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), x, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_PRINT_NUMBER);
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_SPECIES_NAME: {
        s16 nameWidth = 0;
        u16 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        PokemonSpeciesStats *stats = pokemonDataBiosGetPtr(species);
        if (stats == NULL) {
            break;
        }
        unsigned long nameMsgID = pokemonDataBiosGetName(stats);
        if (nameMsgID != 0) {
            msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(nameMsgID));
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_POKEMON_NAME);
            nameWidth = (s16)(GSmsgGetMessageSize(MSG_POKEMON_NAME) >> 16);
        }
        u8 sex = menuSubGetPokemonSexForDisp(pokemon);
        u32 sexMarkMsgID;
        switch (sex) {
        case POKEMON_SEX_MALE: sexMarkMsgID = MSG_SEX_MALE; break;
        case POKEMON_SEX_FEMALE: sexMarkMsgID = MSG_SEX_FEMALE; break;
        case POKEMON_SEX_NONE:
        default: sexMarkMsgID = 0; break;
        }
        if (sexMarkMsgID != 0) {
            msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(sexMarkMsgID));
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), nameWidth, 0, color, MSG_PRINT_TEXT);
        }
        break;
    }
    case BT_ITEM_SECRET_POKEMON_STATUS_NICKNAME: {
        u16 *nickname = (u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NICKNAME, 0);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)nickname);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_POKEMON_NAME);
        break;
    }
    }
}

extern "C" void menuFightDrawTargetI(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_FIGHT_TARGET_SLOT *data = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
    s32 selector;
    u32 color = menuSubCalcColor(window, sprite);

    for (s32 i = 0; i < 4; i++) {
        switch (data[i].mMenuId) {
        case MENU_ID_FIGHT_STATUS_UPPER_LEFT:
            switch (window->mMenuID) {
            case MENU_ID_FIGHT_TARGET:
                selector = BT_ITEM_TARGET_I_CURSOR_LEFT;
                break;
            case MENU_ID_FIGHT_TARGET_2P:
                selector = BT_ITEM_TARGET_I_CURSOR_LEFT_B;
                break;
            }
            switch (sprite->mItemID) {
            case BT_ITEM_TARGET_I_TEXT_LEFT_B:
            case BT_ITEM_TARGET_I_TEXT_LEFT:
                msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)data[i].mNickname);
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0,
                               sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
                return;
            }
            break;
        case MENU_ID_FIGHT_STATUS_LOWER_LEFT:
            switch (window->mMenuID) {
            case MENU_ID_FIGHT_TARGET:
                selector = BT_ITEM_TARGET_I_CURSOR_DOWN;
                break;
            case MENU_ID_FIGHT_TARGET_2P:
                selector = BT_ITEM_TARGET_I_CURSOR_DOWN_B;
                break;
            }
            switch (sprite->mItemID) {
            case BT_ITEM_TARGET_I_TEXT_DOWN_B:
            case BT_ITEM_TARGET_I_TEXT_DOWN:
                msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)data[i].mNickname);
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0,
                               sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
                return;
            }
            break;
        case MENU_ID_FIGHT_STATUS_UPPER_RIGHT:
            switch (window->mMenuID) {
            case MENU_ID_FIGHT_TARGET:
                selector = BT_ITEM_TARGET_I_CURSOR_UP;
                break;
            case MENU_ID_FIGHT_TARGET_2P:
                selector = BT_ITEM_TARGET_I_CURSOR_UP_B;
                break;
            }
            switch (sprite->mItemID) {
            case BT_ITEM_TARGET_I_TEXT_UP_B:
            case BT_ITEM_TARGET_I_TEXT_UP:
                msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)data[i].mNickname);
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0,
                               sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
                return;
            }
            break;
        case MENU_ID_FIGHT_STATUS_LOWER_RIGHT:
            switch (window->mMenuID) {
            case MENU_ID_FIGHT_TARGET:
                selector = BT_ITEM_TARGET_I_CURSOR_RIGHT;
                break;
            case MENU_ID_FIGHT_TARGET_2P:
                selector = BT_ITEM_TARGET_I_CURSOR_RIGHT_B;
                break;
            }
            switch (sprite->mItemID) {
            case BT_ITEM_TARGET_I_TEXT_RIGHT_B:
            case BT_ITEM_TARGET_I_TEXT_RIGHT:
                msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)data[i].mNickname);
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0,
                               sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
                return;
            }
            break;
        default:
            selector = 0;
            break;
        }
        if (selector == sprite->mItemID) {
            winSpriteSetDisp(sprite, true);
            return;
        }
    }
    winSpriteSetDisp(sprite, false);
}

void menuFightDrawIwaitStatusWaza(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
    Pokemon *pokemon, u8 wazaSlot, u32 color)
{
    u32 nameID;

    if (pokemon == 0) {
        return;
    }
    if (pokemonWazaCheckValid(pokemon, wazaSlot) == 0) {
        return;
    }

    nameID = wazaGetStatus(0, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_MOVE, wazaSlot),
        WAZA_STATUS_NAME_MESSAGE_ID, 0);
    if (nameID == 0) {
        return;
    }

    msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(nameID));
    GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth,
        sprite->mHeight, color, MSG_TEXT_IN_RECT);
}

extern "C" void menuFightDrawIwaitStatus(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u32 spriteId;
    u32 color;
    Pokemon *poke1;
    Pokemon *poke0;
    FightOutPokemon *outPoke0;
    FightOutPokemon *outPoke1;
    FIGHT_TRAINER *trainer;

    if (window->mMenuID == MENU_ID_FIGHT_WAZA_DETAIL_2P) {
        trainer = fightSideGetValidFightTrainerPtr(
            fightFloorGetValidFightSidePtr((FIGHT_FLOOR *)fightFloorGetNowPtr(), 1), 0);
    } else {
        trainer = fightSideGetValidFightTrainerPtr(
            fightFloorGetValidFightSidePtr((FIGHT_FLOOR *)fightFloorGetNowPtr(), 0), 0);
    }

    outPoke0 = fightTrainerGetValidFightOutPokemonPtr(trainer, 0);
    outPoke1 = fightTrainerGetValidFightOutPokemonPtr(trainer, 1);
    poke0 = (outPoke0 != 0)
        ? fightOutPokemonGetPokemonPtr(fightTrainerGetValidFightOutPokemonPtr(trainer, 0)) : 0;
    poke1 = (outPoke1 != 0)
        ? fightOutPokemonGetPokemonPtr(fightTrainerGetValidFightOutPokemonPtr(trainer, 1)) : 0;

    color = menuSubCalcColor(window, sprite);

    switch (sprite->mItemID) {
    case BT_ITEM_IWAIT_FIGHT_KIND:
        if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_QUICK_BATTLE_GBA)) {
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor,
                window, SPRITE_KIND_ICON_QUICK_BATTLE_GBA, 0, winSpriteGetLayerID(window, sprite));
        } else {
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor,
                window, SPRITE_KIND_ICON, 0, winSpriteGetLayerID(window, sprite));
        }
        return;

    case BT_ITEM_IWAIT_POKEMON0_TYPE_FIRST: case BT_ITEM_IWAIT_POKEMON0_TYPE_FIRST_B:
        if (poke0 == 0) return;
        spriteId = zokuseiGetPokemonSpriteID(poke0->getPokemonDataPtr(), 0);
        if (spriteId == 0) return;
        windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor,
            window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        return;

    case BT_ITEM_IWAIT_POKEMON0_TYPE_SECOND: case BT_ITEM_IWAIT_POKEMON0_TYPE_SECOND_B: {
        u8 type0;
        if (poke0 == 0) return;
        type0 = (u8)pokemonDataBiosGetZokuseiDataId(poke0->getPokemonDataPtr(), 0);
        if (type0 == (u8)pokemonDataBiosGetZokuseiDataId(poke0->getPokemonDataPtr(), 1))
            return;
        spriteId = zokuseiGetPokemonSpriteID(poke0->getPokemonDataPtr(), 1);
        windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor,
            window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case BT_ITEM_IWAIT_POKEMON0_NAME: case BT_ITEM_IWAIT_POKEMON0_NAME_B:
        if (poke0 == 0) return;
        msgctrlSetValue(MSGCTRL_TEXT, pokemonGetStatus(poke0, 0, POKEMON_STATUS_NICKNAME, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_POKEMON_NAME);
        return;

    case BT_ITEM_IWAIT_POKEMON0_ABILITY: case BT_ITEM_IWAIT_POKEMON0_ABILITY_B:
        if (poke0 == 0) return;
        {
            u16 tokuseiId = fightOutPokemon_GetTokuseiDataId(outPoke0);
            msgctrlSetValue(MSGCTRL_TEXT,
                (u32)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(
                    pokemonTokuseiDataBiosGetPtr((u32)tokuseiId))));
        }
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_TEXT_IN_RECT);
        return;

    case BT_ITEM_IWAIT_POKEMON0_MOVE_0: case BT_ITEM_IWAIT_POKEMON0_MOVE_0_B:
        menuFightDrawIwaitStatusWaza(window, sprite, poke0, 0, color);
        return;
    case BT_ITEM_IWAIT_POKEMON0_MOVE_1: case BT_ITEM_IWAIT_POKEMON0_MOVE_1_B:
        menuFightDrawIwaitStatusWaza(window, sprite, poke0, 1, color);
        return;
    case BT_ITEM_IWAIT_POKEMON0_MOVE_2: case BT_ITEM_IWAIT_POKEMON0_MOVE_2_B:
        menuFightDrawIwaitStatusWaza(window, sprite, poke0, 2, color);
        return;
    case BT_ITEM_IWAIT_POKEMON0_MOVE_3: case BT_ITEM_IWAIT_POKEMON0_MOVE_3_B:
        menuFightDrawIwaitStatusWaza(window, sprite, poke0, 3, color);
        return;

    case BT_ITEM_IWAIT_POKEMON1_TYPE_FIRST: case BT_ITEM_IWAIT_POKEMON1_TYPE_FIRST_B:
        if (poke1 == 0) return;
        spriteId = zokuseiGetPokemonSpriteID(poke1->getPokemonDataPtr(), 0);
        if (spriteId == 0) return;
        windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, color,
            window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        return;

    case BT_ITEM_IWAIT_POKEMON1_TYPE_SECOND: case BT_ITEM_IWAIT_POKEMON1_TYPE_SECOND_B: {
        u8 type0;
        if (poke1 == 0) return;
        type0 = (u8)pokemonDataBiosGetZokuseiDataId(poke1->getPokemonDataPtr(), 0);
        if (type0 == (u8)pokemonDataBiosGetZokuseiDataId(poke1->getPokemonDataPtr(), 1))
            return;
        spriteId = zokuseiGetPokemonSpriteID(poke1->getPokemonDataPtr(), 1);
        windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, color,
            window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case BT_ITEM_IWAIT_POKEMON1_NAME: case BT_ITEM_IWAIT_POKEMON1_NAME_B:
        if (poke1 == 0) return;
        msgctrlSetValue(MSGCTRL_TEXT, pokemonGetStatus(poke1, 0, POKEMON_STATUS_NICKNAME, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_POKEMON_NAME);
        return;

    case BT_ITEM_IWAIT_POKEMON1_ABILITY: case BT_ITEM_IWAIT_POKEMON1_ABILITY_B:
        if (poke1 == 0) return;
        {
            u16 tokuseiId = fightOutPokemon_GetTokuseiDataId(outPoke1);
            msgctrlSetValue(MSGCTRL_TEXT,
                (u32)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(
                    pokemonTokuseiDataBiosGetPtr((u32)tokuseiId))));
        }
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_TEXT_IN_RECT);
        return;

    case BT_ITEM_IWAIT_POKEMON1_MOVE_0: case BT_ITEM_IWAIT_POKEMON1_MOVE_0_B:
        if (poke1 == 0) return;
        menuFightDrawIwaitStatusWaza(window, sprite, poke1, 0, color);
        return;
    case BT_ITEM_IWAIT_POKEMON1_MOVE_1: case BT_ITEM_IWAIT_POKEMON1_MOVE_1_B:
        if (poke1 == 0) return;
        menuFightDrawIwaitStatusWaza(window, sprite, poke1, 1, color);
        return;
    case BT_ITEM_IWAIT_POKEMON1_MOVE_2: case BT_ITEM_IWAIT_POKEMON1_MOVE_2_B:
        if (poke1 == 0) return;
        menuFightDrawIwaitStatusWaza(window, sprite, poke1, 2, color);
        return;
    case BT_ITEM_IWAIT_POKEMON1_MOVE_3: case BT_ITEM_IWAIT_POKEMON1_MOVE_3_B:
        if (poke1 == 0) return;
        menuFightDrawIwaitStatusWaza(window, sprite, poke1, 3, color);
        return;

    case BT_ITEM_IWAIT_POKEMON0_HELD_ICON: case BT_ITEM_IWAIT_POKEMON0_HELD_ICON_B:
        winSpriteSetDisp(sprite, false);
        if (poke0 == 0) return;
        if (pokemonGetSoubiItemDataId(poke0) == 0) return;
        winSpriteSetDisp(sprite, true);
        return;

    case BT_ITEM_IWAIT_POKEMON1_HELD_ICON: case BT_ITEM_IWAIT_POKEMON1_HELD_ICON_B:
        winSpriteSetDisp(sprite, false);
        if (poke1 == 0) return;
        if (pokemonGetSoubiItemDataId(poke1) == 0) return;
        winSpriteSetDisp(sprite, true);
        return;

    case BT_ITEM_IWAIT_POKEMON0_HELD_NAME: case BT_ITEM_IWAIT_POKEMON0_HELD_NAME_B:
        if (poke0 == 0) return;
        if (pokemonGetSoubiItemDataId(poke0) == 0) return;
        msgctrlSetValue(MSGCTRL_TEXT,
            (u32)GSmsgGetStringPtr(itemDataBiosGetName(
                itemDataBiosGetPtr(pokemonGetSoubiItemDataId(poke0)))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_TEXT_IN_RECT);
        return;

    case BT_ITEM_IWAIT_POKEMON1_HELD_NAME: case BT_ITEM_IWAIT_POKEMON1_HELD_NAME_B:
        if (poke1 == 0) return;
        if (pokemonGetSoubiItemDataId(poke1) == 0) return;
        msgctrlSetValue(MSGCTRL_TEXT,
            (u32)GSmsgGetStringPtr(itemDataBiosGetName(
                itemDataBiosGetPtr(pokemonGetSoubiItemDataId(poke1)))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_TEXT_IN_RECT);
        return;
    }
}

extern "C" void menuFightDrawIPokemonStatus(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    u32 color;
    MENU_WAZA_STATUS *work;
    Pokemon *pokemon;
    s32 wazaNo;

    wazaNo = 0;
    work = (MENU_WAZA_STATUS *)windowGetAllocPtr(window);
    pokemon = fightOutPokemonGetPokemonPtr(work->fightOutPokemon);
    color = menuSubCalcColor(window, sprite);

    switch (sprite->mItemID) {
    case BT_ITEM_IPOKEMON_STATUS_NICKNAME:
    case BT_ITEM_IPOKEMON_STATUS_NICKNAME_B:
    {
        u16 *nickname = (u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NICKNAME, 0);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)nickname);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_TYPE_FIRST:
    case BT_ITEM_IPOKEMON_STATUS_TYPE_FIRST_B:
    {
        u16 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        PokemonSpeciesStats *data = pokemonDataBiosGetPtr(species);
        u8 type1 = (u8)pokemonDataBiosGetZokuseiDataId(data, 0);
        u16 spriteId = (u16)zokuseiBiosGetSpriteID(type1);
        windowDrawSprite(0, 0, window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_TYPE_SECOND:
    case BT_ITEM_IPOKEMON_STATUS_TYPE_SECOND_B:
    {
        u16 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        u8 type1;
        PokemonSpeciesStats *data = pokemonDataBiosGetPtr(species);
        type1 = (u8)pokemonDataBiosGetZokuseiDataId(data, 0);
        u8 type2 = (u8)pokemonDataBiosGetZokuseiDataId(data, 1);
        if (type1 != type2) {
            u16 spriteId = (u16)zokuseiBiosGetSpriteID(type2);
            windowDrawSprite(0, 0, window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_ABILITY:
    case BT_ITEM_IPOKEMON_STATUS_ABILITY_B:
    {
        u16 tokuseiId = (u16)fightOutPokemon_GetTokuseiDataId(work->fightOutPokemon);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(tokuseiId))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), (s16)(GSmsgGetMessageSize(MSG_ABILITY_LABEL) >> 16), 0, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_MOVE_3_TYPE:
        wazaNo = 1;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_2_TYPE:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_1_TYPE:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_0_TYPE:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            u16 moveType = (u16)wazaGetStatus(0, moveId, WAZA_FIELD_TYPE, 0);
            u16 spriteId = (u16)zokuseiBiosGetSpriteID(moveType);
            windowDrawSprite(0, 0, window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_MOVE_3_PP:
        wazaNo = 1;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_2_PP:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_1_PP:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_0_PP:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            s16 width = (s16)(GSmsgGetMessageSize(MSG_PP_SLASH) >> 16);
            s16 x = (sprite->mWidth - width) / 2;
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 0, color, MSG_PP_SLASH);
            msgctrlSetValue(MSGCTRL_NUM_VALUE, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, wazaNo));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight, color, MSG_PP_NUMBER);
            msgctrlSetValue(MSGCTRL_NUM_VALUE, pokemonWazaGetMaxPP(pokemon, wazaNo));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PP_NUMBER);
        }
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_MOVE_3_NAME:
        wazaNo = 1;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_2_NAME:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_1_NAME:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_0_NAME:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            u32 nameMsgId = wazaGetStatus(0, moveId, WAZA_FIELD_NAME, 0);
            if (nameMsgId != 0) {
                msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(nameMsgId));
                s16 height = (s16)(u16)GSmsgGetMessageSize(MSG_POKEMON_NAME);
                s16 y = sprite->mHeight / 2 - height / 2;
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, y, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
            }
        }
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_MOVE_3_TYPE_B:
        wazaNo = 1;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_2_TYPE_B:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_1_TYPE_B:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_0_TYPE_B:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            u16 moveType = (u16)wazaGetStatus(0, moveId, WAZA_FIELD_TYPE, 0);
            u16 spriteId = (u16)zokuseiBiosGetSpriteID(moveType);
            windowDrawSprite(0, 0, window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_MOVE_3_PP_B:
        wazaNo = 1;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_2_PP_B:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_1_PP_B:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_0_PP_B:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            s16 width = (s16)(GSmsgGetMessageSize(MSG_PP_SLASH) >> 16);
            s16 x = (sprite->mWidth - width) / 2;
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 0, color, MSG_PP_SLASH);
            msgctrlSetValue(MSGCTRL_NUM_VALUE, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, wazaNo));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight, color, MSG_PP_NUMBER);
            msgctrlSetValue(MSGCTRL_NUM_VALUE, pokemonWazaGetMaxPP(pokemon, wazaNo));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PP_NUMBER);
        }
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_MOVE_3_NAME_B:
        wazaNo = 1;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_2_NAME_B:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_1_NAME_B:
        wazaNo++;
    case BT_ITEM_IPOKEMON_STATUS_MOVE_0_NAME_B:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            u32 nameMsgId = wazaGetStatus(0, moveId, WAZA_FIELD_NAME, 0);
            if (nameMsgId != 0) {
                msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(nameMsgId));
                s16 height = (s16)(u16)GSmsgGetMessageSize(MSG_POKEMON_NAME);
                s16 y = sprite->mHeight / 2 - height / 2;
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, y, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
            }
        }
        break;
    }
    case BT_ITEM_IPOKEMON_STATUS_HELD_ITEM_NAME:
    case BT_ITEM_IPOKEMON_STATUS_HELD_ITEM_NAME_B:
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(itemDataBiosGetName(itemDataBiosGetPtr(pokemonGetSoubiItemDataId(pokemon)))));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
        }
        break;
    case BT_ITEM_IPOKEMON_STATUS_HELD_ITEM_ICON:
    case BT_ITEM_IPOKEMON_STATUS_HELD_ITEM_ICON_B:
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case BT_ITEM_IPOKEMON_STATUS_TARGET_MARKER_0:
    case BT_ITEM_IPOKEMON_STATUS_TARGET_MARKER_1:
    case BT_ITEM_IPOKEMON_STATUS_TARGET_MARKER_0_B:
    case BT_ITEM_IPOKEMON_STATUS_TARGET_MARKER_1_B:
        if (work->fightOutPokemon !=
            fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE_FIRST_POKEMON, work->fightOutPokemon,
                              fightFloor_GetFightType(0))) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
}

extern "C" void menuFightCtrlTarget(tagWINDOW_WORK *window)
{
}

extern "C" void menuFightDrawTargetCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (menuItemBiosGetSelectFlag(window->mMenuID, sprite->mItemID)) {
        if (sprite->mItemID == menuGetCursorItemID(window->mMenuID)) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
    } else {
        winSpriteSetDisp(sprite, false);
    }
}

extern "C" void menuFightButtonTargetSecret(tagWINDOW_WORK *window)
{
    tagWINDOW_KEY *key = windowGetKeyInfo();
    s32 dir = -1;
    MENU_FIGHT_TARGET_SLOT *entry = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
    s32 row;
    s32 i;

    if (key->mKeyTrg & WINDOW_KEY_UP) {
        dir = 0;
    } else if (key->mKeyTrg & WINDOW_KEY_RIGHT) {
        dir = 1;
    } else if (key->mKeyTrg & WINDOW_KEY_LEFT) {
        dir = 2;
    } else if (key->mKeyTrg & WINDOW_KEY_DOWN) {
        dir = 3;
    } else if (key->mKeyTrg & WINDOW_KEY_TRIGGER_L) {
        window->mAccept = 1;
        window->mCancel = 1;
        window->mCursorIndex = -1;
    }
    if (dir >= 0) {
        for (i = 0; i < 4; i++) {
            switch (entry[i].mMenuId) {
            case MENU_ID_FIGHT_STATUS_UPPER_LEFT:  row = 2; break;
            case MENU_ID_FIGHT_STATUS_LOWER_LEFT:  row = 3; break;
            case MENU_ID_FIGHT_STATUS_UPPER_RIGHT:  row = 0; break;
            case MENU_ID_FIGHT_STATUS_LOWER_RIGHT:  row = 1; break;
            default:  row = -1; break;
            }
            if (dir == row) {
                if (dir > 2) {
                    dir = 2;
                }
                window->mAccept = 1;
                window->mCursorIndex = dir;
                return;
            }
        }
    }
    menuFightButtonTimeOutCheck(window);
}

extern "C" void menuFightDrawTargetSecret(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_FIGHT_TARGET_SLOT *target = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
    s32 itemID;
    int i;

    for (i = 0; i < 4; i++) {
        switch (target[i].mMenuId) {
        case MENU_ID_FIGHT_STATUS_UPPER_LEFT:
            itemID = BT_ITEM_TARGET_SECRET_MARKER_LEFT;
            break;
        case MENU_ID_FIGHT_STATUS_LOWER_LEFT:
            itemID = BT_ITEM_TARGET_SECRET_MARKER_DOWN;
            break;
        case MENU_ID_FIGHT_STATUS_UPPER_RIGHT:
            itemID = BT_ITEM_TARGET_SECRET_MARKER_UP;
            break;
        case MENU_ID_FIGHT_STATUS_LOWER_RIGHT:
            itemID = BT_ITEM_TARGET_SECRET_MARKER_RIGHT;
            break;
        default:
            itemID = 0;
            break;
        }
        if (itemID == sprite->mItemID) {
            winSpriteSetDisp(sprite, true);
            return;
        }
    }
    winSpriteSetDisp(sprite, false);
}

extern "C" void menuFightOpenTimeup(void)
{
    menuOpen(MENU_ID_FIGHT_TIMEUP, 0);
}

extern "C" void menuFightButtonSecretKousan(tagWINDOW_WORK *window)
{
    menuButtonNormal(window);
    menuFightButtonTimeOutCheck(window);
}

void menuFightOpenCountDown(void)
{
    menuOpenCustom(MENU_ID_FIGHT_COUNTDOWN, -1, NULL, 0, false, 0);
}

void menuFightCloseCountDown(void)
{
    menuCloseCustom(MENU_ID_FIGHT_COUNTDOWN, 0, false);
}

u8 menuFightCloseCheckCountDown(void)
{
    return menuIsCheck(MENU_ID_FIGHT_COUNTDOWN);
}

void menuFightOpenTotalTimer(void)
{
    menuOpenCustom(MENU_ID_FIGHT_TOTAL_TIMER, -1, NULL, 0, false, 0);
}

void menuFightCloseTotalTimer(void)
{
    menuCloseCustom(MENU_ID_FIGHT_TOTAL_TIMER, 0, false);
}

u8 menuFightCloseCheckTotalTimer(void)
{
    return menuIsCheck(MENU_ID_FIGHT_TOTAL_TIMER);
}

void menuFightButtonTimeOutCheck(struct tagWINDOW_WORK *window)
{
    if (_menuFightCheckTimeOut() != 0) {
        window->mAccept = true;
        window->mCancel = true;
    }
}

s32 menuFightCtrlTimer(struct tagWINDOW_WORK *window)
{
    static long oldTime = 0;
    if (window->mState == WINDOW_STATE_INIT) {
        oldTime = 0;
        if (window->mMenuID == MENU_ID_FIGHT_TOTAL_TIMER) {
            windowSetParam(window, 0, (long)fightTimerAllGetNokoriTime());
        }
    }
    switch (window->mMenuID) {
    case MENU_ID_FIGHT_TOTAL_TIMER:
        if (menuIsCheck(MENU_ID_FIGHT_COUNTDOWN) == 0) {
            windowSetParam(window, 0, (long)fightTimerAllGetNokoriTime());
        }
        break;
    case MENU_ID_FIGHT_COUNTDOWN: {
        long time = (long)fightTimerCommandGetNokoriTime();
        windowSetParam(window, 0, time);
        if (oldTime != time) {
            if (time <= 5) {
                if (time != 0) {
                    GSsndPlayNow(MENU_SE_COUNTDOWN_TICK);
                } else if (oldTime == 1) {
                    GSsndPlayNow(MENU_SE_ERROR);
                }
            }
            oldTime = time;
            if (menuIsCheck(MENU_ID_FIGHT_TOTAL_TIMER) != 0) {
                windowSetParam(windowSearchID(MENU_ID_FIGHT_TOTAL_TIMER), 0,
                    (long)fightTimerAllGetNokoriTime());
            }
        }
        break;
    }
    }
    return 0;
}

void menuFightDrawTimer(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite)
{
    static u16 half_digit_code[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    };
    static u16 buff[8];
    switch (sprite->mItemID) {
    case BT_ITEM_TIMER_SEPARATOR_0:
    case BT_ITEM_TIMER_SEPARATOR_1:
    case BT_ITEM_TIMER_SEPARATOR_2:
    case BT_ITEM_TIMER_SEPARATOR_3:
        winSpriteSetDisp(sprite, false);
        if (0.5f <= menuGetTotalTime()) {
            winSpriteSetDisp(sprite, true);
        }
        break;
    case BT_ITEM_TIMER_MINUTES: {
        long time = windowGetParam(window, 0);
        long minutes = time / 60;
        buff[0] = half_digit_code[minutes / 10];
        buff[2] = 0;
        buff[1] = half_digit_code[minutes % 10];
        msgctrlSetValue(MSGCTRL_TEXT, (u32)buff);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
            menuSubCalcColor(window, sprite), MSG_PRINT_TEXT);
        break;
    }
    case BT_ITEM_TIMER_SECONDS: {
        long time = windowGetParam(window, 0);
        long seconds = time % 60;
        buff[0] = half_digit_code[seconds / 10];
        buff[2] = 0;
        buff[1] = half_digit_code[seconds % 10];
        msgctrlSetValue(MSGCTRL_TEXT, (u32)buff);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
            menuSubCalcColor(window, sprite), MSG_PRINT_TEXT);
        break;
    }
    case BT_ITEM_TIMER_COUNT:
        msgctrlSetValue(MSGCTRL_NUM_VALUE, windowGetParam(window, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth,
            sprite->mHeight, menuSubCalcColor(window, sprite), MSG_PRINT_NUMBER_WIDE);
        break;
    }
}

s32 getKane(void)
{
    FIGHT_TRAINER *trainer;
    u8 maxLevel;
    u8 okaneBai;
    void *kindData;
    FIGHT_TRAINER *targetTrainer;
    FightPokemon *pokemon;
    u32 playerNum;
    u16 bairitu;
    u16 multi;
    s32 i;

    playerNum = fightFloor_GetFightTypeFightoutPokemonNum(NULL);
    if (fightFloor_GetFightKindGetOkaneFlag(NULL) == 0) {
        return -1;
    }
    trainer = fightFloorGetGcHeroFightTrainerPtr(NULL);
    if (trainer == NULL) {
        return -1;
    }
    if (fightTrainer_GetHeroPtr(trainer) == NULL) {
        return -1;
    }
    okaneBai = fightTrainer_GetOkaneBai(trainer);
    targetTrainer =
        (FIGHT_TRAINER *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OPPONENT_TRAINER_1, trainer);
    kindData = fightTrainerKindDataBiosGetPtr(
        fightTrainerDB_GetKindDataId(fightTrainer_GetFightTrainerDataId(targetTrainer)));
    bairitu = fightTrainerKindDataBiosGetSyoukinBairitu(kindData);
    maxLevel = 0;
    for (i = 0; i < 6; i++) {
        pokemon = fightTrainer_GetFightPokemonPtr(targetTrainer, i);
        if (fightPokemonCheckValid(pokemon) != 0) {
            u8 level = figthPokemonGetLevel(pokemon);
            if (maxLevel < level) {
                maxLevel = level;
            }
        }
    }
    for (i = 0; i < 6; i++) {
        pokemon = fightTrainer_GetFightPokemonPtr(trainer, i);
        if (fightPokemonCheckValid(pokemon) != 0) {
            if (fightPokemon_GetCatchEntryFlag(pokemon) == 1) {
                u8 level = figthPokemonGetLevel(pokemon);
                if (maxLevel < level) {
                    maxLevel = level;
                }
            }
        }
    }
    if (fightFloor_GetFightKindOkanePoolFlag(NULL) == 0) {
        if (playerNum <= 1) {
            multi = 1;
        } else {
            multi = 2;
        }
    } else {
        multi = 4;
    }
    return multi * (bairitu * (maxLevel * okaneBai));
}

extern "C" void menuFightDrawKane(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite)
{
    s32 kane;
    s16 width;

    kane = getKane();
    winSpriteSetDisp(sprite, false);
    if (kane == -1) {
        return;
    }
    if (sprite->mItemID == BT_ITEM_KANE_VALUE) {
        msgctrlSetValue(MSG_VAR_KANE, kane + heroGetStatus(NULL, HERO_STATUS_POKEDORU, 0));
        width = (s16)(GSmsgGetMessageSize(MSG_KANE) >> 16);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), sprite->mWidth - width, 0, -1,
            MSG_KANE);
    }
    winSpriteSetDisp(sprite, true);
}

void menuFightDrawTurn(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case BT_ITEM_TURN_NUMBER:
        msgctrlSetValue(MSGCTRL_NUM_VALUE, fightFloor_GetTurnCount(0));
        if (fightFloor_GetTurnCount(0) > 9) {
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, -1, MSG_PRINT_NUMBER);
        } else {
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 8, 0, -1, MSG_PRINT_NUMBER);
        }
        break;
    case BT_ITEM_TURN_CURSOR_ROW_0_VARIANT_A:
    case BT_ITEM_TURN_CURSOR_ROW_0_VARIANT_B:
    case BT_ITEM_TURN_CURSOR_ROW_0_VARIANT_C:
        if (window->mCursor.y == 0) {
            menuSubDrawCursorBack(winSpriteGetLayerID(window, sprite), 0, 0,
                sprite->mWidth, sprite->mHeight, MENU_FIGHT_CURSOR_BACK_ALPHA_OPAQUE,
                MENU_SUB_CURSOR_BACK_STYLE_VARIANT);
        }
        break;
    case BT_ITEM_TURN_CURSOR_ROW_1_VARIANT_A:
    case BT_ITEM_TURN_CURSOR_ROW_1_VARIANT_B:
    case BT_ITEM_TURN_CURSOR_ROW_1_VARIANT_C:
        if (window->mCursor.y == 1) {
            menuSubDrawCursorBack(winSpriteGetLayerID(window, sprite), 0, 0,
                sprite->mWidth, sprite->mHeight, MENU_FIGHT_CURSOR_BACK_ALPHA_OPAQUE,
                MENU_SUB_CURSOR_BACK_STYLE_VARIANT);
        }
        break;
    }
}

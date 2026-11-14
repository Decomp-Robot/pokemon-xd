#include <types.h>
#include <camera.h>
#include <app/floor/floorModule.h>
#include <dbgMenu.h>
#include <fight.h>
#include <fightAction.h>
#include <fightFloor.h>
#include <fightGSfloor.h>
#include <fightMenu.h>
#include <fightPokemon.h>
#include <fightSeq.h>
#include <floorManager.h>
#include <GSmem.h>
#include <GSlog.h>
#include <GSthread.h>
#include <hero.h>
#include <menu.h>
#include <menuFight.h>
#include <msgctrl.h>
#include <sound.h>
#include <string.h>

enum {
    FIGHT_MODULE_UID = 18,
    FIGHT_USER_WORK_SIZE = 131072,
    FIGHT_MAIN_THREAD_PRIORITY = 20,
    FIGHT_MAIN_THREAD_STACK_SIZE = 16384,
    FIGHT_STATUS_MENU_COUNT = 4,
    FIGHT_STATUS_MENU_INACTIVE = -1,
    FIGHT_STATUS_MENU_CLOSE_MODE = 2,
    FIGHT_DEFAULT_CHARACTER_NAME_ID = 6002,
    FIGHT_SOUND_VOLUME_MAX = 255
};

struct FIGHT_ACTION_WORK : FIGHT_ACTION {
    FIGHT_ACTION_WORK() {}
};

FIGHT_ACTION_WORK fight_action;

FloorModuleInterface _floorModuleFight;

Hero _menuCtrlHero;

GSthread *_gsthread_ptr_fight_main;

static u8 _fightFlag;

u16 _fight_result_id;

u16 _fight_encount_data_id;

static u8 _dbgMenuOpenFlag;

static u8 _cameraType;

u32 dbgEffLoadFsysGroupID;

struct FightStatusMenuTable {
    u8 menuID[FIGHT_STATUS_MENU_COUNT];
};

static const FightStatusMenuTable statusMenuTable = {
    {FIGHT_STATUS_MENU_LEFT, FIGHT_STATUS_MENU_UP, FIGHT_STATUS_MENU_DOWN, FIGHT_STATUS_MENU_RIGHT}
};

void fightSetMenuCtrlHero(Hero *hero)
{
    memcpy(&_menuCtrlHero, hero, sizeof(_menuCtrlHero));
}

Hero *fightGetMenuCtrlHero(void)
{
    return &_menuCtrlHero;
}

void _fsysLoadCallbak(long fileMode, void *context, long argument)
{
}

const char *_fightGetModuleName(void)
{
    return "fight";
}

FloorModuleInterface *fightGetFloorModule(void)
{
    return &_floorModuleFight;
}

unsigned long _fightGetUID(void)
{
    return FIGHT_MODULE_UID;
}

unsigned long _fightGetStateSize(FloorStateMode mode)
{
    if (mode == FLOOR_STATE_MODE_FIGHT) {
        return fightGSfloorGetPushDataSize();
    }
    return 0;
}

void _fightRestoreStateData(FloorStateMode mode, void *data, unsigned long size)
{
    if (mode == FLOOR_STATE_MODE_FIGHT) {
        fightGSfloorPopData(data, size);
    }
}

void _fightMakeStateData(FloorStateMode mode, void *data, unsigned long size)
{
    if (mode == FLOOR_STATE_MODE_FIGHT) {
        fightGSfloorPushData(data, size);
    }
}

void fightCloseAllMenu(void)
{
    menuFightCloseAll();
}

void _fightFree(unsigned int groupID, FloorEnterMode mode)
{
    if (mode == FLOOR_ENTER_PUSH) {
        fightGSfloorFree();
    }
}

void _fightFinalize(unsigned int groupID, FloorEnterMode mode)
{
    unsigned long envSoundId;

    if (mode == FLOOR_ENTER_PUSH) {
        return;
    }
    GSthreadTerminateGroup(GSfloorGetCurrentFloorGroupID());
    GSthreadTerminateGroup(GSfloorGetCurrentFloorGroupID());
    fightCloseAllMenu();
    cameraSetType(_cameraType);
    envSoundId = fightFloor_GetEnvSndId(0);
    if (envSoundId != 0) {
        soundStop(envSoundId, 0);
    }
    soundOpenPlay(SOUND_SILENT_BGM, 0, FIGHT_SOUND_VOLUME_MAX);
    if (UserWork != 0) {
        GSmemFree(UserWork);
        UserWork = 0;
    }
    dbgMenuSetOpenFlag(_dbgMenuOpenFlag);
    fightSetFightFlag(0);
    fightFreeDeck(fightGetFightEncountDataId());
    fightSetFightEncountDataId(0);
    msgctrlSetValue(MSG_VAR_CHARACTER_NAME, FIGHT_DEFAULT_CHARACTER_NAME_ID);
    GSlogWrite("---------- fight end !! ---------- \n");
}

void _fightUpdate(unsigned int groupID, FloorEnterMode mode)
{
    fightFloorGetNowPtr();
    fightFloor_GetFightType(0);

    FightStatusMenuTable statusMenus = statusMenuTable;

    for (u8 i = 0; i < FIGHT_STATUS_MENU_COUNT; i++) {
        u8 menuID = statusMenus.menuID[i];
        s8 count = fightFloor_GetStatusMenuCnt(menuID);

        if (count < 0) {
            continue;
        }
        count--;
        fightFloor_SetStatusMenuCnt(menuID, count);
        if (count > 0) {
            continue;
        }
        if (!menuIsCheck(menuID)) {
            return;
        }
        menuCloseCustom(menuID, FIGHT_STATUS_MENU_CLOSE_MODE, 0);
        fightFloor_SetStatusMenuCnt(menuID, FIGHT_STATUS_MENU_INACTIVE);
    }

    if (fightSeqWazaWork[FIGHT_SEQ_WAZA_WORK_MESSAGE_STATE] == FIGHT_SEQ_MESSAGE_WAIT_FRAMES) {
        if (msgFrame-- <= 0) {
            fightMenuCloseMsg();
            fightSeqWazaWork[FIGHT_SEQ_WAZA_WORK_MESSAGE_STATE] = FIGHT_SEQ_MESSAGE_IDLE;
        }
    }
}

void _fightInitialize(unsigned int groupID, FloorEnterMode mode)
{
    if (GSfloorIsPop()) {
        dbgMenuClose();
        dbgMenuSetOpenFlag(0);
        return;
    }
    GSlogWrite("---------- fight start !! ---------- \n");
    fightSetFightFlag(1);
    fightReadDeck(fightGetFightEncountDataId());
    if (UserWork == 0) {
        UserWork = (u8 *)GSmemAlloc(FIGHT_USER_WORK_SIZE);
        fightSeq_ClearUserWork();
    }
    fightPokemonSetNonTrainerPokemonIsActiveFlag(1);
    fightDataRewrite();
    battleCameraStartRandom(1);
    _dbgMenuOpenFlag = dbgMenuGetOpenFlag();
    dbgMenuClose();
    dbgMenuSetOpenFlag(0);
    fightActionFifoInit();
    _cameraType = cameraGetType();
    fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAIJOU, 0, fight_action_data_kaisi);
    if (fightFloorIsNowEncountSnatchOnly(0) == 1) {
        fightSeqEventKurein(0);
        fightSeqEventKurein(1);
    }
    fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAISI_NYUUJOU_TRAINER,
        FIGHT_TARGET_ID_HOST_ENEMY_SIDE, fight_action_data_kaisi);
    if (fightFloorIsNowEncountHostEnemyPokemonKime(0) == 1) {
        fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAISI_NYUUJOU_POKEMON,
            FIGHT_TARGET_ID_HOST_ENEMY_SIDE, fight_action_data_kaisi);
    }
    fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAISI_NYUUJOU_TRAINER,
        FIGHT_TARGET_ID_HOST_SIDE, fight_action_data_kaisi);
    if (fightFloorIsNowEncountHostPokemonKime(0) == 1) {
        fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAISI_NYUUJOU_POKEMON,
            FIGHT_TARGET_ID_HOST_SIDE, fight_action_data_kaisi);
    }
    fightActionDispFifoAll();
    _gsthread_ptr_fight_main = GSthreadCreate(FIGHT_MAIN_THREAD_PRIORITY, GSfloorGetCurrentFloorGroupID(),
        FIGHT_MAIN_THREAD_STACK_SIZE, 1, 0, fightMain);
    GSthreadSetName(_gsthread_ptr_fight_main, "fightMain");
}

void fightInit(void)
{
    fightSetFightFlag(0);
    fightSetFightResultId(0);
    fightSetFightEncountDataId(0);
    fightSetGsthreadPtrFightMain(0);
    FloorModuleInterface_clear(&_floorModuleFight);
    _floorModuleFight.initialize = _fightInitialize;
    _floorModuleFight.update = _fightUpdate;
    _floorModuleFight.finalize = _fightFinalize;
    _floorModuleFight.free = _fightFree;
    _floorModuleFight.makeStateData = _fightMakeStateData;
    _floorModuleFight.restoreStateData = _fightRestoreStateData;
    _floorModuleFight.getStateSize = _fightGetStateSize;
    _floorModuleFight.getUID = _fightGetUID;
    _floorModuleFight.getModuleName = _fightGetModuleName;
}

void fightSetFightEncountDataId(u16 encountNo)
{
    _fight_encount_data_id = encountNo;
}

u16 fightGetFightEncountDataId(void)
{
    return _fight_encount_data_id;
}

void fightSetGsthreadPtrFightMain(GSthread *thread)
{
    _gsthread_ptr_fight_main = thread;
}

void fightSetFightResultId(u16 result)
{
    _fight_result_id = result;
}

u16 fightGetFightResultId(void)
{
    return _fight_result_id;
}

void fightSetFightFlag(u8 flag)
{
    _fightFlag = flag;
}

u8 fightIsFight(void)
{
    return _fightFlag;
}

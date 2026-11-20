#include <types.h>

#include <app/fight/fightTrainer.h>

#include <GSfsys.h>
#include <GSgfxVideo.h>
#include <GSthread.h>
#include <FightResult.h>
#include <battleGrid.h>
#include <camera.h>
#include <dbgMenu.h>
#include <dbgMenuFight.h>
#include <deckDataVirtualBios.h>
#include <fade.h>
#include <fight.h>
#include <fightAction.h>
#include <fightEncount.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTrainer.h>
#include <floor.h>
#include <sequence.h>
#include <sound.h>
#include <waza.h>
#include <winMsg.h>
#include <winSprite.h>

extern "C" {
/* Battle services used by the main battle loop. */
u8 fightFloorIsNowEncountOpeningVirtualBattle(FIGHT_FLOOR *floor);
u8 fightFloorIsNowEncountVirtualBattle(FIGHT_FLOOR *floor);
u8 fightFloorIsUseFightTimerAll(FIGHT_FLOOR *floor);
void fightActionFlowKaisiPreSubLoad(void);
void fightSeqCheckZenmetu(void);
void fightTimerAllInit(void);
void fightTimerAllStart(void);
u8 fightTimerAllIsOver(void);
u8 fightTimerAllBlock(void);
void fightTimerAllTerminate(void);
void fightTimerCommandInit(void);
void fightTimerCommandStart(void);
u8 fightTimerCommandBlock(void);
void fightTimerCommandTerminate(void);
void fightMenuOpenInfoMenu(s8 menu);
void fightMenuCloseInfoMenu(u8 menu);
void menuFightOpenTimeup(void);
s32 menuFightChkSeq(void);
void menuGetKeyInfo(tagWINDOW_KEY *key, u32 port);
u32 wazaSequenceSysGetWazaGroupID(u16 wazaNo, u16 resourceCategory);
u8 battleCameraIsSimple(void);
void battleCameraDoFull(void);
void battleCameraDoSimple(void);
void battleCameraStop(void);
void battleCameraDisable(void);
u8 fightRegWzxLoadGenric(u16 wazaNo);
void fightMainSetAllDispFlag(bool visible);
void fightMainWaitFrameEffect(u32 frames);
void fadeEffectHookFunction_virtual_end_Init(void);
void fightMainDebugMenu(void);

}

void _fightMainWaitFrameSub(u32 frames);

enum FightMainInfoMenu { FIGHT_MAIN_INFO_MENU_COUNTDOWN = -1 };

enum { FIGHT_MAIN_ENCOUNT_VIRTUAL_BATTLE = 217 };
enum { FIGHT_MAIN_COMM_ERROR_MESSAGE = 20497 };
enum { FIGHT_MAIN_GENERIC_RESOURCE_CATEGORY = 4 };

enum FightMainSnatchSequence {
    FIGHT_MAIN_SNATCH_SEQUENCE_PREPARE = 3,
    FIGHT_MAIN_SNATCH_SEQUENCE_START = 4,
    FIGHT_MAIN_SNATCH_SEQUENCE_END = 5
};

// Move IDs whose WZX resources are preloaded together. The second word of
// each pair is compared with the caller's requested filter value.
u16 _fight_reg_wzx_generic_data_id_ary[] = {
    WAZA_DATA_ID_THUNDER, 0, WAZA_DATA_ID_HYPER_FANG, 0, WAZA_DATA_ID_DOUBLE_EDGE, 0, WAZA_DATA_ID_TAIL_WHIP, 0,
    WAZA_DATA_ID_POISON_STING, 0, WAZA_DATA_ID_TWINEEDLE, 0, WAZA_DATA_ID_PIN_MISSILE, 0, WAZA_DATA_ID_LEER, 0,
    WAZA_DATA_ID_BITE, 0, WAZA_DATA_ID_GROWL, 0, WAZA_DATA_ID_PSYBEAM, 0, WAZA_DATA_ID_ROAR, 3,
    WAZA_DATA_ID_SING, 3, WAZA_DATA_ID_SUPERSONIC, 3, WAZA_DATA_ID_SONIC_BOOM, 3, WAZA_DATA_ID_DISABLE, 3,
    WAZA_DATA_ID_ACID, 3, WAZA_DATA_ID_EMBER, 3, WAZA_DATA_ID_WATER_GUN, 3, WAZA_DATA_ID_SURF, 3,
    WAZA_DATA_ID_ROCK_THROW, 3, 0, 0,
};
u16 _fight_reg_wzx_generic_data_id_ary_num = 42;
static FIGHT_ACTION fight_action;
static u8 _fightBgmOpenPlayed;
static u8 _fightEnvOpenPlayed;

void fightRegWzxLoadAll(s8 resourceCategory)
{
    u16 i;
    for (i = 0; i < _fight_reg_wzx_generic_data_id_ary_num; i += 2) {
        if (resourceCategory < 0 || _fight_reg_wzx_generic_data_id_ary[i + 1] == resourceCategory) {
            fightRegWzxLoadGenric(_fight_reg_wzx_generic_data_id_ary[i]);
        }
    }
}

u8 fightRegWzxLoadGenric(u16 wazaNo)
{
    u32 groupID = wazaSequenceSysGetWazaGroupID(wazaNo, FIGHT_MAIN_GENERIC_RESOURCE_CATEGORY);
    if (!GSfsysPreReadKeepCache(groupID)) {
        return 0;
    }
    while (true) {
        s32 status = GSfsysGetStatus(groupID);
        if (status == 0 || status < 0) {
            break;
        }
        GSthreadSwitch();
    }
    return 1;
}

void fightSoundOpenPlayEnv(bool enable)
{
    if (_fightEnvOpenPlayed != 1 && enable) {
        u16 encountId = fightGetFightEncountDataId();
        if (encountId != 0) {
            u32 soundId = fightEncountGetEnvSndDataId(encountId);
            if (soundId != 0) {
                soundOpenPlayEnv(soundId, 1000, 255);
            }
            _fightEnvOpenPlayed = 1;
        }
    }
}

void fightSoundOpenPlay(bool enable)
{
    if (_fightBgmOpenPlayed != 1 && enable) {
        u16 encountId = fightGetFightEncountDataId();
        if (encountId != 0) {
            u32 soundId = fightEncountGetBgmSndDataId(encountId);
            if (soundId != 0) {
                soundOpenPlay(soundId, 0, 255);
            }
            _fightBgmOpenPlayed = 1;
        }
    }
}

void fightSoundOpenPlayInit(void)
{
    _fightBgmOpenPlayed = 0;
    _fightEnvOpenPlayed = 0;
}

void fightMainSetAllDispFlag(bool visible)
{
    FIGHT_SIDE *fightSide;
    u16 trainer;
    u16 pokemon;
    u16 side;
    u16 trainerNum;
    u16 pokemonNum;

    fightFloor_GetFightType(0);
    trainerNum = fightFloor_GetFightTypeTrainerNum(0);
    pokemonNum = fightFloor_GetFightTypeFightoutPokemonNum(0);
    for (side = 0; side < 2; side++) {
        fightSide = fightFloorGetValidFightSidePtr(0, side);
        if (fightSide == 0) {
            continue;
        }
        for (trainer = 0; trainer < trainerNum; trainer++) {
            FIGHT_TRAINER *fightTrainer = fightSideGetValidFightTrainerPtr(fightSide, trainer);
            if (fightTrainer == 0) {
                continue;
            }
            NullSequence *sequence = fightTrainer_GetSequencePtr(fightTrainer);
            if (sequence == 0) {
                continue;
            }
            Sequence_SetVisibility(sequence, visible);
            for (pokemon = 0; pokemon < pokemonNum; pokemon++) {
                FightOutPokemon *outPokemon = fightTrainerGetValidFightOutPokemonPtr(fightTrainer, pokemon);
                if (outPokemon == 0) {
                    continue;
                }
                sequence = fightOutPokemon_GetSequencePtr(outPokemon);
                if (sequence == 0) {
                    continue;
                }
                Sequence_SetVisibility(sequence, visible);
            }
        }
    }
    battleGridStoreModelVisibilityFlags();
}

u8 fightMainAllCamera(u8 simpleCamera, u8 stopCamera)
{
    u8 wasSimple = battleCameraIsSimple();
    if (simpleCamera == 1) {
        battleCameraDoSimple();
    } else {
        battleCameraDoFull();
    }
    if (stopCamera != 0) {
        battleCameraStop();
    }
    battleCameraStartRandom(1);
    return wasSimple;
}

void fightMainDebugMenu(void)
{
    u8 frames;
    s32 openFlag = dbgMenuGetOpenFlag();
    dbgMenuSetOpenFlag(1);
    if (dbgMenuFightGetFightDebugMenuOpenFlag() == 1) {
        dbgMenuMain(1);
    } else {
        frames = 0;
        while (frames < 4) {
            tagWINDOW_KEY key;
            menuGetKeyInfo(&key, 1);
            if ((key.mKeyOn & WINDOW_KEY_START) != 0) {
                break;
            }
            GSthreadSwitch();
            frames += GSgfxVideoGetLastRenderTime();
        }
        if (frames < 4) {
            dbgMenuMain(1);
        }
    }
    dbgMenuSetOpenFlag(openFlag);
}

void fightMain(void)
{
    u16 diskId = fightEncountDataBiosGetDiskId(fightEncountDataBiosGetPtr(fightGetFightEncountDataId()));
    u32 envSoundId = fightFloor_GetEnvSndId(0);

    if (envSoundId != 0) {
        soundOpenPlayEnv(envSoundId, 1000, 255);
    }

    fightActionFlowKaisiPreSubLoad();
    if (fightFloorIsNowEncountOpeningVirtualBattle(0) == 0) {
        fadeSet(FADE_FLAG_EFFECT, 1.0f);
    }
    fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAISI_PRE, 0,
                                 fight_action_data_kaisi);
    if (fightFloorIsNowEncountOpeningVirtualBattle(0) == 0) {
        fadeCheck(1);
    }
    fightMainDebugMenu();

    if (fightGetFightResultId() == 0) {
        if (fightFloorIsNowEncountSnatchOnly(0) == 1) {
            fightSeqEventKurein(FIGHT_MAIN_SNATCH_SEQUENCE_PREPARE);
            fightSeqEventKurein(FIGHT_MAIN_SNATCH_SEQUENCE_START);
        }
        if (fightFloorIsNowEncountHostEnemyPokemonKime(0) == 0) {
            fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAISI_NYUUJOU_POKEMON,
                                         FIGHT_TARGET_ID_HOST_ENEMY_SIDE,
                                         fight_action_data_kaisi);
        }
        if (fightFloorIsNowEncountHostPokemonKime(0) == 0) {
            fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAISI_NYUUJOU_POKEMON,
                                         FIGHT_TARGET_ID_HOST_SIDE,
                                         fight_action_data_kaisi);
        }
        if (fightFloorIsNowEncountOpeningVirtualBattle(0) == 1) {
            fadeSet(FADE_IN_BLACK, 0.5f);
            fadeCheck(1);
            fightMainWaitFrame(100);
        }
        fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_KAISI_POST, 0,
                                 fight_action_data_kaisi);
        if (fightFloorIsNowEncountSnatchOnly(0) == 1) {
            fightSeqEventKurein(FIGHT_MAIN_SNATCH_SEQUENCE_END);
        }
        fightActionDispFifoAll();
        if (fightFloorIsUseFightTimerAll(0) == 1) {
            fightTimerAllInit();
            fightTimerAllStart();
        }

        while (fightGetFightResultId() == 0) {
            fightMainDebugMenu();
            if (fightGetFightResultId() != 0) {
                break;
            }
            if (fightFloorIsUseFightTimerAll(0) == 1 && fightTimerAllIsOver() == 1) {
                menuFightOpenTimeup();
                if (fightFloorIsNowEncountHostEnemyPokemonKime(0) == 0) {
                    if (fightFloorIsNowEncountOpeningVirtualBattle(0) == 0) {
                        fightMainAllCamera(1, 0);
                    }
                }
                if (fightFloor_GetEncountDataId(0) != FIGHT_MAIN_ENCOUNT_VIRTUAL_BATTLE) {
                    fightMenuOpenInfoMenu(0);
                }
                fightFloorSetTimeOutAllFightResult(0);
                fightMainWaitFrame(180);
                break;
            }
            if (deckDataVirtualDB_GetTurnLimit(diskId) != 0 &&
                fightFloor_GetTurnCount(0) > deckDataVirtualDB_GetTurnLimit(diskId)) {
                fightMainAllCamera(1, 0);
                fightMenuOpenInfoMenu(0);
                fightSetFightResultId(kFightResultOtherDraw);
                fightMainWaitFrame(100);
                break;
            }
            if (fightFloorIsUseFightTimerCommand(0) == 1) {
                fightTimerCommandInit();
                fightTimerCommandStart();
            }
            battleGridUpdate();
            fightMainAllCamera(0, 1);
            if (fightFloor_GetEncountDataId(0) != FIGHT_MAIN_ENCOUNT_VIRTUAL_BATTLE) {
                fightMenuOpenInfoMenu(FIGHT_MAIN_INFO_MENU_COUNTDOWN);
            }
            if (dbgMenuFightGetFightPokeStMenuOpenFlag() == 0) {
                fightMainWaitFrame(600);
            }
            fightActionDispFifoAll();
            fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_ALL_FIGHT_TRAINER_SELECT_FIGHT_ACTION, 0, fight_action_data_turn);
            while (true) {
                if (menuFightChkSeq() == 0) {
                    break;
                }
                GSthreadSwitch();
            }
            GSthreadSwitch();
            GSthreadSwitch();
            fightActionDispFifoAll();
            fightMenuCloseInfoMenu(0);
            battleCameraDoSimple();
            if (fightFloorIsUseFightTimerCommand(0) == 1) {
                fightTimerCommandBlock();
                fightTimerCommandTerminate();
            }
            fightSeqCheckZenmetu();
            if (fightGetFightResultId() != 0) {
                break;
            }
            fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_ALL_FIGHT_OUT_POKEMON_DO_FIGHT_ACTION, 0, fight_action_data_turn);
            if (fightGetFightResultId() != 0) {
                break;
            }
            fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_ONE_TURN_POST, 0, fight_action_data_turn);
            if (fightGetFightResultId() != 0) {
                break;
            }
        }
        fightActionDispFifoAll();
    }

    fightMainAllCamera(1, 0);
    fightCloseAllMenu();
    if (fightFloorIsUseFightTimerAll(0) == 1) {
        fightTimerAllBlock();
        fightTimerAllTerminate();
    }
    u16 fightResult = fightGetFightResultId();
    if (fightResult == kFightResultCommunicationError) {
        winMsgOpen(WINMSG_WINDOW_MENU, FIGHT_MAIN_COMM_ERROR_MESSAGE, true, true);
        fightMainWaitFrame(100);
        winMsgClose(true);
    }
    fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_SYUURYOU_PRE, 0,
                                 fight_action_data_syuuryou);
    fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_SYUURYOU, fightGetFightResultId(),
                                 fight_action_data_syuuryou);
    fightActionDispFifoAll();
    envSoundId = fightFloor_GetEnvSndId(0);
    if (envSoundId != 0) {
        soundStop(envSoundId, 1000);
    }
    if (fightFloor_GetEnvSndId(0) == 0) {
        soundOpenPlayFadeOutIn(SOUND_SILENT_BGM, 1000, 1000, 255);
    }
    battleCameraDisable();
    if (fightFloorIsNowEncountVirtualBattle(0) == 1 || fightFloorIsNowEncountOpeningVirtualBattle(0) == 1) {
        fadeSetEX(FADE_FLAG_EFFECT | FADE_FLAG_BLACK_AFTER_EFFECT, 2.5f, 2.5f,
                  fadeEffectHookFunction_virtual_end_Init, true);
    } else {
        fadeSet(FADE_OUT_BLACK, 1.0f);
    }
    fadeCheck(1);
    fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_SYUURYOU_POST, 0,
                                 fight_action_data_syuuryou);
    fightActionCreateAndFlowFifo(&fight_action, 0, 0, FIGHT_ACTION_KIND_HEIJOU, 0,
                                 fight_action_data_syuuryou);
    fightEncountCheckZenmetu();
    GSfloorPop();
    floorSetFadeScript(0, 0);
}

void fightMainWaitFrameEffect(u32 frames)
{
    _fightMainWaitFrameSub(frames);
}

void fightMainWaitFrame(s32 frameCount)
{
    _fightMainWaitFrameSub(dbgMenuFightGetMsgSpeedToFrame(frameCount));
}

void _fightMainWaitFrameSub(u32 frames)
{
    u32 elapsed;
    if (frames != 0) {
        elapsed = 0;
        while (elapsed < frames) {
            GSthreadSwitch();
            elapsed += GSgfxVideoGetLastRenderTime();
        }
    }
}

#include <dbgMenuFight.h>
#include <dbgMenu.h>
#include <dbgMenuPokemon.h>
#include <fight.h>
#include <FightResult.h>
#include <fightEncount.h>
#include <fightEncountBios.h>
#include <fightFloor.h>
#include <fightKindDataBios.h>
#include <fightMenu.h>
#include <fightPokemon.h>
#include <fightResource.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <fightTrainer.h>
#include <fightTrainerStatus.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <battleGrid.h>
#include <deck.h>
#include <sequence.h>
#include <menu.h>
#include <menuIds.h>
#include <menuFace.h>
#include <menuSub.h>
#include <msgctrl.h>
#include <waza.h>
#include <winMsg.h>
#include <winSprite.h>
#include <zokusei.h>
#include <floorManager.h>
#include <GSfsys.h>
#include <GSinput.h>
#include <GSmem.h>
#include <GSmsg.h>
#include <GSthread.h>
#include <types.h>

#pragma force_active on

u8 _dbgMenuFightMsgSpeed = 5;
u16 _dbgmenu_fight_waza_edit_waza_data_id = 1;
u16 _dbgmenu_fight_deck_data_id = 1;
u16 _dbgmenu_fight_sample_encount_data_id = 1;
u16 _dbgmenu_fight_story_encount_data_id = 1;
u16 _dbgmenu_fight_hundred_encount_data_id = 1;
u16 _dbgmenu_fight_imasugu_encount_data_id = 1;
u16 _dbgmenu_fight_virtual_encount_data_id = 1;
u16 _dbgmenu_fight_bingo_encount_data_id = 1;
u16 _dbgmenu_fight_colosseum_encount_data_id = 1;
u8 _dbgMenuFightTrainerAiLastValueReviseFlag = 1;
u8 _dbgMenuFightPokeStMenuOpenFlag = 1;
u16 _dbgmenu_fight_trainer_pokemon_part_data_id = 1;
u32 *_readEncountDataName;
u16 *_readEncountDataID;
u16 _readEncountDataCnt;
u16 _dbgmenu_fight_trainer_ai_addsub_data_id;
u16 _dbgmenu_fight_trainer_data_id;
u16 _yubiwohuru_waza_data_id;
u8 _dbgMenuFightSoubikoukaFlag;
FightPokemon *_target_fight_pokemon_ptr;
FIGHT_TRAINER *_target_fight_trainer_ptr;
FIGHT_SIDE *_target_fight_side_ptr;
u8 _dbgMenuFightTuikakoukaFlag;
u8 _dbgMenuFightTrainerAiFlowDispFlag;
u8 _dbgMenuFightDebugMenuOpenFlag;
u8 _dbgMenuFightAllFightTrainerOperateFlag;
u8 _dbgMenuFightMsgKeyWaitFlag;

/* Menu entry ids of the encounter editor. */
enum {
    ENCOUNT_EDIT_ITEM_KIND = 266,
    ENCOUNT_EDIT_ITEM_TRAINERS_PER_SIDE,
    ENCOUNT_EDIT_ITEM_STYLE,
    ENCOUNT_EDIT_ITEM_POKEMON_PER_PLAYER,
    ENCOUNT_EDIT_ITEM_FIELD,
    ENCOUNT_EDIT_ITEM_TRAINER_DATA_0,
    ENCOUNT_EDIT_ITEM_TRAINER_DATA_1,
    ENCOUNT_EDIT_ITEM_TRAINER_DATA_2,
    ENCOUNT_EDIT_ITEM_TRAINER_DATA_3
};


/* Dialogue test entry points; kind 0 is the scene where the trainer shows up,
 * 1 the victory line, 2 and 3 the two defeat lines. */
s32 dbgMenuFightFightTrainerSerihuTestHaiboku2(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(_dbgmenu_fight_trainer_data_id, 3);
}

s32 dbgMenuFightFightTrainerSerihuTestHaiboku(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(_dbgmenu_fight_trainer_data_id, 2);
}

s32 dbgMenuFightFightTrainerSerihuTestSyouri(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(_dbgmenu_fight_trainer_data_id, 1);
}

s32 dbgMenuFightFightTrainerSerihuTestToujou(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(_dbgmenu_fight_trainer_data_id, 0);
}

/* Play one dialogue scene and wait for the message to be read. */
s32 _dbgMenuFightFightTrainerSerihuTestSub(u16 trainerDataId, u8 kind)
{
    u32 status = fightTrainerGetStatus(0, trainerDataId, FIGHT_TRAINER_QUERY_DIALOGUE, kind);

    if (status == 0) {
        return -1;
    }
    winMsgOpenFight(status, 1, 1);
    winMsgCloseFight(1);
    return 1;
}

/* Take the trainer data id the dialogue tests are played with. */
s32 dbgMenuFightFightTrainerSerihuTest(void)
{
    s32 id = dbgMenuFightGetFightTrainerDataId(_dbgmenu_fight_trainer_data_id);

    if (id < 0) {
        return -1;
    }
    _dbgmenu_fight_trainer_data_id = id;
    return 1;
}

/* Encounter data editor: opens the encounter editor on the given record,
 * edits one field per menu entry and puts the record back when it is
 * cancelled. */
s32 dbgMenuFightEncountDataEditSub(u16 encountNo)
{
    FightEncountRecord backup;
    u8* data;
    s32 result;
    s32 id;

    if (encountNo == 0) {
        return 1;
    }
    if (encountNo >= *fight_encount_data_number) {
        return 1;
    }
    data = fightEncountDataBiosGetPtr(encountNo);
    backup = *(FightEncountRecord*)data;

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_ENCOUNTER_EDITOR, 0, 0, 0, 1, 1, encountNo);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_ENCOUNTER_EDITOR, 0, 1);
            *(FightEncountRecord*)data = backup;
            return -1;
        }
        if (result == -2) {
            if (menuOpen(MENU_ID_DEBUG_EDIT_CONFIRMATION, 1) != 0) {
                menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
                continue;
            }
            menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
            break;
        }
        switch (menuGetCursorItemID(MENU_ID_DEBUG_ENCOUNTER_EDITOR)) {
        case ENCOUNT_EDIT_ITEM_KIND:
            id = dbgMenuFightGetFightKindDataId(fightEncountDataBiosGetFightKind(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightKind(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINERS_PER_SIDE:
            id = dbgMenuFightGetFightTypeDataId(fightEncountDataBiosGetTrainer(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetTrainer(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_STYLE:
            id = dbgMenuFightGetPokemonFightType(fightEncountDataBiosGetPokemon(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetPokemon(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_POKEMON_PER_PLAYER:
            id = dbgMenuFightGetPokemonEntry(fightEncountDataBiosGetEntry(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetEntry(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_FIELD:
            id = dbgMenuFightGetFightFloorDataId(fightEncountDataBiosGetFightFloorDataId(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightFloorDataId(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_0:
            id = dbgMenuFightGetFightTrainerDataId(fightEncountDataBiosGetFightTrainerDataId(data, 0));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 0, id);
            id = dbgMenuFightGetGsinputDevId(fightEncountDataBiosGetGSInputDevice(data, 0));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetGSInputDevice(data, 0, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_1:
            id = dbgMenuFightGetFightTrainerDataId(fightEncountDataBiosGetFightTrainerDataId(data, 1));
            /* The second player does not take data id 0. */
            if (id < 1) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 1, id);
            id = dbgMenuFightGetGsinputDevId(fightEncountDataBiosGetGSInputDevice(data, 1));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetGSInputDevice(data, 1, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_2:
            id = dbgMenuFightGetFightTrainerDataId(fightEncountDataBiosGetFightTrainerDataId(data, 2));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 2, id);
            id = dbgMenuFightGetGsinputDevId(fightEncountDataBiosGetGSInputDevice(data, 2));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetGSInputDevice(data, 2, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_3:
            id = dbgMenuFightGetFightTrainerDataId(fightEncountDataBiosGetFightTrainerDataId(data, 3));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 3, id);
            id = dbgMenuFightGetGsinputDevId(fightEncountDataBiosGetGSInputDevice(data, 3));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetGSInputDevice(data, 3, id);
            continue;
        default:
            continue;
        }
    }
    menuCloseCustom(MENU_ID_DEBUG_ENCOUNTER_EDITOR, 0, 1);
    return 1;
}

/* Message id of the game-cube input device number the debug fight menu
 * writes into its status line. */
s32 dbgMenuGetGsinputMsgId(s32 deviceId)
{
    s32 msg;

    if (deviceId == 0) {
        msg = 0xED04;
    } else if (deviceId == 1) {
        msg = 0xED05;
    } else if (deviceId == 2) {
        msg = 0xED06;
    } else if (deviceId == 3) {
        msg = 0xED07;
    } else if (deviceId == 4) {
        msg = 0xED08;
    } else {
        msg = 0xED04;
    }
    return msg;
}

/* Game-cube pad bits the debug windows react to. */
enum {
    FIGHT_PAD_A = 0x100,
    FIGHT_PAD_B = 0x200,
    FIGHT_PAD_START = 0x1000
};

void dbmMenuFightButtonNormal(tagWINDOW_WORK *window)
{
    if (window != 0) {
        if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1)) & FIGHT_PAD_A) {
            window->mAccept = 1;
            window->mCursorIndex = menuGetCursorItemID(window->mMenuID);
        }
        if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1)) & FIGHT_PAD_START) {
            window->mAccept = 1;
            window->mCursorIndex = -2;
        }
        if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1)) & FIGHT_PAD_B) {
            window->mAccept = 1;
            window->mCancel = 1;
            window->mCursorIndex = -1;
        }
    }
}
s32 dbgMenuFightStopTimeOut(void)
{
    if (fightIsFight() == 0) return -1;
    fightFloorSetTimeOutAllFightResult(0);
    dbgMenuClose();
    return 0;
}
s32 dbgMenuFightStopHostLose(void)
{
    if (fightIsFight() == 0) return -1;
    _dbgMenuFightStopSub(kFightResultOtherSideVictory);
    return 0;
}
s32 dbgMenuFightStopHostWin(void)
{
    if (fightIsFight() == 0) return -1;
    _dbgMenuFightStopSub(kFightResultHostVictory);
    return 0;
}
void _dbgMenuFightStopSub(u16 result)
{
    fightSetFightResultId(result);
    dbgMenuClose();
}
s32 dbgMenuFightFightPokemonSelect5(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_pokemon_ptr = fightTrainerGetValidFightPokemonPtr(_target_fight_trainer_ptr, 5);
    if (_target_fight_pokemon_ptr == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub(_target_fight_pokemon_ptr);
}
s32 dbgMenuFightFightPokemonSelect4(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_pokemon_ptr = fightTrainerGetValidFightPokemonPtr(_target_fight_trainer_ptr, 4);
    if (_target_fight_pokemon_ptr == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub(_target_fight_pokemon_ptr);
}
s32 dbgMenuFightFightPokemonSelect3(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_pokemon_ptr = fightTrainerGetValidFightPokemonPtr(_target_fight_trainer_ptr, 3);
    if (_target_fight_pokemon_ptr == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub(_target_fight_pokemon_ptr);
}
s32 dbgMenuFightFightPokemonSelect2(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_pokemon_ptr = fightTrainerGetValidFightPokemonPtr(_target_fight_trainer_ptr, 2);
    if (_target_fight_pokemon_ptr == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub(_target_fight_pokemon_ptr);
}
s32 dbgMenuFightFightPokemonSelect1(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_pokemon_ptr = fightTrainerGetValidFightPokemonPtr(_target_fight_trainer_ptr, 1);
    if (_target_fight_pokemon_ptr == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub(_target_fight_pokemon_ptr);
}
s32 dbgMenuFightFightPokemonSelect0(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_pokemon_ptr = fightTrainerGetValidFightPokemonPtr(_target_fight_trainer_ptr, 0);
    if (_target_fight_pokemon_ptr == 0) return -1;
    return _dbgMenuFightFightPokemonSelectSub(_target_fight_pokemon_ptr);
}
s32 _dbgMenuFightFightPokemonSelectSub(FightPokemon *fightPokemon)
{
    u16 i;
    u8 level;
    FIGHT_TRAINER *trainer;
    Pokemon *pokemon;
    s32 result;
    FightOutPokemon *outPokemon;
    void *oldSequence;
    void *newSequence;
    u16 species;
    u16 fightType;
    u8 appearanceChanged = 0;
    u8 statusChanged = 0;
    u8 abilityChanged = 0;
    u8 itemChanged = 0;
    fightType = fightFloorGetStatus(0, 0, FIGHT_FLOOR_STATUS_FIGHT_TYPE, 0);
    pokemon = fightPokemonGetPokemonPtr(fightPokemon);
    if (pokemon == 0) return -1;
    trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
    if (trainer == 0) return -1;
    level = figthPokemonGetLevel(fightPokemon);
    if (fightTrainerCheckCanIrekaeFightPokemon(trainer, fightPokemon) == FIGHT_TRAINER_IREKAE_FIGHT_OUT) {
        outPokemon = fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(trainer, fightPokemon);
        if (fightOutPokemonIsUseHensinBuff(outPokemon) == 1) return -1;
        if (outPokemon != 0) {
            u16 typeIndex;
            fightOutPokemonGetTokuseiDataId(outPokemon);
            for (typeIndex = 0; typeIndex < 2; typeIndex++) fightOutPokemonGetZokuseiDataId(outPokemon, 0);
        }
        result = dbgMenuPokemonEditSub(pokemon, 1, &appearanceChanged, &statusChanged, &abilityChanged, &itemChanged);
        if (result == 1) {
            species = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIES, 0);
            if (outPokemon != 0) {
                oldSequence = (void *)pokemonGetStatus(outPokemon, 0, POKEMON_QUERY_FIGHT_SEQUENCE, 0);
                if (oldSequence != 0) {
                    menuFaceLoad(pokemon, 0, 0);
                    while (menuFaceSearchEnable(menuFaceGetID(pokemon)) == 0) GSthreadSwitch();
                    newSequence = pokemonCreateSequence(fightPokemon);
                    battleGridReplacePokemon(oldSequence, newSequence);
                    battleGridUpdate();
                    pokemonSetStatus(outPokemon, 0, POKEMON_QUERY_FIGHT_SEQUENCE, 0, (u32)newSequence);
                    Sequence_Free(oldSequence);
                    Sequence_SetVisibility(newSequence, 1);
                    fightOutPokemonInitJoutaiDataId(outPokemon, JOUTAI_SUBSTITUTE);
                }
                fightMenuFightOutPokemonRenewStatusMenu(outPokemon, fightFloorGetStatus(0, 0, FIGHT_FLOOR_STATUS_FIGHT_TYPE, 0), 1);
                pokemonSetSequenceStatus(pokemon, (void *)pokemonGetStatus(outPokemon, 0, POKEMON_QUERY_FIGHT_SEQUENCE, 0));
                fightOutPokemonSetTokuseiDataId(outPokemon, pokemonGetTokuseiDataId(pokemon));
                for (i = 0; i < 2; i++) {
                    fightOutPokemonSetZokuseiDataId(outPokemon, i, pokemonGetStatus(0, species, POKEMON_QUERY_SPECIES_TYPE, i));
                }
            }
        }
    } else {
        result = dbgMenuPokemonEditSub(pokemon, 1, 0, 0, 0, 0);
    }
    if (figthPokemonGetLevel(fightPokemon) > level) pokemonSetStatus(fightPokemon, 0, POKEMON_STATUS_FIELD_LEVEL_UP_FLAG, 0, 1);
    fightMenuFightTrainerRenewStatusMenu(trainer, fightType);
    return result;
}
s32 dbgMenuFightFightTrainerPokemonPartDataEdit(void)
{
    if (fightIsFight() == 0) return -1;
    for (;;) {
        s32 id = dbgMenuFightGetFightTrainerPokemonPartDataId(_dbgmenu_fight_trainer_pokemon_part_data_id);
        if (id < 0) return 1;
        _dbgmenu_fight_trainer_pokemon_part_data_id = id;
        dbgMenuFightTrainerPokemonPartDataEditSub((u16)id);
    }
}
s32 dbgMenuFightFightTrainerAiDataEdit(void)
{
    u16 trainerID = fightTrainerGetStatus(_target_fight_trainer_ptr, 0, FIGHT_TRAINER_QUERY_DATA_ID, 0);
    u16 aiID = fightTrainerGetStatus(0, trainerID, FIGHT_TRAINER_QUERY_AI_DATA_ID, 0);
    if (fightIsFight() == 0) return -1;
    return dbgMenuFightTrainerAiDataEditSub(aiID);
}
s32 dbgMenuFightFightTrainerAiAddsubCombo(void)
{
    return dbgMenuFightTrainerAiAddsubDataComboEditSub();
}
s32 dbgMenuFightFightTrainerAiAddsubItem(void)
{
    return dbgMenuFightTrainerAiAddsubDataItemEditSub();
}
s32 dbgMenuFightFightTrainerAiAddsubWazaUsewaza(void)
{
    return dbgMenuFightTrainerAiAddsubDataWazaUsewazaEditSub();
}
s32 dbgMenuFightFightTrainerAiAddsubWazaDefpokemon(void)
{
    return dbgMenuFightTrainerAiAddsubDataWazaDefpokemonEditSub();
}
s32 dbgMenuFightFightTrainerAiAddsubWaza(void)
{
    return 1;
}
s32 dbgMenuFightFightTrainerAiAddsubIrekaeDasu(void)
{
    return dbgMenuFightTrainerAiAddsubDataIrekaeDasuEditSub();
}
s32 dbgMenuFightFightTrainerAiAddsubIrekaeModosu(void)
{
    return dbgMenuFightTrainerAiAddsubDataIrekaeModosuEditSub();
}
s32 dbgMenuFightFightTrainerAiAddsubIrekae(void)
{
    return 1;
}
s32 dbgMenuFightFightTrainerAiAddsubDataEdit(void)
{
    for (;;) {
        s32 id = dbgMenuFightGetFightTrainerAiAddsubValueDataId(_dbgmenu_fight_trainer_ai_addsub_data_id, 1);
        if (id < 0) return 1;
        _dbgmenu_fight_trainer_ai_addsub_data_id = id;
        dbgMenuFightTrainerDataStatusInputDigit((u16)id, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
    }
}
s32 dbgMenuFightFightTrainerSelect1(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_trainer_ptr = fightSideGetValidFightTrainerPtr(_target_fight_side_ptr, 1);
    if (_target_fight_trainer_ptr == 0) return -1;
    return 1;
}
s32 dbgMenuFightFightTrainerSelect0(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_trainer_ptr = fightSideGetValidFightTrainerPtr(_target_fight_side_ptr, 0);
    if (_target_fight_trainer_ptr == 0) return -1;
    return 1;
}
s32 dbgMenuFightFightSideSelectHostEnemy(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_side_ptr = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_HOST_ENEMY_SIDE, 0);
    if (_target_fight_side_ptr == 0) return -1;
    return 1;
}
s32 dbgMenuFightFightSideSelectHost(void)
{
    if (fightIsFight() == 0) return -1;
    _target_fight_side_ptr = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_HOST_SIDE, 0);
    if (_target_fight_side_ptr == 0) return -1;
    return 1;
}
s32 dbgMenuFightFightFloorEditTenkou(void)
{
    if (fightIsFight() == 0) return -1;
    return 1;
}
s32 dbgMenuFightWazaEdit(void)
{
    if (fightIsFight() == 0) return -1;
    for (;;) {
        s32 id = dbgMenuFightGetWazaDataId(_dbgmenu_fight_waza_edit_waza_data_id);
        if (id < 0) return 1;
        _dbgmenu_fight_waza_edit_waza_data_id = id;
        dbgMenuFightWazaEditSub((u16)id);
    }
}
/* Menu entry ids of the fight flag editor. */
enum {
    FIGHT_FLAG_EDIT_ITEM_TUIKAKOUKA = 114,
    FIGHT_FLAG_EDIT_ITEM_SOUBIKOUKA = 116,
    FIGHT_FLAG_EDIT_ITEM_MSG_SPEED = 118,
    FIGHT_FLAG_EDIT_ITEM_MSG_KEY_WAIT = 120,
    FIGHT_FLAG_EDIT_ITEM_EFFECT_ANIME = 122,
    FIGHT_FLAG_EDIT_ITEM_ALL_TRAINER_OPERATE = 124,
    FIGHT_FLAG_EDIT_ITEM_DEBUG_MENU_OPEN = 126,
    FIGHT_FLAG_EDIT_ITEM_AI_FLOW_DISPLAY = 128,
    FIGHT_FLAG_EDIT_ITEM_YUBIWO_HURU_WAZA = 130,
    FIGHT_FLAG_EDIT_ITEM_AI_LAST_VALUE_REVISE = 132,
    FIGHT_FLAG_EDIT_ITEM_POKE_ST_MENU_OPEN = 134
};

s32 dbgMenuFightFlagEdit(void)
{
    u8 msgSpeed;
    u8 tuikakouka;
    u8 msgKeyWait;
    u8 allOperate;
    u8 debugMenu;
    u8 effectAnime;
    u8 aiFlow;
    u8 soubikouka;
    u16 yubiwohuru;
    u8 aiLastValue;
    u32 input;
    s32 result;

    msgSpeed = dbgMenuFightGetMsgSpeed();
    tuikakouka = dbgMenuFightGetTuikakoukaFlag();
    msgKeyWait = dbgMenuFightGetMsgKeyWaitFlag();
    allOperate = dbgMenuFightGetAllFightTrainerOperateFlag();
    debugMenu = dbgMenuFightGetFightDebugMenuOpenFlag();
    effectAnime = fightSeqGetEffectAminFlag();
    aiFlow = dbgMenuFightGetFightTrainerAiFlowDispFlag();
    soubikouka = dbgMenuFightGetSoubikoukaFlag();
    yubiwohuru = dbgMenuFightGetYubiwohuruWazaDataId();
    aiLastValue = dbgMenuFightGetFightTrainerAiLastValueReviseFlag();
    dbgMenuFightGetFightPokeStMenuOpenFlag();
    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_FIGHT_FLAGS, 0, 0, 0, 1, 0, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_FIGHT_FLAGS, 0, 1);
            dbgMenuFightSetMsgSpeed(msgSpeed);
            dbgMenuFightSetTuikakoukaFlag(tuikakouka);
            dbgMenuFightSetMsgKeyWaitFlag(msgKeyWait);
            dbgMenuFightSetAllFightTrainerOperateFlag(allOperate);
            dbgMenuFightSetFightDebugMenuOpenFlag(debugMenu);
            fightSeqSetEffectAminFlag(effectAnime);
            dbgMenuFightSetFightTrainerAiFlowDispFlag(aiFlow);
            dbgMenuFightSetSoubikoukaFlag(soubikouka);
            dbgMenuFightSetYubiwohuruWazaDataId(yubiwohuru);
            dbgMenuFightSetFightTrainerAiLastValueReviseFlag(aiLastValue);
            return -1;
        }
        else if (result == -2) {
            if (menuOpen(MENU_ID_DEBUG_EDIT_CONFIRMATION, 1)) {
                menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
                continue;
            }
            menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
            break;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_TUIKAKOUKA) {
            result = dbgMenuFightGetTuikakoukaFlag();
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, result == 0);
            if (result == 0) {
                dbgMenuFightSetTuikakoukaFlag(1);
                continue;
            }
            if (result == 1) dbgMenuFightSetTuikakoukaFlag(0);
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_MSG_SPEED) {
            if (menuSubOpenNumberInput(dbgMenuFightGetMsgSpeed(), &input, 1, 50, 50, 0) == 0) {
                menuSubCloseNumberInput();
                continue;
            }
            dbgMenuFightSetMsgSpeed(input);
            menuSubCloseNumberInput();
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_MSG_KEY_WAIT) {
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, dbgMenuFightGetMsgKeyWaitFlag() == 0);
            if (result == 0) {
                dbgMenuFightSetMsgKeyWaitFlag(1);
                continue;
            }
            if (result == 1) dbgMenuFightSetMsgKeyWaitFlag(0);
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_EFFECT_ANIME) {
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, fightSeqGetEffectAminFlag() == 0);
            if (result == 0) {
                fightSeqSetEffectAminFlag(1);
                continue;
            }
            if (result == 1) fightSeqSetEffectAminFlag(0);
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_ALL_TRAINER_OPERATE) {
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, dbgMenuFightGetAllFightTrainerOperateFlag() == 0);
            if (result == 0) {
                dbgMenuFightSetAllFightTrainerOperateFlag(1);
                continue;
            }
            if (result == 1) dbgMenuFightSetAllFightTrainerOperateFlag(0);
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_DEBUG_MENU_OPEN) {
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, dbgMenuFightGetFightDebugMenuOpenFlag() == 0);
            if (result == 0) {
                dbgMenuFightSetFightDebugMenuOpenFlag(1);
                continue;
            }
            if (result == 1) dbgMenuFightSetFightDebugMenuOpenFlag(0);
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_AI_FLOW_DISPLAY) {
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, dbgMenuFightGetFightTrainerAiFlowDispFlag() == 0);
            if (result == 0) {
                dbgMenuFightSetFightTrainerAiFlowDispFlag(1);
                continue;
            }
            if (result == 1) dbgMenuFightSetFightTrainerAiFlowDispFlag(0);
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_SOUBIKOUKA) {
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, dbgMenuFightGetSoubikoukaFlag() == 0);
            if (result == 0) {
                dbgMenuFightSetSoubikoukaFlag(1);
                continue;
            }
            if (result == 1) dbgMenuFightSetSoubikoukaFlag(0);
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_YUBIWO_HURU_WAZA) {
            if (fightIsFight() == 0) continue;
            result = dbgMenuFightGetYubiwohuruWazaDataId();
            for (;;) {
                result = dbgMenuFightGetWazaDataId((u16)result);
                if (result < 0) break;
                if (fightSeqCheckYubiwohuruWazaDataId((u16)result) == 1) {
                    dbgMenuFightSetYubiwohuruWazaDataId((u16)result);
                    break;
                }
            }
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_AI_LAST_VALUE_REVISE) {
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, dbgMenuFightGetFightTrainerAiLastValueReviseFlag() == 0);
            if (result == 0) {
                dbgMenuFightSetFightTrainerAiLastValueReviseFlag(1);
                continue;
            }
            if (result == 1) dbgMenuFightSetFightTrainerAiLastValueReviseFlag(0);
            continue;
        }
        else if (result == FIGHT_FLAG_EDIT_ITEM_POKE_ST_MENU_OPEN) {
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, dbgMenuFightGetFightPokeStMenuOpenFlag() == 0);
            if (result == 0) {
                dbgMenuFightSetFightPokeStMenuOpenFlag(1);
                continue;
            }
            if (result == 1) dbgMenuFightSetFightPokeStMenuOpenFlag(0);
            continue;
        }
    }
    menuCloseCustom(MENU_ID_DEBUG_FIGHT_FLAGS, 0, 1);
    return 1;
}
s32 dbgMenuFightStop(void)
{
    if (fightIsFight() == 0) return -1;
    return 1;
}
s32 dbgMenuFightFightFloorEdit(void)
{
    if (fightIsFight() == 0) return -1;
    return 1;
}
s32 dbgMenuFightEncountSelect(void)
{
    s32 result;
    u16 i;
    u32 loadedDeck;
    u32 resourceLoaded = 0;
    u16 encounter;
    _readEncountDataName = (u32 *)GSmemAlloc(4608);
    _readEncountDataID = (u16 *)GSmemAlloc(1536);
    for (;;) {
        result = dbgMenuFightGetFightDeckDataId(_dbgmenu_fight_deck_data_id);
        if (result < 0) break;
        _dbgmenu_fight_deck_data_id = result;
        if (fightIsFight() == 0) {
            fightFreeResourceDeclArchive();
            GSfsysFreeCache(DECK_RESOURCE_GROUP);
            fightFreeResource(COMMON_RESOURCE_GROUP, DECK_RESOURCE_DARK_POKEMON);
            fightReadResource(DECK_RESOURCE_GROUP, DECK_RESOURCE_DARK_POKEMON);
            resourceLoaded = fightReadResource(DECK_RESOURCE_GROUP, getDeckResourceID((u8)result));
            if (resourceLoaded == 1) loadedDeck = result;
            _readEncountDataCnt = 0;
            for (i = 0; i < *fight_encount_data_number; i++) {
                if (result == fightEncountDataBiosGetFightDeckGrpId(fightEncountDataBiosGetPtr(i), 1)) {
                    _readEncountDataName[_readEncountDataCnt] = fightEncountDataBiosGetFightName(fightEncountDataBiosGetPtr(i));
                    _readEncountDataID[_readEncountDataCnt] = i;
                    _readEncountDataCnt++;
                }
            }
        }
        for (;;) {
            switch (loadedDeck) {
            case 1: encounter = _dbgmenu_fight_story_encount_data_id; break;
            case 2: encounter = _dbgmenu_fight_hundred_encount_data_id; break;
            case 3: encounter = _dbgmenu_fight_imasugu_encount_data_id; break;
            case 4: encounter = _dbgmenu_fight_virtual_encount_data_id; break;
            case 5: encounter = _dbgmenu_fight_bingo_encount_data_id; break;
            case 6: encounter = _dbgmenu_fight_colosseum_encount_data_id; break;
            case 7: encounter = _dbgmenu_fight_sample_encount_data_id; break;
            }
            if (encounter > _readEncountDataCnt) encounter = _readEncountDataCnt;
            result = dbgMenuFightGetFightEncountDataId(encounter);
            if (result < 0) break;
            switch (loadedDeck) {
            case 1: _dbgmenu_fight_story_encount_data_id = result; break;
            case 2: _dbgmenu_fight_hundred_encount_data_id = result; break;
            case 3: _dbgmenu_fight_imasugu_encount_data_id = result; break;
            case 4: _dbgmenu_fight_virtual_encount_data_id = result; break;
            case 5: _dbgmenu_fight_bingo_encount_data_id = result; break;
            case 6: _dbgmenu_fight_colosseum_encount_data_id = result; break;
            case 7: _dbgmenu_fight_sample_encount_data_id = result; break;
            }
            if (_readEncountDataID[result - 1] == 0) break;
            if (dbgMenuFightEncountDataEditSub(_readEncountDataID[result - 1]) < 0) continue;
            if (fightIsFight()) break;
            dbgMenuFightEncount(_readEncountDataID[result - 1]);
            GSmemFree(_readEncountDataName);
            GSmemFree(_readEncountDataID);
            if (resourceLoaded == 1)
                fightFreeResource(DECK_RESOURCE_GROUP, getDeckResourceID((u8)loadedDeck));
            return 0;
        }
    }
    if (resourceLoaded == 1)
        fightFreeResource(DECK_RESOURCE_GROUP, getDeckResourceID((u8)loadedDeck));
    GSmemFree(_readEncountDataName);
    GSmemFree(_readEncountDataID);
    return -1;
}
s32 dbgMenuFightEncount(u16 encount)
{
    GSthread *thread = GSthreadCreate(1, GSfloorGetCurrentFloorGroupID(), 16384, 1, 1, (GSthreadEntry)fightEncount);
    if (thread != 0) {
        GSthreadSetArgs(thread, 1, encount);
        GSthreadSetName(thread, "fightEnc(Debug)");
    }
    return 0;
}
s32 dbgMenuFightMain(void) { return 1; }
void dbgMenuFightSetYubiwohuruWazaDataId(u16 id) { _yubiwohuru_waza_data_id = id; }
void dbgMenuFightSetSoubikoukaFlag(u8 value) { _dbgMenuFightSoubikoukaFlag = value; }
u16 dbgMenuFightGetYubiwohuruWazaDataId(void) { return _yubiwohuru_waza_data_id; }
u8 dbgMenuFightGetSoubikoukaFlag(void) { return _dbgMenuFightSoubikoukaFlag; }
void dbgMenuFightSetFightPokeStMenuOpenFlag(u8 value) { _dbgMenuFightPokeStMenuOpenFlag = value; }
u8 dbgMenuFightGetFightPokeStMenuOpenFlag(void) { return _dbgMenuFightPokeStMenuOpenFlag; }
void dbgMenuFightSetFightTrainerAiLastValueReviseFlag(u8 value) { _dbgMenuFightTrainerAiLastValueReviseFlag = value; }
u8 dbgMenuFightGetFightTrainerAiLastValueReviseFlag(void) { return _dbgMenuFightTrainerAiLastValueReviseFlag; }
void dbgMenuFightSetFightTrainerAiFlowDispFlag(u8 value) { _dbgMenuFightTrainerAiFlowDispFlag = value; }
u8 dbgMenuFightGetFightTrainerAiFlowDispFlag(void) { return _dbgMenuFightTrainerAiFlowDispFlag; }
void dbgMenuFightSetFightDebugMenuOpenFlag(u8 value) { _dbgMenuFightDebugMenuOpenFlag = value; }
u8 dbgMenuFightGetFightDebugMenuOpenFlag(void) { return _dbgMenuFightDebugMenuOpenFlag; }
void dbgMenuFightSetAllFightTrainerOperateFlag(u8 value) { _dbgMenuFightAllFightTrainerOperateFlag = value; }
u8 dbgMenuFightGetAllFightTrainerOperateFlag(void) { return _dbgMenuFightAllFightTrainerOperateFlag; }
void dbgMenuFightSetMsgKeyWaitFlag(u8 value) { _dbgMenuFightMsgKeyWaitFlag = value; }
u8 dbgMenuFightGetMsgKeyWaitFlag(void) { return _dbgMenuFightMsgKeyWaitFlag; }
void dbgMenuFightSetTuikakoukaFlag(u8 value) { _dbgMenuFightTuikakoukaFlag = value; }
u8 dbgMenuFightGetTuikakoukaFlag(void) { return _dbgMenuFightTuikakoukaFlag; }
void dbgMenuFightSetMsgSpeed(u8 speed)
{
    if (speed < 1) speed = 1;
    if (speed > 9) speed = 9;
    _dbgMenuFightMsgSpeed = speed;
}


u8 dbgMenuFightGetMsgSpeed(void) { return _dbgMenuFightMsgSpeed; }
u32 dbgMenuFightGetMsgSpeedToFrame(u32 frame)
{
    u8 speed;
    u8 percent[] = { 0, 10, 25, 50, 75, 100, 125, 150, 175, 200 };
    speed = dbgMenuFightGetMsgSpeed();
    if (winMsgCloseCheckFight() == 1) {
        frame = frame * percent[speed] / 100;
        if (dbgMenuFightGetMsgKeyWaitFlag() == 1) {
            frame = 0;
            for (;;) {
                if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & GSinputButtonsChanged(GS_INPUT_PORT_1) & 0x100) != 0) break;
                GSthreadSwitch();
            }
        }
    }
    return frame;
}
s32 dbgMenuFightGetFightTrainerAiAddsubValueDataId(u16 id, u8 close)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub) == 0) return -1;
    if (value > *fight_trainer_ai_value_addsub_data_number) value = *fight_trainer_ai_value_addsub_data_number - 1;
    if (close == 1) menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub(u32 id)
{
    u32 name;
    u32 comment;
    u32 category;
    u32 value;
    if (id >= *fight_trainer_ai_value_addsub_data_number) {
        value = 0;
        name = 0xeb63;
        comment = name;
        category = name;
    } else {
        name = fightTrainerGetStatus(0, id, 0x40, 0);
        comment = fightTrainerGetStatus(0, id, 0x41, 0);
        category = fightTrainerGetStatus(0, id, 0x3f, 0);
        value = fightTrainerGetStatus(0, id, 0x3e, 0);
    }
    if (category == 0) category = 0xeb63;
    if (name == 0) name = 0xeb63;
    if (comment == 0) comment = 0xeb63;
    msgctrlSetValue(0xd, (u32)GSmsgGetStringPtr(name));
    msgctrlSetValue(0xe, (u32)GSmsgGetStringPtr(comment));
    msgctrlSetValue(0x4d, (u32)GSmsgGetStringPtr(category));
    msgctrlSetValue(0x2f, value);
    return GSmsgGetStringPtr(0xf159);
}
s32 dbgMenuFightGetFightTrainerPokemonPartDataId(u16 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetFightTrainerPokemonPartDataIdSub) == 0) return -1;
    if (value >= *fight_trainer_pokemon_part_data_number) value = *fight_trainer_pokemon_part_data_number - 1;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetFightTrainerPokemonPartDataIdSub(u32 id)
{
    if (id == 0) return GSmsgGetStringPtr(0xeb63);
    if (id >= *fight_trainer_pokemon_part_data_number) return GSmsgGetStringPtr(0xeb63);
    return GSmsgGetStringPtr(fightTrainerGetStatus(0, id, 0xb, 0));
}
s32 dbgMenuFightGetWazaDataId(u16 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetWazaDataIdSub) == 0) return -1;
    if (value >= 355) value = 354;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetWazaDataIdSub(u32 id)
{
    if (id == 0) return GSmsgGetStringPtr(0xeb63);
    if (id >= 355) return GSmsgGetStringPtr(0xeb63);
    return GSmsgGetStringPtr(wazaGetStatus(0, id, 1, 0));
}
s32 dbgMenuFightGetWazaTypeId(u8 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetWazaTypeIdSub) == 0) return -1;
    if (value > 13) value = 13;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetWazaTypeIdSub(u32 id)
{
    if (id == 0) return GSmsgGetStringPtr(0xeb63);
    if (id > 13) return GSmsgGetStringPtr(0xeb63);
    return GSmsgGetStringPtr(wazaGetWazaTypeIdName(id));
}
s32 dbgMenuFightGetZokuseiDataId(u16 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetZokuseiDataIdSub) == 0) return -1;
    if (value >= *zokusei_data_number) value = *zokusei_data_number - 1;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetZokuseiDataIdSub(u32 id)
{
    if (id == 9) return GSmsgGetStringPtr(0xeb63);
    if (id >= *zokusei_data_number) return GSmsgGetStringPtr(0xeb63);
    return GSmsgGetStringPtr(zokuseiBiosGetName(id));
}
s32 dbgMenuFightGetGsinputDevId(u16 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetGsinputDevIdSub) == 0) return -1;
    if (value >= 5) value = 4;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetGsinputDevIdSub(u32 id)
{
    u32 message;
    if ((s32)id >= 5) message = 0xeb63;
    else message = dbgMenuGetGsinputMsgId(id);
    if (message == 0) message = 0xeb63;
    return GSmsgGetStringPtr(message);
}
s32 dbgMenuFightGetFightTrainerDataId(u16 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetFightTrainerDataIdSub) == 0) return -1;
    if (value > _readEncountDataCnt && value < 5000) value = _readEncountDataCnt;
    if (value > 5006) value = 5006;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetFightTrainerDataIdSub(u32 id)
{
    u32 message;
    if (id >= deckGetTrainerCount()) message = 0xeb63;
    else message = fightTrainerGetStatus(0, id, 3, 0);
    if (message == 0) message = 0xeb63;
    switch (id) {
    case 5000: message = 0xed00; break;
    case 5001: message = 0xf60a; break;
    case 5002: message = 0xf60b; break;
    case 5003: message = 0xf60c; break;
    case 5004: message = 0xf60d; break;
    case 5005: message = 0xf60e; break;
    case 5006: message = 0xf60f; break;
    }
    return GSmsgGetStringPtr(message);
}
s32 dbgMenuFightGetPokemonEntry(u8 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetPokemonEntrySub) == 0) return -1;
    if (value > 6) value = 6;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetPokemonEntrySub(u32 id)
{
    u32 message;
    if (id > 6) message = 0xeb63;
    else {
        message = id;
        switch (id) {
        case 1: message = 0xf604; break;
        case 2: message = 0xf605; break;
        case 3: message = 0xf606; break;
        case 4: message = 0xf607; break;
        case 5: message = 0xf608; break;
        case 6: message = 0xf609; break;
        }
    }
    if (message == 0) message = 0xeb63;
    return GSmsgGetStringPtr(message);
}
s32 dbgMenuFightGetPokemonFightType(u8 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetPokemonFightTypeSub) == 0) return -1;
    if (value > 2) value = 2;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetPokemonFightTypeSub(u32 id)
{
    u32 message;
    if (id > 2) message = 0xeb63;
    else {
        message = id;
        switch (id) {
        case 1: message = 0xf602; break;
        case 2: message = 0xf603; break;
        }
    }
    if (message == 0) message = 0xeb63;
    return GSmsgGetStringPtr(message);
}
s32 dbgMenuFightGetFightTypeDataId(u8 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetFightTypeDataIdSub) == 0) return -1;
    if (value > 2) value = 2;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetFightTypeDataIdSub(u32 id)
{
    u32 message;
    if (id > 2) message = 0xeb63;
    else {
        message = id;
        switch (id) {
        case 0: message = 0xf5ff; break;
        case 1: message = 0xf600; break;
        case 2: message = 0xf601; break;
        }
    }
    if (message == 0) message = 0xeb63;
    return GSmsgGetStringPtr(message);
}
s32 dbgMenuFightGetFightKindDataId(u8 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetFightKindDataIdSub) == 0) return -1;
    if (value >= *fight_kind_data_number) value = *fight_kind_data_number - 1;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetFightKindDataIdSub(u32 id)
{
    u32 message;
    if (id >= *fight_kind_data_number) message = 0xeb63;
    else message = fightKindDataBiosGetName(fightKindDataBiosGetPtr(id));
    if (message == 0) message = 0xeb63;
    return GSmsgGetStringPtr(message);
}
s32 dbgMenuFightGetFightFloorDataId(u16 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetFightFloorDataIdSub) == 0) return -1;
    if (value >= *fight_floor_data_number) value = *fight_floor_data_number - 1;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetFightFloorDataIdSub(u32 id)
{
    u32 message;
    if (id >= *fight_floor_data_number) message = 0xeb63;
    else message = fightFloorGetStatus(0, id, 1, 0);
    if (message == 0) message = 0xeb63;
    return GSmsgGetStringPtr(message);
}
s32 dbgMenuFightGetFightDeckDataId(u16 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetFightDeckDataIdSub) == 0) return -1;
    if (value >= deck_group_data_number) value = deck_group_data_number - 1;
    if (value == 0) value = 1;
    menuSubCloseNumberInput();
    return value;
}
u16 *_dbgMenuFightGetFightDeckDataIdSub(u32 id)
{
    u32 message;
    if (id == 0) message = 0xeb63;
    else if (id >= deck_group_data_number) message = 0xeb63;
    else message = deck_group_data[id];
    if (message == 0) message = 0xeb63;
    return GSmsgGetStringPtr(message);
}
s32 dbgMenuFightGetFightEncountDataId(u16 id)
{
    u32 value;
    if (menuSubOpenNumberInputID(id, &value, _dbgMenuFightGetFightEncountDataIdSub) == 0) return -1;
    if (value > _readEncountDataCnt) value = _readEncountDataCnt;
    if (value == 0) value = 1;
    menuSubCloseNumberInput();
    return value;
}
#pragma pool_strings off

u16 *_dbgMenuFightGetFightEncountDataIdSub(u32 id)
{
    static u16 strbuf[64];
    u16 suffix[2];
    u32 message;
    char *deckMessage;
    if (id >= 1 && id <= _readEncountDataCnt) message = _readEncountDataName[id - 1];
    else message = 0xeb63;
    if (message == 0) message = 0xeb63;
    if (id <= _readEncountDataCnt) {
        deckMessage = getDeckMsgID(_readEncountDataID[id - 1], 1);
        GScharMakeFromSJIS(strbuf, "[");
        GScharMakeFromSJIS(strbuf + 1, deckMessage);
        GScharMakeFromSJIS(suffix, "]");
        GScharCat(strbuf, suffix);
    } else strbuf[0] = 0;
    return GScharCat(strbuf, GSmsgGetStringPtr(message));
}



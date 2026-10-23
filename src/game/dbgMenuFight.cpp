#include <menu.h>
#include <fightTrainer.h>
#include <fightEncountData.h>
#include <fightEncountBios.h>
#include <types.h>

/* Debug fight menu: the fight-trainer dialogue ("serihu") tests and the
 * encounter data editor. The menu engine itself is C, so the menu entry
 * points below keep C linkage. */

/* The debug menu reaches these entry points through its dispatch tables, so
 * they must survive the linker even when nothing references them. */
#pragma force_active on

extern "C" {
s32 dbgMenuFightFightTrainerSerihuTestHaiboku2(void);
s32 dbgMenuFightFightTrainerSerihuTestHaiboku(void);
s32 dbgMenuFightFightTrainerSerihuTestSyouri(void);
s32 dbgMenuFightFightTrainerSerihuTestToujou(void);
s32 _dbgMenuFightFightTrainerSerihuTestSub(u16 trainerDataId, u8 kind);
s32 dbgMenuFightFightTrainerSerihuTest(void);
s32 dbgMenuFightEncountDataEditSub(u16 encountNo);
s32 dbgMenuGetGsinputMsgId(s32 id);

extern u16 dbgFightDialogueTrainerDataId;   /* trainer data id the dialogue tests play */
extern u32* fightEncounterRecordCount;  /* encounter record count */

extern void winMsgOpenFight(s32 status, s32 a, s32 b);
extern void winMsgWaitForConfirm(s32 a);
extern s32 dbgMenuFightGetFightTrainerDataId(u16 trainerDataId);
extern s32 dbgMenuFightGetFightKindDataId(u8 kind);
extern s32 dbgMenuFightGetFightTypeDataId(u8 type);
extern s32 dbgMenuFightGetPokemonEntry(u8 entry);
extern s32 dbgMenuFightGetGsinputDevId(u16 devId);
extern s32 dbgMenuFightGetTrainersPerSide(u8 value);
extern s32 dbgMenuFightGetBattleField(u16 value);
extern u32 fightEncountDataBiosGetBattleKind(void* data);
extern void fightEncountDataBiosSetBattleKind(void* data, u8 value);
extern u32 fightEncountDataBiosGetTrainersPerSide(void* data);
extern void fightEncountDataBiosSetTrainersPerSide(void* data, u8 value);
extern u32 fightEncountDataBiosGetPokemon(void* data);
extern void fightEncountDataBiosSetBattleStyle(void* data, u8 value);
extern u32 fightEncountDataBiosGetPokemonPerPlayer(void* data);
extern void fightEncountDataBiosSetPokemonPerPlayer(void* data, u8 value);
extern u32 fightEncountDataBiosGetBattleField(void* data);
extern void fightEncountDataBiosSetBattleField(void* data, u16 value);
extern u32 fightEncountDataBiosGetPlayerController(void* data, s32 player);
extern void fightEncountDataBiosSetPlayerController(void* data, s32 player, s32 devId);
extern s32 menuGetCursorItemID(s32 menuId);
}

/* Head of the encounter record; the editor puts it back when the cancel
 * button closes the window. */
typedef struct {
    u32 mField[15];
} EncountBackup;

/* Menu entry ids of the encounter editor. */
enum {
    ENCOUNT_EDIT_ITEM_KIND = 0x10A,
    ENCOUNT_EDIT_ITEM_TRAINERS_PER_SIDE,
    ENCOUNT_EDIT_ITEM_STYLE,
    ENCOUNT_EDIT_ITEM_POKEMON_PER_PLAYER,
    ENCOUNT_EDIT_ITEM_FIELD,
    ENCOUNT_EDIT_ITEM_TRAINER_DATA_0,
    ENCOUNT_EDIT_ITEM_TRAINER_DATA_1,
    ENCOUNT_EDIT_ITEM_TRAINER_DATA_2,
    ENCOUNT_EDIT_ITEM_TRAINER_DATA_3
};

#pragma peephole off

/* Dialogue test entry points; kind 0 is the scene where the trainer shows up,
 * 1 the victory line, 2 and 3 the two defeat lines. */
s32 dbgMenuFightFightTrainerSerihuTestHaiboku2(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(dbgFightDialogueTrainerDataId, 3);
}

s32 dbgMenuFightFightTrainerSerihuTestHaiboku(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(dbgFightDialogueTrainerDataId, 2);
}

s32 dbgMenuFightFightTrainerSerihuTestSyouri(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(dbgFightDialogueTrainerDataId, 1);
}

s32 dbgMenuFightFightTrainerSerihuTestToujou(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(dbgFightDialogueTrainerDataId, 0);
}

/* Play one dialogue scene and wait for the message to be read. */
s32 _dbgMenuFightFightTrainerSerihuTestSub(u16 trainerDataId, u8 kind)
{
    u32 status = fightTrainerGetStatus(0, trainerDataId, 8, kind);

    if (status == 0) {
        return -1;
    }
    winMsgOpenFight(status, 1, 1);
    winMsgWaitForConfirm(1);
    return 1;
}

/* Take the trainer data id the dialogue tests are played with. */
s32 dbgMenuFightFightTrainerSerihuTest(void)
{
    s32 id = dbgMenuFightGetFightTrainerDataId(dbgFightDialogueTrainerDataId);

    if (id < 0) {
        return -1;
    }
    dbgFightDialogueTrainerDataId = id;
    return 1;
}

/* Encounter data editor: opens the editor on the given encounter record,
 * edits one field per menu entry and puts the record back when it is
 * cancelled. */
s32 dbgMenuFightEncountDataEditSub(u16 encountNo)
{
    EncountBackup backup;
    u8* data;
    s32 result;
    s32 id;

    if (encountNo == 0) {
        return 1;
    }
    if (encountNo >= *fightEncounterRecordCount) {
        return 1;
    }
    data = fightEncountDataBiosGetPtr(encountNo);
    backup = *(EncountBackup*)data;

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_ENCOUNTER_EDITOR, 0, 0, 0, 1, 1, encountNo);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_ENCOUNTER_EDITOR, 0, 1);
            *(EncountBackup*)data = backup;
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
            id = dbgMenuFightGetFightKindDataId(fightEncountDataBiosGetBattleKind(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetBattleKind(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINERS_PER_SIDE:
            id = dbgMenuFightGetTrainersPerSide(fightEncountDataBiosGetTrainersPerSide(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetTrainersPerSide(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_STYLE:
            id = dbgMenuFightGetFightTypeDataId(fightEncountDataBiosGetPokemon(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetBattleStyle(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_POKEMON_PER_PLAYER:
            id = dbgMenuFightGetPokemonEntry(fightEncountDataBiosGetPokemonPerPlayer(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetPokemonPerPlayer(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_FIELD:
            id = dbgMenuFightGetBattleField(fightEncountDataBiosGetBattleField(data));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetBattleField(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_0:
            id = fightEncountDataBiosGetFightTrainerDataId(data, 0);
            id = dbgMenuFightGetFightTrainerDataId(id);
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 0, id);
            id = dbgMenuFightGetGsinputDevId(fightEncountDataBiosGetPlayerController(data, 0));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetPlayerController(data, 0, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_1:
            id = fightEncountDataBiosGetFightTrainerDataId(data, 1);
            id = dbgMenuFightGetFightTrainerDataId(id);
            /* The second player refuses data id 0; kept as in the original. */
            if (id < 1) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 1, id);
            id = dbgMenuFightGetGsinputDevId(fightEncountDataBiosGetPlayerController(data, 1));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetPlayerController(data, 1, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_2:
            id = fightEncountDataBiosGetFightTrainerDataId(data, 2);
            id = dbgMenuFightGetFightTrainerDataId(id);
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 2, id);
            id = dbgMenuFightGetGsinputDevId(fightEncountDataBiosGetPlayerController(data, 2));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetPlayerController(data, 2, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_3:
            id = fightEncountDataBiosGetFightTrainerDataId(data, 3);
            id = dbgMenuFightGetFightTrainerDataId(id);
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 3, id);
            id = dbgMenuFightGetGsinputDevId(fightEncountDataBiosGetPlayerController(data, 3));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetPlayerController(data, 3, id);
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
s32 dbgMenuGetGsinputMsgId(s32 id)
{
    s32 msg;

    if (id == 0) {
        msg = 0xED04;
    } else if (id == 1) {
        msg = 0xED05;
    } else if (id == 2) {
        msg = 0xED06;
    } else if (id == 3) {
        msg = 0xED07;
    } else if (id == 4) {
        msg = 0xED08;
    } else {
        msg = 0xED04;
    }
    return msg;
}

#pragma peephole reset

#pragma force_active reset

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
s32 _dbgMenuFightFightTrainerSerihuTestSub(u16 trainerDataId, u32 kind);
s32 dbgMenuFightFightTrainerSerihuTest(void);
s32 dbgMenuFightEncountDataEditSub(u16 encountNo);
s32 dbgMenuGetGsinputMsgId(s32 id);

extern u16 lbl_804EA658;   /* trainer data id the dialogue tests play */
extern u32* lbl_804E8980;  /* encounter record count */

extern u32 fightTrainerGetStatus(s32 trainerNo, u16 trainerDataId, s32 mode, u8 kind);
extern void winMsgOpenFight(s32 status, s32 a, s32 b);
extern void zz_message_wait_input_a(s32 a);
extern s32 dbgMenuFightGetFightTrainerDataId(u16 trainerDataId);
extern s32 dbgMenuFightGetFightKindDataId(u8 kind);
extern s32 dbgMenuFightGetFightTypeDataId(u8 type);
extern s32 dbgMenuFightGetPokemonEntry(u8 entry);
extern s32 dbgMenuFightGetGsinputDevId(u16 devId);
extern s32 zz_0007b80_(u8 value);
extern s32 zz_0007d28_(u16 value);
extern u32 zz_battle_get_battle_type(void* data);
extern void zz_battle_set_byte_0x00(void* data, u8 value);
extern u32 zz_battle_get_trainers_per_side(void* data);
extern void zz_battle_set_trainers_per_side(void* data, u8 value);
extern u32 fightEncountDataBiosGetPokemon(void* data);
extern void zz_battle_set_battle_style(void* data, u8 value);
extern u32 zz_battle_get_pokemon_per_player(void* data);
extern void zz_battle_set_pokemon_per_player(void* data, u8 value);
extern u32 zz_battle_get_battle_field(void* data);
extern void zz_battle_set_battle_field(void* data, u16 value);
extern u32 fightEncountDataBiosGetFightTrainerDataId(void* data, s32 player);
extern void fightEncountDataBiosSetFightTrainerDataId(void* data, s32 player, u16 trainerDataId);
extern u32 zz_battle_player_get_controller(void* data, s32 player);
extern void zz_battle_set_controller_for_player_with_index(void* data, s32 player, s32 devId);
extern s32 menuOpenCustom(s32 menuId, ...);
extern void menuCloseCustom(s32 menuId, s32 a, s32 b);
extern s32 menuOpen(s32 menuId, s32 a);
extern s32 menuGetCursorItemID(s32 menuId);
}

/* Encounter record accessor. It is a C++ unit, so the name is not mangled
 * here by hand. */
extern u8* deckDataVirtual_GetPtr(u16 encountNo);

/* Head of the encounter record; the editor puts it back when the cancel
 * button closes the window. */
typedef struct {
    u32 mField[15];
} EncountBackup;

/* Menu entry ids of the encounter editor (window 0x25). */
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
    return _dbgMenuFightFightTrainerSerihuTestSub(lbl_804EA658, 3);
}

s32 dbgMenuFightFightTrainerSerihuTestHaiboku(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(lbl_804EA658, 2);
}

s32 dbgMenuFightFightTrainerSerihuTestSyouri(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(lbl_804EA658, 1);
}

s32 dbgMenuFightFightTrainerSerihuTestToujou(void)
{
    return _dbgMenuFightFightTrainerSerihuTestSub(lbl_804EA658, 0);
}

/* Play one dialogue scene and wait for the message to be read. */
s32 _dbgMenuFightFightTrainerSerihuTestSub(u16 trainerDataId, u32 kind)
{
    u32 status = fightTrainerGetStatus(0, trainerDataId, 8, kind);

    if (status == 0) {
        return -1;
    }
    winMsgOpenFight(status, 1, 1);
    zz_message_wait_input_a(1);
    return 1;
}

/* Take the trainer data id the dialogue tests are played with. */
s32 dbgMenuFightFightTrainerSerihuTest(void)
{
    s32 id = dbgMenuFightGetFightTrainerDataId(lbl_804EA658);

    if (id < 0) {
        return -1;
    }
    lbl_804EA658 = id;
    return 1;
}

/* Encounter data editor: opens window 0x25 on the given encounter record,
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
    if (encountNo >= *lbl_804E8980) {
        return 1;
    }
    data = deckDataVirtual_GetPtr(encountNo);
    backup = *(EncountBackup*)data;

    for (;;) {
        result = menuOpenCustom(0x25, 0, 0, 0, 1, 1, encountNo);
        if (result == -1) {
            menuCloseCustom(0x25, 0, 1);
            *(EncountBackup*)data = backup;
            return -1;
        }
        if (result == -2) {
            if (menuOpen(0x55, 1) != 0) {
                menuCloseCustom(0x55, 0, 1);
                continue;
            }
            menuCloseCustom(0x55, 0, 1);
            break;
        }
        switch (menuGetCursorItemID(0x25)) {
        case ENCOUNT_EDIT_ITEM_KIND:
            id = dbgMenuFightGetFightKindDataId(zz_battle_get_battle_type(data));
            if (id < 0) {
                continue;
            }
            zz_battle_set_byte_0x00(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINERS_PER_SIDE:
            id = zz_0007b80_(zz_battle_get_trainers_per_side(data));
            if (id < 0) {
                continue;
            }
            zz_battle_set_trainers_per_side(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_STYLE:
            id = dbgMenuFightGetFightTypeDataId(fightEncountDataBiosGetPokemon(data));
            if (id < 0) {
                continue;
            }
            zz_battle_set_battle_style(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_POKEMON_PER_PLAYER:
            id = dbgMenuFightGetPokemonEntry(zz_battle_get_pokemon_per_player(data));
            if (id < 0) {
                continue;
            }
            zz_battle_set_pokemon_per_player(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_FIELD:
            id = zz_0007d28_(zz_battle_get_battle_field(data));
            if (id < 0) {
                continue;
            }
            zz_battle_set_battle_field(data, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_0:
            id = dbgMenuFightGetFightTrainerDataId(fightEncountDataBiosGetFightTrainerDataId(data, 0));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 0, id);
            id = dbgMenuFightGetGsinputDevId(zz_battle_player_get_controller(data, 0));
            if (id < 0) {
                continue;
            }
            zz_battle_set_controller_for_player_with_index(data, 0, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_1:
            id = dbgMenuFightGetFightTrainerDataId(fightEncountDataBiosGetFightTrainerDataId(data, 1));
            /* The second player refuses data id 0; kept as in the original. */
            if (id < 1) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 1, id);
            id = dbgMenuFightGetGsinputDevId(zz_battle_player_get_controller(data, 1));
            if (id < 0) {
                continue;
            }
            zz_battle_set_controller_for_player_with_index(data, 1, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_2:
            id = dbgMenuFightGetFightTrainerDataId(fightEncountDataBiosGetFightTrainerDataId(data, 2));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 2, id);
            id = dbgMenuFightGetGsinputDevId(zz_battle_player_get_controller(data, 2));
            if (id < 0) {
                continue;
            }
            zz_battle_set_controller_for_player_with_index(data, 2, id);
            continue;
        case ENCOUNT_EDIT_ITEM_TRAINER_DATA_3:
            id = dbgMenuFightGetFightTrainerDataId(fightEncountDataBiosGetFightTrainerDataId(data, 3));
            if (id < 0) {
                continue;
            }
            fightEncountDataBiosSetFightTrainerDataId(data, 3, id);
            id = dbgMenuFightGetGsinputDevId(zz_battle_player_get_controller(data, 3));
            if (id < 0) {
                continue;
            }
            zz_battle_set_controller_for_player_with_index(data, 3, id);
            continue;
        default:
            continue;
        }
    }
    menuCloseCustom(0x25, 0, 1);
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

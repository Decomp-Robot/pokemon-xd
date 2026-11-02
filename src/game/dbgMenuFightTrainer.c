#include <dbgMenuFight.h>
#include <fightTrainer.h>
#include <fightTrainerStatus.h>
#include <menu.h>
#include <menuIds.h>
#include <menuSub.h>
#include <types.h>

/* Trainer data editors of the fight debug menu.  The AI additional-value sub
 * menus work on the whole additional-value table and keep a copy of it in
 * _old_fight_trainer_ai_value_addsub_data so the table can be restored when
 * the user cancels; the pokemon-part and AI-data editors do the same for the
 * one record they edit. */

#define FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX 727

extern FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA _old_fight_trainer_ai_value_addsub_data[FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX];

#pragma peephole off

s32 dbgMenuFightTrainerAiAddsubDataComboEditSub(void)
{
    u16 i;
    s32 result;

    for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_COMBO_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_COMBO_EDITOR, 0, 1);
            for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
                FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
                *subData = _old_fight_trainer_ai_value_addsub_data[i];
            }
            return -1;
        }
        if (result != -2) {
            continue;
        }
        if (menuOpen(MENU_ID_DEBUG_EDIT_CONFIRMATION, 1)) {
            menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
            continue;
        }
        menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
        menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_COMBO_EDITOR, 0, 1);
        return 1;
    }
}

s32 dbgMenuFightTrainerAiAddsubDataItemEditSub(void)
{
    u16 i;
    s32 result;

    for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_ITEM_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_ITEM_EDITOR, 0, 1);
            for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
                FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
                *subData = _old_fight_trainer_ai_value_addsub_data[i];
            }
            return -1;
        }
        if (result != -2) {
            continue;
        }
        if (menuOpen(MENU_ID_DEBUG_EDIT_CONFIRMATION, 1)) {
            menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
            continue;
        }
        menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
        menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_ITEM_EDITOR, 0, 1);
        return 1;
    }
}

s32 dbgMenuFightTrainerAiAddsubDataWazaUsewazaEditSub(void)
{
    u16 i;
    s32 result;

    for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_USE_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_USE_EDITOR, 0, 1);
            for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
                FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
                *subData = _old_fight_trainer_ai_value_addsub_data[i];
            }
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
        switch (menuGetCursorItemID(MENU_ID_DEBUG_TRAINER_AI_MOVE_USE_EDITOR)) {
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_56:
            dbgMenuFightTrainerDataStatusInputDigit(56, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_57:
            dbgMenuFightTrainerDataStatusInputDigit(57, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_58:
            dbgMenuFightTrainerDataStatusInputDigit(58, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_59:
            dbgMenuFightTrainerDataStatusInputDigit(59, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_60:
            dbgMenuFightTrainerDataStatusInputDigit(60, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_64:
            dbgMenuFightTrainerDataStatusInputDigit(64, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_68:
            dbgMenuFightTrainerDataStatusInputDigit(68, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_69:
            dbgMenuFightTrainerDataStatusInputDigit(69, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_71:
            dbgMenuFightTrainerDataStatusInputDigit(71, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_72:
            dbgMenuFightTrainerDataStatusInputDigit(72, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_70:
            dbgMenuFightTrainerDataStatusInputDigit(70, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_65:
            dbgMenuFightTrainerDataStatusInputDigit(65, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_66:
            dbgMenuFightTrainerDataStatusInputDigit(66, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_67:
            dbgMenuFightTrainerDataStatusInputDigit(67, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_61:
            dbgMenuFightTrainerDataStatusInputDigit(61, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_62:
            dbgMenuFightTrainerDataStatusInputDigit(62, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_MOVE_USE_ADDSUB_63:
            dbgMenuFightTrainerDataStatusInputDigit(63, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        }
    }
    menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_USE_EDITOR, 0, 1);
    return 1;
}

s32 dbgMenuFightTrainerAiAddsubDataWazaDefpokemonEditSub(void)
{
    u16 i;
    s32 result;

    for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR, 0, 1);
            for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
                FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
                *subData = _old_fight_trainer_ai_value_addsub_data[i];
            }
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
        /* The move-target editor has no entries that can be edited here. */
        switch (menuGetCursorItemID(MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR)) {
        }
    }
    menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR, 0, 1);
    return 1;
}

s32 dbgMenuFightTrainerAiAddsubDataIrekaeDasuEditSub(void)
{
    s32 result;
    u16 i;

    for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    while (1) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_IN_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_IN_EDITOR, 0, 1);
            for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
                FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
                *subData = _old_fight_trainer_ai_value_addsub_data[i];
            }
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
        switch (menuGetCursorItemID(MENU_ID_DEBUG_TRAINER_AI_SWITCH_IN_EDITOR)) {
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_29:
            dbgMenuFightTrainerDataStatusInputDigit(29, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_30:
            dbgMenuFightTrainerDataStatusInputDigit(30, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_31:
            dbgMenuFightTrainerDataStatusInputDigit(31, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_33:
            dbgMenuFightTrainerDataStatusInputDigit(33, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_34:
            dbgMenuFightTrainerDataStatusInputDigit(34, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_35:
            dbgMenuFightTrainerDataStatusInputDigit(35, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_36:
            dbgMenuFightTrainerDataStatusInputDigit(36, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_37:
            dbgMenuFightTrainerDataStatusInputDigit(37, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_38:
            dbgMenuFightTrainerDataStatusInputDigit(38, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_39:
            dbgMenuFightTrainerDataStatusInputDigit(39, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_32:
            dbgMenuFightTrainerDataStatusInputDigit(32, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_48:
            dbgMenuFightTrainerDataStatusInputDigit(48, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_40:
            dbgMenuFightTrainerDataStatusInputDigit(40, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_41:
            dbgMenuFightTrainerDataStatusInputDigit(41, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_42:
            dbgMenuFightTrainerDataStatusInputDigit(42, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_IN_ADDSUB_43:
            dbgMenuFightTrainerDataStatusInputDigit(43, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        }
    }

    menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_IN_EDITOR, 0, 1);
    return 1;
}

s32 dbgMenuFightTrainerAiAddsubDataIrekaeModosuEditSub(void)
{
    s32 result;
    u16 i;

    for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    while (1) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_OUT_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_OUT_EDITOR, 0, 1);
            for (i = 0; i < FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA_MAX; i++) {
                FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
                *subData = _old_fight_trainer_ai_value_addsub_data[i];
            }
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
        switch (menuGetCursorItemID(MENU_ID_DEBUG_TRAINER_AI_SWITCH_OUT_EDITOR)) {
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_1:
            dbgMenuFightTrainerDataStatusInputDigit(1, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_2:
            dbgMenuFightTrainerDataStatusInputDigit(2, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_3:
            dbgMenuFightTrainerDataStatusInputDigit(3, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_4:
            dbgMenuFightTrainerDataStatusInputDigit(4, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_5:
            dbgMenuFightTrainerDataStatusInputDigit(5, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_6:
            dbgMenuFightTrainerDataStatusInputDigit(6, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_7:
            dbgMenuFightTrainerDataStatusInputDigit(7, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_8:
            dbgMenuFightTrainerDataStatusInputDigit(8, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_25:
            dbgMenuFightTrainerDataStatusInputDigit(25, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_9:
            dbgMenuFightTrainerDataStatusInputDigit(9, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_10:
            dbgMenuFightTrainerDataStatusInputDigit(10, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_11:
            dbgMenuFightTrainerDataStatusInputDigit(11, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_12:
            dbgMenuFightTrainerDataStatusInputDigit(12, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_13:
            dbgMenuFightTrainerDataStatusInputDigit(13, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_14:
            dbgMenuFightTrainerDataStatusInputDigit(14, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_15:
            dbgMenuFightTrainerDataStatusInputDigit(15, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_16:
            dbgMenuFightTrainerDataStatusInputDigit(16, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_17:
            dbgMenuFightTrainerDataStatusInputDigit(17, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_18:
            dbgMenuFightTrainerDataStatusInputDigit(18, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_19:
            dbgMenuFightTrainerDataStatusInputDigit(19, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_20:
            dbgMenuFightTrainerDataStatusInputDigit(20, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_21:
            dbgMenuFightTrainerDataStatusInputDigit(21, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_22:
            dbgMenuFightTrainerDataStatusInputDigit(22, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_23:
            dbgMenuFightTrainerDataStatusInputDigit(23, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_SWITCH_OUT_ADDSUB_24:
            dbgMenuFightTrainerDataStatusInputDigit(24, FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0, 200, -200);
            break;
        }
    }

    menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_OUT_EDITOR, 0, 1);
    return 1;
}

/* Single-record editors: the pokemon-part editor and the AI-data editor.  Each
 * keeps a copy of its record while the menu is open and writes the copy back
 * if the user cancels. */

s32 dbgMenuFightTrainerPokemonPartDataEditSub(u16 index)
{
    FIGHT_TRAINER_POKEMON_PART_DATA *data;
    FIGHT_TRAINER_POKEMON_PART_DATA savedData;
    s32 result;

    if (index == 0) {
        return 1;
    }
    data = fightTrainerPokemonPartDataBiosGetPtr(index);
    savedData = *data;
    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_POKEMON_PART_EDITOR, 0, 0, 0, 1, 1, index);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_POKEMON_PART_EDITOR, 0, 1);
            *data = savedData;
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
        switch (menuGetCursorItemID(MENU_ID_DEBUG_TRAINER_POKEMON_PART_EDITOR)) {
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_1:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 1, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_2:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 2, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_3:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 3, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_4:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 4, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_5:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 5, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_6:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 6, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_7:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 7, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_8:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 8, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_9:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 9, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_10:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 10, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_POKEMON_PART_WAZA_TYPE_REVISE_13:
            dbgMenuFightTrainerDataStatusInputDigit(
                index, FIGHT_TRAINER_QUERY_POKEMON_PART_WAZA_TYPE_REVISE, 13, 100, 0);
            break;
        }
    }
    menuCloseCustom(MENU_ID_DEBUG_TRAINER_POKEMON_PART_EDITOR, 0, 1);
    return 1;
}

s32 dbgMenuFightTrainerAiDataEditSub(u16 index)
{
    FIGHT_TRAINER_AI_DATA *data;
    FIGHT_TRAINER_AI_DATA savedData;
    s32 result;
    s32 value;

    if (index == 0) {
        return 1;
    }
    data = fightTrainerAiDataBiosGetPtr(index);
    savedData = *data;
    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_EDITOR, 0, 0, 0, 1, 1, index);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_EDITOR, 0, 1);
            *data = savedData;
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
        switch (menuGetCursorItemID(MENU_ID_DEBUG_TRAINER_AI_EDITOR)) {
        case MENU_ITEM_DEBUG_TRAINER_AI_COMBO_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_COMBO_VALUE, 0, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_IREKAE_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_IREKAE_VALUE, 0, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_ITEM_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_ITEM_VALUE, 0, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_NOKORI_HP_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_NOKORI_HP_VALUE, 0, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_NOKORI_PP_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_WAZA_NOKORI_PP_VALUE, 0, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_POKEMON_SELECT_RANDOM_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_POKEMON_SELECT_RANDOM_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_POKEMON_SELECT_WEAK_POINT_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_POKEMON_SELECT_WEAK_POINT_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_KEY_PLAYER_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_KEY_PLAYER_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_POKEMON_JOUTAI_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_POKEMON_JOUTAI_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_PARAM_EXPECT_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_PARAM_EXPECT_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_PARAM_STORE_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_PARAM_STORE_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_ZOKUSEI_CHECK_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_ZOKUSEI_CHECK_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_TOKUSEI_CHECK_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_TOKUSEI_CHECK_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_DAMAGE_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_WAZA_DAMAGE_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_DEFENSE_POKEMON_RND_SELECT_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_DEFENSE_POKEMON_RND_SELECT_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_RND_SELECT_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_WAZA_RND_SELECT_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_INIT_VALUE_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_WAZA_INIT_VALUE_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_PART_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_PART_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_HIT_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_WAZA_HIT_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_RISK_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_WAZA_RISK_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_AVG_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_WAZA_AVG_VALUE, 0, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_ABICNT_MAX_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_ABICNT_MAX_VALUE, 0, 84, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_ABICNT_MIN_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_ABICNT_MIN_VALUE, 0, 84, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_ZOKUSEI_REVISE_0_DATA_ID:
            value = dbgMenuFightGetZokuseiDataId(
                (u16)fightTrainerGetStatus(0, index, FIGHT_TRAINER_QUERY_AI_ZOKUSEI_REVISE_ZOKUSEI_DATA_ID, 0));
            if (value >= 0) {
                fightTrainerSetStatus(0, index, FIGHT_TRAINER_QUERY_AI_ZOKUSEI_REVISE_ZOKUSEI_DATA_ID, 0, value);
            }
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_ZOKUSEI_REVISE_0_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_ZOKUSEI_REVISE_VALUE, 0, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_ZOKUSEI_REVISE_1_DATA_ID:
            value = dbgMenuFightGetZokuseiDataId(
                (u16)fightTrainerGetStatus(0, index, FIGHT_TRAINER_QUERY_AI_ZOKUSEI_REVISE_ZOKUSEI_DATA_ID, 1));
            if (value >= 0) {
                fightTrainerSetStatus(0, index, FIGHT_TRAINER_QUERY_AI_ZOKUSEI_REVISE_ZOKUSEI_DATA_ID, 1, value);
            }
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_ZOKUSEI_REVISE_1_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_ZOKUSEI_REVISE_VALUE, 1, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_TYPE_REVISE_0_TYPE_DATA_ID:
            value = dbgMenuFightGetWazaTypeId(
                (u8)fightTrainerGetStatus(0, index, FIGHT_TRAINER_QUERY_AI_WAZA_TYPE_REVISE_TYPE_DATA_ID, 0));
            if (value >= 0) {
                fightTrainerSetStatus(0, index, FIGHT_TRAINER_QUERY_AI_WAZA_TYPE_REVISE_TYPE_DATA_ID, 0, value);
            }
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_TYPE_REVISE_0_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_WAZA_TYPE_REVISE_VALUE, 0, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_TYPE_REVISE_1_TYPE_DATA_ID:
            value = dbgMenuFightGetWazaTypeId(
                (u8)fightTrainerGetStatus(0, index, FIGHT_TRAINER_QUERY_AI_WAZA_TYPE_REVISE_TYPE_DATA_ID, 1));
            if (value >= 0) {
                fightTrainerSetStatus(0, index, FIGHT_TRAINER_QUERY_AI_WAZA_TYPE_REVISE_TYPE_DATA_ID, 1, value);
            }
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_WAZA_TYPE_REVISE_1_VALUE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_WAZA_TYPE_REVISE_VALUE, 1, 100, 0);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_POKEMON_DATA_ORDER_OUT_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_POKEMON_DATA_ORDER_OUT_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_POKEMON_DATA_ORDER_ACE_BOSS_FLAG:
            dbgMenuFightTrainerDataStatusInputFlag(index, FIGHT_TRAINER_QUERY_AI_POKEMON_DATA_ORDER_ACE_BOSS_FLAG);
            break;
        case MENU_ITEM_DEBUG_TRAINER_AI_LAST_VALUE_REVISE:
            dbgMenuFightTrainerDataStatusInputDigit(index, FIGHT_TRAINER_QUERY_AI_LAST_VALUE_REVISE, 1, 100, 0);
            break;
        }
    }
    menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_EDITOR, 0, 1);
    return 1;
}

void dbgMenuFightTrainerDataStatusInputDigit(u16 id, u16 field, u16 sub, s32 max, s32 min)
{
    s32 value;

    if (menuSubOpenNumberInput(fightTrainerGetStatus(0, id, field, sub), (u32*)&value, 0, 50, 50, 0) == 1) {
        if (value > max) {
            value = max;
        }
        if (value < min) {
            value = min;
        }
        fightTrainerSetStatus(0, id, field, sub, value);
    }
    menuSubCloseNumberInput();
}

void dbgMenuFightTrainerDataStatusInputFlag(u16 id, u16 field)
{
    s8 answer;

    answer = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, fightTrainerGetStatus(0, id, field, 0) == 0);
    if (answer == 0) {
        fightTrainerSetStatus(0, id, field, 0, 1);
    } else if (answer == 1) {
        fightTrainerSetStatus(0, id, field, 0, 0);
    }
}


#include <fightTrainer.h>
#include <menu.h>
#include <types.h>
#include <menuSub.h>

/* Fight-debug sub menus of the trainer AI data editor.  The additional-data
 * sub menus work on the trainer AI table: 0x2D7 records of 0x14 bytes, which
 * each sub menu snapshots into _old_fight_trainer_ai_value_addsub_data and
 * restores when the user cancels the menu.  The pokemon-part and AI-data sub
 * menus work the same way on a single record of the part or AI data block. */

typedef struct {
    u32 lo;
    u32 hi;
} DbgSubDataPair;

typedef struct {
    DbgSubDataPair pair0;
    DbgSubDataPair pair1;
    u32 last;
} DbgFightTrainerAiSubData;

extern DbgFightTrainerAiSubData _old_fight_trainer_ai_value_addsub_data[0x2D7];

DbgFightTrainerAiSubData *fightTrainerAiValueAddsubDataBiosGetPtr(u16 index);
void dbgMenuFightTrainerDataStatusInputDigit(u16 id, u16 field, u16 sub, s32 max, s32 min);

#pragma peephole off

s32 dbgMenuFightTrainerAiAddsubDataComboEditSub(void)
{
    u16 i;
    s32 result;

    for (i = 0; i < 0x2D7; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_COMBO_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_COMBO_EDITOR, 0, 1);
            for (i = 0; i < 0x2D7; i++) {
                DbgFightTrainerAiSubData *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
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

    for (i = 0; i < 0x2D7; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_ITEM_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_ITEM_EDITOR, 0, 1);
            for (i = 0; i < 0x2D7; i++) {
                DbgFightTrainerAiSubData *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
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

    for (i = 0; i < 0x2D7; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_USE_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_USE_EDITOR, 0, 1);
            for (i = 0; i < 0x2D7; i++) {
                DbgFightTrainerAiSubData *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
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
        switch (menuGetCursorItemID(MENU_ID_DEBUG_TRAINER_AI_MOVE_USE_EDITOR) - 0xE7) {
        case 0:
            dbgMenuFightTrainerDataStatusInputDigit(0x38, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 1:
            dbgMenuFightTrainerDataStatusInputDigit(0x39, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 2:
            dbgMenuFightTrainerDataStatusInputDigit(0x3A, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 3:
            dbgMenuFightTrainerDataStatusInputDigit(0x3B, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 4:
            dbgMenuFightTrainerDataStatusInputDigit(0x3C, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 5:
            dbgMenuFightTrainerDataStatusInputDigit(0x40, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 6:
            dbgMenuFightTrainerDataStatusInputDigit(0x44, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 7:
            dbgMenuFightTrainerDataStatusInputDigit(0x45, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 8:
            dbgMenuFightTrainerDataStatusInputDigit(0x47, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 9:
            dbgMenuFightTrainerDataStatusInputDigit(0x48, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 10:
            dbgMenuFightTrainerDataStatusInputDigit(0x46, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 11:
            dbgMenuFightTrainerDataStatusInputDigit(0x41, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 12:
            dbgMenuFightTrainerDataStatusInputDigit(0x42, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 13:
            dbgMenuFightTrainerDataStatusInputDigit(0x43, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 14:
            dbgMenuFightTrainerDataStatusInputDigit(0x3D, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 15:
            dbgMenuFightTrainerDataStatusInputDigit(0x3E, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 16:
            dbgMenuFightTrainerDataStatusInputDigit(0x3F, 0x3E, 0, 0xC8, -0xC8);
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

    for (i = 0; i < 0x2D7; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR, 0, 1);
            for (i = 0; i < 0x2D7; i++) {
                DbgFightTrainerAiSubData *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
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
        menuGetCursorItemID(MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR);
    }
    menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_MOVE_TARGET_EDITOR, 0, 1);
    return 1;
}

s32 dbgMenuFightTrainerAiAddsubDataIrekaeDasuEditSub(void)
{
    s32 result;
    u16 i;

    for (i = 0; i < 0x2D7; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    while (1) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_IN_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_IN_EDITOR, 0, 1);
            for (i = 0; i < 0x2D7; i++) {
                DbgFightTrainerAiSubData *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
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
        case 0xCD:
            dbgMenuFightTrainerDataStatusInputDigit(0x1D, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xCE:
            dbgMenuFightTrainerDataStatusInputDigit(0x1E, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xCF:
            dbgMenuFightTrainerDataStatusInputDigit(0x1F, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD0:
            dbgMenuFightTrainerDataStatusInputDigit(0x21, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD1:
            dbgMenuFightTrainerDataStatusInputDigit(0x22, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD2:
            dbgMenuFightTrainerDataStatusInputDigit(0x23, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD3:
            dbgMenuFightTrainerDataStatusInputDigit(0x24, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD4:
            dbgMenuFightTrainerDataStatusInputDigit(0x25, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD5:
            dbgMenuFightTrainerDataStatusInputDigit(0x26, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD6:
            dbgMenuFightTrainerDataStatusInputDigit(0x27, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD7:
            dbgMenuFightTrainerDataStatusInputDigit(0x20, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD8:
            dbgMenuFightTrainerDataStatusInputDigit(0x30, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xD9:
            dbgMenuFightTrainerDataStatusInputDigit(0x28, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xDA:
            dbgMenuFightTrainerDataStatusInputDigit(0x29, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xDB:
            dbgMenuFightTrainerDataStatusInputDigit(0x2A, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xDC:
            dbgMenuFightTrainerDataStatusInputDigit(0x2B, 0x3E, 0, 0xC8, -0xC8);
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

    for (i = 0; i < 0x2D7; i++) {
        _old_fight_trainer_ai_value_addsub_data[i] = *fightTrainerAiValueAddsubDataBiosGetPtr(i);
    }

    while (1) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_OUT_EDITOR, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_OUT_EDITOR, 0, 1);
            for (i = 0; i < 0x2D7; i++) {
                DbgFightTrainerAiSubData *subData = fightTrainerAiValueAddsubDataBiosGetPtr(i);
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
        case 0xB3:
            dbgMenuFightTrainerDataStatusInputDigit(0x01, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xB4:
            dbgMenuFightTrainerDataStatusInputDigit(0x02, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xB5:
            dbgMenuFightTrainerDataStatusInputDigit(0x03, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xB6:
            dbgMenuFightTrainerDataStatusInputDigit(0x04, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xB7:
            dbgMenuFightTrainerDataStatusInputDigit(0x05, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xB8:
            dbgMenuFightTrainerDataStatusInputDigit(0x06, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xB9:
            dbgMenuFightTrainerDataStatusInputDigit(0x07, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xBA:
            dbgMenuFightTrainerDataStatusInputDigit(0x08, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xBB:
            dbgMenuFightTrainerDataStatusInputDigit(0x19, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xBC:
            dbgMenuFightTrainerDataStatusInputDigit(0x09, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xBD:
            dbgMenuFightTrainerDataStatusInputDigit(0x0A, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xBE:
            dbgMenuFightTrainerDataStatusInputDigit(0x0B, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xBF:
            dbgMenuFightTrainerDataStatusInputDigit(0x0C, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC0:
            dbgMenuFightTrainerDataStatusInputDigit(0x0D, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC1:
            dbgMenuFightTrainerDataStatusInputDigit(0x0E, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC2:
            dbgMenuFightTrainerDataStatusInputDigit(0x0F, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC3:
            dbgMenuFightTrainerDataStatusInputDigit(0x10, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC4:
            dbgMenuFightTrainerDataStatusInputDigit(0x11, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC5:
            dbgMenuFightTrainerDataStatusInputDigit(0x12, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC6:
            dbgMenuFightTrainerDataStatusInputDigit(0x13, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC7:
            dbgMenuFightTrainerDataStatusInputDigit(0x14, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC8:
            dbgMenuFightTrainerDataStatusInputDigit(0x15, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xC9:
            dbgMenuFightTrainerDataStatusInputDigit(0x16, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xCA:
            dbgMenuFightTrainerDataStatusInputDigit(0x17, 0x3E, 0, 0xC8, -0xC8);
            break;
        case 0xCB:
            dbgMenuFightTrainerDataStatusInputDigit(0x18, 0x3E, 0, 0xC8, -0xC8);
            break;
        }
    }

    menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_SWITCH_OUT_EDITOR, 0, 1);
    return 1;
}


/* Single-record editors: the pokemon part sub menu and the AI data sub menu.
 * Each snapshots its record into a local array while the menu is open and
 * writes it back when the user accepts the menu. */

typedef struct FightTrainerPokemonPartData {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0C;
    u32 unk10;
    u32 unk14;
} FightTrainerPokemonPartData;

typedef struct FightTrainerAiData {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0C;
    u32 unk10;
    u32 unk14;
    u32 unk18;
    u32 unk1C;
} FightTrainerAiData;

void *fightTrainerPokemonPartDataBiosGetPtr(u16 index);
void *fightTrainerAiDataBiosGetPtr(u16 index);
s32 dbgMenuFightGetZokuseiDataId(u16 id);
s32 dbgMenuFightGetWazaTypeId(u8 id);
void dbgMenuFightTrainerDataStatusInputFlag(u16 id, u16 field);

#pragma peephole off

s32 dbgMenuFightTrainerPokemonPartDataEditSub(u16 index)
{
    FightTrainerPokemonPartData* data;
    u32 savedData[6];
    u16 id;
    s32 result;
    s32 i;

    if (index == 0) {
        return 1;
    }
    data = (FightTrainerPokemonPartData*)fightTrainerPokemonPartDataBiosGetPtr(index);
    id = index;
    for (i = 0; i < 6; i++) savedData[i] = ((u32*)data)[i];
    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_POKEMON_PART_EDITOR, 0, 0, 0, 1, 1, id);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_POKEMON_PART_EDITOR, 0, 1);
            for (i = 0; i < 6; i++) ((u32*)data)[i] = savedData[i];
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
        case 0xFC:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 1, 100, 0);
            break;
        case 0xFD:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 2, 100, 0);
            break;
        case 0xFE:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 3, 100, 0);
            break;
        case 0xFF:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 4, 100, 0);
            break;
        case 0x100:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 5, 100, 0);
            break;
        case 0x101:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 6, 100, 0);
            break;
        case 0x102:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 7, 100, 0);
            break;
        case 0x103:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 8, 100, 0);
            break;
        case 0x104:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 9, 100, 0);
            break;
        case 0x105:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 10, 100, 0);
            break;
        case 0x108:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0xC, 13, 100, 0);
            break;
        }
    }
    menuCloseCustom(MENU_ID_DEBUG_TRAINER_POKEMON_PART_EDITOR, 0, 1);
    return 1;
}

s32 dbgMenuFightTrainerAiDataEditSub(u16 index)
{
    FightTrainerAiData* data;
    u32 savedData[8];
    u16 id;
    s32 result;
    s32 value;
    s32 i;

    if (index == 0) {
        return 1;
    }
    data = (FightTrainerAiData*)fightTrainerAiDataBiosGetPtr(index);
    id = index;
    for (i = 0; i < 8; i++) savedData[i] = ((u32*)data)[i];
    for (;;) {
        result = menuOpenCustom(MENU_ID_DEBUG_TRAINER_AI_EDITOR, 0, 0, 0, 1, 1, id);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_TRAINER_AI_EDITOR, 0, 1);
            for (i = 0; i < 8; i++) ((u32*)data)[i] = savedData[i];
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
        case 0x8E:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x25, 0, 100, 0);
            break;
        case 0x8F:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x26, 0, 100, 0);
            break;
        case 0x90:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x27, 0, 100, 0);
            break;
        case 0x91:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x2C, 0, 100, 0);
            break;
        case 0x92:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x37, 0, 100, 0);
            break;
        case 0x93:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x1F);
            break;
        case 0x94:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x20);
            break;
        case 0x95:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x23);
            break;
        case 0x96:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x24);
            break;
        case 0x97:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x28);
            break;
        case 0x98:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x29);
            break;
        case 0x99:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x2A);
            break;
        case 0x9A:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x2B);
            break;
        case 0x9B:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x2D);
            break;
        case 0x9C:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x30);
            break;
        case 0x9D:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x31);
            break;
        case 0x9E:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x32);
            break;
        case 0x9F:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x33);
            break;
        case 0xA0:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x34);
            break;
        case 0xA1:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x36);
            break;
        case 0xA2:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x35, 0, 100, 0);
            break;
        case 0xA3:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x2E, 0, 0x54, 0);
            break;
        case 0xA4:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x2F, 0, 0x54, 0);
            break;
        case 0xA6:
            value = dbgMenuFightGetZokuseiDataId((u16)fightTrainerGetStatus(0, index, 0x39, 0));
            if (value >= 0) {
                fightTrainerSetStatus(0, index, 0x39, 0, value);
            }
            break;
        case 0xA7:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x3A, 0, 100, 0);
            break;
        case 0xA8:
            value = dbgMenuFightGetZokuseiDataId((u16)fightTrainerGetStatus(0, index, 0x39, 1));
            if (value >= 0) {
                fightTrainerSetStatus(0, index, 0x39, 1, value);
            }
            break;
        case 0xA9:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x3A, 1, 100, 0);
            break;
        case 0xAB:
            value = dbgMenuFightGetWazaTypeId((u8)fightTrainerGetStatus(0, index, 0x3B, 0));
            if (value >= 0) {
                fightTrainerSetStatus(0, index, 0x3B, 0, value);
            }
            break;
        case 0xAC:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x3C, 0, 100, 0);
            break;
        case 0xAD:
            value = dbgMenuFightGetWazaTypeId((u8)fightTrainerGetStatus(0, index, 0x3B, 1));
            if (value >= 0) {
                fightTrainerSetStatus(0, index, 0x3B, 1, value);
            }
            break;
        case 0xAE:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x3C, 1, 100, 0);
            break;
        case 0xAF:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x21);
            break;
        case 0xB0:
            dbgMenuFightTrainerDataStatusInputFlag(index, 0x22);
            break;
        case 0xB1:
            dbgMenuFightTrainerDataStatusInputDigit(index, 0x38, 1, 100, 0);
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

    answer = menuSubOpenYesNo(0x7F, -1, -1, fightTrainerGetStatus(0, id, field, 0) == 0);
    if (answer == 0) {
        fightTrainerSetStatus(0, id, field, 0, 1);
    } else if (answer == 1) {
        fightTrainerSetStatus(0, id, field, 0, 0);
    }
}


#pragma peephole reset

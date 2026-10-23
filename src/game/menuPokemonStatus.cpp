#include <PokemonData.h>
#include <menuOffScreen.h>
#include <hero.h>
#include <heroBios.h>
#include <Pokemon.h>
// Pokemon status menu (summary / waza / ribbon pages).
#include <GSgfx.h>
#include <menuItemBios.h>
#include <menuModel.h>
#include <menuPokemonStatus.h>
#include <menuPokemon.h>
#include <waza.h>
#include <winSprite.h>
#include <menu.h>
#include <window.h>
#include <gamedata.h>
#include <savedata.h>
#include <GSsnd.h>
#include <GSmsg.h>
#include <pokemonBios.h>
#include <winMsg.h>
#include <fade.h>
#include <fightFloor.h>
#include <menuCBBios.h>
#include <msgctrl.h>
#include <GSthread.h>
#include <GSflag.h>
#include <floor.h>
#include <floorManager.h>
#include <menuSub.h>
#include <zokusei.h>
#include <libc/memory.h>

extern "C" {
void menuDrawReliveUpdate(void);
void menuDrawReliveMeter(Pokemon *pokemon, s16 x, s16 y, GSgfxLayerID layer);
void menuPlaySe(s32 p1, s32 p2);
u16 zokuseiBiosGetSpriteID(u16 type);
u16 zokuseiBiosGetWazaJoutai(u16 type, u16 state);
u16 mballBiosGetSpriteID(u8 ball);
u32 floorGetPlaceName(u32 place);
u8 fightKind_CheckFightKind(s32 kind);
s32 getRibbonCount(void);
u32 getRibbonMsgID(s32 ribbon);
u8 exribbonGetNo(s32 ribbon);
}

// Menus hidden while the waza pages are up.
extern "C" const unsigned long wazaLearnHideMenuIDs[4] = { 0x37, 0x40, 0x38, 0x41 };
extern "C" const unsigned long wazaHideMenuIDs[4] = { 0x37, 0x40, 0x38, 0x41 };


// Status-menu work and model. The work block is cleared each time a pokemon
// status menu is opened; the model is loaded once per menu.
extern "C" {
MenuPokemonStatusWork _menuStatus;
MENU_MODEL _menuModel;
}

// Ribbon ids the grid cells hold, in the order the ribbon table lists them.
extern "C" unsigned long ribbon_id_tbl[32] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006,
};

// Message ids of the ex-ribbons, one per ex-ribbon number.
extern "C" unsigned long ex_ribbon_id[64] = {
    0x2B12, 0x2B13, 0x2B14, 0x2B15, 0x2B16, 0x2B17, 0x2B18, 0x2B19,
    0x2B1A, 0x2B1B, 0x2B1C, 0x2B1D, 0x2B1E, 0x2B1F, 0x2B20, 0x2B21,
    0x2B22, 0x2B23, 0x2B24, 0x2B25, 0x2B26, 0x2B27, 0x2B28, 0x2B29,
    0x2B2A, 0x2B2B, 0x2B2C, 0x2B2D, 0x2B2E, 0x2B2F, 0x2B30, 0x2B31,
    0x2B32, 0x2B33, 0x2B34, 0x2B35, 0x2B36, 0x2B37, 0x2B38, 0x2B39,
    0x2B3A, 0x2B3B, 0x2B3C, 0x2B3D, 0x2B3E, 0x2B3F, 0x2B40, 0x2B41,
    0x2B42, 0x2B43, 0x2B44, 0x2B45, 0x2B46, 0x2B47, 0x2B48, 0x2B49,
    0x2B4A, 0x2B4B, 0x2B4C, 0x2B4D, 0x2B4E, 0x2B4F, 0x2B50, 0x2B51,
};

// Sub-menus the cursor switches between, in the order they are opened.
extern "C" unsigned long sub_menu_id[4] = {
    MENU_POKEMON_STATUS_MENU_TAB_SLOT0, MENU_POKEMON_STATUS_MENU_TAB_SLOT1,
    MENU_POKEMON_STATUS_MENU_WAZA, MENU_POKEMON_STATUS_MENU_RIBBON,
};

// One ribbon kind: the pokemon status field that holds how many ribbons of
// the kind the pokemon wears, the first ribbon id of the kind and the highest
// count the field can hold.
struct RibbonKindData {
    u16 mField; // 0x0: pokemon status field holding the count.
    u8 mFirst;  // 0x2: first ribbon id of the kind.
    u8 mMax;    // 0x3: highest count the field can hold.
};

extern "C" RibbonKindData ribbon_mapping_tbl[10] = {
    { 0x00A3, 0x00, 0x01 }, { 0x00B0, 0x01, 0x04 }, { 0x00B1, 0x05, 0x04 }, { 0x00B2, 0x09, 0x04 },
    { 0x00B3, 0x0D, 0x04 }, { 0x00B4, 0x11, 0x04 }, { 0x00A4, 0x15, 0x01 }, { 0x00A5, 0x16, 0x01 },
    { 0x00A6, 0x17, 0x01 }, { 0x00A7, 0x18, 0x01 },
};

extern "C" RibbonKindData exribbon_mapping_tbl[7] = {
    { 0x00A8, 0x19, 0x01 }, { 0x00A9, 0x1A, 0x01 }, { 0x00AA, 0x1B, 0x01 }, { 0x00AB, 0x1C, 0x01 },
    { 0x00AC, 0x1D, 0x01 }, { 0x00AD, 0x1E, 0x01 }, { 0x00AE, 0x1F, 0x01 },
};

// Grid sprite hook of the ribbon page: draws the ribbon the sprite stands for.
// The item ids 0x21E..0x25B are the ribbon grid cells, 0x1E1..0x1F8 the waza
// slots the ribbon page shows above them.
void menuPokemonStatusDrawRibbon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 color;
    s32 ribbon;
    u32 msg;
    s32 index;
    s32 i = 0;

    if (windowSearchID(MENU_POKEMON_STATUS_MENU_HEAD) == 0)
        return;
    if (_menuStatus.mPokemon == 0)
        return;

    color = window->mRgba.mColorA | 0xFFFFFF00;

    switch (sprite->mItemID) {
    case MENU_POKEMON_STATUS_ITEM_RIBBON_COUNT:
        msgctrlSetValue(0x34, _menuStatus.mRibbon.mCount);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;

    case MENU_POKEMON_STATUS_ITEM_RIBBON_DESCRIPTION: {
        if (_menuStatus.mPage != MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT)
            return;
        s8 grid = _menuStatus.mGridIndex;
        ribbon = _menuStatus.mRibbon.mCells[grid % 9][grid / 9];
        if (ribbon < 0) {
            if ((u32)getRibbonCount() <= (u32)ribbon)
                return;
        }
        {
            s32 kind = ribbon_id_tbl[ribbon];
            u8 count;

            if (kind != -1) {
                if (kind < -1 || kind >= 7) {
                    /* not an ex-ribbon group: leave the message unset */
                } else {
                    s32 idx;

                    count = exribbonGetNo(kind);
                    if (count != 0) {
                        idx = count - 1;
                        msg = ex_ribbon_id[idx];
                    } else {
                        msg = 0;
                    }
                }
            } else {
                msg = getRibbonMsgID(ribbon);
            }
            if (msg == 0)
                return;
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color,
                        msg);
        }
        return;
    }

    // The ribbon cells are numbered backwards from the last one, and the two
    // rows the ex-ribbons do not use are skipped.
    case 0x22A: i = 1;
    case 0x22B: i++;
    case 0x22C: i++;
    case 0x22D: i++;
    case 0x22E: i++;
    case 0x22F: i++;
    case 0x230: i++;
    case 0x231: i++;
    case 0x232: i++;
    case 0x233: i++;
    case 0x234: i++;
    case 0x235: i++;
    case 0x236: i++;
    case 0x237: i++;
    case 0x238: i++;
    case 0x239: i++;
    case 0x23A: i++;
    case 0x23B: i++;
    case 0x23C: i++;
    case 0x23D: i++;
    case 0x23E: i++;
    case 0x23F: i++;
    case 0x240: i++;
    case 0x241: i++;
    case 0x242: i++;
    case 0x243: i++;
    case 0x244: i++;
    case 0x245: i++;
    case 0x246: i++;
    case 0x247: i++;
    case 0x248: i++;
    case 0x249:
        i = 0x20 - (i + 1);
        if (i > 0x18)
            i += 2;
        if (_menuStatus.mRibbon.mCells[i % 9][i / 9] >= 0)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case 0x229: {
        if (_menuStatus.mGridIndex < 0)
            return;
        int column = _menuStatus.mGridIndex % 9;
        int row = (int)_menuStatus.mGridIndex / 9;
        windowDrawSprite((s16)(column * 30), (s16)(row * 52), window, 0x366, 0,
                    winSpriteGetLayerID(window, sprite));
        return;
    }

    case 0x21E:
    case 0x220:
        if (_menuStatus.mPage == MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    default:
        return;
    }
}

// Ribbon page hook: refills the ribbon grid of the pokemon being shown.
void menuPokemonStatusCtrlRibbon(void)
{
    if (_menuStatus.mPokemon != 0)
        _menuPokemonStatusMakeRibbon(_menuStatus.mPokemon, &_menuStatus.mRibbon);
}

// Fill the ribbon grid of `work` from the ribbons `pokemon` wears. Each cell
// of the grid holds the ribbon index of the ribbon in that cell, or -1 when
// the pokemon has no ribbon there.
void _menuPokemonStatusMakeRibbon(Pokemon *pokemon, RIBBON_WORK *work)
{
    long column;
    unsigned long kind;
    long count;
    long index;
    long i;
    long row;
    for (row = 0; row < 4; row++)
        for (column = 0; column < 9; column++)
            work->mCells[column][row] = -1;

    index = 0;
    for (kind = 0; kind < 10; kind++) {
        count = pokemonGetStatus(pokemon, 0, ribbon_mapping_tbl[kind].mField, 0);
        if (count > ribbon_mapping_tbl[kind].mMax)
            count = ribbon_mapping_tbl[kind].mMax;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                column = index % 9;
                row = index / 9;
                work->mCells[column][row] = (s8)(ribbon_mapping_tbl[kind].mFirst + i);
                index++;
            }
        } else {
            index += ribbon_mapping_tbl[kind].mMax;
        }
    }

    index = 0;
    for (kind = 0; kind < 7; kind++) {
        count = pokemonGetStatus(pokemon, 0, exribbon_mapping_tbl[kind].mField, 0);
        if (count > exribbon_mapping_tbl[kind].mMax)
            count = exribbon_mapping_tbl[kind].mMax;
        if (count != 0) {
            for (i = 0; i < count; i++) {
                work->mCells[index][3] = (s8)(i + exribbon_mapping_tbl[kind].mFirst);
                index++;
            }
        } else {
            index += exribbon_mapping_tbl[kind].mMax;
        }
    }

    count = 0;
    for (kind = 0; kind < 4; kind++)
        for (column = 0; column < 9; column++)
            if (work->mCells[column][kind] >= 0)
                count++;
    work->mCount = count;
}

// Waza data id of the waza in `slot` of `pokemon`, for the status pages that
// ask for it by waza slot rather than by waza number.
static inline u16 menuPokemonStatusGetWazaId(Pokemon *pokemon, u16 slot)
{
    return _menuPokemonStatusGetWazaDataID(pokemon, slot);
}

// Message id of the type icon the waza page draws for the move in `slot`, or 0
// when the move is one a dark pokemon hides. A move the pokemon has not seen
// yet is flagged as it draws.
static inline u32 menuPokemonStatusGetSlotIcon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                                               Pokemon *pokemon, u16 slot)
{
    u16 waza = menuPokemonStatusGetWazaId(pokemon, slot);
    u32 icon;

    switch ((s32)NotDarkWaza((u16)waza)) {
    case 0:
        return 0;
    case WAZA_SHADOW_SPECIAL:
        return WAZA_SHADOW_SPECIAL_ICON;
    default:
        break;
    }
    icon = menuPokemonStatusGetWazaTypeIcon((u16)wazaGetStatus(0, waza, WAZA_FIELD_TYPE, 0));
    if (_menuStatus.mWazaNew[slot] != 0 && slot != 0) {
        windowDrawSprite(7, 0x14, window, MENU_POKEMON_STATUS_TEXTURE_WAZA_NEW, 0, winSpriteGetLayerID(window, sprite));
        pokemon->getDarkPokemon()->setWazaNew(slot - 1, 1);
    }
    return icon;
}

// Waza page of the status menu: draws the waza row of the grid and the waza
// slot the cursor stands on. Every sprite of the waza page runs through this
// hook, selected by the item id of the sprite.
void menuPokemonStatusDrawWaza(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 color;
    Pokemon *pokemon = _menuStatus.mPokemon;
    s16 x;
    u16 waza;
    u16 slot;
    u16 selectedMoveID;
    u32 msg = 0;
    s32 i = 0;

    if (pokemon == 0)
        return;
    if ((u8)_menuPokemonStatusCheckWazaItem(window, sprite) == 0)
        return;

    color = window->mRgba.mColorA | 0xFFFFFF00;

    switch (sprite->mItemID) {
    case MENU_POKEMON_STATUS_ITEM_WAZA_CURSOR:
        winSpriteSetDisp(sprite, 0);
        switch (_menuStatus.mPage) {
        case MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE:
            if (_menuStatus.mWazaId != 0) {
                windowDrawSprite(0, (s16)((s8)_menuStatus.mSlotPrev * 44), window, MENU_POKEMON_STATUS_TEXTURE_WAZA_CURSOR_PREV, 0,
                            winSpriteGetLayerID(window, sprite));
                windowDrawSprite(0, (s16)((s8)_menuStatus.mWazaSlot * 44), window, MENU_POKEMON_STATUS_TEXTURE_WAZA_CURSOR, 0,
                            winSpriteGetLayerID(window, sprite));
            } else {
                windowDrawSprite(0, (s16)(((s8)_menuStatus.mSlotPrev + 1) * 44), window, MENU_POKEMON_STATUS_TEXTURE_WAZA_CURSOR_PREV, 0,
                            winSpriteGetLayerID(window, sprite));
                windowDrawSprite(0, (s16)(((s8)_menuStatus.mWazaSlot + 1) * 44), window, MENU_POKEMON_STATUS_TEXTURE_WAZA_CURSOR, 0,
                            winSpriteGetLayerID(window, sprite));
            }
            break;

        case MENU_POKEMON_STATUS_PAGE_WAZA_LIST:
        case MENU_POKEMON_STATUS_PAGE_WAZA_LEARN:
            if (_menuStatus.mWazaId != 0)
                windowDrawSprite(0, (s16)((s8)_menuStatus.mWazaSlot * 44), window, MENU_POKEMON_STATUS_TEXTURE_WAZA_CURSOR, 0,
                            winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, (s16)(((s8)_menuStatus.mWazaSlot + 1) * 44), window, MENU_POKEMON_STATUS_TEXTURE_WAZA_CURSOR, 0,
                            winSpriteGetLayerID(window, sprite));
            break;
        }
        return;

    case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW4:
    case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW3:
    case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW2:
    case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW1:
    case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW0: {
        // The cell draws the type icon of the move it stands on; a move the
        // pokemon has not seen yet is flagged on the way.
        if (_menuStatus.mWazaId == 0) {
            if (sprite->mItemID == MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW0)
                return;
            switch (sprite->mItemID) {
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW1: slot = 0; break;
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW2: slot = 1; break;
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW3: slot = 2; break;
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW4: slot = 3; break;
            }
        } else {
            if (sprite->mItemID == MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW4)
                windowDrawSprite(7, 0x14, window, MENU_POKEMON_STATUS_TEXTURE_WAZA_NEW, 0, winSpriteGetLayerID(window, sprite));
            switch (sprite->mItemID) {
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW0: slot = 0; break;
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW1: slot = 1; break;
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW2: slot = 2; break;
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW3: slot = 3; break;
            case MENU_POKEMON_STATUS_ITEM_WAZA_ICON_ROW4: slot = 4; break;
            }
        }
        msg = menuPokemonStatusGetSlotIcon(window, sprite, pokemon, slot);
        if (msg == 0)
            return;
        windowDrawSprite(0, 0, window, (u16)msg, 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case 0x24F:
    case 0x250:
    case 0x251:
    case 0x252:
    case 0x253:
        if (_menuStatus.mWazaId == 0) {
            if (sprite->mItemID == 0x253)
                return;
            switch (sprite->mItemID) {
            case 0x252: slot = 0; break;
            case 0x251: slot = 1; break;
            case 0x250: slot = 2; break;
            case 0x24F: slot = 3; break;
            }
        } else {
            switch (sprite->mItemID) {
            case 0x253: slot = 0; break;
            case 0x252: slot = 1; break;
            case 0x251: slot = 2; break;
            case 0x250: slot = 3; break;
            case 0x24F: slot = 4; break;
            }
        }
        waza = menuPokemonStatusGetWazaId(pokemon, slot);
        if ((u16)waza == 0) {
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x3AF4);
            return;
        }
        u32 nameID = wazaGetStatus(0, waza, WAZA_FIELD_NAME, 0);
        if (nameID == 0)
            return;
        msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(nameID));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4277);
        return;

    case 0x254:
    case 0x255:
    case 0x256:
    case 0x257:
    case 0x258:
        if (_menuStatus.mWazaId == 0) {
            if (sprite->mItemID == 0x258)
                return;
            switch (sprite->mItemID) {
            case 0x257: slot = 0; break;
            case 0x256: slot = 1; break;
            case 0x255: slot = 2; break;
            case 0x254: slot = 3; break;
            }
        } else {
            switch (sprite->mItemID) {
            case 0x258: slot = 0; break;
            case 0x257: slot = 1; break;
            case 0x256: slot = 2; break;
            case 0x255: slot = 3; break;
            case 0x254: slot = 4; break;
            }
        }
        waza = menuPokemonStatusGetWazaId(pokemon, slot);
        x = (s16)(((s16)sprite->mWidth - (s16)(GSmsgGetMessageSize(0x3AE8) >> 16)) / 2);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 0, color, 0x3AE8);
        msg = (s32)NotDarkWaza(waza);
        switch (msg) {
        case 0:
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight, color, 0x3AF5);
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x3AF5);
            return;
        case WAZA_SHADOW_SPECIAL:
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight, color, 0x3AD1);
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x3AD1);
            return;
        default:
            break;
        }
        if (slot == 4)
            msg = wazaGetStatus(0, waza, WAZA_FIELD_PP, 0);
        else
            msg = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, slot);
        msgctrlSetValue(0x34, msg);
        GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, x, sprite->mHeight, color, 0x427B);
        if (slot == 4)
            msg = wazaGetStatus(0, waza, WAZA_FIELD_PP, 0);
        else
            msg = pokemonWazaGetMaxPP(pokemon, slot);
        msgctrlSetValue(0x34, msg);
        GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x427B);
        return;

    case 0x24C:
        waza = menuPokemonStatusGetWazaId(pokemon, (u16)_menuStatus.mWazaSlot);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color,
                    wazaGetStatus(0, waza, WAZA_FIELD_DESCRIPTION, 0));
        return;

    case 0x24D:
        switch (_menuStatus.mPage) {
        case MENU_POKEMON_STATUS_PAGE_WAZA_LIST:
        case MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE:
        case MENU_POKEMON_STATUS_PAGE_WAZA_LEARN:
            u32 accuracy = wazaGetStatus(0, menuPokemonStatusGetWazaId(pokemon, (u16)_menuStatus.mWazaSlot), WAZA_FIELD_ACCURACY, 0);
            if (accuracy <= 1) {
                GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x3AF6);
            } else {
                msgctrlSetValue(0x34, accuracy);
                GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4270);
            }
            winSpriteSetDisp(sprite, 1);
            break;

        default:
            winSpriteSetDisp(sprite, 0);
            break;
        }
        return;

    case 0x24E:
        switch (_menuStatus.mPage) {
        case MENU_POKEMON_STATUS_PAGE_WAZA_LIST:
        case MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE:
        case MENU_POKEMON_STATUS_PAGE_WAZA_LEARN:
            u32 power = wazaGetStatus(0, menuPokemonStatusGetWazaId(pokemon, (u16)_menuStatus.mWazaSlot), WAZA_FIELD_POWER, 0);
            if (power <= 1) {
                GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x3AF6);
            } else {
                msgctrlSetValue(0x34, power);
                GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4270);
            }
            winSpriteSetDisp(sprite, 1);
            break;

        default:
            winSpriteSetDisp(sprite, 0);
            break;
        }
        return;

    case 0x1FD:
        if (_menuStatus.mWazaId == 0)
            winSpriteSetDisp(sprite, 0);
        else
            winSpriteSetDisp(sprite, 1);
        return;

    case 0x388:
    case 0x389:
    case 0x38A:
    case 0x38B:
        if (_menuStatus.mWazaId == 0)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case 0x1E8: i = 1;
    case 0x1E9: i++;
    case 0x1EB: i++;
    case 0x1EA: i++;
    case 0x1ED: i++;
    case 0x1EC: i++;
    case 0x1EE: i++;
    case 0x1EF: i += 2;
    case 0x1F0: i++;
    case 0x1F1: i++;
    case 0x1F3: i++;
    case 0x1F2: i++;
    case 0x1F5: i++;
    case 0x1F6: i++;
    case 0x1F4: i++;
    case 0x1F7: i++;
    case 0x1F8:
        selectedMoveID = menuPokemonStatusGetWazaId(pokemon, (u16)_menuStatus.mWazaSlot);
        switch (wazaDB_GetCompatible(selectedMoveID)) {
        case 0:
        case 2:
        case 3:
            break;
        case 1: {
            s32 state = zokuseiBiosGetWazaJoutai(wazaGetStatus(0, selectedMoveID, WAZA_FIELD_TYPE, 0), i);

            switch (state) {
            case 0x3F: color = 0x35D; break;
            case 0x41: color = 0x35F; break;
            case 0x42: color = 0x360; break;
            case 0x43: color = 0x35E; break;
            default: break;
            }
            windowDrawSprite(0, 0, window, (u16)color, 0, winSpriteGetLayerID(window, sprite));
            return;
        }
        case 4:
            if (i == 7)
                windowDrawSprite(0, 0, window, 0x35E, 0, winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 5:
            if (i == 0xC)
                windowDrawSprite(0, 0, window, 0x35E, 0, winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 6:
            if (i == 3 || i == 8)
                windowDrawSprite(0, 0, window, 0x35E, 0, winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 7:
            if (i == 4)
                windowDrawSprite(0, 0, window, 0x35E, 0, winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 8:
            if (i == 2)
                windowDrawSprite(0, 0, window, 0x35E, 0, winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 9:
            if (i == 0)
                windowDrawSprite(0, 0, window, 0x35E, 0, winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 10:
        case 11:
        case 14:
            windowDrawSprite(0, 0, window, 0x376, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 12:
            if (i == 0x11)
                windowDrawSprite(0, 0, window, 0x35E, 0, winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 13:
            if (i == 0xA)
                windowDrawSprite(0, 0, window, 0x35E, 0, winSpriteGetLayerID(window, sprite));
            else
                windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        case 15:
            windowDrawSprite(0, 0, window, 0x35D, 0, winSpriteGetLayerID(window, sprite));
            return;
        default:
            return;
        }

    default:
        return;
    }
}

// Callback the status menu passes to the waza pages while the pokemon is not
// a dark pokemon: the waza of a dark pokemon are hidden.
long NotDarkWaza(long waza)
{
    // The waza of a dark pokemon are hidden until the pokemon is purified.
    if (waza < WAZA_SHADOW_SPECIAL && waza >= WAZA_SHADOW_FIRST)
        return 0;
    return waza;
}

// Waza data id of the waza in `slot` of `pokemon`; slot 4 is the waza the
// pokemon would learn next, which is not part of the waza data.
u16 _menuPokemonStatusGetWazaDataID(Pokemon *pokemon, u16 slot)
{
    u16 waza;

    if (slot == 4) {
        waza = _menuStatus.mWazaId;
    } else {
        waza = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, slot);
        if (pokemonWazaCheckValid(pokemon, slot) == 0)
            return 0;
    }
    return waza;
}

// Sprite hook of the waza pages: shows the sprite when the sprite stands for
// a cell the page has something to draw in.
bool _menuPokemonStatusCheckWazaItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool disp = true;

    switch (sprite->mItemID) {
    case 0x1E1:
    case 0x1E2:
    case 0x1E3:
    case 0x1E4:
    case 0x1E5:
    case 0x1E6:
    case 0x1E7:
    case 0x1E8:
    case 0x1E9:
    case 0x1EA:
    case 0x1EB:
    case 0x1EC:
    case 0x1ED:
    case 0x1EE:
    case 0x1EF:
    case 0x1F0:
    case 0x1F1:
    case 0x1F2:
    case 0x1F3:
    case 0x1F4:
    case 0x1F5:
    case 0x1F6:
    case 0x1F7:
    case 0x1F8:
    case 0x24C:
        switch (_menuStatus.mPage) {
        case MENU_POKEMON_STATUS_PAGE_WAZA_LIST:
        case MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE:
        case MENU_POKEMON_STATUS_PAGE_WAZA_LEARN: {
            s8 slot = _menuStatus.mWazaSlot;

            if (slot >= 0 && slot <= 4)
                disp = 1;
            else
                disp = 0;
            break;
        }
        default:
            disp = 0;
            break;
        }
        winSpriteSetDisp(sprite, disp);
        break;

    case 0x83D:
    case MENU_POKEMON_STATUS_ITEM_RELIVE_METER:
    case 0x83F:
    case 0x840:
    case 0x841:
    case 0x842:
        winSpriteSetDisp(sprite, 0);
        break;

    case 0x837:
    case MENU_POKEMON_STATUS_ITEM_EXP_BAR:
    case MENU_POKEMON_STATUS_ITEM_EXP_REMAINING:
    case 0x847:
    case MENU_POKEMON_STATUS_ITEM_EXP:
        if (pokemonGetStatus(_menuStatus.mPokemon, 0, POKEMON_STATUS_FIELD_WAZA_SEALED, 0) != 0)
            disp = 0;
        else
            disp = 1;
        winSpriteSetDisp(sprite, disp);
        break;

    default:
        break;
    }
    return disp;
}

// Status screen sprite hook: draws the field of the summary page the sprite
// stands for. The item ids are the fields of the summary pages.
inline u32 dispSummaryPlaceName(u32 place)
{
    return floorGetPlaceName(place);
}

void menuPokemonStatusDispSummary(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 x;
    s32 digit;
    u32 div;
    Pokemon *pokemon = _menuStatus.mPokemon;
    u32 color;
    s32 value;
    u32 placeName;
    u32 place;
    u32 next;
    u8 level;
    u16 icon;
    PokemonSpeciesStats *stats;

    if (pokemon == 0)
        return;
    stats = pokemonDataBiosGetPtr(
        (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0));
    if (stats == 0)
        return;
    if ((u8)_menuPokemonStatusCheckWazaItem(window, sprite) == 0)
        return;

    color = window->mRgba.mColorA | 0xFFFFFF00;

    switch (sprite->mItemID) {
    case MENU_POKEMON_STATUS_ITEM_SEALED_LABEL:
        if (pokemonGetStatus(_menuStatus.mPokemon, 0, POKEMON_STATUS_FIELD_WAZA_SEALED, 0) != 0)
            sprite->mMsgID = 0x3C11;
        else
            sprite->mMsgID = 0x3C10;
        return;

    case MENU_POKEMON_STATUS_ITEM_TYPE_FIRST: {
        icon = zokuseiGetPokemonSpriteID(stats, 0);

        if (icon == 0)
            return;
        windowDrawSprite(0, 0, window, (s32)icon, 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_TYPE_SECOND: {
        u8 type0 = (u8)pokemonDataBiosGetZokuseiDataId(stats, 0);

        if (type0 == (u8)pokemonDataBiosGetZokuseiDataId(stats, 1))
            return;
        icon = zokuseiGetPokemonSpriteID(stats, 1);
        windowDrawSprite(0, 0, window, (s32)icon, 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_MET_LINE: {

        if ((u8)pokemonGetDarkPokemonLevel(pokemon) < 3) {
            value = 0x945;
        } else {
            s32 natureMessageId = pokemonSeikakuDataGetMessageId(
                pokemonSeikakuDataBiosGetPtr((u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NATURE, 0)));

            value = natureMessageId;
        }
        msgctrlSetValue(0x55, (unsigned long)value);
        switch (value) {
        case 0xC86:
        case 0xC96:
            msgctrlSetValue(0x56, 1);
            break;
        default:
            msgctrlSetValue(0x56, 0x3AEC);
            break;
        }

        value = (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_MET_LEVEL, 0);
        if ((u32)value == 0)
            value = 5;
        msgctrlSetValue(0x34, (unsigned long)value);

        if (_menuPokemonStatusCheckOya(pokemon)) {
            s32 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);

            switch (species) {
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0xC4:
            case 0xC5:
                value = 0x3AF7;
                break;
            default:
                value = 0x3AE1;
                break;
            }
        } else {
            value = 0x3AE1;
        }
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, value);

        value = 0x3AFA;
        place = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_MET_PLACE, 0);
        placeName = dispSummaryPlaceName(place);
        if (_menuPokemonStatusCheckOya(pokemon)) {
            s32 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);

            switch (species) {
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0xC4:
            case 0xC5:
                if (fightKind_CheckFightKind(0xE) == 0)
                    value = 0x3AF8;
                break;
            default:
                if (placeName != 0) {
                    msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(placeName));
                    value = 0x3AEB;
                }
                break;
            }
        } else {
            u8 origin = (u8)(u32)gamedataGetStatus((void *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTEST, 0), 2);

            switch (origin) {
            case 0xB: {
                if (_menuPokemonStatusCheckGinzaru(pokemon)) {
                    if (placeName != 0) {
                        msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(placeName));
                        value = 0x3AEB;
                    }
                } else if (_menuPokemonStatusCheckDanny(pokemon)) {
                    if (placeName != 0) {
                        msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(placeName));
                        value = 0x3AEB;
                    }
                } else if (place == 0xFF) {
                    value = 0x3AF9;
                }
                break;
            }
            case 8:
            case 9:
            case 0xA: {
                if (place == 0xFF)
                    value = 0x3AF9;
                break;
            }
            default:
                break;
            }
        }
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0x20, sprite->mWidth, sprite->mHeight, color, value);
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_SHADOW_STATE: {
        u8 level;

        if (pokemonIsDarkPokemon(pokemon) == 1) {
            level = pokemonGetDarkPokemonLevel(pokemon);
            switch (level) {
            case 0: sprite->mMsgID = 0x3AED; break;
            case 1: sprite->mMsgID = 0x3AEE; break;
            case 2: sprite->mMsgID = 0x3AEF; break;
            case 3: sprite->mMsgID = 0x3AF0; break;
            case 4: sprite->mMsgID = 0x3AF1; break;
            case 5: sprite->mMsgID = 0x3AF2; break;
            case 6: sprite->mMsgID = 0x3AF3; break;
            default: break;
            }
        } else {
            sprite->mMsgID = 0x3ADF;
        }
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_EXP_REMAINING: {
        // Exp bar of the level the pokemon is on.
        u8 level = (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0);
        u32 next = pokemonGetLevelToExp(pokemon, (u8)(level + 1));

        if (next == 0)
            value = 0;
        else
            value = next - pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0);
        msgctrlSetValue(0x34, value);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_EXP:
        msgctrlSetValue(0x34, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;

    case MENU_POKEMON_STATUS_ITEM_SPEED:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;

    case MENU_POKEMON_STATUS_ITEM_SP_DEFENSE:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;

    case MENU_POKEMON_STATUS_ITEM_SP_ATTACK:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;

    case MENU_POKEMON_STATUS_ITEM_DEFENSE:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;

    case MENU_POKEMON_STATUS_ITEM_ATTACK:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;

    case MENU_POKEMON_STATUS_ITEM_HP:
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, 0x37, sprite->mHeight, color, 0x4287);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0x37, 0, color, 0x3AE8);
        msgctrlSetValue(0x34, (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4287);
        return;

    case MENU_POKEMON_STATUS_ITEM_ABILITY_DESCRIPTION:
        msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(
            pokemonTokuseiDataBiosGetDoc(
                pokemonTokuseiDataBiosGetPtr((u16)pokemonGetTokuseiDataId(pokemon)))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4276);
        return;

    case MENU_POKEMON_STATUS_ITEM_ABILITY_NAME:
        msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(
            pokemonTokuseiDataBiosGetName(
                pokemonTokuseiDataBiosGetPtr((u16)pokemonGetTokuseiDataId(pokemon)))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4276);
        return;


    case MENU_POKEMON_STATUS_ITEM_TRAINER_ID: {
        // The trainer id of the pokemon, one digit at a time.
        x = (s16)(GSmsgGetMessageSize(sprite->mMsgID) >> 16);
        if (pokemonIsDarkPokemon(pokemon) == 1) {
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 0, color, 0x3AD4);
            return;
        }
        value = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0);
        {
            // Each digit is printed at its own position to the right of the
            // previous one.
            s32 px = x;
            u32 div = 10000;

            for (digit = 0; digit < 5; digit++) {
                s32 cur = value / div;

                value -= cur * div;
                div /= 10;
                msgctrlSetValue(0x34, (unsigned long)cur);
                GSmsgPrint2(winSpriteGetLayerID(window, sprite), px, 0, color, 0x426F);
                px += 13;
            }
        }
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_TRAINER_NAME: {

        if (pokemonIsDarkPokemon(pokemon) == 1)
            msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(0x3AD4));
        else
            msgctrlSetValue(0x37, (unsigned long)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_NAME, 0));
        x = (s16)(GSmsgGetMessageSize(sprite->mMsgID) >> 16);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 0, color, 0x4276);
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_HP_BAR: {
        u16 spriteID;

        // HP bar: the texture shows how full the bar is, and the bar is drawn
        // as wide as the HP ratio.
        s16 hp = (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP, 0);
        s16 hpMax = (s16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0);
        f32 ratio = (f32)hp / (f32)hpMax;

        if (ratio < 0.2)
            spriteID = MENU_POKEMON_STATUS_TEXTURE_HP_BAR_LOW;
        else if (ratio < 0.5)
            spriteID = MENU_POKEMON_STATUS_TEXTURE_HP_BAR_HALF;
        else
            spriteID = MENU_POKEMON_STATUS_TEXTURE_HP_BAR_FULL;
        // The bar is as wide as the ratio of the window.
        windowDrawSprite2(0, 0, (s16)((f32)sprite->mWidth * ratio), sprite->mHeight,
                                      color, window, spriteID, 0,
                                      winSpriteGetLayerID(window, sprite));
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_EXP_BAR: {
        s32 base;
        s32 span;

        // Exp bar: as wide as the exp of the current level is complete.
        u8 level = (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0);
        u32 next = pokemonGetLevelToExp(pokemon, (u8)(level + 1));

        if (next == 0)
            return;
        base = pokemonGetLevelToExp(pokemon, level);
        span = next - base;
        {
            s32 progress = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0) - base;
            s32 total = span - 1 + progress * sprite->mWidth;
            s32 width = (s16)((u32)total / span);

            windowDrawSprite2(0, 0, width, sprite->mHeight, color, window, MENU_POKEMON_STATUS_TEXTURE_EXP_BAR, 0,
                                          winSpriteGetLayerID(window, sprite));
        }
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_RELIVE_METER: {
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MENU_POKEMON_STATUS_MENU_TAB_SLOT1,
                                                     MENU_POKEMON_STATUS_ITEM_RELIVE_METER);

        menuDrawReliveMeter(pokemon, item->mWidth, item->mHeight,
                            winSpriteGetLayerID(window, sprite));
        return;
    }

    default:
        return;
    }
}

// Non-zero when the pokemon was met by the player of the other game.
u8 _menuPokemonStatusCheckDanny(Pokemon *pokemon)
{
    if ((int)(u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0)
        == (int)MENU_POKEMON_STATUS_DANNY_TRAINER_ID) {
        if (GScharCmp((u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_NAME, 0),
                      GSmsgGetStringPtr(MENU_POKEMON_STATUS_DANNY_TRAINER_NAME_MSG)) == 0)
            return 1;
    }
    return 0;
}

// Non-zero when the pokemon was met by "Ginzaru".
u8 _menuPokemonStatusCheckGinzaru(Pokemon *pokemon)
{
    if ((int)(u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0)
        == (int)MENU_POKEMON_STATUS_GINZARU_TRAINER_ID) {
        if (GScharCmp((u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_NAME, 0),
                      GSmsgGetStringPtr(MENU_POKEMON_STATUS_GINZARU_TRAINER_NAME_MSG)) == 0)
            return 1;
    }
    return 0;
}

// Non-zero when the pokemon was met by the trainer the menu acts on: the
// trainer id and name recorded on the pokemon must both match.
u8 _menuPokemonStatusCheckOya(Pokemon *pokemon)
{
    FIGHT_TRAINER *trainer = _menuStatus.mTrainer;
    Hero *hero;
    u16 *info;
    u32 tid;

    if (pokemon == 0)
        return 0;
    if ((u8)(u32)gamedataGetStatus((void *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTEST, 0), 2) != 0xB)
        return 0;

    if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FIGHT_TRAINER) {
        if (trainer == 0)
            trainer = fightFloorGetGcHeroFightTrainerPtr(0);
        if (trainer == 0)
            return 0;
        hero = fightTrainerBiosGetHeroPtr(trainer);
    } else if (GSflagGet(0x30D) == 0) {
        hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    } else {
        hero = menuCBBios_GetRegisteredHero();
    }

    info = heroBiosGetNamePtr(hero);
    tid = heroBiosGetRnd(hero);
    if (tid == (u32)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0)) {
        if (GScharCmp((u16 *)info, (u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_NAME, 0)) == 0)
            return 1;
    }
    return 0;
}

// Nature page of the status menu: draws the met/origin line, the item and the
// sex symbol of the pokemon.
void menuPokemonStatusDispNature(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    Pokemon *pokemon = _menuStatus.mPokemon;
    Pokemon *statusPokemon;
    tagMENU_ITEM_DATA *item;
    unsigned long messageParameter;
    u32 color;
    u16 textureID;
    u8 ball;

    if (pokemon == 0)
        return;
    statusPokemon = _menuStatus.mPokemon;
    if (pokemonDataBiosGetPtr(
            (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0)) == 0)
        return;

    color = window->mRgba.mColorA | 0xFFFFFF00;

    switch (sprite->mItemID) {
    case MENU_POKEMON_STATUS_ITEM_TAB_SELECTOR0: {
        if (window->mCursor.y == 0)
            sprite->mColor = -1;
        else
            sprite->mColor = 0x808080FF;
        return;
    }
    case MENU_POKEMON_STATUS_ITEM_TAB_SELECTOR1: {
        if (window->mCursor.y == 1)
            sprite->mColor = -1;
        else
            sprite->mColor = 0x808080FF;
        return;
    }
    case MENU_POKEMON_STATUS_ITEM_TAB_SELECTOR2: {
        if (window->mCursor.y == 2)
            sprite->mColor = -1;
        else
            sprite->mColor = 0x808080FF;
        return;
    }
    case MENU_POKEMON_STATUS_ITEM_TAB_SELECTOR3: {
        if (window->mCursor.y == 3)
            sprite->mColor = -1;
        else
            sprite->mColor = 0x808080FF;
        return;
    }

    case 0x81C:
        winSpriteSetDisp(sprite, 1);
        switch (window->mCursor.y) {
        case 2:
            switch (_menuStatus.mPage) {
            case MENU_POKEMON_STATUS_PAGE_WAZA_LIST:
            case MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE:
                if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_WAZA_REPLACE)
                    sprite->mMsgID = 0x3AE4;
                else
                    sprite->mMsgID = 0x3AE3;
                break;
            case MENU_POKEMON_STATUS_PAGE_WAZA_LEARN:
                sprite->mMsgID = 0x3C3B;
                break;
            case MENU_POKEMON_STATUS_PAGE_CANCEL:
                break;
            default:
                sprite->mMsgID = 0x3AE3;
                break;
            }
            if (_menuStatus.mPage == MENU_POKEMON_STATUS_PAGE_WAZA_LIST &&
                (pokemonIsDarkPokemon(pokemon) ||
                 (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_WAZA_REPLACE) == 0))
                winSpriteSetDisp(sprite, 0);
            break;
        case 3:
            if (_menuStatus.mRibbon.mCount != 0) {
                if (_menuStatus.mPage == MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT)
                    winSpriteSetDisp(sprite, 0);
                else
                    sprite->mMsgID = 0x3AE6;
            } else {
                winSpriteSetDisp(sprite, 0);
            }
            break;
        default:
            winSpriteSetDisp(sprite, 0);
            break;
        }
        return;

    case 0x811:
    case 0x812:
    case 0x815:
        winSpriteSetDisp(sprite, 0);
        switch (window->mCursor.y) {
        case 2:
            winSpriteSetDisp(sprite, 1);
            if (_menuStatus.mPage == MENU_POKEMON_STATUS_PAGE_WAZA_LIST &&
                (pokemonIsDarkPokemon(pokemon) ||
                 (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_WAZA_REPLACE) == 0))
                winSpriteSetDisp(sprite, 0);
            break;
        case 3:
            if (_menuStatus.mRibbon.mCount != 0 && _menuStatus.mPage != MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT)
                winSpriteSetDisp(sprite, 1);
            break;
        default:
            break;
        }
        return;

    case MENU_POKEMON_STATUS_ITEM_MODEL: {
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MENU_POKEMON_STATUS_MENU_HEAD,
                                                     MENU_POKEMON_STATUS_ITEM_MODEL);
        f32 angle[3];

        menuModelSetSize(&_menuModel, item->mMessageX, item->mOffsetA,
                         item->mWidth, item->mHeight);
        if (menuModelSetupPokemon(&_menuModel, pokemon) != 1)
            return;
        if (menuModelCheck(&_menuModel, 0) == 0)
            menuModelAdjustCamera(&_menuModel);
        angle[0] = -0.1f;
        angle[1] = 0.3f;
        angle[2] = 0.0f;
        menuModelSetAngle(&_menuModel, angle);
        menuModelRender(&_menuModel);
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_MARKING_SLOT0:
    case MENU_POKEMON_STATUS_ITEM_MARKING_SLOT1:
    case MENU_POKEMON_STATUS_ITEM_MARKING_SLOT2:
    case MENU_POKEMON_STATUS_ITEM_MARKING_SLOT3: {
        u8 id = 0;
        u8 markings = (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_MARKINGS, 0);

        switch (sprite->mItemID) {
        case MENU_POKEMON_STATUS_ITEM_MARKING_SLOT0: id = POKEMON_MARKING_SLOT0; break;
        case MENU_POKEMON_STATUS_ITEM_MARKING_SLOT1: id = POKEMON_MARKING_SLOT1; break;
        case MENU_POKEMON_STATUS_ITEM_MARKING_SLOT2: id = POKEMON_MARKING_SLOT2; break;
        case MENU_POKEMON_STATUS_ITEM_MARKING_SLOT3: id = POKEMON_MARKING_SLOT3; break;
        default: break;
        }
        winSpriteSetDisp(sprite, (markings & id) != 0);
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_BALL: {
        ball = pokemonBiosGetCatchBallId(pokemon);

        if (ball == 0)
            return;
        item = menuItemBiosGetPtr(MENU_POKEMON_STATUS_MENU_HEAD, MENU_POKEMON_STATUS_ITEM_BALL);
        windowDrawSprite2(0, 0, item->mWidth, item->mHeight, 0xFFFFFFFF, window,
                                      mballBiosGetSpriteID((u8)ball), 0,
                                      winSpriteGetLayerID(window, sprite));
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_POKERUS: {
        u8 pokerus = (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POKERUS, 0);

        if ((pokerus & POKEMON_POKERUS_ACTIVE_MASK) != 0)
            textureID = MENU_POKEMON_STATUS_TEXTURE_POKERUS_ACTIVE;
        else if (pokerus != 0)
            textureID = MENU_POKEMON_STATUS_TEXTURE_POKERUS_CURED;
        else
            textureID = 0;
        windowDrawSprite(0, 0, window, textureID, 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case 0x805: {
        u16 statusSpriteID = menuPokemonGetJoutaiSpriteID(pokemon, 1);

        windowDrawSprite(0, 0, window, statusSpriteID, 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_HELD_ITEM: {
        messageParameter = (u16)pokemonGetSoubiItemDataId(pokemon);

        if (messageParameter != 0) {
            msgctrlSetValue(0x2D, (unsigned long)messageParameter);
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x3AE7);
        } else {
            msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(0x3C1A));
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, 0x4277);
        }
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_LEVEL: {
        s32 width = (s16)(GSmsgGetMessageSize(sprite->mMsgID) >> 16);

        msgctrlSetValue(0x5F, (unsigned long)(u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), width + 3, 0, sprite->mWidth, sprite->mHeight, color, 0x4351);
        return;
    }

    case MENU_POKEMON_STATUS_ITEM_SPECIES_NAME:
    case MENU_POKEMON_STATUS_ITEM_NICKNAME: {
        u32 sex;

        if (sprite->mItemID == MENU_POKEMON_STATUS_ITEM_SPECIES_NAME) {
            messageParameter = (unsigned long)GSmsgGetStringPtr(
                pokemonGetStatus(0, (u16)pokemonGetStatus(statusPokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0), 1, 0));
        } else {
            messageParameter = (unsigned long)pokemonGetStatus(statusPokemon, 0, POKEMON_STATUS_FIELD_NICKNAME, 0);
            if (pokemonCheckRare2(pokemon))
                color = window->mRgba.mColorA | 0xFFDC0000;
        }
        msgctrlSetValue(0x37, (unsigned long)messageParameter);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, 0x4277);
        GSmsgGetMessageSize(0x4277);
        sex = menuSubGetPokemonSexForDisp(statusPokemon);
        switch (sex) {
        case 0: sex = 0x9C5; break;
        case 1: sex = 0x9C6; break;
        case 2:
        default: sex = 0; break;
        }
        if (sex != 0) {
            msgctrlSetValue(0x37, (unsigned long)GSmsgGetStringPtr(sex));
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0x78, 0, color, 0x4276);
        }
        return;
    }

    default:
        return;
    }
}

// Draw hook of the status menu chain head; it only returns.
void menuPokemonStatusDispNoOp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
}

// Type icon of the summary page, drawn big.
u16 menuPokemonStatusGetWazaTypeIcon(u16 type)
{
    return zokuseiBiosGetSpriteID(type);
}

// Button hook of the status menu window: steps the pages with A and B and
// moves the waza slot with the cross pad.
void menuPokemonStatusCtrlPage(tagWINDOW_WORK *window)
{
    s32 key = windowGetKeyTrg();
    s8 i;

    if (key & MENU_POKEMON_STATUS_KEY_A) {
        switch (_menuStatus.mPage) {
        case MENU_POKEMON_STATUS_PAGE_SELECT:
        case MENU_POKEMON_STATUS_PAGE_TAB_SLOT1:
        case MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT:
        case MENU_POKEMON_STATUS_PAGE_TAB_SLOT0:
        case MENU_POKEMON_STATUS_PAGE_CANCEL:
            break;

        case MENU_POKEMON_STATUS_PAGE_WAZA:
            _menuStatus.mWazaSlot = 0;
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_WAZA_LIST;
            GSsndPlayNow(0x11);
            break;

        case MENU_POKEMON_STATUS_PAGE_WAZA_LIST:
            if (_menuStatus.mPokemon == 0)
                break;
            if (pokemonGetStatus(_menuStatus.mPokemon, 0, POKEMON_STATUS_FIELD_WAZA_SEALED, 0) != 0) {
                GSsndPlayNow(0x13);
                break;
            }
            if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_WAZA_REPLACE) {
                _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE;
                _menuStatus.mSlotPrev = _menuStatus.mWazaSlot;
                GSsndPlayNow(0x11);
            } else {
                GSsndPlayNow(0x13);
            }
            break;

        case MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE: {
            s8 slot;
            s8 prev;

            prev = _menuStatus.mSlotPrev;
            slot = _menuStatus.mWazaSlot;
            if (prev == slot)
                break;
            if (_menuStatus.mPokemon != 0)
                pokemonWazaReplace(_menuStatus.mPokemon, prev, slot);
            _menuStatus.mSlotPrev = -1;
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_WAZA_LIST;
            GSsndPlayNow(0x11);
            break;
        }

        case MENU_POKEMON_STATUS_PAGE_RIBBON:
            if (_menuStatus.mRibbon.mCount <= 0)
                break;
            for (i = 0; i < 0x24; i++) {
                if (_menuStatus.mRibbon.mCells[i % 9][i / 9] >= 0)
                    break;
            }
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT;
            _menuStatus.mGridIndex = i;
            GSsndPlayNow(0x11);
            break;

        case MENU_POKEMON_STATUS_PAGE_WAZA_LEARN:
            if (!(_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_WAZA_LEARN_OK))
                break;
            window->mAccept = 1;
            _menuStatus.mResult = (s8)_menuStatus.mWazaSlot;
            break;

        default:
            break;
        }
    } else if (key & MENU_POKEMON_STATUS_KEY_B) {
        switch (_menuStatus.mPage) {
        case MENU_POKEMON_STATUS_PAGE_SELECT:
        case MENU_POKEMON_STATUS_PAGE_CANCEL:
            break;

        case MENU_POKEMON_STATUS_PAGE_TAB_SLOT1:
        case MENU_POKEMON_STATUS_PAGE_WAZA:
        case MENU_POKEMON_STATUS_PAGE_RIBBON:
        case MENU_POKEMON_STATUS_PAGE_WAZA_LEARN:
        case MENU_POKEMON_STATUS_PAGE_TAB_SLOT0:
            window->mAccept = 1;
            window->mCancel = 1;
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_CANCEL;
            break;

        case MENU_POKEMON_STATUS_PAGE_WAZA_LIST:
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_WAZA;
            GSsndPlayNow(0x12);
            break;

        case MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE:
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_WAZA_LIST;
            GSsndPlayNow(0x12);
            break;

        case MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT:
            _menuStatus.mGridIndex = -1;
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_RIBBON;
            GSsndPlayNow(0x12);
            break;

        default:
            break;
        }
    }
}

// Cursor of the status menu: moves the page cursor, the waza slot and the
// grid cursor, and tells the callback when the pokemon changes.
void menuPokemonStatusCursor(tagWINDOW_WORK *window)
{
    s8 slot;
    s32 dir = 0;
    u16 key;
    s8 newGrid;
    long numWaza;
    s32 i;
    Pokemon *pokemon;
    DarkPokemon *shadow;
    s32 waza;

    numWaza = _menuStatus.mWazaId != 0 ? 5 : 4;
    key = windowGetKeyRepeat();

    switch (_menuStatus.mPage) {
    case MENU_POKEMON_STATUS_PAGE_TAB_SLOT1:
    case MENU_POKEMON_STATUS_PAGE_WAZA:
    case MENU_POKEMON_STATUS_PAGE_RIBBON:
    case MENU_POKEMON_STATUS_PAGE_TAB_SLOT0:
        key = (u16)key;
        slot = window->mCursor.y;
        if ((u16)key & MENU_POKEMON_STATUS_KEY_RIGHT)
            slot++;
        else if ((u16)key & MENU_POKEMON_STATUS_KEY_LEFT)
            slot--;
        if ((s8)slot > 3)
            slot = 3;
        if ((s8)slot < 0)
            slot = 0;
        window->mCursor.y = (s8)slot;
        switch ((s8)slot) {
        case 0: _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_TAB_SLOT0; break;
        case 1: _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_TAB_SLOT1; break;
        case 2: _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_WAZA; break;
        case 3: _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_RIBBON; break;
        }
        if ((u16)key & MENU_POKEMON_STATUS_KEY_UP)
            dir = MENU_POKEMON_CB_CURSOR_LEFT;
        else if ((u16)key & MENU_POKEMON_STATUS_KEY_DOWN)
            dir = MENU_POKEMON_CB_CURSOR_RIGHT;
        if (dir == 0)
            break;
        if (_menuStatus.mCallback == 0)
            break;
        pokemon = _menuStatus.mCallback(_menuStatus.mPokemon,
                                                  (MENU_POKEMON_CB_TYPE)dir,
                                                  _menuStatus.mCallbackArg);
        if (_menuStatus.mPokemon == pokemon)
            break;
        if (pokemon != 0) {
            menuPlaySe(window->mMenuID, 1);
            if (menuModelSetupPokemon(&_menuModel, pokemon) == 0)
                menuModelLoadPokemon(&_menuModel, pokemon);
        }
        _menuStatus.mPokemon = pokemon;
        for (i = 0; i < 4; i++) {
            _menuStatus.mWazaNew[i] = 0;
            if (!pokemon->isDarkPokemon())
                continue;
            shadow = pokemon->getDarkPokemon();
            if (shadow == 0)
                continue;
            waza = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, (u8)i);
            switch (waza) {
            case 0:
            case WAZA_SHADOW_FIRST:
            case WAZA_SHADOW_FIRST + 1:
            case WAZA_SHADOW_FIRST + 2:
            case WAZA_SHADOW_FIRST + 3:
            case WAZA_SHADOW_FIRST + 4:
            case WAZA_SHADOW_FIRST + 5:
            case WAZA_SHADOW_FIRST + 6:
            case WAZA_SHADOW_FIRST + 7:
            case WAZA_SHADOW_FIRST + 8:
            case WAZA_SHADOW_FIRST + 9:
            case WAZA_SHADOW_FIRST + 10:
            case WAZA_SHADOW_FIRST + 11:
            case WAZA_SHADOW_FIRST + 12:
            case WAZA_SHADOW_FIRST + 13:
            case WAZA_SHADOW_FIRST + 14:
            case WAZA_SHADOW_FIRST + 15:
            case WAZA_SHADOW_FIRST + 16:
            case WAZA_SHADOW_LAST:
            case WAZA_SHADOW_SPECIAL:
                continue;
            default:
                break;
            }
            if (shadow->isWazaNew((u8)(i - 1)) == 0)
                _menuStatus.mWazaNew[i] = 1;
        }
        break;

    case MENU_POKEMON_STATUS_PAGE_WAZA_LIST:
    case MENU_POKEMON_STATUS_PAGE_WAZA_REPLACE:
    case MENU_POKEMON_STATUS_PAGE_WAZA_LEARN: {
        waza = (u16)key;
        slot = _menuStatus.mWazaSlot;
        if (waza & MENU_POKEMON_STATUS_KEY_UP)
            slot--;
        else if (waza & MENU_POKEMON_STATUS_KEY_DOWN)
            slot++;
        if ((s8)slot >= (s8)numWaza)
            slot = (s8)(numWaza - 1);
        if ((s8)slot < 0)
            slot = 0;
        if ((u16)_menuPokemonStatusGetWazaDataID(_menuStatus.mPokemon, (u16)slot) == 0)
            break;
        if ((s8)slot == (s8)_menuStatus.mWazaSlot)
            break;
        menuPlaySe(window->mMenuID, 1);
        _menuStatus.mWazaSlot = slot;
        break;
    }

    case MENU_POKEMON_STATUS_PAGE_RIBBON_SELECT: {
        s8 grid = _menuStatus.mGridIndex;
        s32 cursorColumn;
        s32 cursorRow;
        s32 column;
        s32 row;
        s32 found;

        key = (u16)key;
        cursorColumn = grid % 9;
        cursorRow = (long)grid / (long)9;
        if ((u16)key & MENU_POKEMON_STATUS_KEY_UP) {
            row = cursorRow;
            while (row-- > 0) {
                column = cursorColumn;
                do {
                    if (_menuStatus.mRibbon.mCells[column][row] >= 0) {
                        cursorRow = row;
                        cursorColumn = column;
                        row = -1;
                        break;
                    }
                } while (column-- > 0);
            }
        } else if ((u16)key & MENU_POKEMON_STATUS_KEY_DOWN) {
            for (row = cursorRow; ++row < 4;) {
                column = cursorColumn;
                do {
                    if (_menuStatus.mRibbon.mCells[column][row] >= 0) {
                        cursorColumn = column;
                        cursorRow = row;
                        row = 5;
                        break;
                    }
                } while (column-- > 0);
            }
        } else if ((u16)key & MENU_POKEMON_STATUS_KEY_RIGHT) {
            found = 0;
            for (row = cursorRow; row < 4; row++) {
                if (row == cursorRow)
                    column = cursorColumn + 1;
                else
                    column = 0;
                for (; column < 9; column++) {
                    if (_menuStatus.mRibbon.mCells[column][row] >= 0) {
                        cursorColumn = column;
                        cursorRow = row;
                        found = 1;
                        break;
                    }
                }
                if (found != 0)
                    break;
            }
        } else if ((u16)key & MENU_POKEMON_STATUS_KEY_LEFT) {
            found = 0;
            for (row = cursorRow; row >= 0; row--) {
                if (row == cursorRow)
                    column = cursorColumn - 1;
                else
                    column = 8;
                for (; column >= 0; column--) {
                    if (_menuStatus.mRibbon.mCells[column][row] >= 0) {
                        cursorColumn = column;
                        cursorRow = row;
                        found = 1;
                        break;
                    }
                }
                if (found != 0)
                    break;
            }
        }
        newGrid = (s8)(cursorColumn + cursorRow * 9);
        if (newGrid != grid) {
            menuPlaySe(window->mMenuID, 1);
            _menuStatus.mGridIndex = newGrid;
        }
        break;
    }

    case MENU_POKEMON_STATUS_PAGE_SELECT:
    case MENU_POKEMON_STATUS_PAGE_CANCEL:
    default:
        break;
    }
}

// Sequence hook of the status menu window.
s32 menuPokemonStatusCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case 0:
        window->cursorTo = -1;
        _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_SELECT;
        _menuStatus.mWazaSlot = window->mCursor.y;
        _menuStatus.mSlotPrev = -1;
        _menuStatus.mGridIndex = -1;
        if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_OPEN_FIRST_TAB) {
            window->mCursor.y = 0;
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_TAB_SLOT0;
        } else {
            window->mCursor.y = 2;
            _menuStatus.mPage = MENU_POKEMON_STATUS_PAGE_WAZA_LEARN;
        }
        _menuPokemonStatusChangeSubMenu(*(CURSOR *)&window->mCursor.x);
        break;

    case 2:
        _menuPokemonStatusChangeSubMenu(*(CURSOR *)&window->mCursor.x);
        menuDrawReliveUpdate();
        break;

    default:
        break;
    }
    return 0;
}

// Open or close the sub-menu of the cursor page the window is on. The status
// menu itself is the sub-menu of the last page.
long _menuPokemonStatusChangeSubMenu(CURSOR cursor)
{
    u32 i;
    s32 id = 0;

    if (_menuStatus.mPokemon == 0)
        return -1;

    switch (cursor.y) {
    case 0: id = MENU_POKEMON_STATUS_MENU_TAB_SLOT0; break;
    case 1: id = MENU_POKEMON_STATUS_MENU_TAB_SLOT1; break;
    case 2: id = MENU_POKEMON_STATUS_MENU_WAZA; break;
    case 3: id = MENU_POKEMON_STATUS_MENU_RIBBON; break;
    default: break;
    }

    for (i = 0; i < 4; i++) {
        if ((s32)sub_menu_id[i] == id) {
            if (menuIsOpen(id) == 0)
                menuOpen(id, 0);
        } else if (menuIsOpen((s32)sub_menu_id[i]) != 0) {
            menuClose((s32)sub_menu_id[i]);
        }
    }
    return -1;
}

// Status menu loop: runs the menu until the window closes it and returns the
// result of the page the cursor was on.
long menuPokemonStatusExecute(u8 flags, FIGHT_TRAINER *trainer, Pokemon *pokemon, u16 wazaId,
                 MENU_POKEMON_CB callback, long callbackArg)
{
    tagMENU_ITEM_DATA *item;
    s32 cursor;
    s32 result;

    item = menuItemBiosGetPtr(MENU_POKEMON_STATUS_MENU_HEAD, MENU_POKEMON_STATUS_ITEM_MODEL);
    menuModelInit(&_menuModel, item->mWidth, item->mHeight);
    menuModelFree(&_menuModel);
    menuModelLoadPokemon(&_menuModel, pokemon);

    if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FLOOR_FADE) {
        fadeCheck(1);
        if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FAST_FADE)
            fadeSet(2, 0.25f);
        else
            fadeSet(2, 0.5f);
    }

    result = 0;
    cursor = 0;
    menuOffScreenSetPriority(0);
    for (;;) {
        result = menuOpenCustom(MENU_POKEMON_STATUS_MENU_HEAD, 0, &cursor, 0, 1, 0);
        if (result == -1) {
            _menuStatus.mResult = (s32)result;
            break;
        }
        _menuStatus.mResult = (s8)_menuStatus.mWazaSlot;
        if (!(_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_WAZA_LOOP))
            break;
        {
            u16 id;

            id = _menuPokemonStatusGetWazaDataID(_menuStatus.mPokemon,
                                                 (u16)_menuStatus.mWazaSlot);
            if (wazaGetStatus(0, id, WAZA_FIELD_HIDEN_FLAG, 0) != 0) {
                GSsndPlayNow(0x13);
                winMsgOpen(2, 0x3AFC, 1, 0);
                winMsgClose(1);
                cursor = (s8)_menuStatus.mWazaSlot;
            } else {
                break;
            }
        }
    }

    if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FLOOR_FADE) {
        fadeCheck(1);
        if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FAST_FADE)
            fadeSet(3, 0.25f);
        else
            fadeSet(3, 0.5f);
        fadeCheck(1);
    }
    menuPokemonStatusClose();
    menuModelTerm(&_menuModel);
    menuOffScreenSetPriority(1);
    GSthreadSwitch();
    return _menuStatus.mResult;
}

// Open the status menu for one pokemon. Waits for the pokemon models of the
// menu to be loaded, sets the work block up and runs the menu loop unless the
// menu is only being pushed.
long _menuPokemonStatusOpen(u8 type, FIGHT_TRAINER *trainer, Pokemon *pokemon, u16 waza,
                            MENU_POKEMON_CB callback, long callbackArg)
{
    s32 i;
    DarkPokemon *dark;

    while (menuFaceCheckLoading())
        GSthreadSwitch();

    memset(&_menuStatus, 0, sizeof(_menuStatus));
    _menuStatus.mFlags = type;
    _menuStatus.mTrainer = trainer;
    _menuStatus.mPokemon = pokemon;
    _menuStatus.mWazaId = waza;
    _menuStatus.mCallback = callback;
    _menuStatus.mCallbackArg = callbackArg;
    _menuStatus.mResult = -1;

    for (i = 0; i < 4; i++) {
        _menuStatus.mWazaNew[i] = 0;
        if (!pokemon->isDarkPokemon())
            continue;
        dark = pokemon->getDarkPokemon();
        if (dark == 0)
            continue;
        waza = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, (u8)i);
        switch ((s32)waza) {
        case 0:
        case WAZA_SHADOW_FIRST:
        case WAZA_SHADOW_FIRST + 1:
        case WAZA_SHADOW_FIRST + 2:
        case WAZA_SHADOW_FIRST + 3:
        case WAZA_SHADOW_FIRST + 4:
        case WAZA_SHADOW_FIRST + 5:
        case WAZA_SHADOW_FIRST + 6:
        case WAZA_SHADOW_FIRST + 7:
        case WAZA_SHADOW_FIRST + 8:
        case WAZA_SHADOW_FIRST + 9:
        case WAZA_SHADOW_FIRST + 10:
        case WAZA_SHADOW_FIRST + 11:
        case WAZA_SHADOW_FIRST + 12:
        case WAZA_SHADOW_FIRST + 13:
        case WAZA_SHADOW_FIRST + 14:
        case WAZA_SHADOW_FIRST + 15:
        case WAZA_SHADOW_FIRST + 16:
        case WAZA_SHADOW_LAST:
        case WAZA_SHADOW_SPECIAL:
            continue;
        default:
            break;
        }
        if (i == 0)
            continue;
        if (dark->isWazaNew((u8)(i - 1)) != 0)
            continue;
        _menuStatus.mWazaNew[i] = 1;
    }

    if (type & MENU_POKEMON_STATUS_FLAG_FIELD) {
        GSfloorPush(MENU_POKEMON_STATUS_FLOOR_ID);
        if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FLOOR_FADE)
            floorSetFadeScript(0, 0);
        floorWaitForChange();
    } else {
        menuPokemonStatusExecute(type, trainer, pokemon, waza, callback, callbackArg);
    }
    return _menuStatus.mResult;
}

// Close the status menu: fades the screen out, closes the sub-menus and
// gives the floor back.
void menuPokemonStatusClose(void)
{
    if ((_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FIELD) && !(_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FLOOR_FADE)) {
        fadeCheck(1);
        fadeSet(3, 0.5f);
        fadeCheck(1);
    }

    if (menuIsOpen(MENU_POKEMON_STATUS_MENU_TAB_SLOT0) != 0)
        menuCloseCustom(MENU_POKEMON_STATUS_MENU_TAB_SLOT0, 0, 0);
    if (menuIsOpen(MENU_POKEMON_STATUS_MENU_TAB_SLOT1) != 0)
        menuCloseCustom(MENU_POKEMON_STATUS_MENU_TAB_SLOT1, 0, 0);
    if (menuIsOpen(MENU_POKEMON_STATUS_MENU_WAZA) != 0)
        menuCloseCustom(MENU_POKEMON_STATUS_MENU_WAZA, 0, 0);
    if (menuIsOpen(MENU_POKEMON_STATUS_MENU_RIBBON) != 0)
        menuCloseCustom(MENU_POKEMON_STATUS_MENU_RIBBON, 0, 0);
    menuCloseCustom(MENU_POKEMON_STATUS_MENU_HEAD, 0, 1);

    if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FIELD) {
        GSfloorPop();
        if (_menuStatus.mFlags & MENU_POKEMON_STATUS_FLAG_FLOOR_FADE)
            floorSetFadeScript(0, 0);
    }
}

// Sequence hook of the status menu: runs the menu loop of the page the window
// is on.
void menuPokemonStatus(void)
{
    _menuStatus.mResult = menuPokemonStatusExecute(_menuStatus.mFlags,
                                                _menuStatus.mTrainer,
                                                _menuStatus.mPokemon,
                                                _menuStatus.mWazaId,
                                                _menuStatus.mCallback,
                                                _menuStatus.mCallbackArg);
}

// Hide the menus the waza page list covers, then open it.
long menuPokemonStatusOpenWazaLearnList(Pokemon *pokemon, u16 arg)
{
    s32 i;
    long result;
    s32 ids[4];

    for (i = 0; i < 4; i++)
        ids[i] = wazaHideMenuIDs[i];
    for (i = 0; i < 4; i++) {
        if (menuIsOpen(ids[i]) != 0)
            menuSetDisp(ids[i], 0);
    }
    result = _menuPokemonStatusOpen(MENU_POKEMON_STATUS_WAZA_LEARN_LIST, 0, pokemon, arg, 0, 0);
    for (i = 0; i < 4; i++) {
        if (menuIsOpen(ids[i]) != 0)
            menuSetDisp(ids[i], 1);
    }
    return result;
}

// Same, for the page the waza are learned on.
long menuPokemonStatusOpenWazaLearn(Pokemon *pokemon, u16 arg)
{
    s32 i;
    long result;
    s32 ids[4];

    for (i = 0; i < 4; i++)
        ids[i] = wazaLearnHideMenuIDs[i];
    for (i = 0; i < 4; i++) {
        if (menuIsOpen(ids[i]) != 0)
            menuSetDisp(ids[i], 0);
    }
    result = _menuPokemonStatusOpen(MENU_POKEMON_STATUS_WAZA_LEARN, 0, pokemon, arg, 0, 0);
    for (i = 0; i < 4; i++) {
        if (menuIsOpen(ids[i]) != 0)
            menuSetDisp(ids[i], 1);
    }
    return result;
}

// Status menu opened from the waza machine of the fight menu.
#pragma force_active on
long menuPokemonStatusOpenWazaMachine(Pokemon *pokemon, u16 arg)
{
    return _menuPokemonStatusOpen(MENU_POKEMON_STATUS_FIGHT_MENU, 0, pokemon, arg, 0, 0);
}
#pragma force_active reset

// Status menu opened from the waza list of the fight menu.
long menuPokemonStatusOpenWazaList(u8 slot)
{
    Pokemon *pokemon = 0;
    Hero *hero;

    if (slot < 6) {
        hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
        if (hero != 0) {
            pokemon = heroBiosGetPokemonPtr(hero, (u8)slot);
            if (pokemonCheckValid(pokemon) == 0)
                pokemon = 0;
        }
    }
    if (pokemon == 0)
        return -1;
    return _menuPokemonStatusOpen(MENU_POKEMON_STATUS_WAZA_LIST, 0, pokemon, 0, 0, 0);
}

// Status menu opened after the party order changed.
long menuPokemonStatusOpenFromPokemonChange(Pokemon *pokemon, MENU_POKEMON_CB callback, long arg)
{
    return _menuPokemonStatusOpen(MENU_POKEMON_STATUS_FROM_POKEMON_CHANGE, 0, pokemon, 0, callback, arg);
}

// Status menu opened from the PC box.
long menuPokemonStatusOpenFromPcBox(Pokemon *pokemon, MENU_POKEMON_CB callback, long arg)
{
    return _menuPokemonStatusOpen(MENU_POKEMON_STATUS_FROM_PC_BOX, 0, pokemon, 0, callback, arg);
}

// Status menu opened from the pokemon list of the fight menu.
long menuPokemonStatusOpenFightMenu(FIGHT_TRAINER *trainer, Pokemon *pokemon, MENU_POKEMON_CB callback, long arg)
{
    return _menuPokemonStatusOpen(MENU_POKEMON_STATUS_FIGHT_LIST, trainer, pokemon, 0, callback, arg);
}

// Status menu opened from the field menu.
long menuPokemonStatusOpenFieldMenu(Pokemon *pokemon, MENU_POKEMON_CB callback, long arg)
{
    return _menuPokemonStatusOpen(MENU_POKEMON_STATUS_FIELD_MENU, 0, pokemon, 0, callback, arg);
}

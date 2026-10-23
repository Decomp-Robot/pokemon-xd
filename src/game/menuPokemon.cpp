#include <string.h>
#include <textureBios.h>
#include <floor.h>
#include <GSmsg.h>
#include <fade.h>
#include <waza.h>
#include <menuWazaStatus.h>
#include <winSpriteDisplay.h>
#include <menu.h>
#include <menuPokemon.h>
#include <menuCBBios.h>
#include <fightTrainerPokemon.h>
#include <fightTrainerStatus.h>
#include <fightFloor.h>
#include <menuPocket2.h>
#include <menuSub.h>
#include <msgctrl.h>
#include <menuPokemonMessages.h>
#include <Pokemon.h>
#include <PokemonData.h>
#include <hero.h>
#include <heroBios.h>
#include <gsFlag.h>
#include <saveData.h>
#include <heroItem.h>
#define itemDataBiosGetWazaIDByWazaMachineNo itemDataBiosGetWazaIDByWazaMachineNo_header
#include <itemData.h>
#undef itemDataBiosGetWazaIDByWazaMachineNo
extern "C" u16 itemDataBiosGetWazaIDByWazaMachineNo(u8 machineNo);
#include <time.h>
#include <window.h>
#include <winMsg.h>
#include <menuOffScreen.h>
#include <menuItemBios.h>
#include <menuMoveSelect.h>
#include <winSeq.h>
#include <GSgfx.h>
#include <GSthread.h>
#include <sound.h>
#include <PokemonBios.h>
#include <libc/math.h>

// Party menus and panel callbacks.

// Party-menu messages.
enum {
    MSG_HELP_PARTY       = 0x3AC1, // "choose a pokemon."
    MSG_HELP_ITEM        = 0x3AC2, // name-dependent item help
    MSG_HELP_MOVE_SUB    = 0x3AC3,
    MSG_HELP_STATUS_SUB  = 0x3AC4,
    MSG_HELP_GIVE        = 0x3AC5,
    MSG_HELP_TAKE        = 0x3AC6,
    MSG_HELP_TM          = 0x3AC7,
    MSG_HELP_TM_SELECT   = 0x3C3A,

    MSG_TM_ABLE          = 0x3AC8, // pokemon can learn the move
    MSG_TM_CANT          = 0x3AC9, // pokemon cannot learn the move
    MSG_ITEM_PROMPT      = 0x3ACA, // give/take confirmation prompt
    MSG_ITEM_SWAPPED     = 0x3ACB, // held item swapped
    MSG_ITEM_GAVE        = 0x3ACC, // item given to the pokemon
    MSG_ITEM_TOOK        = 0x3ACD, // held item put away
    MSG_ITEM_HELD_NONE   = 0x3ACE, // pokemon holds no item
    MSG_ITEM_NOROOM      = 0x3ACF, // bag has no room
    MSG_TM_ALREADY       = 0x3C3C, // pokemon already knows the move

    MSG_HP_TEXT          = 0x4357,

    MSG_JOUTAI_LIST      = 0x3AE8,
    MSG_BATTLE_COMMAND   = 0x3BB4,
    MSG_FIELD_COMMAND    = 0x3C3E,
    MSG_SHADOW_HINT_A    = 0xC54C,
    MSG_SHADOW_HINT_B    = 0xC54D,
};

enum PartyFightCommand {
    PARTY_FIGHT_CHANGE = 0,
    PARTY_FIGHT_STATUS = 1,
    PARTY_FIGHT_CANCEL = 2,
};

enum PartyMenuKeyMask {
    PARTY_KEY_UP = 0x01,
    PARTY_KEY_DOWN = 0x02,
    PARTY_KEY_LEFT = 0x04,
    PARTY_KEY_RIGHT = 0x08,
    PARTY_KEY_CONFIRM = 0x10,
    PARTY_KEY_CANCEL = 0x20,
};

// Item pockets (itemDataBiosGetBagPocket).
enum { ITEM_POCKET_TM = 4 };

// Item ids the party panel's sprite hook dispatches on. The panel window
// assigns one id per sprite; ids that share a case label draw the same
// panel element (the second id belongs to the alternate panel layout).
enum {
    PANEL_ITEM_FACE_A    = 0x131, // caught-ball face icon
    PANEL_ITEM_BALL_A    = 0x132, // ball sprite, normal pokemon
    PANEL_ITEM_NAME_A    = 0x133, // pokemon name and gender mark
    PANEL_ITEM_LEVEL_A   = 0x135, // level text
    PANEL_ITEM_HPTEXT_A  = 0x136, // current/max HP text
    PANEL_ITEM_HPGAUGE_A = 0x138, // HP gauge texture
    PANEL_ITEM_HELD_A    = 0x13A, // held item icon
    PANEL_ITEM_JOUTAI_A  = 0x13B, // joutai condition icon
    PANEL_ITEM_BALL_B    = 0x13C, // ball sprite, normal pokemon
    PANEL_ITEM_BALL_C    = 0x13D, // ball sprite, shadow pokemon
    PANEL_ITEM_NAME_B    = 0x13E, // pokemon name and gender mark
    PANEL_ITEM_LEVEL_B   = 0x140, // level text
    PANEL_ITEM_HPTEXT_B  = 0x141, // current/max HP text
    PANEL_ITEM_HPGAUGE_B = 0x143, // HP gauge texture
    PANEL_ITEM_HELD_B    = 0x145, // held item icon
    PANEL_ITEM_JOUTAI_B  = 0x146, // joutai condition icon
    PANEL_ITEM_CURSOR_W1 = 0x156, // selection ball, white tint
    PANEL_ITEM_CURSOR_W2 = 0x157,
    PANEL_ITEM_CURSOR_B1 = 0x160, // selection ball, blue tint
    PANEL_ITEM_CURSOR_B2 = 0x161,
    PANEL_ITEM_SPARE_A1  = 0x134, // sprite ids the panel hook leaves untouched
    PANEL_ITEM_MOVE_COMPATIBILITY_A = 0x137, // TM/tutor compatibility text
    PANEL_ITEM_SPARE_A3  = 0x139,
    PANEL_ITEM_MOVE_COMPATIBILITY_B = 0x142, // TM/tutor compatibility text
    PANEL_ITEM_SPARE_B2  = 0x144,
    PANEL_ITEM_SIXTH     = 0x162, // sprite shown while no panel is selected
    PANEL_ITEM_COMMAND_A = 0x163, // sub-menu command sprites
    PANEL_ITEM_COMMAND_B = 0x164,
    PANEL_ITEM_BALL_D    = 0x1C4, // ball sprite, shadow pokemon
    PANEL_ITEM_FACE_B    = 0x2DD, // caught-ball face icon
    PANEL_ITEM_BALL_E    = 0x2CF, // ball sprite, hyper shadow
    PANEL_ITEM_BALL_F    = 0x2D0, // ball sprite, hyper shadow

    PANEL_HP_GAUGE_GREEN  = 0xF3,
    PANEL_HP_GAUGE_YELLOW = 0xF4,
    PANEL_HP_GAUGE_RED    = 0xF5,
};

enum {
    PARTY_WINDOW_PARAM_STATUS = 0,
    PARTY_WINDOW_PARAM_PARTY_INDEX = 1,
};

// Ball item ids the party panel can show for the caught-ball face icon.
enum {
    TUTOR_MOVE_INDEX_MAX = 12,
    PARTY_MOVE_SLOT_MAX = 4,
};

// Condition sprites shared by Pokemon's menu-status query.
const PokemonConditionMenuSprite pokemonConditionMenuSprites[] = {
    {POKEMON_CONDITION_POISON, CONDITION_SPRITE_POISON},
    {POKEMON_CONDITION_BAD_POISON, CONDITION_SPRITE_POISON},
    {POKEMON_CONDITION_BURN, CONDITION_SPRITE_BURN},
    {POKEMON_CONDITION_FREEZE, CONDITION_SPRITE_FREEZE},
    {POKEMON_CONDITION_PARALYSIS, CONDITION_SPRITE_PARALYSIS},
    {POKEMON_CONDITION_SLEEP, CONDITION_SPRITE_SLEEP},
    {POKEMON_CONDITION_NONE, 0},
};

// Party menu helpers with C++ linkage.
s32 _menuPokemonActionGive(s8 index, s16 bagSlot, u16 itemId, u16 *outItem);
bool _menuPokemonCheckItem(tagSPRITE_WORK *sprite, s8 panelState);
Pokemon *_menuPokemonStatusCB(Pokemon *pokemon, MENU_POKEMON_CB_TYPE type, long arg);
s32 _menuPokemonSub(unsigned char mode, unsigned long itemId, FIGHT_TRAINER *trainer);
s32 _menuPokemonOpen(unsigned char mode, unsigned char bagPocket, unsigned char bagSlot,
                     unsigned short itemId, FIGHT_TRAINER *trainer, bool listEnabled);
void *_menuPokemonCbLoadPokemonList(void *arg);
s32 _menuPokemonActionFight(signed char index);
s32 _menuPokemonActionField(signed char index);
u8 _menuPokemonMovePanel(long slot, unsigned char step);
s32 _menuPokemonActionFieldItem(signed char index);
s8 _menuPokemonActionStatus(signed char index, bool field);
s8 _menuPokemonOpenList(bool flag);
void _menuPokemonMakeStatus(MENU_POKEMON_STATUS_TBL *tbl);
Pokemon *_menuPokemonGetPokemonPtr(unsigned short index);
Hero *_menuPokemonGetHeroPtr(void);


// Party panels and the frame counter used to blink their icons.
MenuPokemonWork menuPokemonWork;
MENU_POKEMON_STATUS_TBL menuPokemonStatusTbl;
s16 partyMenuFrameCounter;

MENU_INFO menu_info[2][6] = {
    {
        {MENU_ID_PARTY_PRIMARY, PARTY_ICON_PRIMARY, 18, 82},
        {MENU_ID_PARTY_RESERVE_FIRST, PARTY_ICON_RESERVE_FIRST, 275, 38},
        {MENU_ID_PARTY_RESERVE_SECOND, PARTY_ICON_RESERVE_SECOND, 275, 105},
        {MENU_ID_PARTY_RESERVE_THIRD, PARTY_ICON_RESERVE_THIRD, 275, 172},
        {MENU_ID_PARTY_RESERVE_FOURTH, PARTY_ICON_RESERVE_FOURTH, 275, 239},
        {MENU_ID_PARTY_RESERVE_FIFTH, PARTY_ICON_RESERVE_FIFTH, 275, 306}
    },
    {
        {MENU_ID_PARTY_PRIMARY, PARTY_ICON_PRIMARY, 18, 68},
        {MENU_ID_PARTY_SECOND_PRIMARY, PARTY_ICON_SECOND_PRIMARY, 18, 234},
        {MENU_ID_PARTY_RESERVE_FIRST, PARTY_ICON_RESERVE_FIRST, 275, 49},
        {MENU_ID_PARTY_RESERVE_SECOND, PARTY_ICON_RESERVE_SECOND, 275, 131},
        {MENU_ID_PARTY_RESERVE_THIRD, PARTY_ICON_RESERVE_THIRD, 275, 215},
        {MENU_ID_PARTY_RESERVE_FOURTH, PARTY_ICON_RESERVE_FOURTH, 275, 297}
    }
};

enum PartyConditionPanelIndex {
    CONDITION_PANEL_HEALTHY = 0,
    CONDITION_PANEL_FAINTED = 1,
    CONDITION_PANEL_POISON = 2,
    CONDITION_PANEL_BURN = 3,
    CONDITION_PANEL_FREEZE = 4,
    CONDITION_PANEL_PARALYSIS = 5,
    CONDITION_PANEL_SLEEP = 6,
};

static inline u16 getJoutaiDispId(Pokemon *pokemon, bool sprite)
{
    static u16 menuPokemonConditionIconTable[7] = {CONDITION_ICON_NONE, CONDITION_ICON_FAINTED, CONDITION_ICON_POISON, CONDITION_ICON_BURN, CONDITION_ICON_FREEZE, CONDITION_ICON_PARALYSIS, CONDITION_ICON_SLEEP};
    static u16 menuPokemonConditionSpriteTable[7] = {CONDITION_SPRITE_NONE, CONDITION_SPRITE_FAINTED, CONDITION_SPRITE_POISON, CONDITION_SPRITE_BURN, CONDITION_SPRITE_FREEZE, CONDITION_SPRITE_PARALYSIS, CONDITION_SPRITE_SLEEP};
    u16 id;
    s32 joutaiStatus = pokemonGetStatus(pokemon, 0, POKEMON_QUERY_IS_FAINTED, 0);

    if (((bool)joutaiStatus) == true) {
        id = CONDITION_PANEL_FAINTED;
    } else {
        switch ((u16)pokemonGetJoutaiMenuSpriteId(pokemon)) {
        case CONDITION_SPRITE_POISON:
            id = CONDITION_PANEL_POISON;
            break;
        case CONDITION_SPRITE_BURN:
            id = CONDITION_PANEL_BURN;
            break;
        case CONDITION_SPRITE_FREEZE:
            id = CONDITION_PANEL_FREEZE;
            break;
        case CONDITION_SPRITE_PARALYSIS:
            id = CONDITION_PANEL_PARALYSIS;
            break;
        case CONDITION_SPRITE_SLEEP:
            id = CONDITION_PANEL_SLEEP;
            break;
        default:
            id = CONDITION_PANEL_HEALTHY;
            break;
        }
    }

    if (sprite == 0)
        return menuPokemonConditionIconTable[id];

    return menuPokemonConditionSpriteTable[id];
}

// Clears the party-menu work block before the menu is opened.
void menuPokemonInitWork(void)
{
    memset(&menuPokemonWork, 0, sizeof(menuPokemonWork));
}

// Menu ctrl hook of the party menu data record: nothing to do, report the step
// as unfinished.
s32 menuPokemonCtrlWaza(void)
{
    return 0;
}

// Closes the move-panel light window the party menu opened.
void menuPokemonCloseWazaLight(void)
{
    menuCloseCustom(MENU_ID_WAZA_LIGHT, 0, 1);
}

// Opens the move-select window over the pokemon's known moves and returns the
// chosen move slot.
s32 menuPokemonOpenWazaLight(s8 index)
{
    static MENU_WAZA_STATUS partyMenuMoveStatus;
    s32 selected;
    u32 moveIds[PARTY_MOVE_SLOT_MAX];
    u32 *dst;
    s32 count;
    Pokemon *pokemon = _menuPokemonGetPokemonPtr((u16)index);
    s32 i;

    if (pokemon == 0)
        return -1;

    pokemonToMenuWazaStatus(pokemon, &partyMenuMoveStatus);

    i = count = 0;
    dst = moveIds + i;
    while (i < PARTY_MOVE_SLOT_MAX) {
        s32 id = pokemonGetStatus(pokemon, 0, POKEMON_QUERY_MOVE_ID, i);

        if ((u16)id != 0) {
            *dst = wazaGetStatus(0, (u16)id, WAZA_STATUS_NAME_MESSAGE_ID, 0);
            count++;
        }

        dst++;
        i++;
    }

    GSgfxResetRestore(0);
    menuPokemonWork.mSelecting = 1;
    selected = menuSubOpenSelect2(0, moveIds, count, 0x1bd - menuPokemonMoveSelectXAdjustment, menuPokemonMoveSelectYAdjustment + 0x12c,
                                  0, count, 0);
    menuPokemonWork.mSelecting = 0;
    GSgfxResetRestore(1);
    return selected;
}

// Status-menu callback: steps the party cursor and returns the pokemon the
// status menu should show.
Pokemon *_menuPokemonStatusCB(Pokemon *pokemon, MENU_POKEMON_CB_TYPE type, long arg)
{
    s8 next = menuPokemonWork.mCursorIndex;

    switch (type) {
    case MENU_POKEMON_CB_PREV:
        next--;
        break;

    case MENU_POKEMON_CB_NEXT:
        next++;
        break;

    default:
        return pokemon;
    }

    if (next >= 6)
        next = 5;
    if (next < 0)
        next = 0;

    if (next != menuPokemonWork.mCursorIndex) {
        Pokemon *other = _menuPokemonGetPokemonPtr((u16)next);

        if (other != 0) {
            pokemon = other;
            menuPokemonWork.mCursorIndex = next;
        }
    }

    return pokemon;
}

// Sprite hook of the party panels: prints whether the selected pokemon can
// learn the TM or tutor move being used, then draws the panel element the
// sprite stands for.
void menuPokemonDrawItem(tagWINDOW_WORK *win, tagSPRITE_WORK *sprite)
{
    Pokemon *pokemon;
    s16 loopIndex;
    s16 width;
    s16 gaugeWidth;
    s16 tutorIndex;
    u16 gauge;
    u32 sexId;
    MENU_POKEMON_STATUS *info = (MENU_POKEMON_STATUS *)windowGetParam(win, PARTY_WINDOW_PARAM_STATUS);
    sexId = 0;
    u32 color;
    ItemData *item;
    u32 msgId;
    u16 partyIndex;
    s8 panelState;
    u8 machineNo;

    pokemon = 0;
    panelState = PARTY_PANEL_NORMAL;

    if (info == 0)
        return;

    if (info->mName[0] == 0)
        panelState = PARTY_PANEL_EMPTY;
    else if (info->mHp == 0)
        panelState = PARTY_PANEL_FAINTED;

    if (_menuPokemonCheckItem(sprite, panelState) == 0)
        return;

    color = menuSubCalcColor(win, sprite);
    partyIndex = (u16)windowGetParam(win, PARTY_WINDOW_PARAM_PARTY_INDEX);

    if (menuPokemonWork.mMode == PARTY_MODE_USE_ITEM_FIELD || menuPokemonWork.mMode == PARTY_MODE_USE_ITEM_FIGHT) {
        item = itemDataBiosGetPtr(menuPokemonWork.mItemId);

        if (itemDataBiosGetBagPocket(item) == ITEM_POCKET_TM) {
            switch (sprite->mItemID) {
            case PANEL_ITEM_MOVE_COMPATIBILITY_A:
            case PANEL_ITEM_MOVE_COMPATIBILITY_B: {
                machineNo = itemDataBiosGetWazaMachineNo(item);

                pokemon = _menuPokemonGetPokemonPtr(partyIndex);

                if (pokemonIsShadow(pokemon) != 0) {
                    msgId = MSG_TM_CANT;
                } else {
                    PokemonSpeciesStats *stats = pokemonDataGetSpeciesStats(
                        battlePokemonGetSpecies(pokemon));

                    if (pokemonDataBiosGetWazaMcn(stats, machineNo) == 0) {
                        msgId = MSG_TM_CANT;
                    } else {
                        u16 wazaId = itemDataBiosGetWazaIDByWazaMachineNo(machineNo);

                        for (loopIndex = 0; loopIndex < PARTY_MOVE_SLOT_MAX; loopIndex++) {
                            if ((u16)pokemonBiosGetPokemonWazaDataId(pokemon, loopIndex) == wazaId)
                                break;
                        }

                        if (loopIndex < PARTY_MOVE_SLOT_MAX)
                            msgId = MSG_TM_ALREADY;
                        else
                            msgId = MSG_TM_ABLE;
                    }
                }

                GSmsgPrint2(winSpriteGetLayerID(win, sprite), 0, -4, color, msgId);
            }
            case PANEL_ITEM_HPGAUGE_A:
            case PANEL_ITEM_HPGAUGE_B:
                winSpriteSetDisp(sprite, 0);
                return;
            }

            /* the remaining sprites draw their panel element below */
        }
    } else if (menuPokemonWork.mMode == PARTY_MODE_MOVE_TUTOR) {
        switch (sprite->mItemID) {
        case PANEL_ITEM_MOVE_COMPATIBILITY_A:
        case PANEL_ITEM_MOVE_COMPATIBILITY_B: {
            pokemon = _menuPokemonGetPokemonPtr(partyIndex);

            if (pokemonIsShadow(pokemon) != 0) {
                msgId = MSG_TM_CANT;
            } else if (pokemonDataCanLearnTutorMove(
                           battlePokemonGetSpecies(pokemon), menuPokemonWork.mItemId) == 0) {
                msgId = MSG_TM_CANT;
            } else {
                u8 order[12] = {7, 10, 2, 0, 5, 1, 4, 3, 6, 9, 11, 8};
                msgId = pokemonTutorMoveTable[order[menuPokemonWork.mItemId]].mMoveId;
                for (tutorIndex = 0; tutorIndex < PARTY_MOVE_SLOT_MAX; tutorIndex++) {
                    if ((u16)pokemonBiosGetPokemonWazaDataId(pokemon, tutorIndex) == msgId)
                        break;
                }

                if (tutorIndex < PARTY_MOVE_SLOT_MAX)
                    msgId = MSG_TM_ALREADY;
                else
                    msgId = MSG_TM_ABLE;
            }

            GSmsgPrint2(winSpriteGetLayerID(win, sprite), 0, -4, color, msgId);
        }
        case PANEL_ITEM_HPGAUGE_A:
        case PANEL_ITEM_HPGAUGE_B:
            winSpriteSetDisp(sprite, 0);
            return;
        }
    }

    switch (sprite->mItemID) {
    case PANEL_ITEM_BALL_A:
    case PANEL_ITEM_BALL_B:
        if (info->mBallType == POKEMON_BALL_TYPE_NORMAL)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case PANEL_ITEM_BALL_C:
    case PANEL_ITEM_BALL_D:
        if (info->mBallType == POKEMON_BALL_TYPE_SHADOW)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case PANEL_ITEM_BALL_E:
    case PANEL_ITEM_BALL_F:
        if (info->mBallType == POKEMON_BALL_TYPE_HYPER)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case PANEL_ITEM_JOUTAI_A:
    case PANEL_ITEM_JOUTAI_B:
        if (info->mJoutaiId != 0)
            textureBiosLoadSpriteId(0, 0, sprite->mWidth, sprite->mHeight, color, win,
                                    info->mJoutaiId, 0, winSpriteGetLayerID(win, sprite));
        return;

    case PANEL_ITEM_HELD_A:
    case PANEL_ITEM_HELD_B:
        if (info->mHeldItem != 0)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case PANEL_ITEM_HPTEXT_A:
    case PANEL_ITEM_HPTEXT_B:
        GSmsgPrint2(winSpriteGetLayerID(win, sprite), 0x2C, 0, color, MSG_JOUTAI_LIST);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, info->mHp);
        GSmsgPrint(winSpriteGetLayerID(win, sprite), 0, 0, 0x2A, sprite->mHeight, color,
                    MSG_HP_TEXT);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, info->mMaxHp);
        GSmsgPrint(winSpriteGetLayerID(win, sprite), 0, 0, sprite->mWidth - 2, sprite->mHeight,
                    color, MSG_HP_TEXT);
        return;

    case PANEL_ITEM_LEVEL_A:
    case PANEL_ITEM_LEVEL_B:
        msgctrlSetValue(MSGCTRL_NUM_VALUE, info->mLevel);
        GSmsgPrint(winSpriteGetLayerID(win, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color,
                    MSG_LEVEL_TEXT);
        return;

    case PANEL_ITEM_NAME_A:
    case PANEL_ITEM_NAME_B: {
        msgctrlSetValue(MSGCTRL_TEXT, (u32)info);
        GSmsgPrint2(winSpriteGetLayerID(win, sprite), 0, 0, color, MSG_POKEMON_NAME);
        width = (s16)((GSmsgGetMessageSize(MSG_POKEMON_NAME) >> 16) + 4);

        switch (info->mSex) {
        case POKEMON_SEX_MALE:
            sexId = MSG_SEX_MALE;
            break;
        case POKEMON_SEX_FEMALE:
            sexId = MSG_SEX_FEMALE;
            break;
        case POKEMON_SEX_NONE:
            sexId = 0;
            break;
        }

        if (sexId != 0) {
            msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(sexId));
            GSmsgPrint2(winSpriteGetLayerID(win, sprite), width - 2, 0, color, MSG_SEX_MARK);
        }
        return;
    }

    case PANEL_ITEM_FACE_A:
    case PANEL_ITEM_FACE_B:
        menuFaceDisplay(win, sprite, info->mFaceId);
        return;

    case PANEL_ITEM_HPGAUGE_A:
    case PANEL_ITEM_HPGAUGE_B: {
        s16 maxHp = info->mMaxHp;

        if (info->mHp <= 0)
            gauge = 0;
        else if (info->mHp <= (maxHp * 20) / 100)
            gauge = PANEL_HP_GAUGE_RED;
        else if (info->mHp <= (maxHp * 50) / 100)
            gauge = PANEL_HP_GAUGE_YELLOW;
        else
            gauge = PANEL_HP_GAUGE_GREEN;

        {
            gaugeWidth = (info->mHp * sprite->mWidth + (maxHp - 1)) / maxHp;

            if (gauge == 0)
                return;

            textureBiosLoadSpriteId(0, 0, gaugeWidth, sprite->mHeight, color, win, gauge, 0,
                                    winSpriteGetLayerID(win, sprite));
        }
        return;
    }

    case PANEL_ITEM_CURSOR_B1:
    case PANEL_ITEM_CURSOR_B2: {
        s32 windowId;
        s32 slot = menuPokemonWork.mRearrangeSourceSlot;

        if (slot >= 0 && slot < 6)
            windowId = menu_info[menuPokemonWork.mPanelLayout][slot].mWindowId;
        else
            windowId = 0;

        if (windowId == win->mMenuID) {
            s32 phase = partyMenuFrameCounter % 60;

            sprite->mColor = 0xFFFF00FF;
            if (phase < 30) {
                f32 t = sin(3.14f * ((f32)phase / 30.0f));

                sprite->mRgba.mAlpha -= (u8)(128.0f * t);
            }
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        return;
    }

    case PANEL_ITEM_CURSOR_W1:
    case PANEL_ITEM_CURSOR_W2: {
        s32 windowId;
        s32 cursor = menuPokemonWork.mCursorIndex;

        if (cursor >= 0 && cursor < 6)
            windowId = menu_info[menuPokemonWork.mPanelLayout][cursor].mWindowId;
        else
            windowId = 0;

        if (windowId == win->mMenuID) {
            s32 phase = partyMenuFrameCounter % 60;

            sprite->mColor = -1;
            if (phase < 30) {
                f32 t = sin(3.14f * ((f32)phase / 30.0f));

                sprite->mRgba.mAlpha -= (u8)(128.0f * t);
            }
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        return;
    }

    case PANEL_ITEM_SIXTH:
        if (menuPokemonWork.mCursorIndex == 6)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case PANEL_ITEM_COMMAND_A:
    case PANEL_ITEM_COMMAND_B:
        switch (menuPokemonWork.mSubMenu) {
        case PARTY_SUBMENU_LIST:
        case PARTY_SUBMENU_REARRANGE:
            winSpriteSetDisp(sprite, 1);

            if (sprite->mItemID == PANEL_ITEM_COMMAND_B) {
                s32 msgId;
                s32 msgSize;

                if (menuPokemonWork.mMode == PARTY_MODE_SELECT_PURIFICATION_READY || menuPokemonWork.mMode == PARTY_MODE_SELECT_SLOT)
                    msgId = MSG_BATTLE_COMMAND;
                else
                    msgId = MSG_FIELD_COMMAND;

                msgSize = GSmsgGetMessageSize(msgId) >> 16;
                GSmsgPrint2(winSpriteGetLayerID(win, sprite), (sprite->mWidth - msgSize) / 2, 0,
                            win->mRgba.mColorA | 0xFFFFFF00, msgId);
            }
            break;

        case PARTY_SUBMENU_COMMAND:
        case PARTY_SUBMENU_ITEM:
            winSpriteSetDisp(sprite, 0);
            break;
        }
        return;
    }
}

enum PartyCursorIndicator {
    PARTY_CURSOR_SLOT_0_VARIANT_A = 0x16E,
    PARTY_CURSOR_SLOT_1_VARIANT_A = 0x170,
    PARTY_CURSOR_SLOT_2_VARIANT_A = 0x172,
    PARTY_CURSOR_SLOT_3_VARIANT_A = 0x174,
    PARTY_CURSOR_SLOT_0_VARIANT_B = 0x191,
    PARTY_CURSOR_SLOT_1_VARIANT_B = 0x193,
    PARTY_CURSOR_SLOT_2_VARIANT_B = 0x195,
    PARTY_CURSOR_SLOT_1_VARIANT_C = 0x183,
    PARTY_CURSOR_SLOT_0_VARIANT_D = 0x1A8,
    PARTY_CURSOR_SLOT_2_VARIANT_D = 0x1AA,
    PARTY_CURSOR_SLOT_0_VARIANT_E = 0x1AC,
    PARTY_CURSOR_SLOT_1_VARIANT_E = 0x1AE,
    PARTY_CURSOR_SLOT_2_VARIANT_E = 0x1B0,
    PARTY_CURSOR_SLOT_0_VARIANT_F = 0x1BB,
    PARTY_CURSOR_SLOT_1_VARIANT_F = 0x1BD,
    PARTY_CURSOR_SLOT_2_VARIANT_F = 0x1BF,
};

// Cursor sprite hook: shows the cursor sprite belonging to the window the
// menu cursor currently sits on.
void menuPokemonDrawCur(tagWINDOW_WORK *win, tagSPRITE_WORK *sprite)
{
    winSpriteSetDisp(sprite, 0);

    switch (menuGetCursor(win->mMenuID)) {
    case 0:
        switch (sprite->mItemID) {
        case PARTY_CURSOR_SLOT_0_VARIANT_A:
        case PARTY_CURSOR_SLOT_0_VARIANT_B:
        case PARTY_CURSOR_SLOT_0_VARIANT_D:
        case PARTY_CURSOR_SLOT_0_VARIANT_E:
        case PARTY_CURSOR_SLOT_0_VARIANT_F:
            winSpriteSetDisp(sprite, 1);
            break;
        }
        break;

    case 1:
        switch (sprite->mItemID) {
        case PARTY_CURSOR_SLOT_1_VARIANT_A:
        case PARTY_CURSOR_SLOT_1_VARIANT_C:
        case PARTY_CURSOR_SLOT_1_VARIANT_B:
        case PARTY_CURSOR_SLOT_1_VARIANT_E:
        case PARTY_CURSOR_SLOT_1_VARIANT_F:
            winSpriteSetDisp(sprite, 1);
            break;
        }
        break;

    case 2:
        switch (sprite->mItemID) {
        case PARTY_CURSOR_SLOT_2_VARIANT_A:
        case PARTY_CURSOR_SLOT_2_VARIANT_B:
        case PARTY_CURSOR_SLOT_2_VARIANT_D:
        case PARTY_CURSOR_SLOT_2_VARIANT_E:
        case PARTY_CURSOR_SLOT_2_VARIANT_F:
            winSpriteSetDisp(sprite, 1);
            break;
        }
        break;

    case 3:
        switch (sprite->mItemID) {
        case PARTY_CURSOR_SLOT_3_VARIANT_A:
            winSpriteSetDisp(sprite, 1);
            break;
        }
        break;
    }
}

// Shows or hides a panel's data sprites for an empty slot, and tints the ball
// sprites of a pokemon that cannot act.
bool _menuPokemonCheckItem(tagSPRITE_WORK *sprite, s8 panelState)
{
    bool disp = true;
    u32 tint = -1;

    if (panelState == PARTY_PANEL_EMPTY) {
        switch (sprite->mItemID) {
        case PANEL_ITEM_FACE_A:
        case PANEL_ITEM_NAME_A:
        case PANEL_ITEM_LEVEL_A:
        case PANEL_ITEM_HPTEXT_A:
        case PANEL_ITEM_MOVE_COMPATIBILITY_A:
        case PANEL_ITEM_HPGAUGE_A:
        case PANEL_ITEM_SPARE_A3:
        case PANEL_ITEM_HELD_A:
        case PANEL_ITEM_JOUTAI_A:
        case PANEL_ITEM_NAME_B:
        case PANEL_ITEM_LEVEL_B:
        case PANEL_ITEM_HPTEXT_B:
        case PANEL_ITEM_MOVE_COMPATIBILITY_B:
        case PANEL_ITEM_HPGAUGE_B:
        case PANEL_ITEM_SPARE_B2:
        case PANEL_ITEM_HELD_B:
        case PANEL_ITEM_JOUTAI_B:
        case PANEL_ITEM_FACE_B:
            disp = 0;
            break;
        }
    } else if (panelState == PARTY_PANEL_FAINTED) {
        switch (sprite->mItemID) {
        case PANEL_ITEM_FACE_A:
        case PANEL_ITEM_BALL_A:
        case PANEL_ITEM_BALL_B:
        case PANEL_ITEM_BALL_C:
        case PANEL_ITEM_BALL_D:
        case PANEL_ITEM_BALL_E:
        case PANEL_ITEM_BALL_F:
        case PANEL_ITEM_FACE_B:
            tint = 0x808080FF;
            break;
        }
    }

    winSpriteSetDisp(sprite, disp);
    sprite->mColor = tint;
    return disp;
}

// Help message hook of the party menu windows.
void menuPokemonDrawHelp(tagWINDOW_WORK *win, tagSPRITE_WORK *sprite)
{
    u32 msgId = 0;

    switch (menuPokemonWork.mSubMenu) {
    case PARTY_SUBMENU_LIST:
        switch (menuPokemonWork.mMode) {
        case PARTY_MODE_MOVE_TUTOR:
            msgId = MSG_HELP_TM;
            break;

        case PARTY_MODE_USE_ITEM_FIELD:
        case PARTY_MODE_USE_ITEM_FIGHT: {
            ItemData *item = itemDataBiosGetPtr(menuPokemonWork.mItemId);

            if (itemDataBiosGetBagPocket(item) == ITEM_POCKET_TM)
                msgId = MSG_HELP_TM;
            else if (menuPokemonWork.mSelecting != 0)
                msgId = MSG_HELP_TM_SELECT;
            else
                msgId = MSG_HELP_GIVE;
            break;
        }

        case PARTY_MODE_GIVE_ITEM:
            msgId = MSG_HELP_TAKE;
            break;

        case PARTY_MODE_FIGHT:
        case PARTY_MODE_FIELD:
        default:
            msgId = MSG_HELP_PARTY;
            break;
        }
        break;

    case PARTY_SUBMENU_COMMAND:
        msgctrlSetValue(MSGCTRL_POKEMON_NAME,
                        (u32)&menuPokemonStatusTbl.mStatus[menuPokemonWork.mCursorIndex]);
        msgId = MSG_HELP_ITEM;
        break;

    case PARTY_SUBMENU_REARRANGE:
        msgId = MSG_HELP_MOVE_SUB;
        break;

    case PARTY_SUBMENU_ITEM:
        msgId = MSG_HELP_STATUS_SUB;
        break;
    }

    if (msgId != 0)
        GSmsgPrint2(winSpriteGetLayerID(win, sprite), 0, 0, win->mRgba.mColorA | 0xFFFFFF00,
                    msgId);
}

// Decide/cancel handling of the party list window.
void menuPokemonButton(tagWINDOW_WORK *win)
{
    u8 decide = 0;

    if (menuPokemonWork.mListEnabled == 0) {
        win->mAccept = 1;
        return;
    }

    if ((u8)winSeqCheckMove(MENU_ID_PARTY_PRIMARY) != 0)
        return;

    {
        u16 keys = windowGetKeyTrg();

        if (keys & PARTY_KEY_CONFIRM) {
            if ((u16)menuGetCursorItemID(win->mMenuID) == PANEL_ITEM_SIXTH)
                decide = 1;
        } else if (keys & PARTY_KEY_CANCEL) {
            decide = 1;
        }
    }

    if (decide != 0) {
        if (menuPokemonWork.mFightFlag == 0) {
            GSsndPlayNow(MENU_SE_ERROR);
        } else {
            win->mAccept = 1;
            win->mCancel = 1;
        }
    } else {
        menuButtonNormal(win);
    }
}

// Cursor movement over the party panels: pokemon slots first, then the sub
// panels, skipping empty party slots.
void menuPokemonCursor(tagWINDOW_WORK *win)
{
    u8 freeSlots[7];
    s8 cursor = win->mCursorState.mSelection.mCursor;
    s8 moved = 0;
    s32 i;

    for (i = 0; i < 6; i++) {
        if (_menuPokemonGetPokemonPtr(i) != 0)
            freeSlots[i] = 1;
        else
            freeSlots[i] = 0;
    }
    freeSlots[i] = 1;

    {
        u16 keys = windowGetKeyRepeat();

        if (keys & PARTY_KEY_RIGHT) {
            moved = 1;

            if (menuPokemonWork.mPanelLayout == PARTY_LAYOUT_ONE_PRIMARY_FIVE_RESERVE) {
                if (cursor == 0) {
                    if (menuPokemonWork.mCursor == 3)
                        cursor = menuPokemonWork.mCursor;
                    else
                        cursor = 2;
                }
            } else {
                if (cursor == 0) {
                    if (menuPokemonWork.mCursor == 3)
                        cursor = menuPokemonWork.mCursor;
                    else
                        cursor = 2;
                } else if (cursor == 1) {
                    if (menuPokemonWork.mCursor == 5)
                        cursor = menuPokemonWork.mCursor;
                    else
                        cursor = 4;
                }
            }
        } else if (keys & PARTY_KEY_LEFT) {
            moved = -1;

            if (menuPokemonWork.mPanelLayout == PARTY_LAYOUT_ONE_PRIMARY_FIVE_RESERVE) {
                switch (cursor) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    cursor = 0;
                    break;
                }
            } else if (menuPokemonWork.mPanelLayout == PARTY_LAYOUT_TWO_PRIMARY_FOUR_RESERVE) {
                switch (cursor) {
                case 2:
                case 3:
                    cursor = 0;
                    break;

                case 4:
                case 5:
                    cursor = 1;
                    break;
                }
            }
        }

        if (keys & PARTY_KEY_UP) {
            cursor--;
            moved = -1;

            if (cursor < 0)
                cursor = 6;
        } else if (keys & PARTY_KEY_DOWN) {
            moved = 1;

            s32 max = menuDataBiosGetDispMax(win->mMenuID);
            cursor++;
            if (cursor >= (u8)max)
                cursor = 0;
        }
    }

    if (moved != 0) {
        u8 *slot = &freeSlots[cursor];

        while (*slot == 0) {
            cursor += moved;
            slot += moved;

            if (cursor < 0 || cursor >= 7) {
                cursor = 7;
                break;
            }
        }
    }

    if (win->mCursorState.mSelection.mCursor != cursor) {
        menuPokemonWork.mCursor = win->mCursorState.mSelection.mCursor;
        menuPokemonWork.mCursorIndex = cursor;
        menuPokemonWork.mNextIndex = cursor;
        win->mCursorState.mSelection.mCursor = cursor;
    }
}

// Opens the party panel windows on the first frame and closes them on the
// last; also accumulates the frame fraction the panel icons blink with.
s32 menuPokemonCtrl(tagWINDOW_WORK *win)
{
    MENU_POKEMON_STATUS *info = (MENU_POKEMON_STATUS *)windowGetParam(win, PARTY_WINDOW_PARAM_STATUS);

    if (info == 0)
        return 0;

    GSgfxResetRestore(0);

    if (win->mState == WINDOW_SEQUENCE_OPEN) {
        win->cursorTo = -1;

        if (win->mFlag == 0) {
            s32 i;

            for (i = 0; i < 6; i++) {
                menuItemSetPosition(win->mMenuID, menu_info[menuPokemonWork.mPanelLayout][i].mSlotIcon,
                            menu_info[menuPokemonWork.mPanelLayout][i].mX,
                            menu_info[menuPokemonWork.mPanelLayout][i].mY);
                menuDataBiosSetXY(menu_info[menuPokemonWork.mPanelLayout][i].mWindowId,
                                  menu_info[menuPokemonWork.mPanelLayout][i].mX,
                                  menu_info[menuPokemonWork.mPanelLayout][i].mY);
                menuOpenCustom(menu_info[menuPokemonWork.mPanelLayout][i].mWindowId,
                               MENU_ID_PARTY_LIST, 0, 0, 0, 2, info + i, i);
                _menuPokemonMovePanel(i, PARTY_PANEL_SHOW);
            }

            winSeqSetMenu(win->mMenuID, PARTY_LIST_TARGET_OPEN);
            win->mFlag = 1;
        }

        GSgfxResetRestore(0);
    } else if (win->mState == WINDOW_SEQUENCE_CLOSE && win->mFlag == 0) {
        s32 i;

        for (i = 0; i < 6; i++)
            _menuPokemonMovePanel(i, PARTY_PANEL_HIDE);

        winSeqSetMenu(win->mMenuID, PARTY_LIST_TARGET_CLOSE);
        win->mFlag = 1;
    }

    partyMenuFrameCounter = (s16)((s32)(timeGetLastFrameSec() * 60.0f + partyMenuFrameCounter) % 1000);
    return 0;
}

// Tears the party menu down and hands control back to the floor.
void menuPokemonClose(void)
{
    menuCloseCustom(MENU_ID_PARTY_LIST, 0, 1);

    if (menuPokemonWork.mState != PARTY_OPEN_OVERLAY) {
        GSgfxResetRestore(1);
        GSfloorPop();

        if (menuPokemonWork.mFadeState != PARTY_FADE_FIGHT)
            floorSetFadeScript(0, 0);

        floorWaitForChange();
    }
}

// Runs the party list until an action reports completion.
s32 _menuPokemonSub(unsigned char mode, unsigned long itemId, FIGHT_TRAINER *trainer)
{
    static u16 partyMenuLoadCounter;
    s32 result;
    u16 i;

    menuPokemonWork.mCursorIndex = menuPokemonWork.mNextIndex;

    if (menuPokemonWork.mNextIndex < 0)
        menuPokemonWork.mCursorIndex = 0;

    menuPokemonWork.mRearrangeSourceSlot = -1;

    if (menuPokemonWork.mListEnabled == 0)
        menuPokemonWork.mCursorIndex = -1;

    menuPokemonRefreshStatus();
    partyMenuLoadCounter = 0;
    menuFaceLoadPokemon(0, _menuPokemonCbLoadPokemonList, &partyMenuLoadCounter);

    for (i = 0; i < 6; i++) {
        if (_menuPokemonGetPokemonPtr(i) == 0)
            break;
    }

    if (i <= menuPokemonWork.mCursorIndex)
        menuPokemonWork.mCursorIndex = i - 1;

    for (;;) {
        s8 selected;

        menuPokemonWork.mSubMenu = PARTY_SUBMENU_LIST;
        selected = _menuPokemonOpenList(1);
        menuPokemonWork.mCursorIndex = selected;

        if (selected == PARTY_LIST_DISABLED)
            break;

        if (menuGetCursorItemID(MENU_ID_PARTY_LIST) == PANEL_ITEM_COMMAND_A || selected == PARTY_LIST_CANCELLED) {
            menuPokemonWork.mCursorIndex = -1;
            break;
        }

        switch (menuPokemonWork.mMode) {
        case PARTY_MODE_FIELD:
            result = _menuPokemonActionField(selected);
            break;

        case PARTY_MODE_FIGHT:
            result = _menuPokemonActionFight(selected);
            break;

        case PARTY_MODE_USE_ITEM_FIELD:
        case PARTY_MODE_USE_ITEM_FIGHT:
        case PARTY_MODE_MOVE_TUTOR:
            result = -1;
            break;

        case PARTY_MODE_GIVE_ITEM: {
            u16 outItem;

            result = _menuPokemonActionGive(selected, menuPokemonWork.mBagSlot,
                                         menuPokemonWork.mItemId, &outItem);
            break;
        }

        case PARTY_MODE_SELECT_SLOT:
            result = -1;
            break;

        case PARTY_MODE_SELECT_PURIFICATION_READY: {
            Pokemon *pokemon = _menuPokemonGetPokemonPtr((u16)selected);

            if (pokemon == 0)
                continue;

            if (pokemonIsShadow(pokemon) == 0) {
                winMsgOpen(2, MSG_SHADOW_HINT_A, 1, 0);
                winMsgClose(1);
                continue;
            }

            if (pokemonGetDp(pokemon) != 0.0f) {
                winMsgOpen(2, MSG_SHADOW_HINT_B, 1, 0);
                winMsgClose(1);
                continue;
            }

            result = -1;
            break;
        }

        default:
            break;
        }

        if (result < 0)
            break;
    }

    while (menuFaceCheckLoading() != 0)
        GSthreadSwitch();

    menuPokemonWork.mResult = menuPokemonWork.mCursorIndex;
    return result;
}

// Opens the party menu in the given mode.
s32 _menuPokemonOpen(unsigned char mode, unsigned char bagPocket, unsigned char bagSlot,
                     unsigned short itemId, FIGHT_TRAINER *trainer, bool listEnabled)
{
    menuPokemonWork.mMode = mode;
    menuPokemonWork.mBagPocket = bagPocket;
    menuPokemonWork.mBagSlot = bagSlot;
    menuPokemonWork.mItemId = itemId;
    menuPokemonWork.mTrainer = trainer;
    menuPokemonWork.mListEnabled = listEnabled;
    menuPokemonWork.mResult = 0;
    menuPokemonWork.mFightFlag = 1;
    menuPokemonWork.mSelecting = 0;

    switch ((s32)mode) {
    case PARTY_MODE_USE_ITEM_FIGHT:
    case PARTY_MODE_FIGHT:
        menuPokemonWork.mPartySource = PARTY_SOURCE_FIGHT_TRAINER;
        break;

    case PARTY_MODE_USE_ITEM_FIELD:
    default:
        menuPokemonWork.mPartySource = PARTY_SOURCE_HERO;
        break;
    }

    switch ((s32)menuPokemonWork.mMode) {
    case PARTY_MODE_USE_ITEM_FIELD:
    case PARTY_MODE_USE_ITEM_FIGHT:
    case PARTY_MODE_GIVE_ITEM:
        GSgfxResetRestore(0);
        menuPokemonWork.mState = PARTY_OPEN_OVERLAY;
        menuPokemonWork.mFadeState = PARTY_FADE_NORMAL;
        break;

    case PARTY_MODE_FIELD:
        menuPokemonWork.mState = PARTY_OPEN_FLOOR;
        menuPokemonWork.mFadeState = PARTY_FADE_NORMAL;
        break;

    case PARTY_MODE_FIGHT:
        menuPokemonWork.mState = PARTY_OPEN_FLOOR;
        menuPokemonWork.mFadeState = PARTY_FADE_FIGHT;

        if (itemId == 0)
            menuPokemonWork.mFightFlag = 0;

        /* fall through */

    case PARTY_MODE_SELECT_SLOT:
    case PARTY_MODE_SELECT_PURIFICATION_READY:
    case PARTY_MODE_MOVE_TUTOR:
    default:
        menuCreateOffScreen(20.0f / (f32)GSgfxVideoGetVsyncRate(), 50.0f);
        while (menuDisableOffScreenCheck() == 0)
            GSthreadSwitch();
        menuPokemonWork.mState = PARTY_OPEN_FLOOR;
        menuPokemonWork.mFadeState = PARTY_FADE_OFFSCREEN;
        break;
    }

    if (menuPokemonWork.mState == PARTY_OPEN_FLOOR) {
        GSfloorPush(FLOOR_ID_PARTY_MENU);

        if (menuPokemonWork.mFadeState != PARTY_FADE_FIGHT)
            floorSetFadeScript(0, 0);

        floorWaitForChange();

        if (menuPokemonWork.mFadeState == PARTY_FADE_OFFSCREEN)
            menuReleaseOffScreen(20.0f / (f32)GSgfxVideoGetVsyncRate());
    } else {
        _menuPokemonSub(mode, itemId, trainer);
    }

    switch (menuPokemonWork.mMode) {
    case PARTY_MODE_USE_ITEM_FIELD:
    case PARTY_MODE_USE_ITEM_FIGHT:
    case PARTY_MODE_GIVE_ITEM:
        GSgfxResetRestore(1);
        break;
    }

    menuPokemonWork.mPanelLayout = PARTY_LAYOUT_TWO_PRIMARY_FOUR_RESERVE;
    return menuPokemonWork.mResult;
}

s32 menuPokemonOpenItemGive(unsigned char bagPocket, unsigned char bagSlot, unsigned short itemId,
                             FIGHT_TRAINER *trainer)
{
    menuPokemonWork.mPanelLayout = PARTY_LAYOUT_TWO_PRIMARY_FOUR_RESERVE;
    return _menuPokemonOpen(PARTY_MODE_GIVE_ITEM, bagPocket, bagSlot, itemId, trainer, 1);
}

s32 menuPokemonOpenItemUse(unsigned char mode, bool listEnabled, unsigned short itemId,
                            FIGHT_TRAINER *trainer)
{
    menuPokemonWork.mPanelLayout = PARTY_LAYOUT_TWO_PRIMARY_FOUR_RESERVE;
    return _menuPokemonOpen(mode, 0, 0, itemId, trainer, listEnabled);
}

s32 menuPokemonOpenFight(unsigned char flag, unsigned char battleSelection, FIGHT_TRAINER *trainer,
                          FightOutPokemon *outgoingPokemon)
{
    if (flag == 1)
        menuPokemonWork.mPanelLayout = PARTY_LAYOUT_ONE_PRIMARY_FIVE_RESERVE;
    else
        menuPokemonWork.mPanelLayout = PARTY_LAYOUT_TWO_PRIMARY_FOUR_RESERVE;

    menuPokemonWork.mOutgoingPokemon = outgoingPokemon;
    return _menuPokemonOpen(PARTY_MODE_FIGHT, 0, 0, battleSelection, trainer, 1);
}

s32 menuPokemonOpen(unsigned char mode, unsigned short itemId, FIGHT_TRAINER *trainer)
{
    menuPokemonWork.mPanelLayout = PARTY_LAYOUT_TWO_PRIMARY_FOUR_RESERVE;
    return _menuPokemonOpen(mode, 0, 0, itemId, trainer, 1);
}

#pragma force_active on
s32 menuPokemonMain(void)
{
    GSgfxResetRestore(0);
    _menuPokemonSub(menuPokemonWork.mMode, menuPokemonWork.mItemId, menuPokemonWork.mTrainer);
    menuPokemonClose();
    GSgfxResetRestore(1);
    return 0;
}

#pragma force_active reset

// Face-load callback: fetches the pokemon of the current counter slot and
// advances the counter.
void *_menuPokemonCbLoadPokemonList(void *arg)
{
    u16 *counter = (u16 *)arg;
    void *pokemon = _menuPokemonGetPokemonPtr(*counter);

    (*counter)++;
    return pokemon;
}

// Gives the item to the pokemon, takes the held item back, or swaps the two;
// reports the result in the party message window.
s32 _menuPokemonActionGive(s8 index, s16 bagSlot, u16 itemId, u16 *outItem)
{
    s32 answer;
    u16 old;
    u16 held;
    Pokemon *pokemon = _menuPokemonGetPokemonPtr((u16)index);
    Hero *hero;

    msgctrlSetValue(MSGCTRL_POKEMON_NAME,
                    (u32)&menuPokemonStatusTbl.mStatus[menuPokemonWork.mCursorIndex]);
    held = battlePokemonGetHeldItem(pokemon);
    hero = _menuPokemonGetHeroPtr();

    if (hero == 0)
        return -1;

    if (itemId == 0) {
        if (held != 0) {
            if (heroItemCheckAddItemDataId(hero, held) <= 0) {
                winMsgOpen(2, MSG_ITEM_NOROOM, 1, 1);
            } else {
                heroItemAddItemDataId(hero, held, 1, -1);
                old = pokemonDoItemSoubi(pokemon, 0, 0);

                if (outItem != 0)
                    *outItem = old;

                msgctrlSetValue(MSGCTRL_NUM_ITEM, old);
                winMsgOpen(2, MSG_ITEM_TOOK, 1, 1);
            }
        } else {
            winMsgOpen(2, MSG_ITEM_HELD_NONE, 1, 1);
        }
    } else if (held != 0) {
        msgctrlSetValue(MSGCTRL_NUM_ITEM, held);
        winMsgOpen(2, MSG_ITEM_PROMPT, 1, 0);

        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);

        if (answer != 0)
            return -1;

        if (heroItemDecItemDataId(hero, itemId, 1, bagSlot) != 0) {
            winMsgOpen(2, MSG_ITEM_NOROOM, 1, 1);
        } else if (heroItemCheckAddItemDataId(hero, held) <= 0) {
            heroItemAddItemDataId(hero, itemId, 1, -1);
            winMsgOpen(2, MSG_ITEM_NOROOM, 1, 1);
        } else {
            heroItemAddItemDataId(hero, held, 1, -1);
            old = pokemonDoItemSoubi(pokemon, 0, 0);

            if (outItem != 0)
                *outItem = old;

            pokemonDoItemSoubi(pokemon, itemId, 1);
            msgctrlSetValue(MSGCTRL_NUM_ITEM, old);
            msgctrlSetValue(MSGCTRL_NUM_SWAP_ITEM, itemId);
            winMsgOpen(2, MSG_ITEM_SWAPPED, 1, 0);
        }
    } else {
        if (heroItemDecItemDataId(hero, itemId, 1, bagSlot) == 0) {
            pokemonDoItemSoubi(pokemon, itemId, 1);
            msgctrlSetValue(MSGCTRL_NUM_ITEM, itemId);
            winMsgOpen(2, MSG_ITEM_GAVE, 1, 0);
        }
    }

    winMsgClose(1);
    menuPokemonRefreshStatus();
    return -1;
}

// Opens the party fight menu and handles its commands.
s32 _menuPokemonActionFight(signed char index)
{
    s32 step;

    menuPokemonWork.mSubMenu = PARTY_SUBMENU_COMMAND;

    for (;;) {
        if (menuPokemonWork.mFightFlag != 0) {
            step = menuOpen(MENU_ID_PARTY_FIGHT_A, 1);
            menuClose(MENU_ID_PARTY_FIGHT_A);
        } else {
            step = menuOpen(MENU_ID_PARTY_FIGHT_B, 1);
            menuClose(MENU_ID_PARTY_FIGHT_B);
        }

        switch (step) {
        case PARTY_FIGHT_CHANGE:
            if (menuPokemonCheckPokemonChange(menuPokemonWork.mOutgoingPokemon,
                                              menuPokemonWork.mTrainer, index) != 0)
                step = -1;
            else
                step = 1;
            break;

        case PARTY_FIGHT_STATUS:
            index = _menuPokemonActionStatus(index, 0);
            continue;

        case PARTY_FIGHT_CANCEL:
        case -1:
            step = 1;
            break;

        default:
            break;
        }

        break;
    }

    return step;
}

// Opens the field menu for the active hero, closes it, and yields the command.
static inline s32 fieldMenuOpenClose(s32 id)
{
    s32 sel = menuOpen(id, 1);
    menuClose(id);
    return sel;
}

// Opens the party field menu and handles its commands.
s32 _menuPokemonActionField(signed char index)
{
    s32 command;
    long target;
    Pokemon *from;
    Pokemon *to;

    for (;;) {
        menuPokemonWork.mSubMenu = PARTY_SUBMENU_COMMAND;

        command = GSflagGet(HERO_PTR_FLAG) == 0 ? fieldMenuOpenClose(MENU_ID_PARTY_FIELD_A)
                                            : fieldMenuOpenClose(MENU_ID_PARTY_FIELD_B);

        switch (command) {
        case PARTY_COMMAND_STATUS_A:
        case PARTY_COMMAND_STATUS_B:
            index = _menuPokemonActionStatus(index, 1);
            command = 0;
            break;

        case PARTY_COMMAND_REARRANGE_A:
        case PARTY_COMMAND_REARRANGE_B: {
            menuPokemonWork.mRearrangeSourceSlot = index;
            menuPokemonWork.mSubMenu = PARTY_SUBMENU_REARRANGE;
            target = _menuPokemonOpenList(1);
            menuPokemonWork.mRearrangeSourceSlot = -1;

            if (menuGetCursorItemID(MENU_ID_PARTY_LIST) != PANEL_ITEM_COMMAND_A && target != PARTY_LIST_CANCELLED) {
                from = _menuPokemonGetPokemonPtr((u16)index);
                to = _menuPokemonGetPokemonPtr((u16)target);

                if (from != 0 && from != 0) {
                    _menuPokemonMovePanel(index, PARTY_PANEL_HIDE);
                    _menuPokemonMovePanel(target, PARTY_PANEL_HIDE);

                    while (_menuPokemonMovePanel(index, PARTY_PANEL_CHECK_MOVE) != 0)
                        GSthreadSwitch();

                    pokemonReplace(from, to);
                    menuPokemonRefreshStatus();
                    menuSubWait(0.25f);

                    _menuPokemonMovePanel(index, PARTY_PANEL_SHOW);
                    _menuPokemonMovePanel(target, PARTY_PANEL_SHOW);

                    while (_menuPokemonMovePanel(index, PARTY_PANEL_CHECK_MOVE) != 0)
                        GSthreadSwitch();
                }
            }

            command = -1;
            break;
        }

        case PARTY_COMMAND_ITEM:
            command = _menuPokemonActionFieldItem(index);
            command = command == -1 ? 0 : -1;
            break;

        case -1:
        case PARTY_COMMAND_CANCEL_A:
        case PARTY_COMMAND_CANCEL_B:
            command = -1;
            break;
        }

        if (command == -1)
            break;
    }

    return 0;
}

// Starts the party panel show or hide sequence, and
// reports whether the slide is still running.
u8 _menuPokemonMovePanel(long slot, unsigned char step)
{
    s32 windowId = menu_info[menuPokemonWork.mPanelLayout][slot].mWindowId;
    s16 x;
    s16 y;

    menuDataBiosGetXY(windowId, &x, &y);

    switch (step) {
    case PARTY_PANEL_SHOW:
        winSeqSetMenu(windowId,
                      x > PARTY_PANEL_TARGET_SPLIT_X ? PARTY_PANEL_TARGET_RIGHT_SHOW
                                                     : PARTY_PANEL_TARGET_LEFT_SHOW);
        break;

    case PARTY_PANEL_HIDE:
        winSeqSetMenu(windowId,
                      x > PARTY_PANEL_TARGET_SPLIT_X ? PARTY_PANEL_TARGET_RIGHT_HIDE
                                                     : PARTY_PANEL_TARGET_LEFT_HIDE);
        break;

    case PARTY_PANEL_CHECK_MOVE:
        return winSeqCheckMove(windowId);

    default:
        break;
    }

    return 1;
}

// Opens the held-item command menu and applies the choice.
s32 _menuPokemonActionFieldItem(signed char index)
{
    s32 sel;
    u16 item;
    u16 old;

    menuPokemonWork.mSubMenu = PARTY_SUBMENU_ITEM;
    sel = menuOpen(MENU_ID_PARTY_HELD_ITEM, 1);
    menuClose(MENU_ID_PARTY_HELD_ITEM);

    switch (sel) {
    case PARTY_HELD_ITEM_GIVE: {
        u8 slotId;

        menuClose(MENU_ID_PARTY_LIST);
        item = menuPocket2Call(POCKET_MODE_GIVE, 0, 0);
        slotId = menuPocket2GetLastSlotID();
        _menuPokemonOpenList(0);

        if ((u16)item == 0)
            return -1;

        _menuPokemonActionGive(index, slotId, item, &old);
        sel = 0;
        break;
    }

    case PARTY_HELD_ITEM_TAKE:
        _menuPokemonActionGive(index, -1, 0, &old);
        sel = 0;
        break;

    case PARTY_HELD_ITEM_CANCEL:
    case PARTY_HELD_ITEM_DISMISSED:
        sel = -1;
        break;

    default:
        break;
    }

    return sel;
}

// Opens the pokemon status menu over the party menu.
s8 _menuPokemonActionStatus(signed char index, bool field)
{
    Pokemon *pokemon;

    menuCloseCustom(MENU_ID_PARTY_LIST, 0, 1);
    pokemon = _menuPokemonGetPokemonPtr((u16)index);

    fadeSet(FADE_OUT_BLACK, 0.25f);
    fadeCheck(1);
    GSgfxResetRestore(0);

    if (field)
        menuPokemonStatusOpenFieldMenu(pokemon, _menuPokemonStatusCB, 0);
    else
        menuPokemonStatusOpenFightMenu(menuPokemonWork.mTrainer, pokemon,
                                   _menuPokemonStatusCB, 0);

    GSgfxResetRestore(1);
    _menuPokemonOpenList(0);

    fadeSet(FADE_IN_BLACK, 0.25f);
    fadeCheck(1);

    return menuPokemonWork.mCursorIndex;
}

// Opens the party list; with the flag cleared the list auto-confirms.
s8 _menuPokemonOpenList(bool flag)
{
    s32 cursor = menuPokemonWork.mCursorIndex;
    s8 result = (s8)menuOpenCustom(MENU_ID_PARTY_LIST, 0, &cursor, 0, flag, 1,
                                   &menuPokemonStatusTbl);

    if (flag == 0) {
        tagWINDOW_WORK *win = windowSearchID(MENU_ID_PARTY_LIST);

        if (win != 0)
            win->mAccept = 1;

        result = (s8)cursor;
    }

    if (menuPokemonWork.mListEnabled == 0)
        result = PARTY_LIST_DISABLED;

    return result;
}

// Fills the party panel record table from the current party.
void _menuPokemonMakeStatus(MENU_POKEMON_STATUS_TBL *tbl)
{
    MENU_POKEMON_STATUS *status;
    Pokemon *pokemon;
    u16 i;

    memset(tbl, 0, sizeof(*tbl));

    for (i = 0; i < 6; i++) {
        status = &tbl->mStatus[i];
        pokemon = _menuPokemonGetPokemonPtr(i);

        if (pokemon == 0) {
            status->mName[0] = 0;
            continue;
        }

        pokemonToMenuPokemonStatus(pokemon, 0, status, -1);

        {
            s32 joutaiStatus = pokemonGetStatus(pokemon, 0, POKEMON_QUERY_IS_FAINTED, 0);

            if (((bool)joutaiStatus) == true)
                status->mHp = 0;
        }

        status->mJoutaiId = menuPokemonGetJoutaiSpriteID(pokemon, 0);
    }
}

// Refresh the party panel records.
void menuPokemonRefreshStatus(void)
{
    _menuPokemonMakeStatus(&menuPokemonStatusTbl);
}


// Select the condition or fainted icon for the requested panel layout.
u16 menuPokemonGetJoutaiSpriteID(Pokemon *pokemon, bool sprite)
{
    return getJoutaiDispId(pokemon, sprite);
}
// Pokemon list the menu currently reads: the hero party, the battle trainer's
// party, or none.
Pokemon *_menuPokemonGetPokemonPtr(unsigned short index)
{
    FIGHT_TRAINER *trainer = menuPokemonWork.mTrainer;
    Hero *hero;
    Pokemon *pokemon = 0;

    switch (menuPokemonWork.mPartySource) {
    case PARTY_SOURCE_HERO: {
        if (index >= 6)
            return 0;

        hero = _menuPokemonGetHeroPtr();

        if (hero == 0)
            return 0;

        pokemon = heroBiosGetPokemonPtr(hero, index);
        break;
    }

    case PARTY_SOURCE_FIGHT_TRAINER: {
        FightPokemon *trainerPokemon;

        if (index >= 6)
            return 0;

        if (trainer == 0) {
            fightFloorGetGcHeroFightTrainerPtr(0);
            return 0;
        }

        trainerPokemon = fightTrainerGetFightPokemonPtrValid(trainer, index);

        if (trainerPokemon == 0)
            return 0;

        pokemon = (Pokemon *)pokemonGetStatus(trainerPokemon, 0, POKEMON_QUERY_PARTY_RECORD, 0);
        break;
    }

    case PARTY_SOURCE_INACTIVE_30_SLOT:
        if (index >= 30)
            return 0;

        pokemon = 0;
        break;
    }

    if ((u8)pokemonCheckValid(pokemon) == 0)
        return 0;

    return pokemon;
}

// Hero bios whose party and inventory the menu reads.
Hero *_menuPokemonGetHeroPtr(void)
{
    FIGHT_TRAINER *trainer = menuPokemonWork.mTrainer;

    switch (menuPokemonWork.mPartySource) {
    case PARTY_SOURCE_HERO:
        if (GSflagGet(HERO_PTR_FLAG) == 0)
            return (Hero *)savedataGetStatus(0, TRAINER_DATA_HERO);

        return menuCBBios_GetBattleHeroPtr();

    case PARTY_SOURCE_FIGHT_TRAINER:
        if (trainer == 0) {
            fightFloorGetGcHeroFightTrainerPtr(0);
            return 0;
        }

        return (Hero *)fightTrainerGetStatus(trainer, 0, FIGHT_TRAINER_QUERY_HERO, 0);
    }

    return 0;
}

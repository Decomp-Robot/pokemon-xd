#include <types.h>
#include <window.h>
#include <winSprite.h>
#include <menu.h>
#include <menuFight.h>
#include <menuItemBios.h>
#include <menuSub.h>
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
#include <GSsndPlayback.h>
#include <app/fight/fightTimer/fightTimer.h>
#include <app/fight/fightKind.h>
#include <app/fight/fightTrainerBios.h>
#include <fightTrainerDB.h>
#include <fightPokemonBios.h>
#include <fightSeq.h>
#include <hero.h>

// Message templates the fight menu windows print.
enum {
    MSG_NO_VALUE_TEXT = 15094,
    MSG_TEXT_IN_RECT = 17017,
    MSG_SLASH_TEXT = 17092,
    MSG_PP_LABEL = 17105,
    MSG_LEVEL_LABEL = 17108,
    MSG_HP_LABEL = 17109,
    MSG_ABILITY_LABEL = 17242,
    MSG_KANE = 17230,
    MSG_PP_SLASH = 17090,
    MSG_PP_NUMBER = 17240,
};

// Item ids of the rows of the secret move window. Each of the four move slots
// has a name, a type, a PP and a spare row.
enum {
    SECRET_WAZA_ITEM_NAME = 87,
    SECRET_WAZA_ITEM_TYPE = 92,
    SECRET_WAZA_ITEM_PP = 96,
    SECRET_WAZA_ITEM_SPARE = 100,
};

// Menu ids of the fight menu windows this module drives.
enum {
    MENU_FIGHT_SECRET_SELECT = 146,
    MENU_FIGHT_SECRET_WAZA_DETAIL = 147,
    MENU_FIGHT_SECRET_WAZA_LIST = 148,
    MENU_FIGHT_SECRET_POKEMON_LIST = 149,
    MENU_FIGHT_SECRET_POKEMON_DETAIL = 150,
    MENU_FIGHT_TOTAL_TIMER = 156,
    MENU_FIGHT_COUNTDOWN = 157,
    MENU_FIGHT_IWAIT_ENEMY_STATUS = 162,
    MENU_FIGHT_TARGET = 160,
    MENU_FIGHT_TARGET_B = 163,
    MENU_FIGHT_TIMEUP = 267,
};

// Sound the countdown timer plays on each remaining second.
enum {
    MENU_SE_COUNTDOWN_TICK = 1089,
};

// Key bits that move the secret pokemon window's cursor onto the last two
// party slots.
enum {
    MENU_FIGHT_KEY_PARTY_SLOT_FIVE = 0x80,
    MENU_FIGHT_KEY_PARTY_SLOT_SIX = 0x40,
};

// Sprite items of the three move windows that share menuFightDrawSecretWazaDoc.
// Each window draws the same ten rows at its own item ids. The rows of the
// move slots follow the window layout, not the slot order.
enum {
    WAZA_DOC_ITEM_TYPE = 121,
    WAZA_DOC_ITEM_MOVE_0 = 122,
    WAZA_DOC_ITEM_MOVE_1 = 123,
    WAZA_DOC_ITEM_MOVE_2 = 125,
    WAZA_DOC_ITEM_MOVE_3 = 124,
    WAZA_DOC_ITEM_POWER = 126,
    WAZA_DOC_ITEM_ACCURACY = 127,
    WAZA_DOC_ITEM_NAME = 128,
    WAZA_DOC_ITEM_DESCRIPTION = 129,
    WAZA_DOC_ITEM_PP = 130,

    WAZA_DOC_ITEM_TYPE_B = 594,
    WAZA_DOC_ITEM_MOVE_0_B = 595,
    WAZA_DOC_ITEM_MOVE_1_B = 596,
    WAZA_DOC_ITEM_MOVE_2_B = 598,
    WAZA_DOC_ITEM_MOVE_3_B = 597,
    WAZA_DOC_ITEM_POWER_B = 601,
    WAZA_DOC_ITEM_ACCURACY_B = 602,
    WAZA_DOC_ITEM_NAME_B = 599,
    WAZA_DOC_ITEM_DESCRIPTION_B = 603,
    WAZA_DOC_ITEM_PP_B = 600,

    WAZA_DOC_ITEM_TYPE_C = 616,
    WAZA_DOC_ITEM_MOVE_0_C = 617,
    WAZA_DOC_ITEM_MOVE_1_C = 618,
    WAZA_DOC_ITEM_MOVE_2_C = 620,
    WAZA_DOC_ITEM_MOVE_3_C = 619,
    WAZA_DOC_ITEM_POWER_C = 623,
    WAZA_DOC_ITEM_ACCURACY_C = 624,
    WAZA_DOC_ITEM_NAME_C = 621,
    WAZA_DOC_ITEM_DESCRIPTION_C = 625,
    WAZA_DOC_ITEM_PP_C = 622,
};

// Sprite items of the secret command window: the two label rows its draw hook
// shows and hides with the window.
enum {
    SECRET_SELECT_ITEM_LABEL_0 = 33,
    SECRET_SELECT_ITEM_LABEL_1 = 35,
};

// Sprite items of the secret pokemon list window: one nickname, one HP and
// one condition row per party slot, plus a spare row.
enum {
    SECRET_POKEMON_ITEM_NICKNAME_0 = 141,
    SECRET_POKEMON_ITEM_NICKNAME_1 = 142,
    SECRET_POKEMON_ITEM_NICKNAME_2 = 143,
    SECRET_POKEMON_ITEM_NICKNAME_3 = 144,
    SECRET_POKEMON_ITEM_NICKNAME_4 = 145,
    SECRET_POKEMON_ITEM_NICKNAME_5 = 146,

    SECRET_POKEMON_ITEM_SPARE = 147,

    SECRET_POKEMON_ITEM_HP_0 = 148,
    SECRET_POKEMON_ITEM_HP_1 = 149,
    SECRET_POKEMON_ITEM_HP_2 = 150,
    SECRET_POKEMON_ITEM_HP_3 = 151,
    SECRET_POKEMON_ITEM_HP_4 = 152,
    SECRET_POKEMON_ITEM_HP_5 = 153,
    SECRET_POKEMON_ITEM_STATE_0 = 154,
    SECRET_POKEMON_ITEM_STATE_1 = 155,
    SECRET_POKEMON_ITEM_STATE_2 = 156,
    SECRET_POKEMON_ITEM_STATE_3 = 157,
    SECRET_POKEMON_ITEM_STATE_4 = 158,
    SECRET_POKEMON_ITEM_STATE_5 = 159,
};

// Sprite items of the secret pokemon status window: one row per move slot for
// the move name, the type icon, the PP and the known flag, the cursor rows of
// the six party slots and the field rows of the pokemon.
enum {
    SECRET_POKEMON_STATUS_ITEM_MOVE_0_NAME = 692,
    SECRET_POKEMON_STATUS_ITEM_MOVE_0_TYPE = 696,
    SECRET_POKEMON_STATUS_ITEM_MOVE_0_PP = 700,
    SECRET_POKEMON_STATUS_ITEM_MOVE_1_NAME = 693,
    SECRET_POKEMON_STATUS_ITEM_MOVE_1_TYPE = 697,
    SECRET_POKEMON_STATUS_ITEM_MOVE_1_PP = 701,
    SECRET_POKEMON_STATUS_ITEM_MOVE_2_NAME = 694,
    SECRET_POKEMON_STATUS_ITEM_MOVE_2_TYPE = 698,
    SECRET_POKEMON_STATUS_ITEM_MOVE_2_PP = 702,
    SECRET_POKEMON_STATUS_ITEM_MOVE_3_NAME = 695,
    SECRET_POKEMON_STATUS_ITEM_MOVE_3_TYPE = 699,
    SECRET_POKEMON_STATUS_ITEM_MOVE_3_PP = 703,

    SECRET_POKEMON_STATUS_ITEM_TYPE_FIRST = 704,
    SECRET_POKEMON_STATUS_ITEM_TYPE_SECOND = 705,
    SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_0 = 706,
    SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_1 = 707,
    SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_3 = 708,
    SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_2 = 709,
    SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_5 = 710,
    SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_4 = 711,
    SECRET_POKEMON_STATUS_ITEM_NICKNAME = 712,
    SECRET_POKEMON_STATUS_ITEM_SPECIES_NAME = 713,
    SECRET_POKEMON_STATUS_ITEM_ABILITY_NAME = 714,
    SECRET_POKEMON_STATUS_ITEM_HP = 715,
    SECRET_POKEMON_STATUS_ITEM_LEVEL = 716,
    SECRET_POKEMON_STATUS_ITEM_HELD_ITEM_NAME = 717,
    SECRET_POKEMON_STATUS_ITEM_HELD_ITEM_ICON = 718,
    SECRET_POKEMON_STATUS_ITEM_MOVE_0_KNOWN = 719,
    SECRET_POKEMON_STATUS_ITEM_MOVE_1_KNOWN = 720,
    SECRET_POKEMON_STATUS_ITEM_MOVE_2_KNOWN = 721,
    SECRET_POKEMON_STATUS_ITEM_MOVE_3_KNOWN = 722,
};

// Sprite items of the two target windows: the cursor and the target name row of
// each panel. The second window repeats both rows at its own item ids.
enum {
    TARGET_CURSOR_LEFT = 528,
    TARGET_CURSOR_UP = 529,
    TARGET_CURSOR_DOWN = 530,
    TARGET_CURSOR_RIGHT = 531,
    TARGET_TEXT_LEFT = 533,
    TARGET_TEXT_UP = 534,
    TARGET_TEXT_DOWN = 535,
    TARGET_TEXT_RIGHT = 536,

    TARGET_CURSOR_LEFT_B = 563,
    TARGET_CURSOR_UP_B = 564,
    TARGET_CURSOR_DOWN_B = 565,
    TARGET_CURSOR_RIGHT_B = 566,
    TARGET_TEXT_LEFT_B = 568,
    TARGET_TEXT_UP_B = 569,
    TARGET_TEXT_DOWN_B = 570,
    TARGET_TEXT_RIGHT_B = 571,
};

// Sprite items of the markers the target window draws over the picked panel.
enum {
    TARGET_MARKER_RIGHT = 63,
    TARGET_MARKER_UP = 64,
    TARGET_MARKER_DOWN = 65,
    TARGET_MARKER_LEFT = 66,
};

// Sprite items of the two pokemon status windows the iwait hook draws. Both
// windows show the same rows for their two pokemon; the second window repeats
// them at its own item ids and has no fight-kind row.
enum {
    IWAIT_ITEM_POKEMON0_TYPE_FIRST = 447,
    IWAIT_ITEM_POKEMON0_TYPE_SECOND = 448,
    IWAIT_ITEM_POKEMON0_HELD_ICON = 449,
    IWAIT_ITEM_POKEMON0_NAME = 450,
    IWAIT_ITEM_POKEMON0_ABILITY = 451,
    IWAIT_ITEM_POKEMON0_HELD_NAME = 452,
    IWAIT_ITEM_POKEMON0_MOVE_0 = 453,
    IWAIT_ITEM_POKEMON0_MOVE_1 = 454,
    IWAIT_ITEM_POKEMON0_MOVE_2 = 455,
    IWAIT_ITEM_POKEMON0_MOVE_3 = 456,
    IWAIT_ITEM_POKEMON1_TYPE_FIRST = 457,
    IWAIT_ITEM_POKEMON1_TYPE_SECOND = 458,
    IWAIT_ITEM_POKEMON1_HELD_ICON = 459,
    IWAIT_ITEM_POKEMON1_NAME = 460,
    IWAIT_ITEM_POKEMON1_ABILITY = 461,
    IWAIT_ITEM_POKEMON1_HELD_NAME = 462,
    IWAIT_ITEM_POKEMON1_MOVE_0 = 463,
    IWAIT_ITEM_POKEMON1_MOVE_1 = 464,
    IWAIT_ITEM_POKEMON1_MOVE_2 = 465,
    IWAIT_ITEM_POKEMON1_MOVE_3 = 466,
    IWAIT_ITEM_FIGHT_KIND = 467,

    IWAIT_ITEM_POKEMON0_TYPE_FIRST_B = 492,
    IWAIT_ITEM_POKEMON0_TYPE_SECOND_B = 493,
    IWAIT_ITEM_POKEMON0_HELD_ICON_B = 494,
    IWAIT_ITEM_POKEMON0_NAME_B = 495,
    IWAIT_ITEM_POKEMON0_ABILITY_B = 496,
    IWAIT_ITEM_POKEMON0_HELD_NAME_B = 497,
    IWAIT_ITEM_POKEMON0_MOVE_0_B = 498,
    IWAIT_ITEM_POKEMON0_MOVE_1_B = 499,
    IWAIT_ITEM_POKEMON0_MOVE_2_B = 500,
    IWAIT_ITEM_POKEMON0_MOVE_3_B = 501,
    IWAIT_ITEM_POKEMON1_TYPE_FIRST_B = 502,
    IWAIT_ITEM_POKEMON1_TYPE_SECOND_B = 503,
    IWAIT_ITEM_POKEMON1_HELD_ICON_B = 504,
    IWAIT_ITEM_POKEMON1_NAME_B = 505,
    IWAIT_ITEM_POKEMON1_ABILITY_B = 506,
    IWAIT_ITEM_POKEMON1_HELD_NAME_B = 507,
    IWAIT_ITEM_POKEMON1_MOVE_0_B = 508,
    IWAIT_ITEM_POKEMON1_MOVE_1_B = 509,
    IWAIT_ITEM_POKEMON1_MOVE_2_B = 510,
    IWAIT_ITEM_POKEMON1_MOVE_3_B = 511,
};

// Sprite items of the two pokemon status windows the IPokemonStatus hook draws.
// Both windows show the same rows; the second window repeats them at its own
// item ids. The PP and move-name rows cover the four move slots.
enum {
    IPOKEMON_STATUS_ITEM_NICKNAME = 332,
    IPOKEMON_STATUS_ITEM_ABILITY = 333,
    IPOKEMON_STATUS_ITEM_HELD_ITEM_NAME = 334,
    IPOKEMON_STATUS_ITEM_MOVE_0_NAME = 335,
    IPOKEMON_STATUS_ITEM_MOVE_0_PP = 336,
    IPOKEMON_STATUS_ITEM_MOVE_1_NAME = 337,
    IPOKEMON_STATUS_ITEM_MOVE_1_PP = 338,
    IPOKEMON_STATUS_ITEM_MOVE_2_NAME = 339,
    IPOKEMON_STATUS_ITEM_MOVE_2_PP = 340,
    IPOKEMON_STATUS_ITEM_MOVE_3_NAME = 341,
    IPOKEMON_STATUS_ITEM_MOVE_3_PP = 342,
    IPOKEMON_STATUS_ITEM_TARGET_MARKER_0 = 344,
    IPOKEMON_STATUS_ITEM_TARGET_MARKER_1 = 349,
    IPOKEMON_STATUS_ITEM_HELD_ITEM_ICON = 350,
    IPOKEMON_STATUS_ITEM_TYPE_FIRST = 351,
    IPOKEMON_STATUS_ITEM_TYPE_SECOND = 352,
    IPOKEMON_STATUS_ITEM_MOVE_0_TYPE = 353,
    IPOKEMON_STATUS_ITEM_MOVE_1_TYPE = 354,
    IPOKEMON_STATUS_ITEM_MOVE_2_TYPE = 355,
    IPOKEMON_STATUS_ITEM_MOVE_3_TYPE = 356,

    IPOKEMON_STATUS_ITEM_NICKNAME_B = 385,
    IPOKEMON_STATUS_ITEM_ABILITY_B = 386,
    IPOKEMON_STATUS_ITEM_HELD_ITEM_NAME_B = 387,
    IPOKEMON_STATUS_ITEM_MOVE_0_NAME_B = 388,
    IPOKEMON_STATUS_ITEM_MOVE_0_PP_B = 389,
    IPOKEMON_STATUS_ITEM_MOVE_1_NAME_B = 390,
    IPOKEMON_STATUS_ITEM_MOVE_1_PP_B = 391,
    IPOKEMON_STATUS_ITEM_MOVE_2_NAME_B = 392,
    IPOKEMON_STATUS_ITEM_MOVE_2_PP_B = 393,
    IPOKEMON_STATUS_ITEM_MOVE_3_NAME_B = 394,
    IPOKEMON_STATUS_ITEM_MOVE_3_PP_B = 395,
    IPOKEMON_STATUS_ITEM_TARGET_MARKER_0_B = 397,
    IPOKEMON_STATUS_ITEM_TARGET_MARKER_1_B = 402,
    IPOKEMON_STATUS_ITEM_HELD_ITEM_ICON_B = 403,
    IPOKEMON_STATUS_ITEM_TYPE_FIRST_B = 404,
    IPOKEMON_STATUS_ITEM_TYPE_SECOND_B = 405,
    IPOKEMON_STATUS_ITEM_MOVE_0_TYPE_B = 406,
    IPOKEMON_STATUS_ITEM_MOVE_1_TYPE_B = 407,
    IPOKEMON_STATUS_ITEM_MOVE_2_TYPE_B = 408,
    IPOKEMON_STATUS_ITEM_MOVE_3_TYPE_B = 409,
};

// Sprite items of the two timer windows: the blinking separator of the total
// timer, its minute and second digits, and the countdown number.
enum {
    TIMER_ITEM_SEPARATOR_0 = 167,
    TIMER_ITEM_SEPARATOR_1 = 168,
    TIMER_ITEM_SEPARATOR_2 = 169,
    TIMER_ITEM_SEPARATOR_3 = 170,
    TIMER_ITEM_MINUTES = 172,
    TIMER_ITEM_SECONDS = 173,
    TIMER_ITEM_COUNT = 175,
};

// Sprite items of the turn window: the turn number and the cursor rows of the
// two rows the window shows.
enum {
    TURN_ITEM_COUNT = 2,
    TURN_ITEM_CURSOR_ROW_0_VARIANT_A = 636,
    TURN_ITEM_CURSOR_ROW_0_VARIANT_B = 661,
    TURN_ITEM_CURSOR_ROW_0_VARIANT_C = 723,
    TURN_ITEM_CURSOR_ROW_1_VARIANT_A = 637,
    TURN_ITEM_CURSOR_ROW_1_VARIANT_B = 662,
    TURN_ITEM_CURSOR_ROW_1_VARIANT_C = 724,
};

// Sprite item of the money row of the fight menu.
enum {
    KANE_ITEM_VALUE = 6,
};

// Fight kind whose status window draws the alternate fight-kind icon, and the
// two icons themselves.
enum {
    FIGHT_KIND_ALTERNATE_ICON = 17,
    SPRITE_KIND_ICON = 490,
    SPRITE_KIND_ICON_ALTERNATE = 1015,
};

// Status flags of the fight menu.
u32 menuFightStatus;

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

    if (menuIsCheck(MENU_FIGHT_SECRET_WAZA_LIST))
        openMenuID = MENU_FIGHT_SECRET_WAZA_LIST;
    else if (menuIsCheck(MENU_FIGHT_SECRET_WAZA_DETAIL))
        openMenuID = MENU_FIGHT_SECRET_WAZA_DETAIL;

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

        nextMenuID = (waza_no < 0) ? MENU_FIGHT_SECRET_WAZA_LIST
                                          : MENU_FIGHT_SECRET_WAZA_DETAIL;
    }

    menuFightButtonTimeOutCheck(window);
    if (openMenuID != nextMenuID) {
        if (openMenuID != 0)
            menuClose(openMenuID);
        if (!window->mAccept) {
            if (nextMenuID == MENU_FIGHT_SECRET_WAZA_LIST) {
                menuOpenCustom(MENU_FIGHT_SECRET_WAZA_LIST, window->mMenuID, 0, 0, false, 1,
                    window->mMenuID);
                menuSetDisp(MENU_FIGHT_SECRET_WAZA_LIST, true);
                menuSetDisp(MENU_FIGHT_SECRET_WAZA_DETAIL, false);
            } else if (nextMenuID == MENU_FIGHT_SECRET_WAZA_DETAIL) {
                menuOpenCustom(MENU_FIGHT_SECRET_WAZA_DETAIL, window->mMenuID, 0, 0, false, 2,
                    window->mMenuID, &waza_no);
                menuSetDisp(MENU_FIGHT_SECRET_WAZA_DETAIL, true);
                menuSetDisp(MENU_FIGHT_SECRET_WAZA_LIST, false);
            }
        }
    }
}

// Draw hook of the secret move window rows. Each of the four move slots has a
// name, a type, a PP and a spare row.
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
    case SECRET_WAZA_ITEM_NAME:
    case SECRET_WAZA_ITEM_TYPE:
    case SECRET_WAZA_ITEM_PP:
    case SECRET_WAZA_ITEM_SPARE:
        index = 0;
        break;
    case SECRET_WAZA_ITEM_NAME + 1:
    case SECRET_WAZA_ITEM_TYPE + 1:
    case SECRET_WAZA_ITEM_PP + 1:
    case SECRET_WAZA_ITEM_SPARE + 1:
        index = 1;
        break;
    case SECRET_WAZA_ITEM_NAME + 2:
    case SECRET_WAZA_ITEM_TYPE + 2:
    case SECRET_WAZA_ITEM_PP + 2:
    case SECRET_WAZA_ITEM_SPARE + 2:
        index = 2;
        break;
    case SECRET_WAZA_ITEM_NAME + 3:
    case SECRET_WAZA_ITEM_TYPE + 3:
    case SECRET_WAZA_ITEM_PP + 3:
    case SECRET_WAZA_ITEM_SPARE + 3:
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
    case SECRET_WAZA_ITEM_TYPE:
    case SECRET_WAZA_ITEM_TYPE + 1:
    case SECRET_WAZA_ITEM_TYPE + 2:
    case SECRET_WAZA_ITEM_TYPE + 3: {
        u16 type = status->moves[index].type;

        windowDrawSprite(0, 0, window, zokuseiBiosGetSpriteID(type), 0,
            winSpriteGetLayerID(window, sprite));
        break;
    }
    case SECRET_WAZA_ITEM_PP:
    case SECRET_WAZA_ITEM_PP + 1:
    case SECRET_WAZA_ITEM_PP + 2:
    case SECRET_WAZA_ITEM_PP + 3: {
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
    case SECRET_WAZA_ITEM_NAME:
    case SECRET_WAZA_ITEM_NAME + 1:
    case SECRET_WAZA_ITEM_NAME + 2:
    case SECRET_WAZA_ITEM_NAME + 3:
        msgctrlSetValue(MSGCTRL_TEXT, (u32)status->moves[index].moveName);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color,
            MSG_POKEMON_NAME);
        break;
    case SECRET_WAZA_ITEM_SPARE:
    case SECRET_WAZA_ITEM_SPARE + 1:
    case SECRET_WAZA_ITEM_SPARE + 2:
    case SECRET_WAZA_ITEM_SPARE + 3:
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
    case WAZA_DOC_ITEM_MOVE_0: case WAZA_DOC_ITEM_MOVE_1: case WAZA_DOC_ITEM_MOVE_3: case WAZA_DOC_ITEM_MOVE_2:
        switch (sprite->mItemID) {
        case WAZA_DOC_ITEM_MOVE_0: sel = 0; break;
        case WAZA_DOC_ITEM_MOVE_1: sel = 1; break;
        case WAZA_DOC_ITEM_MOVE_2: sel = 2; break;
        case WAZA_DOC_ITEM_MOVE_3: sel = 3; break;
        }
        if (sel == *pSelect)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case WAZA_DOC_ITEM_MOVE_0_B: case WAZA_DOC_ITEM_MOVE_1_B: case WAZA_DOC_ITEM_MOVE_3_B: case WAZA_DOC_ITEM_MOVE_2_B:
        switch (sprite->mItemID) {
        case WAZA_DOC_ITEM_MOVE_0_B: sel = 0; break;
        case WAZA_DOC_ITEM_MOVE_1_B: sel = 1; break;
        case WAZA_DOC_ITEM_MOVE_2_B: sel = 2; break;
        case WAZA_DOC_ITEM_MOVE_3_B: sel = 3; break;
        }
        if (sel == *pSelect)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case WAZA_DOC_ITEM_MOVE_0_C: case WAZA_DOC_ITEM_MOVE_1_C: case WAZA_DOC_ITEM_MOVE_3_C: case WAZA_DOC_ITEM_MOVE_2_C:
        switch (sprite->mItemID) {
        case WAZA_DOC_ITEM_MOVE_0_C: sel = 0; break;
        case WAZA_DOC_ITEM_MOVE_1_C: sel = 1; break;
        case WAZA_DOC_ITEM_MOVE_2_C: sel = 2; break;
        case WAZA_DOC_ITEM_MOVE_3_C: sel = 3; break;
        }
        if (sel == *pSelect)
            winSpriteSetDisp(sprite, 1);
        else
            winSpriteSetDisp(sprite, 0);
        return;

    case WAZA_DOC_ITEM_TYPE: case WAZA_DOC_ITEM_TYPE_B: case WAZA_DOC_ITEM_TYPE_C: {
        u16 zokuseiId = work->moves[*pSelect].type;
        windowDrawSprite(0, 0, window, zokuseiBiosGetSpriteID(zokuseiId), 0, winSpriteGetLayerID(window, sprite));
        return;
    }

    case WAZA_DOC_ITEM_DESCRIPTION: case WAZA_DOC_ITEM_DESCRIPTION_B: case WAZA_DOC_ITEM_DESCRIPTION_C: {
        s32 descID = wazaGetStatus(0, waza, WAZA_FIELD_DESCRIPTION, 0);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, descID);
        return;
    }

    case WAZA_DOC_ITEM_ACCURACY: case WAZA_DOC_ITEM_ACCURACY_B: case WAZA_DOC_ITEM_ACCURACY_C: {
        u32 accuracy = wazaGetStatus(0, waza, WAZA_FIELD_ACCURACY, 0);
        if (accuracy <= 1) {
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_NO_VALUE_TEXT);
        } else {
            msgctrlSetValue(MSGCTRL_NUM_VALUE, accuracy);
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PRINT_NUMBER);
        }
        return;
    }

    case WAZA_DOC_ITEM_POWER: case WAZA_DOC_ITEM_POWER_B: case WAZA_DOC_ITEM_POWER_C: {
        u32 power = wazaGetStatus(0, waza, WAZA_FIELD_POWER, 0);
        if (power <= 1) {
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_NO_VALUE_TEXT);
        } else {
            msgctrlSetValue(MSGCTRL_NUM_VALUE, power);
            GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_PRINT_NUMBER);
        }
        return;
    }

    case WAZA_DOC_ITEM_NAME: case WAZA_DOC_ITEM_NAME_B: case WAZA_DOC_ITEM_NAME_C:
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)work->moves[*pSelect].moveName);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_PRINT_TEXT);
        return;

    case WAZA_DOC_ITEM_PP: case WAZA_DOC_ITEM_PP_B: case WAZA_DOC_ITEM_PP_C: {
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

    if (window->mMenuID == MENU_FIGHT_SECRET_SELECT) {
        bool showLabels = windowGetParam(window, 2);
        if (!showLabels) {
            visible = false;
        }
    }
    switch (sprite->mItemID) {
    case SECRET_SELECT_ITEM_LABEL_0:
    case SECRET_SELECT_ITEM_LABEL_1:
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

    if (menuIsCheck(MENU_FIGHT_SECRET_POKEMON_LIST))
        openMenuID = MENU_FIGHT_SECRET_POKEMON_LIST;
    else if (menuIsCheck(MENU_FIGHT_SECRET_POKEMON_DETAIL))
        openMenuID = MENU_FIGHT_SECRET_POKEMON_DETAIL;

    u16 stick = _menuFightGetRightStickInfo(0, true);
    if (stick & WINDOW_KEY_UP)
        cursorIndex = 0;
    else if (stick & WINDOW_KEY_RIGHT)
        cursorIndex = 1;
    else if (stick & WINDOW_KEY_DOWN)
        cursorIndex = 2;
    else if (stick & WINDOW_KEY_LEFT)
        cursorIndex = 3;
    else if (key->mKeyTrg & MENU_FIGHT_KEY_PARTY_SLOT_FIVE)
        cursorIndex = 4;
    else if (key->mKeyTrg & MENU_FIGHT_KEY_PARTY_SLOT_SIX)
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
            nextMenuID = (pokemon_no < 0) ? MENU_FIGHT_SECRET_POKEMON_LIST
                                                : MENU_FIGHT_SECRET_POKEMON_DETAIL;
        }
    }

    menuFightButtonTimeOutCheck(window);
    if (openMenuID != nextMenuID) {
        if (openMenuID != 0)
            menuClose(openMenuID);
        if (!window->mAccept) {
            if (nextMenuID == MENU_FIGHT_SECRET_POKEMON_LIST) {
                menuOpenCustom(MENU_FIGHT_SECRET_POKEMON_LIST, window->mMenuID, 0, 0, false, 1,
                    trainer);
                menuSetDisp(MENU_FIGHT_SECRET_POKEMON_LIST, true);
                menuSetDisp(MENU_FIGHT_SECRET_POKEMON_DETAIL, false);
            } else if (nextMenuID == MENU_FIGHT_SECRET_POKEMON_DETAIL) {
                menuOpenCustom(MENU_FIGHT_SECRET_POKEMON_DETAIL, window->mMenuID, 0, 0, false, 2,
                    trainer, &pokemon_no);
                menuSetDisp(MENU_FIGHT_SECRET_POKEMON_DETAIL, true);
                menuSetDisp(MENU_FIGHT_SECRET_POKEMON_LIST, false);
            }
        }
    }
}

extern "C" void menuFightDrawSecretPokemon(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 partyIndex = -1;
    FIGHT_TRAINER *trainer = (FIGHT_TRAINER *)windowGetParam(window, 0);

    switch (sprite->mItemID) {
    case SECRET_POKEMON_ITEM_NICKNAME_0: case SECRET_POKEMON_ITEM_HP_0: case SECRET_POKEMON_ITEM_STATE_0: partyIndex = 0; break;
    case SECRET_POKEMON_ITEM_NICKNAME_1: case SECRET_POKEMON_ITEM_HP_1: case SECRET_POKEMON_ITEM_STATE_1: partyIndex = 1; break;
    case SECRET_POKEMON_ITEM_NICKNAME_2: case SECRET_POKEMON_ITEM_HP_2: case SECRET_POKEMON_ITEM_STATE_2: partyIndex = 2; break;
    case SECRET_POKEMON_ITEM_NICKNAME_3: case SECRET_POKEMON_ITEM_HP_3: case SECRET_POKEMON_ITEM_STATE_3: partyIndex = 3; break;
    case SECRET_POKEMON_ITEM_NICKNAME_4: case SECRET_POKEMON_ITEM_HP_4: case SECRET_POKEMON_ITEM_STATE_4: partyIndex = 4; break;
    case SECRET_POKEMON_ITEM_NICKNAME_5: case SECRET_POKEMON_ITEM_HP_5: case SECRET_POKEMON_ITEM_STATE_5: partyIndex = 5; break;
    }
    if (partyIndex < 0) return;

    Pokemon *pokemon = _menuFightGetPokemonPtr(trainer, (u16)partyIndex);
    if (!pokemon) return;

    unsigned long color = menuSubCalcColor(window, sprite);

    switch (sprite->mItemID) {
    case SECRET_POKEMON_ITEM_SPARE:
        break;

    case SECRET_POKEMON_ITEM_STATE_0: case SECRET_POKEMON_ITEM_STATE_1: case SECRET_POKEMON_ITEM_STATE_2: case SECRET_POKEMON_ITEM_STATE_3: case SECRET_POKEMON_ITEM_STATE_4: case SECRET_POKEMON_ITEM_STATE_5: {
        u16 spriteID = menuPokemonGetJoutaiSpriteID(pokemon, true);
        windowDrawSprite(0, 0, window, spriteID, 0, winSpriteGetLayerID(window, sprite));
        break;
    }

    case SECRET_POKEMON_ITEM_HP_0: case SECRET_POKEMON_ITEM_HP_1: case SECRET_POKEMON_ITEM_HP_2: case SECRET_POKEMON_ITEM_HP_3: case SECRET_POKEMON_ITEM_HP_4: case SECRET_POKEMON_ITEM_HP_5: {
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

    case SECRET_POKEMON_ITEM_NICKNAME_0: case SECRET_POKEMON_ITEM_NICKNAME_1: case SECRET_POKEMON_ITEM_NICKNAME_2: case SECRET_POKEMON_ITEM_NICKNAME_3: case SECRET_POKEMON_ITEM_NICKNAME_4: case SECRET_POKEMON_ITEM_NICKNAME_5: {
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
            msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(message));
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
    case SECRET_POKEMON_STATUS_ITEM_HELD_ITEM_NAME: {
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(
                itemDataBiosGetName(itemDataBiosGetPtr(pokemonGetSoubiItemDataId(pokemon)))));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                color, MSG_PRINT_TEXT);
        }
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_HELD_ITEM_ICON: {
        winSpriteSetDisp(sprite, false);
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            winSpriteSetDisp(sprite, true);
        }
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_ABILITY_NAME: {
        u16 *label = GSmsgGetStringPtr(MSG_ABILITY_LABEL);
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)label);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_PRINT_TEXT);
        u16 x = (u16)(GSmsgGetMessageSize(MSG_PRINT_TEXT) >> 16);
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(
            pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr((u16)pokemonGetTokuseiDataId(pokemon)))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), x, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_PRINT_TEXT);
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_0:
    case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_1:
    case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_3:
    case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_2:
    case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_5:
    case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_4: {
        switch (sprite->mItemID) {
        case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_0: slot = 0; break;
        case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_1: slot = 1; break;
        case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_3: slot = 3; break;
        case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_2: slot = 2; break;
        case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_4: slot = 4; break;
        case SECRET_POKEMON_STATUS_ITEM_CURSOR_SLOT_5: slot = 5; break;
        }
        if (slot == *pSelect) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_TYPE_FIRST: {
        u16 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        PokemonSpeciesStats *stats = pokemonDataBiosGetPtr(species);
        u8 type = (u8)pokemonDataBiosGetZokuseiDataId(stats, 0);
        u16 spriteID = (u16)zokuseiBiosGetSpriteID(type);
        windowDrawSprite(0, 0, window, spriteID, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_TYPE_SECOND: {
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
    case SECRET_POKEMON_STATUS_ITEM_MOVE_0_TYPE:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_1_TYPE:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_2_TYPE:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_3_TYPE: {
        switch (sprite->mItemID) {
        case SECRET_POKEMON_STATUS_ITEM_MOVE_3_TYPE: slot = 3; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_2_TYPE: slot = 2; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_1_TYPE: slot = 1; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_0_TYPE: slot = 0; break;
        }
        u16 moveID = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, slot);
        if (moveID != 0) {
            u16 type = (u16)wazaGetStatus(NULL, moveID, WAZA_FIELD_TYPE, 0);
            u16 spriteID = (u16)zokuseiBiosGetSpriteID(type);
            windowDrawSprite(0, 0, window, spriteID, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_MOVE_0_KNOWN:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_1_KNOWN:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_2_KNOWN:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_3_KNOWN: {
        switch (sprite->mItemID) {
        case SECRET_POKEMON_STATUS_ITEM_MOVE_3_KNOWN: slot = 3; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_2_KNOWN: slot = 2; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_1_KNOWN: slot = 1; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_0_KNOWN: slot = 0; break;
        }
        winSpriteSetDisp(sprite, false);
        if ((u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, slot) != 0) {
            winSpriteSetDisp(sprite, true);
        }
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_MOVE_0_PP:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_1_PP:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_2_PP:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_3_PP: {
        switch (sprite->mItemID) {
        case SECRET_POKEMON_STATUS_ITEM_MOVE_3_PP: slot = 3; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_2_PP: slot = 2; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_1_PP: slot = 1; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_0_PP: slot = 0; break;
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
    case SECRET_POKEMON_STATUS_ITEM_MOVE_0_NAME:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_1_NAME:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_2_NAME:
    case SECRET_POKEMON_STATUS_ITEM_MOVE_3_NAME: {
        switch (sprite->mItemID) {
        case SECRET_POKEMON_STATUS_ITEM_MOVE_3_NAME: slot = 3; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_2_NAME: slot = 2; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_1_NAME: slot = 1; break;
        case SECRET_POKEMON_STATUS_ITEM_MOVE_0_NAME: slot = 0; break;
        }
        u16 moveID = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, slot);
        if (moveID == 0) {
            break;
        }
        u32 name = wazaGetStatus(NULL, moveID, WAZA_FIELD_NAME, 0);
        if (name == 0) {
            break;
        }
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(name));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_POKEMON_NAME);
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_HP: {
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
    case SECRET_POKEMON_STATUS_ITEM_LEVEL: {
        s16 x = (s16)(GSmsgGetMessageSize(MSG_LEVEL_LABEL) >> 16);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, color, MSG_LEVEL_LABEL);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), x, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_PRINT_NUMBER);
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_SPECIES_NAME: {
        s16 nameWidth = 0;
        u16 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        PokemonSpeciesStats *stats = pokemonDataBiosGetPtr(species);
        if (stats == NULL) {
            break;
        }
        unsigned long nameMsgID = pokemonDataBiosGetName(stats);
        if (nameMsgID != 0) {
            msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(nameMsgID));
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
            msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(sexMarkMsgID));
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), nameWidth, 0, color, MSG_PRINT_TEXT);
        }
        break;
    }
    case SECRET_POKEMON_STATUS_ITEM_NICKNAME: {
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
        switch (data[i].mItemID) {
        case TARGET_PANEL_LEFT:
            switch (window->mMenuID) {
            case MENU_FIGHT_TARGET:
                selector = TARGET_CURSOR_LEFT;
                break;
            case MENU_FIGHT_TARGET_B:
                selector = TARGET_CURSOR_LEFT_B;
                break;
            }
            switch (sprite->mItemID) {
            case TARGET_TEXT_LEFT_B:
            case TARGET_TEXT_LEFT:
                msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)data[i].mNickname);
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0,
                               sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
                return;
            }
            break;
        case TARGET_PANEL_DOWN:
            switch (window->mMenuID) {
            case MENU_FIGHT_TARGET:
                selector = TARGET_CURSOR_DOWN;
                break;
            case MENU_FIGHT_TARGET_B:
                selector = TARGET_CURSOR_DOWN_B;
                break;
            }
            switch (sprite->mItemID) {
            case TARGET_TEXT_DOWN_B:
            case TARGET_TEXT_DOWN:
                msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)data[i].mNickname);
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0,
                               sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
                return;
            }
            break;
        case TARGET_PANEL_UP:
            switch (window->mMenuID) {
            case MENU_FIGHT_TARGET:
                selector = TARGET_CURSOR_UP;
                break;
            case MENU_FIGHT_TARGET_B:
                selector = TARGET_CURSOR_UP_B;
                break;
            }
            switch (sprite->mItemID) {
            case TARGET_TEXT_UP_B:
            case TARGET_TEXT_UP:
                msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)data[i].mNickname);
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0,
                               sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
                return;
            }
            break;
        case TARGET_PANEL_RIGHT:
            switch (window->mMenuID) {
            case MENU_FIGHT_TARGET:
                selector = TARGET_CURSOR_RIGHT;
                break;
            case MENU_FIGHT_TARGET_B:
                selector = TARGET_CURSOR_RIGHT_B;
                break;
            }
            switch (sprite->mItemID) {
            case TARGET_TEXT_RIGHT_B:
            case TARGET_TEXT_RIGHT:
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

// Draws the move name of a move slot of the pokemon the status window shows.
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

    if (window->mMenuID == MENU_FIGHT_IWAIT_ENEMY_STATUS) {
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
    case IWAIT_ITEM_FIGHT_KIND: // fight-kind icon
        if (fightKind_CheckFightKind(FIGHT_KIND_ALTERNATE_ICON)) {
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor,
                window, SPRITE_KIND_ICON_ALTERNATE, 0, winSpriteGetLayerID(window, sprite));
        } else {
            windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor,
                window, SPRITE_KIND_ICON, 0, winSpriteGetLayerID(window, sprite));
        }
        return;

    case IWAIT_ITEM_POKEMON0_TYPE_FIRST: case IWAIT_ITEM_POKEMON0_TYPE_FIRST_B: // pokemon0 type1 icon
        if (poke0 == 0) return;
        spriteId = zokuseiGetPokemonSpriteID(poke0->getPokemonDataPtr(), 0);
        if (spriteId == 0) return;
        windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, window->mColor,
            window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        return;

    case IWAIT_ITEM_POKEMON0_TYPE_SECOND: case IWAIT_ITEM_POKEMON0_TYPE_SECOND_B: { // pokemon0 type2 icon
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

    case IWAIT_ITEM_POKEMON0_NAME: case IWAIT_ITEM_POKEMON0_NAME_B: // pokemon0 nickname
        if (poke0 == 0) return;
        msgctrlSetValue(MSGCTRL_TEXT, pokemonGetStatus(poke0, 0, POKEMON_STATUS_NICKNAME, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_POKEMON_NAME);
        return;

    case IWAIT_ITEM_POKEMON0_ABILITY: case IWAIT_ITEM_POKEMON0_ABILITY_B: // pokemon0 ability name
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

    case IWAIT_ITEM_POKEMON0_MOVE_0: case IWAIT_ITEM_POKEMON0_MOVE_0_B: // pokemon0 move 0
        menuFightDrawIwaitStatusWaza(window, sprite, poke0, 0, color);
        return;
    case IWAIT_ITEM_POKEMON0_MOVE_1: case IWAIT_ITEM_POKEMON0_MOVE_1_B: // pokemon0 move 1
        menuFightDrawIwaitStatusWaza(window, sprite, poke0, 1, color);
        return;
    case IWAIT_ITEM_POKEMON0_MOVE_2: case IWAIT_ITEM_POKEMON0_MOVE_2_B: // pokemon0 move 2
        menuFightDrawIwaitStatusWaza(window, sprite, poke0, 2, color);
        return;
    case IWAIT_ITEM_POKEMON0_MOVE_3: case IWAIT_ITEM_POKEMON0_MOVE_3_B: // pokemon0 move 3
        menuFightDrawIwaitStatusWaza(window, sprite, poke0, 3, color);
        return;

    case IWAIT_ITEM_POKEMON1_TYPE_FIRST: case IWAIT_ITEM_POKEMON1_TYPE_FIRST_B: // pokemon1 type1 icon
        if (poke1 == 0) return;
        spriteId = zokuseiGetPokemonSpriteID(poke1->getPokemonDataPtr(), 0);
        if (spriteId == 0) return;
        windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, color,
            window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        return;

    case IWAIT_ITEM_POKEMON1_TYPE_SECOND: case IWAIT_ITEM_POKEMON1_TYPE_SECOND_B: { // pokemon1 type2 icon
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

    case IWAIT_ITEM_POKEMON1_NAME: case IWAIT_ITEM_POKEMON1_NAME_B: // pokemon1 nickname
        if (poke1 == 0) return;
        msgctrlSetValue(MSGCTRL_TEXT, pokemonGetStatus(poke1, 0, POKEMON_STATUS_NICKNAME, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_POKEMON_NAME);
        return;

    case IWAIT_ITEM_POKEMON1_ABILITY: case IWAIT_ITEM_POKEMON1_ABILITY_B: // pokemon1 ability name
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

    case IWAIT_ITEM_POKEMON1_MOVE_0: case IWAIT_ITEM_POKEMON1_MOVE_0_B: // pokemon1 move 0
        if (poke1 == 0) return;
        menuFightDrawIwaitStatusWaza(window, sprite, poke1, 0, color);
        return;
    case IWAIT_ITEM_POKEMON1_MOVE_1: case IWAIT_ITEM_POKEMON1_MOVE_1_B: // pokemon1 move 1
        if (poke1 == 0) return;
        menuFightDrawIwaitStatusWaza(window, sprite, poke1, 1, color);
        return;
    case IWAIT_ITEM_POKEMON1_MOVE_2: case IWAIT_ITEM_POKEMON1_MOVE_2_B: // pokemon1 move 2
        if (poke1 == 0) return;
        menuFightDrawIwaitStatusWaza(window, sprite, poke1, 2, color);
        return;
    case IWAIT_ITEM_POKEMON1_MOVE_3: case IWAIT_ITEM_POKEMON1_MOVE_3_B: // pokemon1 move 3
        if (poke1 == 0) return;
        menuFightDrawIwaitStatusWaza(window, sprite, poke1, 3, color);
        return;

    case IWAIT_ITEM_POKEMON0_HELD_ICON: case IWAIT_ITEM_POKEMON0_HELD_ICON_B: // pokemon0 held-item icon visibility
        winSpriteSetDisp(sprite, false);
        if (poke0 == 0) return;
        if (pokemonGetSoubiItemDataId(poke0) == 0) return;
        winSpriteSetDisp(sprite, true);
        return;

    case IWAIT_ITEM_POKEMON1_HELD_ICON: case IWAIT_ITEM_POKEMON1_HELD_ICON_B: // pokemon1 held-item icon visibility
        winSpriteSetDisp(sprite, false);
        if (poke1 == 0) return;
        if (pokemonGetSoubiItemDataId(poke1) == 0) return;
        winSpriteSetDisp(sprite, true);
        return;

    case IWAIT_ITEM_POKEMON0_HELD_NAME: case IWAIT_ITEM_POKEMON0_HELD_NAME_B: // pokemon0 held-item name
        if (poke0 == 0) return;
        if (pokemonGetSoubiItemDataId(poke0) == 0) return;
        msgctrlSetValue(MSGCTRL_TEXT,
            (u32)GSmsgGetStringPtr(itemDataBiosGetName(
                itemDataBiosGetPtr(pokemonGetSoubiItemDataId(poke0)))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
            color, MSG_TEXT_IN_RECT);
        return;

    case IWAIT_ITEM_POKEMON1_HELD_NAME: case IWAIT_ITEM_POKEMON1_HELD_NAME_B: // pokemon1 held-item name
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
    case IPOKEMON_STATUS_ITEM_NICKNAME:
    case IPOKEMON_STATUS_ITEM_NICKNAME_B:
    {
        u16 *nickname = (u16 *)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NICKNAME, 0);
        msgctrlSetValue(MSGCTRL_TEXT, (u32)nickname);
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
        break;
    }
    case IPOKEMON_STATUS_ITEM_TYPE_FIRST:
    case IPOKEMON_STATUS_ITEM_TYPE_FIRST_B:
    {
        u16 species = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        PokemonSpeciesStats *data = pokemonDataBiosGetPtr(species);
        u8 type1 = (u8)pokemonDataBiosGetZokuseiDataId(data, 0);
        u16 spriteId = (u16)zokuseiBiosGetSpriteID(type1);
        windowDrawSprite(0, 0, window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case IPOKEMON_STATUS_ITEM_TYPE_SECOND:
    case IPOKEMON_STATUS_ITEM_TYPE_SECOND_B:
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
    case IPOKEMON_STATUS_ITEM_ABILITY:
    case IPOKEMON_STATUS_ITEM_ABILITY_B:
    {
        u16 tokuseiId = (u16)fightOutPokemon_GetTokuseiDataId(work->fightOutPokemon);
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(tokuseiId))));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), (s16)(GSmsgGetMessageSize(MSG_ABILITY_LABEL) >> 16), 0, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
        break;
    }
    case IPOKEMON_STATUS_ITEM_MOVE_3_TYPE:
        wazaNo = 1;
    case IPOKEMON_STATUS_ITEM_MOVE_2_TYPE:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_1_TYPE:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_0_TYPE:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            u16 moveType = (u16)wazaGetStatus(0, moveId, WAZA_FIELD_TYPE, 0);
            u16 spriteId = (u16)zokuseiBiosGetSpriteID(moveType);
            windowDrawSprite(0, 0, window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case IPOKEMON_STATUS_ITEM_MOVE_3_PP:
        wazaNo = 1;
    case IPOKEMON_STATUS_ITEM_MOVE_2_PP:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_1_PP:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_0_PP:
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
    case IPOKEMON_STATUS_ITEM_MOVE_3_NAME:
        wazaNo = 1;
    case IPOKEMON_STATUS_ITEM_MOVE_2_NAME:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_1_NAME:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_0_NAME:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            u32 nameMsgId = wazaGetStatus(0, moveId, WAZA_FIELD_NAME, 0);
            if (nameMsgId != 0) {
                msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(nameMsgId));
                s16 height = (s16)(u16)GSmsgGetMessageSize(MSG_POKEMON_NAME);
                s16 y = sprite->mHeight / 2 - height / 2;
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, y, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
            }
        }
        break;
    }
    case IPOKEMON_STATUS_ITEM_MOVE_3_TYPE_B:
        wazaNo = 1;
    case IPOKEMON_STATUS_ITEM_MOVE_2_TYPE_B:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_1_TYPE_B:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_0_TYPE_B:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            u16 moveType = (u16)wazaGetStatus(0, moveId, WAZA_FIELD_TYPE, 0);
            u16 spriteId = (u16)zokuseiBiosGetSpriteID(moveType);
            windowDrawSprite(0, 0, window, spriteId, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case IPOKEMON_STATUS_ITEM_MOVE_3_PP_B:
        wazaNo = 1;
    case IPOKEMON_STATUS_ITEM_MOVE_2_PP_B:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_1_PP_B:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_0_PP_B:
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
    case IPOKEMON_STATUS_ITEM_MOVE_3_NAME_B:
        wazaNo = 1;
    case IPOKEMON_STATUS_ITEM_MOVE_2_NAME_B:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_1_NAME_B:
        wazaNo++;
    case IPOKEMON_STATUS_ITEM_MOVE_0_NAME_B:
    {
        u16 moveId = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, wazaNo);
        if (moveId != 0) {
            u32 nameMsgId = wazaGetStatus(0, moveId, WAZA_FIELD_NAME, 0);
            if (nameMsgId != 0) {
                msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(nameMsgId));
                s16 height = (s16)(u16)GSmsgGetMessageSize(MSG_POKEMON_NAME);
                s16 y = sprite->mHeight / 2 - height / 2;
                GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, y, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
            }
        }
        break;
    }
    case IPOKEMON_STATUS_ITEM_HELD_ITEM_NAME:
    case IPOKEMON_STATUS_ITEM_HELD_ITEM_NAME_B:
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)GSmsgGetStringPtr(itemDataBiosGetName(itemDataBiosGetPtr(pokemonGetSoubiItemDataId(pokemon)))));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight, color, MSG_POKEMON_NAME);
        }
        break;
    case IPOKEMON_STATUS_ITEM_HELD_ITEM_ICON:
    case IPOKEMON_STATUS_ITEM_HELD_ITEM_ICON_B:
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            winSpriteSetDisp(sprite, true);
        } else {
            winSpriteSetDisp(sprite, false);
        }
        break;
    case IPOKEMON_STATUS_ITEM_TARGET_MARKER_0:
    case IPOKEMON_STATUS_ITEM_TARGET_MARKER_1:
    case IPOKEMON_STATUS_ITEM_TARGET_MARKER_0_B:
    case IPOKEMON_STATUS_ITEM_TARGET_MARKER_1_B:
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

// Draws the cursor highlight over the currently selected secret-menu target.
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

// Button input of the fight menu's secret target-selection window: maps the
// pressed direction onto the target panel in that direction.
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
            switch (entry[i].mItemID) {
            case TARGET_PANEL_LEFT:  row = 2; break;
            case TARGET_PANEL_DOWN:  row = 3; break;
            case TARGET_PANEL_UP:  row = 0; break;
            case TARGET_PANEL_RIGHT:  row = 1; break;
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

// Shows the target marker over the panel of the currently selected target.
extern "C" void menuFightDrawTargetSecret(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_FIGHT_TARGET_SLOT *target = (MENU_FIGHT_TARGET_SLOT *)windowGetParam(window, 0);
    s32 itemID;
    int i;

    for (i = 0; i < 4; i++) {
        switch (target[i].mItemID) {
        case TARGET_PANEL_LEFT:
            itemID = TARGET_MARKER_LEFT;
            break;
        case TARGET_PANEL_DOWN:
            itemID = TARGET_MARKER_DOWN;
            break;
        case TARGET_PANEL_UP:
            itemID = TARGET_MARKER_UP;
            break;
        case TARGET_PANEL_RIGHT:
            itemID = TARGET_MARKER_RIGHT;
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
    menuOpen(MENU_FIGHT_TIMEUP, 0);
}

// Surrender (kousan) button of the fight menu's secret command window: normal
// menu-button handling plus the window timeout check.
extern "C" void menuFightButtonSecretKousan(tagWINDOW_WORK *window)
{
    menuButtonNormal(window);
    menuFightButtonTimeOutCheck(window);
}

void menuFightOpenCountDown(void)
{
    menuOpenCustom(MENU_FIGHT_COUNTDOWN, -1, NULL, 0, false, 0);
}

void menuFightCloseCountDown(void)
{
    menuCloseCustom(MENU_FIGHT_COUNTDOWN, 0, false);
}

u8 menuFightCloseCheckCountDown(void)
{
    return menuIsCheck(MENU_FIGHT_COUNTDOWN);
}

void menuFightOpenTotalTimer(void)
{
    menuOpenCustom(MENU_FIGHT_TOTAL_TIMER, -1, NULL, 0, false, 0);
}

void menuFightCloseTotalTimer(void)
{
    menuCloseCustom(MENU_FIGHT_TOTAL_TIMER, 0, false);
}

u8 menuFightCloseCheckTotalTimer(void)
{
    return menuIsCheck(MENU_FIGHT_TOTAL_TIMER);
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
    // Previous countdown value, used to detect each second tick.
    static long oldTime = 0;
    if (window->mState == WINDOW_STATE_INIT) {
        oldTime = 0;
        if (window->mMenuID == MENU_FIGHT_TOTAL_TIMER) {
            windowSetParam(window, 0, (long)fightTimerAllGetNokoriTime());
        }
    }
    switch (window->mMenuID) {
    case MENU_FIGHT_TOTAL_TIMER:
        if (menuIsCheck(MENU_FIGHT_COUNTDOWN) == 0) {
            windowSetParam(window, 0, (long)fightTimerAllGetNokoriTime());
        }
        break;
    case MENU_FIGHT_COUNTDOWN: {
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
            if (menuIsCheck(MENU_FIGHT_TOTAL_TIMER) != 0) {
                windowSetParam(windowSearchID(MENU_FIGHT_TOTAL_TIMER), 0,
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
    // Timer digits are formatted into this scratch text before each print.
    static u16 buff[8];
    switch (sprite->mItemID) {
    case TIMER_ITEM_SEPARATOR_0:
    case TIMER_ITEM_SEPARATOR_1:
    case TIMER_ITEM_SEPARATOR_2:
    case TIMER_ITEM_SEPARATOR_3:
        winSpriteSetDisp(sprite, false);
        if (0.5f <= menuGetTotalTime()) {
            winSpriteSetDisp(sprite, true);
        }
        break;
    case TIMER_ITEM_MINUTES: {
        long time = windowGetParam(window, 0);
        long minutes = time / 60;
        buff[0] = half_digit_code[minutes / 10];
        buff[2] = 0;
        buff[1] = half_digit_code[minutes % 10];
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)buff);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
            menuSubCalcColor(window, sprite), MSG_PRINT_TEXT);
        break;
    }
    case TIMER_ITEM_SECONDS: {
        long time = windowGetParam(window, 0);
        long seconds = time % 60;
        buff[0] = half_digit_code[seconds / 10];
        buff[2] = 0;
        buff[1] = half_digit_code[seconds % 10];
        msgctrlSetValue(MSGCTRL_TEXT, (unsigned long)buff);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
            menuSubCalcColor(window, sprite), MSG_PRINT_TEXT);
        break;
    }
    case TIMER_ITEM_COUNT:
        msgctrlSetValue(MSGCTRL_NUM_VALUE, windowGetParam(window, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth,
            sprite->mHeight, menuSubCalcColor(window, sprite), MSG_PRINT_NUMBER_WIDE);
        break;
    }
}

// Reward money paid out for the current fight, or -1 when the fight pays
// nothing.
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
    if (sprite->mItemID == KANE_ITEM_VALUE) {
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
    case TURN_ITEM_COUNT:
        msgctrlSetValue(MSGCTRL_NUM_VALUE, fightFloor_GetTurnCount(0));
        if (fightFloor_GetTurnCount(0) > 9) {
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, -1, MSG_PRINT_NUMBER);
        } else {
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), 8, 0, -1, MSG_PRINT_NUMBER);
        }
        break;
    case TURN_ITEM_CURSOR_ROW_0_VARIANT_A:
    case TURN_ITEM_CURSOR_ROW_0_VARIANT_B:
    case TURN_ITEM_CURSOR_ROW_0_VARIANT_C:
        if (window->mCursor.y == 0) {
            menuSubDrawCursorBack(winSpriteGetLayerID(window, sprite), 0, 0,
                sprite->mWidth, sprite->mHeight, 255,
                MENU_SUB_CURSOR_BACK_STYLE_VARIANT);
        }
        break;
    case TURN_ITEM_CURSOR_ROW_1_VARIANT_A:
    case TURN_ITEM_CURSOR_ROW_1_VARIANT_B:
    case TURN_ITEM_CURSOR_ROW_1_VARIANT_C:
        if (window->mCursor.y == 1) {
            menuSubDrawCursorBack(winSpriteGetLayerID(window, sprite), 0, 0,
                sprite->mWidth, sprite->mHeight, 255,
                MENU_SUB_CURSOR_BACK_STYLE_VARIANT);
        }
        break;
    }
}

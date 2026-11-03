#ifndef DBGMENUPOKEMON_H
#define DBGMENUPOKEMON_H

#include <Pokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Amount one input step adds to the value of the selected entry. The limit
   step pushes the value onto one of its bounds instead. */
enum {
    POKEMON_EDIT_STEP = 1,
    POKEMON_EDIT_STEP_FAST = 10,
    POKEMON_EDIT_STEP_FASTER = 100,
    POKEMON_EDIT_STEP_LIMIT = 65535
};

/* Species the editor creates a record as when it is opened on a pokemon that
   does not check out, so that there is something to edit. */
enum {
    POKEMON_EDIT_DEFAULT_SPECIES = 277
};

/* Results the editor window hands back to the menu that opened it. */
enum {
    POKEMON_EDIT_RESULT_CANCEL = -1, // the edited record is dropped.
    POKEMON_EDIT_RESULT_CONFIRM = -2 // the edited record is kept.
};

/* Rows of the debug pokemon editor menu, in the order the menu lists them.
   The editor body and the input hook both address the rows by these ids. */
enum {
    POKEMON_EDIT_ITEM_SPECIES              = 22, // species record of the pokemon.
    POKEMON_EDIT_ITEM_LEVEL                = 24, // level the pokemon is on.
    POKEMON_EDIT_ITEM_EXP                  = 26, // total experience of the pokemon.
    POKEMON_EDIT_ITEM_HP                   = 28, // current HP.
    POKEMON_EDIT_ITEM_HP_MAX               = 38, // maximum HP.
    POKEMON_EDIT_ITEM_ATTACK               = 39, // battle stats, as the summary page shows them.
    POKEMON_EDIT_ITEM_DEFENSE              = 40,
    POKEMON_EDIT_ITEM_SPEED                = 41,
    POKEMON_EDIT_ITEM_SP_ATTACK            = 42,
    POKEMON_EDIT_ITEM_SP_DEFENSE           = 43,
    POKEMON_EDIT_ITEM_IV_HP                = 44,
    POKEMON_EDIT_ITEM_IV_ATTACK            = 45,
    POKEMON_EDIT_ITEM_IV_DEFENSE           = 46,
    POKEMON_EDIT_ITEM_IV_SPEED             = 47,
    POKEMON_EDIT_ITEM_IV_SP_ATTACK         = 48,
    POKEMON_EDIT_ITEM_IV_SP_DEFENSE        = 49,
    POKEMON_EDIT_ITEM_EV_HP                = 50,
    POKEMON_EDIT_ITEM_EV_ATTACK            = 51,
    POKEMON_EDIT_ITEM_EV_DEFENSE           = 52,
    POKEMON_EDIT_ITEM_EV_SPEED             = 53,
    POKEMON_EDIT_ITEM_EV_SP_ATTACK         = 54,
    POKEMON_EDIT_ITEM_EV_SP_DEFENSE        = 55,
    POKEMON_EDIT_ITEM_WAZA_ID_0            = 57, // waza id of waza slots 0 to 3.
    POKEMON_EDIT_ITEM_WAZA_ID_1            = 58,
    POKEMON_EDIT_ITEM_WAZA_ID_2            = 59,
    POKEMON_EDIT_ITEM_WAZA_ID_3            = 60,
    POKEMON_EDIT_ITEM_WAZA_PP_0            = 61, // current PP of the waza slots.
    POKEMON_EDIT_ITEM_WAZA_PP_1            = 62,
    POKEMON_EDIT_ITEM_WAZA_PP_2            = 63,
    POKEMON_EDIT_ITEM_WAZA_PP_3            = 64,
    POKEMON_EDIT_ITEM_WAZA_PP_COUNT_0      = 65, // PP-ups spent on the waza slots, 0 to 3.
    POKEMON_EDIT_ITEM_WAZA_PP_COUNT_1      = 66,
    POKEMON_EDIT_ITEM_WAZA_PP_COUNT_2      = 67,
    POKEMON_EDIT_ITEM_WAZA_PP_COUNT_3      = 68,
    POKEMON_EDIT_ITEM_ABILITY_SLOT         = 70,
    POKEMON_EDIT_ITEM_ITEM                 = 72,
    POKEMON_EDIT_ITEM_PID                  = 74,
    POKEMON_EDIT_ITEM_SEX                  = 75,
    POKEMON_EDIT_ITEM_NATURE               = 76,
    POKEMON_EDIT_ITEM_RARE                 = 77,
    POKEMON_EDIT_ITEM_TRAINER_ID           = 79,
    POKEMON_EDIT_ITEM_FRIENDSHIP           = 81,
    POKEMON_EDIT_ITEM_POKERUS              = 83,
    POKEMON_EDIT_ITEM_JOUTAI_KIND          = 85,
    POKEMON_EDIT_ITEM_JOUTAI_KAISUU        = 86,
    POKEMON_EDIT_ITEM_JOUTAI_NOW_KAISUU    = 87,
    POKEMON_EDIT_ITEM_DARK_ID              = 89,
    POKEMON_EDIT_ITEM_DARK_POINTS          = 91,
    POKEMON_EDIT_ITEM_DARK_JOUTAI          = 93,
    POKEMON_EDIT_ITEM_POOL_EXP             = 95,
    POKEMON_EDIT_ITEM_POOL_FRIEND          = 97,
    POKEMON_EDIT_ITEM_KEY_PLAYER_FLAG      = 105,
    POKEMON_EDIT_ITEM_FIGHT_PART_DATA      = 107,
    POKEMON_EDIT_ITEM_RIBBON_PAGE          = 109, // ribbon page the editor is showing.
    POKEMON_EDIT_ITEM_RIBBON               = 110,
    POKEMON_EDIT_ITEM_CAUGHT_BALL          = 112,
};

s32 dbgMenuPokemonEditSub(Pokemon *pokemon, u8 inFight, u8 *appearanceChanged,
    u8 *statusChanged, u8 *abilityChanged, u8 *itemChanged);

/* Ribbon page the pokemon editor is showing. */
extern s32 gRibbonNum;

/* Amount the last input added to the value being edited. */
extern s32 pokemon_edit_value;

/* Effort values the pokemon has already earned, over all six stats. */
u32 dbgMenuPokemonEditSubGetTotalEffort(Pokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif

#include "types.h"
#include "Pokemon.h"
#include "pokemonBios.h"
#include "dbgMenuPokemon.h"
#include "menu.h"
#include "menuSub.h"
#include "waza.h"
#include "joutai.h"
#include "fightTrainer.h"
#include "fightSeq.h"
#include "fightTrainerStatus.h"
#include "GSmsg.h"
#include "gamedata.h"
#include "itemData.h"
#include "relglobal.h"
#include "sexData.h"


// Status field id of each ribbon slot, indexed by the edited ribbon page.
static u16 gRibbonStat[18] = {
    POKEMON_STATUS_FIELD_CHAMP_RIBBON,
    POKEMON_STATUS_FIELD_WINNING_RIBBON,
    POKEMON_STATUS_FIELD_VICTORY_RIBBON,
    POKEMON_STATUS_FIELD_BROMIDE_RIBBON,
    POKEMON_STATUS_FIELD_GANBA_RIBBON,
    POKEMON_STATUS_FIELD_MARINE_RIBBON,
    POKEMON_STATUS_FIELD_LAND_RIBBON,
    POKEMON_STATUS_FIELD_SKY_RIBBON,
    POKEMON_STATUS_FIELD_COUNTRY_RIBBON,
    POKEMON_STATUS_FIELD_NATIONAL_RIBBON,
    POKEMON_STATUS_FIELD_EARTH_RIBBON,
    POKEMON_STATUS_FIELD_WORLD_RIBBON,
    POKEMON_STATUS_FIELD_AMARI_RIBBON,
    POKEMON_STATUS_FIELD_STYLE_MEDAL,
    POKEMON_STATUS_FIELD_BEAUTIFUL_MEDAL,
    POKEMON_STATUS_FIELD_CUTE_MEDAL,
    POKEMON_STATUS_FIELD_CLEVER_MEDAL,
    POKEMON_STATUS_FIELD_STRONG_MEDAL,
};

#pragma optimization_level 4
s32 dbgMenuPokemonEditSub(Pokemon *pokemon, u8 inFight, u8 *appearanceChanged,
                           u8 *statusChanged, u8 *abilityChanged, u8 *itemChanged)
{
    u16 defaultName[2] = { 0, 0 };
    u16 ivFieldIds[6] = {
        POKEMON_STATUS_FIELD_HP_IV,
        POKEMON_STATUS_FIELD_ATTACK_IV,
        POKEMON_STATUS_FIELD_DEFENSE_IV,
        POKEMON_STATUS_FIELD_SP_ATTACK_IV,
        POKEMON_STATUS_FIELD_SP_DEFENSE_IV,
        POKEMON_STATUS_FIELD_SPEED_IV,
    };
    u16 effortFieldIds[6] = {
        POKEMON_STATUS_FIELD_HP_EFFORT,
        POKEMON_STATUS_FIELD_ATTACK_EFFORT,
        POKEMON_STATUS_FIELD_DEFENSE_EFFORT,
        POKEMON_STATUS_FIELD_SP_ATTACK_EFFORT,
        POKEMON_STATUS_FIELD_SP_DEFENSE_EFFORT,
        POKEMON_STATUS_FIELD_SPEED_EFFORT,
    };
    u32 numberInputResult;
    Pokemon pokemonBackup;
    u16 trainerIndex;
    s32 menuId;
    s32 editDelta;
    u8 changed;
    s32 joutaiId;
    s32 level;
    s32 exp;
    s32 hp;
    s32 hpMax;
    s32 attack;
    s32 defense;
    s32 speed;
    s32 spAttack;
    s32 spDefense;
    s32 ivHp;
    s32 ivAttack;
    s32 ivDefense;
    s32 ivSpeed;
    s32 ivSpAttack;
    s32 ivSpDefense;
    s32 evHp;
    s32 evAttack;
    s32 evDefense;
    s32 evSpeed;
    s32 evSpAttack;
    s32 evSpDefense;
    s32 move0;
    s32 move1;
    s32 move2;
    s32 move3;
    s32 pp0;
    s32 pp1;
    s32 pp2;
    s32 pp3;
    s32 ppCount0;
    s32 ppCount1;
    s32 ppCount2;
    s32 ppCount3;
    u8 currentSlot;
    s32 newSlot;
    u16 abilitySpecies;
    s32 itemId;
    s32 pid;
    s32 trainerId;
    s32 friendship;
    s32 pokerus;
    s32 kind;
    s32 kaisuu;
    s32 extCount;
    s32 maxKaisuu;
    s32 nowKaisuu;
    s32 value;
    u16 darkId;
    s32 poolExp;
    s32 poolFriend;
    s32 keyPlayerFlag;
    s32 partDataId;
    s32 ball;
    s32 ribbon;

    if (appearanceChanged != NULL) *appearanceChanged = 0;
    if (statusChanged != NULL) *statusChanged = 0;
    if (abilityChanged != NULL) *abilityChanged = 0;
    if (itemChanged != NULL) *itemChanged = 0;

    pokemonBackup = *pokemon;

    if (!pokemonCheckValid(pokemon)) {
        pokemonCreate(pokemon, POKEMON_EDIT_DEFAULT_SPECIES, 1,
                      gamedataGetStatus(NULL, GAMEDATA_STATUS_ATTEST));
        pokemonSetCatchStatus(pokemon, 0, 1, POKEMON_CATCH_BALL_NONE, POKEMON_SEX_NONE, 0,
                              defaultName);
    }
    trainerIndex = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_FIGHT_TRAINER_INDEX, 0);

    for (;;) {
        pokemon_edit_value = 0;
        menuId = menuOpenCustom(MENU_ID_DEBUG_POKEMON_EDITOR, 0, NULL, 0, 1, 2, pokemon,
                                &pokemon_edit_value);
        editDelta = pokemon_edit_value;

        if (menuId == POKEMON_EDIT_RESULT_CANCEL) {
            menuCloseCustom(MENU_ID_DEBUG_POKEMON_EDITOR, 0, 1);
            *pokemon = pokemonBackup;
            return -1;
        }

        if (menuId == POKEMON_EDIT_RESULT_CONFIRM) {
            if (menuOpen(MENU_ID_DEBUG_EDIT_CONFIRMATION, 1) == 0) {
                goto commit;
            }
            menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
            continue;
        }

        if (editDelta == 0) {
            continue;
        }

        changed = 0;

        switch (menuId) {
        case POKEMON_EDIT_ITEM_SPECIES: {
            value = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
            value += editDelta;
            if (value < 0) value = 0;
            if ((u32)value >= *pokemon_data_number) value = *pokemon_data_number - 1;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0, value);
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NICKNAME, 0,
                              (u32)GSmsgGetStringPtr(pokemonGetStatus(NULL, (u16)value,
                                                                       POKEMON_BIOS_FIELD_DEFAULT_NAME, 0)));
            pokemonSetTokuseiFlag(pokemon, 0);
            changed = 1;
            if (appearanceChanged != NULL) *appearanceChanged = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_LEVEL: {
            level = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0);
            level += editDelta;
            if (level <= 0) level = 1;
            if (level > 100) level = 100;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0, level);
            changed = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_EXP: {
            exp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0);
            if (editDelta == -POKEMON_EDIT_STEP_LIMIT) {
                exp = 1;
            } else if (editDelta == POKEMON_EDIT_STEP_LIMIT) {
                exp = 2000000;
            } else {
                if (!menuSubOpenNumberInput(exp, &numberInputResult, 1, 50, 50, 0)) {
                    menuSubCloseNumberInput();
                    break;
                }
                exp = (s32)numberInputResult;
                menuSubCloseNumberInput();
                if (exp <= 0) exp = 1;
                if (exp > 2000000) exp = 2000000;
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0, exp);
            changed = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_HP: {
            hp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP, 0);
            hp += editDelta;
            if (hp < 0) hp = 0;
            if (hp > pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0)) {
                hp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0);
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP, 0, hp);
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_HP_MAX: {
            hpMax = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0);
            hpMax += editDelta;
            if (hpMax < 0) hpMax = 0;
            if (hpMax > 999) hpMax = 999;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_MAX, 0, hpMax);
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP, 0, hpMax);
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_ATTACK: {
            attack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK, 0);
            attack += editDelta;
            if (attack < 0) attack = 0;
            if (attack > 999) attack = 999;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK, 0, attack);
            break;
        }

        case POKEMON_EDIT_ITEM_DEFENSE: {
            defense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE, 0);
            defense += editDelta;
            if (defense < 0) defense = 0;
            if (defense > 999) defense = 999;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE, 0, defense);
            break;
        }

        case POKEMON_EDIT_ITEM_SPEED: {
            speed = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED, 0);
            speed += editDelta;
            if (speed < 0) speed = 0;
            if (speed > 999) speed = 999;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED, 0, speed);
            break;
        }

        case POKEMON_EDIT_ITEM_SP_ATTACK: {
            spAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK, 0);
            spAttack += editDelta;
            if (spAttack < 0) spAttack = 0;
            if (spAttack > 999) spAttack = 999;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK, 0, spAttack);
            break;
        }

        case POKEMON_EDIT_ITEM_SP_DEFENSE: {
            spDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE, 0);
            spDefense += editDelta;
            if (spDefense < 0) spDefense = 0;
            if (spDefense > 999) spDefense = 999;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE, 0, spDefense);
            break;
        }

        case POKEMON_EDIT_ITEM_IV_HP: {
            ivHp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_IV, 0);
            ivHp += editDelta;
            if (ivHp < 0) ivHp = 0;
            if (ivHp > 31) ivHp = 31;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_IV, 0, ivHp);
            changed = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_IV_ATTACK: {
            ivAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK_IV, 0);
            ivAttack += editDelta;
            if (ivAttack < 0) ivAttack = 0;
            if (ivAttack > 31) ivAttack = 31;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK_IV, 0, ivAttack);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_IV_DEFENSE: {
            ivDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE_IV, 0);
            ivDefense += editDelta;
            if (ivDefense < 0) ivDefense = 0;
            if (ivDefense > 31) ivDefense = 31;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE_IV, 0, ivDefense);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_IV_SPEED: {
            ivSpeed = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED_IV, 0);
            ivSpeed += editDelta;
            if (ivSpeed < 0) ivSpeed = 0;
            if (ivSpeed > 31) ivSpeed = 31;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED_IV, 0, ivSpeed);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_IV_SP_ATTACK: {
            ivSpAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK_IV, 0);
            ivSpAttack += editDelta;
            if (ivSpAttack < 0) ivSpAttack = 0;
            if (ivSpAttack > 31) ivSpAttack = 31;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK_IV, 0, ivSpAttack);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_IV_SP_DEFENSE: {
            ivSpDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE_IV, 0);
            ivSpDefense += editDelta;
            if (ivSpDefense < 0) ivSpDefense = 0;
            if (ivSpDefense > 31) ivSpDefense = 31;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE_IV, 0, ivSpDefense);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_EV_HP: {
            if (editDelta > 0 && dbgMenuPokemonEditSubGetTotalEffort(pokemon) >= 510) {
                break;
            }
            evHp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_EFFORT, 0);
            evHp += editDelta;
            if (evHp < 0) evHp = 0;
            if (evHp > 255) evHp = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_HP_EFFORT, 0, evHp);
            changed = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_EV_ATTACK: {
            if (editDelta > 0 && dbgMenuPokemonEditSubGetTotalEffort(pokemon) >= 510) {
                break;
            }
            evAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK_EFFORT, 0);
            evAttack += editDelta;
            if (evAttack < 0) evAttack = 0;
            if (evAttack > 255) evAttack = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ATTACK_EFFORT, 0, evAttack);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_EV_DEFENSE: {
            if (editDelta > 0 && dbgMenuPokemonEditSubGetTotalEffort(pokemon) >= 510) {
                break;
            }
            evDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE_EFFORT, 0);
            evDefense += editDelta;
            if (evDefense < 0) evDefense = 0;
            if (evDefense > 255) evDefense = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DEFENSE_EFFORT, 0, evDefense);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_EV_SPEED: {
            if (editDelta > 0 && dbgMenuPokemonEditSubGetTotalEffort(pokemon) >= 510) {
                break;
            }
            evSpeed = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED_EFFORT, 0);
            evSpeed += editDelta;
            if (evSpeed < 0) evSpeed = 0;
            if (evSpeed > 255) evSpeed = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPEED_EFFORT, 0, evSpeed);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_EV_SP_ATTACK: {
            if (editDelta > 0 && dbgMenuPokemonEditSubGetTotalEffort(pokemon) >= 510) {
                break;
            }
            evSpAttack = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK_EFFORT, 0);
            evSpAttack += editDelta;
            if (evSpAttack < 0) evSpAttack = 0;
            if (evSpAttack > 255) evSpAttack = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_ATTACK_EFFORT, 0, evSpAttack);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_EV_SP_DEFENSE: {
            if (editDelta > 0 && dbgMenuPokemonEditSubGetTotalEffort(pokemon) >= 510) {
                break;
            }
            evSpDefense = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE_EFFORT, 0);
            evSpDefense += editDelta;
            if (evSpDefense < 0) evSpDefense = 0;
            if (evSpDefense > 255) evSpDefense = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SP_DEFENSE_EFFORT, 0, evSpDefense);
            changed = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_ID_0: {
            move0 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, 0);
            if (move0 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            move0 += editDelta;
            if (move0 < 0) {
                pokemonWazaInit(pokemon, 0);
                break;
            }
            if (move0 == WAZA_SHADOW_FIRST - 1) move0 += editDelta;
            if (move0 >= WAZA_SHADOW_SPECIAL) move0 = WAZA_SHADOW_LAST;
            pokemonWazaCreate(pokemon, 0, (u16)move0);
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_ID_1: {
            move1 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, 1);
            if (move1 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            move1 += editDelta;
            if (move1 < 0) {
                pokemonWazaInit(pokemon, 1);
                break;
            }
            if (move1 == WAZA_SHADOW_FIRST - 1) move1 += editDelta;
            if (move1 >= WAZA_SHADOW_SPECIAL) move1 = WAZA_SHADOW_LAST;
            pokemonWazaCreate(pokemon, 1, (u16)move1);
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_ID_2: {
            move2 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, 2);
            if (move2 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            move2 += editDelta;
            if (move2 < 0) {
                pokemonWazaInit(pokemon, 2);
                break;
            }
            if (move2 == WAZA_SHADOW_FIRST - 1) move2 += editDelta;
            if (move2 >= WAZA_SHADOW_SPECIAL) move2 = WAZA_SHADOW_LAST;
            pokemonWazaCreate(pokemon, 2, (u16)move2);
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_ID_3: {
            move3 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, 3);
            if (move3 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            move3 += editDelta;
            if (move3 < 0) {
                pokemonWazaInit(pokemon, 3);
                break;
            }
            if (move3 == WAZA_SHADOW_FIRST - 1) move3 += editDelta;
            if (move3 >= WAZA_SHADOW_SPECIAL) move3 = WAZA_SHADOW_LAST;
            pokemonWazaCreate(pokemon, 3, (u16)move3);
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_PP_0: {
            if (!pokemonWazaCheckValid(pokemon, 0)) {
                break;
            }
            pp0 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 0);
            if (wazaCheckDarkWaza((u16)pp0) == 1 || pp0 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            pp0 += editDelta;
            if (pp0 < 0) pp0 = 0;
            if (pp0 > pokemonWazaGetMaxPP(pokemon, 0)) {
                pp0 = pokemonWazaGetMaxPP(pokemon, 0);
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 0, (u8)pp0);
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_PP_1: {
            if (!pokemonWazaCheckValid(pokemon, 1)) {
                break;
            }
            pp1 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 1);
            if (wazaCheckDarkWaza((u16)pp1) == 1 || pp1 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            pp1 += editDelta;
            if (pp1 < 0) pp1 = 0;
            if (pp1 > pokemonWazaGetMaxPP(pokemon, 1)) {
                pp1 = pokemonWazaGetMaxPP(pokemon, 1);
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 1, (u8)pp1);
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_PP_2: {
            if (!pokemonWazaCheckValid(pokemon, 2)) {
                break;
            }
            pp2 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 2);
            if (wazaCheckDarkWaza((u16)pp2) == 1 || pp2 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            pp2 += editDelta;
            if (pp2 < 0) pp2 = 0;
            if (pp2 > pokemonWazaGetMaxPP(pokemon, 2)) {
                pp2 = pokemonWazaGetMaxPP(pokemon, 2);
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 2, (u8)pp2);
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_PP_3: {
            if (!pokemonWazaCheckValid(pokemon, 3)) {
                break;
            }
            pp3 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 3);
            if (wazaCheckDarkWaza((u16)pp3) == 1 || pp3 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            pp3 += editDelta;
            if (pp3 < 0) pp3 = 0;
            if (pp3 > pokemonWazaGetMaxPP(pokemon, 3)) {
                pp3 = pokemonWazaGetMaxPP(pokemon, 3);
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 3, (u8)pp3);
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_PP_COUNT_0: {
            if (!pokemonWazaCheckValid(pokemon, 0)) {
                break;
            }
            ppCount0 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, 0);
            if (wazaCheckDarkWaza((u16)ppCount0) == 1 || ppCount0 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            ppCount0 += editDelta;
            if (ppCount0 < 0) ppCount0 = 0;
            if (ppCount0 > 3) ppCount0 = 3;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, 0, (u8)ppCount0);
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 0,
                              pokemonWazaGetMaxPP(pokemon, 0));
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_PP_COUNT_1: {
            if (!pokemonWazaCheckValid(pokemon, 1)) {
                break;
            }
            ppCount1 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, 1);
            if (wazaCheckDarkWaza((u16)ppCount1) == 1 || ppCount1 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            ppCount1 += editDelta;
            if (ppCount1 < 0) ppCount1 = 0;
            if (ppCount1 > 3) ppCount1 = 3;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, 1, (u8)ppCount1);
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 1,
                              pokemonWazaGetMaxPP(pokemon, 1));
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_PP_COUNT_2: {
            if (!pokemonWazaCheckValid(pokemon, 2)) {
                break;
            }
            ppCount2 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, 2);
            if (wazaCheckDarkWaza((u16)ppCount2) == 1 || ppCount2 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            ppCount2 += editDelta;
            if (ppCount2 < 0) ppCount2 = 0;
            if (ppCount2 > 3) ppCount2 = 3;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, 2, (u8)ppCount2);
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 2,
                              pokemonWazaGetMaxPP(pokemon, 2));
            break;
        }

        case POKEMON_EDIT_ITEM_WAZA_PP_COUNT_3: {
            if (!pokemonWazaCheckValid(pokemon, 3)) {
                break;
            }
            ppCount3 = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, 3);
            if (wazaCheckDarkWaza((u16)ppCount3) == 1 || ppCount3 == WAZA_SHADOW_SPECIAL) {
                break;
            }
            ppCount3 += editDelta;
            if (ppCount3 < 0) ppCount3 = 0;
            if (ppCount3 > 3) ppCount3 = 3;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, 3, (u8)ppCount3);
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP, 3,
                              pokemonWazaGetMaxPP(pokemon, 3));
            break;
        }

        case POKEMON_EDIT_ITEM_ABILITY_SLOT: {
            currentSlot = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ABILITY_SLOT, 0);
            newSlot = (currentSlot == 1) ? 0 : 1;
            abilitySpecies = (u16)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
            if (!pokemonGetStatus(NULL, abilitySpecies, POKEMON_BIOS_FIELD_HAS_ABILITY2, 1)) {
                newSlot = 0;
            }
            pokemonSetTokuseiFlag(pokemon, newSlot);
            if (abilityChanged != NULL) *abilityChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_ITEM: {
            itemId = pokemonGetSoubiItemDataId(pokemon);
            itemId += editDelta;
            if (itemId < 0) itemId = 0;
            if ((u32)itemId >= *item_data_index_number) itemId = *item_data_index_number - 1;
            pokemonDoItemSoubi(pokemon, (u16)itemId, 1);
            if (itemChanged != NULL) *itemChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_PID: {
            pid = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_PID, 0);
            if (editDelta == -POKEMON_EDIT_STEP_LIMIT) {
                pid = 0;
            } else if (editDelta == POKEMON_EDIT_STEP_LIMIT) {
                pid = -1;
            } else {
                if (!menuSubOpenNumberInput(pid, &numberInputResult, 1, 50, 50, 0)) {
                    menuSubCloseNumberInput();
                    break;
                }
                pid = numberInputResult;
                menuSubCloseNumberInput();
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, pid);
            changed = 1;
            if (appearanceChanged != NULL) *appearanceChanged = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_SEX: {
            value = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SEX, 0);
            value += editDelta;
            if (value < 0) value = 0;
            if ((u32)value >= sex_data_number) value = sex_data_number - 1;
            {
                u32 newPid = pokemonCreateRndFit(
                    pokemon, (s8)value,
                    (s8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NATURE, 0),
                    (s8)pokemonCheckRare2(pokemon),
                    pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0));
                pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, newPid);
            }
            changed = 1;
            if (appearanceChanged != NULL) *appearanceChanged = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_NATURE: {
            value = (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NATURE, 0);
            value += editDelta;
            if (value < 0) value = 0;
            if ((u32)value >= *pokemon_seikaku_data_number) value = *pokemon_seikaku_data_number - 1;
            pokemonSetStatus(
                pokemon, 0, POKEMON_STATUS_FIELD_PID, 0,
                pokemonCreateRndFit(
                    pokemon,
                    (s8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SEX, 0),
                    (s8)value,
                    (s8)pokemonCheckRare2(pokemon),
                    pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0)));
            changed = 1;
            if (appearanceChanged != NULL) *appearanceChanged = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_RARE: {
            value = (editDelta > 0) ? 1 : 0;
            pokemonSetStatus(
                pokemon, 0, POKEMON_STATUS_FIELD_PID, 0,
                pokemonCreateRndFit(
                    pokemon,
                    (s8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SEX, 0),
                    (s8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NATURE, 0),
                    (s8)value,
                    pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0)));
            changed = 1;
            if (appearanceChanged != NULL) *appearanceChanged = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_TRAINER_ID: {
            trainerId = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0);
            if (editDelta == -POKEMON_EDIT_STEP_LIMIT) {
                trainerId = 0;
            } else if (editDelta == POKEMON_EDIT_STEP_LIMIT) {
                trainerId = -1;
            } else {
                if (!menuSubOpenNumberInput(trainerId, &numberInputResult, 1, 50, 50, 0)) {
                    menuSubCloseNumberInput();
                    break;
                }
                trainerId = numberInputResult;
                menuSubCloseNumberInput();
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_TRAINER_ID, 0, trainerId);
            if (appearanceChanged != NULL) *appearanceChanged = 1;
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_FRIENDSHIP: {
            friendship = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0);
            friendship += editDelta;
            if (friendship < 0) friendship = 0;
            if (friendship > 255) friendship = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0, friendship);
            break;
        }

        case POKEMON_EDIT_ITEM_POKERUS: {
            pokerus = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POKERUS, 0);
            pokerus += editDelta;
            if (pokerus < 0) pokerus = 0;
            if (pokerus > 255) pokerus = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POKERUS, 0, pokerus);
            break;
        }

        case POKEMON_EDIT_ITEM_JOUTAI_KIND: {
            kind = pokemonGetJoutaiDataId(pokemon);
            if (kind == 0) {
                /* Stepping off "no status" lands on the first ailment. */
                kind = editDelta + (JOUTAI_POISON - 1);
            } else {
                kind += editDelta;
            }
            if (kind < JOUTAI_POISON) kind = 0;
            if (kind > JOUTAI_SLEEP) kind = JOUTAI_SLEEP;
            pokemonInitJoutai(pokemon);
            if (kind != 0) {
                if (pokemonCheckWriteJoutaiDataId(pokemon, kind) == JOUTAI_WRITE_ACCEPT) {
                    pokemonWriteJoutaiDataId(pokemon, kind, 0);
                }
            }
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_JOUTAI_KAISUU: {
            joutaiId = pokemonGetJoutaiDataId(pokemon);
            if (joutaiId == 0) {
                break;
            }
            kaisuu = pokemonGetKaisuuJoutaiDataId(pokemon, joutaiId);
            if (joutaiId != JOUTAI_BAD_POISON) {
                if (kaisuu < 0) break;
                kaisuu += editDelta;
                if (kaisuu < 0) kaisuu = 0;
                if (kaisuu > 16) kaisuu = 16;
                pokemonSetKaisuuJoutaiDataId(pokemon, joutaiId, (s8)kaisuu);
                if (pokemonGetNowKaisuuJoutaiDataId(pokemon, joutaiId) > kaisuu) {
                    pokemonSetNowKaisuuJoutaiDataId(pokemon, joutaiId, (s8)kaisuu);
                }
            } else {
                extCount = pokemonGetCountAsJoutaiDataId(pokemon, joutaiId);
                extCount += editDelta;
                if (extCount < 0) extCount = 0;
                if (extCount > joutaiDataBiosGetMaxCount(joutaiId)) {
                    extCount = joutaiDataBiosGetMaxCount(joutaiId);
                }
                pokemonSetCountAsJoutaiDataId(pokemon, joutaiId, (s16)extCount);
            }
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_JOUTAI_NOW_KAISUU: {
            joutaiId = pokemonGetJoutaiDataId(pokemon);
            if (joutaiId == 0) {
                break;
            }
            maxKaisuu = pokemonGetKaisuuJoutaiDataId(pokemon, joutaiId);
            if (joutaiId == JOUTAI_BAD_POISON) break;
            if (maxKaisuu < 0) break;
            nowKaisuu = pokemonGetNowKaisuuJoutaiDataId(pokemon, joutaiId);
            nowKaisuu += editDelta;
            if (nowKaisuu < 0) nowKaisuu = 0;
            if (nowKaisuu > pokemonGetKaisuuJoutaiDataId(pokemon, joutaiId)) {
                nowKaisuu = pokemonGetKaisuuJoutaiDataId(pokemon, joutaiId);
            }
            pokemonSetNowKaisuuJoutaiDataId(pokemon, joutaiId, (s8)nowKaisuu);
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_DARK_ID: {
            value = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DARK_ID, 0);
            value += editDelta;
            if (value < 0) value = 0;
            if ((u32)value >= darkPokemonList_number) value = darkPokemonList_number - 1;
            if (value == 0) {
                pokemonInitDarkPokemon(pokemon);
            } else {
                pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DARK_ID, 0, value);
                pokemonSetDarkPokemonStatus(pokemon, (u16)value);
            }
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_DARK_POINTS: {
            darkId = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DARK_ID, 0);
            if (darkId == 0) break;
            value = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DARK_POINTS, 0);
            if (editDelta == -POKEMON_EDIT_STEP_LIMIT) {
                value = -1;
            } else if (editDelta == POKEMON_EDIT_STEP_LIMIT) {
                value = 65535;
            } else {
                if (!menuSubOpenNumberInput(value, &numberInputResult, 0, 50, 50, 0)) {
                    menuSubCloseNumberInput();
                    break;
                }
                value = (s32)numberInputResult;
                menuSubCloseNumberInput();
                if (value < -1) value = -1;
                if (value > 65535.0f) value = 65535;
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DARK_POINTS, 0, value);
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_DARK_JOUTAI: {
            if (!pokemonIsDarkPokemon(pokemon)) break;
            if (editDelta > 0) {
                pokemonWriteJoutaiDataId(pokemon, JOUTAI_DARK_HYPER_MODE, 0);
            } else {
                pokemonInitJoutaiDataId(pokemon, JOUTAI_DARK_HYPER_MODE);
            }
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_POOL_EXP: {
            darkId = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DARK_ID, 0);
            if (darkId == 0) break;
            poolExp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POOL_EXP, 0);
            if (editDelta == -POKEMON_EDIT_STEP_LIMIT) {
                poolExp = 0;
            } else if (editDelta == POKEMON_EDIT_STEP_LIMIT) {
                poolExp = 2000000;
            } else {
                if (!menuSubOpenNumberInput(poolExp, &numberInputResult, 1, 50, 50, 0)) {
                    menuSubCloseNumberInput();
                    break;
                }
                poolExp = (s32)numberInputResult;
                menuSubCloseNumberInput();
                if (poolExp < 0) poolExp = 0;
                if (poolExp > 2000000) poolExp = 2000000;
            }
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POOL_EXP, 0, poolExp);
            break;
        }

        case POKEMON_EDIT_ITEM_POOL_FRIEND: {
            darkId = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DARK_ID, 0);
            if (darkId == 0) break;
            poolFriend = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POOL_FRIEND, 0);
            poolFriend += editDelta;
            if (poolFriend < 0) poolFriend = 0;
            if (poolFriend > 255) poolFriend = 255;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_POOL_FRIEND, 0, poolFriend);
            break;
        }

        case POKEMON_EDIT_ITEM_KEY_PLAYER_FLAG: {
            if (trainerIndex == 0) break;
            if (inFight != 1) break;
            keyPlayerFlag = (u8)fightTrainerGetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_KEY_PLAYER_FLAG, 0);
            keyPlayerFlag += editDelta;
            if (keyPlayerFlag < 0) keyPlayerFlag = 0;
            if (keyPlayerFlag > 3) keyPlayerFlag = 3;
            fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_KEY_PLAYER_FLAG, 0, (u8)keyPlayerFlag);
            break;
        }

        case POKEMON_EDIT_ITEM_FIGHT_PART_DATA: {
            if (trainerIndex == 0) break;
            if (inFight != 1) break;
            partDataId = (u16)fightTrainerGetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_PART_DATA_ID, 0);
            partDataId += editDelta;
            if (partDataId < 0) partDataId = 0;
            if ((u32)partDataId >= *fight_trainer_pokemon_part_data_number) {
                partDataId = *fight_trainer_pokemon_part_data_number - 1;
            }
            fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_PART_DATA_ID, 0, (u16)partDataId);
            break;
        }

        case POKEMON_EDIT_ITEM_CAUGHT_BALL: {
            ball = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_CAUGHT_BALL, 0);
            ball += editDelta;
            if (ball < POKEMON_CATCH_BALL_NONE) ball = POKEMON_CATCH_BALL_NONE;
            if (ball > POKEMON_CATCH_BALL_MAX) ball = POKEMON_CATCH_BALL_MAX;
            pokemonSetStatus(pokemon, 0, POKEMON_STATUS_FIELD_CAUGHT_BALL, 0, ball);
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_RIBBON: {
            ribbon = pokemonGetStatus(pokemon, 0, gRibbonStat[gRibbonNum], 0);
            ribbon += editDelta;
            if (ribbon < 0) ribbon = 0;
            if (ribbon > 4) ribbon = 4;
            pokemonSetStatus(pokemon, 0, gRibbonStat[gRibbonNum], 0, ribbon);
            if (statusChanged != NULL) *statusChanged = 1;
            break;
        }

        case POKEMON_EDIT_ITEM_RIBBON_PAGE:
            gRibbonNum += editDelta;
            if (gRibbonNum < 0) {
                gRibbonNum = 0;
            }
            if (gRibbonNum > 17) {
                gRibbonNum = 17;
            }
            break;
        }

        if (changed == 1) {
            u8 slot = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ABILITY_SLOT, 0);
            pokemonSetTokuseiFlag(pokemon, slot);
            if (menuId == POKEMON_EDIT_ITEM_LEVEL) {
                exp = pokemonGetNowLevelToExp(pokemon);
            } else {
                exp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0);
            }
            pokemonGrowBasisStatus(pokemon, exp);
        }
    }

commit:
    menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
    menuCloseCustom(MENU_ID_DEBUG_POKEMON_EDITOR, 0, 1);
    {
        u16 species = pokemonBiosGetPokemonDataId(pokemon);
        switch (species) {
        /* Mew and Deoxys: the event flag follows the ball the pokemon was
           caught in. */
        case POKEMON_SPECIES_DEOXYS:
        case POKEMON_SPECIES_MEW: {
            u8 ball = (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_CAUGHT_BALL, 0);
            if (ball == POKEMON_CATCH_BALL_POKE_BALL || ball == POKEMON_CATCH_BALL_NONE) {
                pokemonBiosSetEventGetFlag(pokemon, 1);
            } else {
                pokemonBiosSetEventGetFlag(pokemon, 0);
            }
            break;
        }
        }
    }

    if (!pokemonCheckValid(pokemon)) {
        pokemonInit(pokemon);
        goto done;
    }

    if (!itemDataCheckValid(pokemonGetSoubiItemDataId(pokemon))) {
        pokemonDoItemSoubi(pokemon, 0, 0);
    }

    if (trainerIndex != 0 && inFight == 1) {
        value = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SPECIES, 0);
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_SPECIES, 0, value);
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_NICKNAME, 0,
                               pokemonGetStatus(NULL, (u16)value,
                                                POKEMON_BIOS_FIELD_DEFAULT_NAME, 0));
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_LEVEL, 0,
                               pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_LEVEL, 0));
        {
            s32 entry;
            u16 *fieldPtr = ivFieldIds;
            for (entry = 0; entry < 6; entry++) {
                fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_IV, entry,
                                       pokemonGetStatus(pokemon, 0, *fieldPtr, 0));
                fieldPtr++;
            }
        }
        {
            s32 entry;
            u16 *fieldPtr = effortFieldIds;
            for (entry = 0; entry < 6; entry++) {
                fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_EFFORT, entry,
                                       pokemonGetStatus(pokemon, 0, *fieldPtr, 0));
                fieldPtr++;
            }
        }
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_TOKUSEI_SLOT, 0,
                               (u8)pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_ABILITY_SLOT, 0));
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_ITEM, 0,
                               pokemonGetSoubiItemDataId(pokemon));
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_FRIEND_LEVEL, 0,
                               pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0));
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_SEX, 0,
                               pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_SEX, 0));
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_NATURE, 0,
                               pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_NATURE, 0));
        {
            s32 i;
            for (i = 0; i < 4; i++) {
                fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_WAZA_ID, i,
                                       pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_ID, i));
                fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_WAZA_PP_COUNT, i,
                                       pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_WAZA_PP_COUNT, i));
            }
        }
        fightTrainerSetStatus(0, trainerIndex, FIGHT_TRAINER_QUERY_POKEMON_DARK_ID, 0,
                               pokemonGetStatus(pokemon, 0, POKEMON_STATUS_FIELD_DARK_ID, 0));
    }

done:
    return 1;
}

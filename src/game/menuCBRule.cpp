/*
 * menuCBRule.cpp
 *
 * The colosseum regulations of the link (GBA) battles: the checks a rule set
 * runs over an entered party and over a single Pokemon, and the per-slot
 * condition records the entry screens report the failures of.
 */

#include <FightRuleData.h>
#include <menuCBBios.h>
#include <menuCBRule.h>
#include <Pokemon.h>
#include <types.h>

/* -------------------------------------------------------------------------
 * External data and functions
 * ---------------------------------------------------------------------- */

/* The fixed rule set the ore colosseum plays. */
extern const FightRuleData oreRule;

enum { MAX_TOTAL_EFFORT = 510, MAX_STAT_EFFORT = 255 };

/* Count and flat species-ID list for the legendary Pokemon restriction. */
extern u32 *gLegendPokemonCount;
extern u16 *gLegendPokemonList;

extern "C" {
extern FightRuleData *menuCBBios_CurrentRule(void);
extern u8 menuCBRulesetGetDirty(void);
extern Pokemon *heroBiosGetPokemonPtr(Pokemon *party, u16 slot);
extern u16 pokemonGetHeldItem(Pokemon *pokemon);
extern u16 battlePokemonGetSpecies(Pokemon *pokemon);
extern u16 battlePokemonStatusGetMaxHp(Pokemon *pokemon);
extern u16 battlePokemonStatusGetAttack(Pokemon *pokemon);
extern u16 battlePokemonStatusGetDef(Pokemon *pokemon);
extern u16 battlePokemonStatusGetSpAtk(Pokemon *pokemon);
extern u16 battlePokemonStatusGetSpDef(Pokemon *pokemon);
extern u8 pokemonIsShadowPokemon(Pokemon *pokemon);
extern u8 pokemon_GetTamagoFlag(Pokemon *pokemon);
extern u8 pokemon_GetLevel(Pokemon *pokemon);
extern bool pokemonCheckValid(Pokemon *pokemon);
extern s32 pokemonGetStatus(Pokemon *pokemon, s32 arg1, s32 id, u16 arg3);
extern void pokemonBiosCopy(Pokemon *dst, Pokemon *src);
extern u8 pokemonBiosGetEventGetFlag(Pokemon *pokemon);
extern void pokemonResetBasisStatus(Pokemon *pokemon);
extern u32 wazaDataBiosGetPtr(u16 id);
extern u16 pokemonGetMaxHpEffort(Pokemon *pokemon);
extern u16 pokemonGetPhyAtkEffort(Pokemon *pokemon);
extern u16 pokemonGetPhyDefEffort(Pokemon *pokemon);
extern u16 pokemonGetSpeAtkEffort(Pokemon *pokemon);
extern u16 pokemonGetSpeDefEffort(Pokemon *pokemon);
extern u16 pokemonGetSpeedEffort(Pokemon *pokemon);
extern u16 pokemonGetNimbleness(Pokemon *pokemon);
extern bool menuCBRule_CheckValidItem(u16 itemID);
extern bool menuCBRule_CheckRegulationItem(u16 itemID);
extern void menuCBEntrySlotFlagsClear(s32 entry, s32 slot);
}

/* -------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------- */

extern "C" u8 menuCBRule_IsBlankPokemon(Pokemon *pokemon);
extern "C" u8 menuCBRule_CheckPokemonEventFlag(Pokemon *pokemon);
extern "C" u8 menuCBRule_CheckPokemonErrorAll(Pokemon *pokemon);
extern "C" u8 menuCBRule_CheckPokemonRegulationAll(Pokemon *pokemon, FightRuleData *rule);
extern "C" u8 menuCBRule_CheckTrainerRegulationAll(Pokemon *party, FightRuleData *rule);
extern "C" u8 menuCBRule_CheckRegulationAll(Pokemon *party, FightRuleData *rule);
extern "C" u8 menuCBRule_HasTrainerError(Pokemon *party, s32 index);
extern "C" u8 menuCBRule_CheckTrainerErrorAll(Pokemon *party);
extern "C" u8 menuCBRule_CheckErrorAll(Pokemon *party);
extern "C" s32 menuCBRule_CheckHero(Pokemon *party);
u8 _menuCBRule_IsTamagoPokemon(Pokemon *pokemon);

/* -------------------------------------------------------------------------
 * Rule set checks
 * ---------------------------------------------------------------------- */

/* Whether the species is one a legend rule set bars from the entry. */
extern "C" u8 menuCBRule_CheckLegendPokemon(Pokemon *pokemon, FightRuleData *rule)
{
    u32 count;
    const u16 *list;
    s32 i;
    u16 species;

    if (rule->mAllowLegendPokemon == 1) {
        return 1;
    }

    species = battlePokemonGetSpecies(pokemon);
    count = *gLegendPokemonCount;
    list = gLegendPokemonList;

    for (i = 0; i < count; i++) {
        if (list[i] == species) {
            return 0;
        }
    }

    return 1;
}

extern "C" void menuCBEntrySlotConditionsBuildAll(Pokemon *party, s32 entry)
{
    FightRuleData *rule = menuCBBios_CurrentRule();
    u32 i;

    for (i = 0; i < 6; i++) {
        Pokemon *pokemon = heroBiosGetPokemonPtr(party, i);

        pokemonGetHeldItem(pokemon);
        menuCBEntrySlotFlagsClear(entry, i);

        menuCBTaisenWork.entryFlags[entry][i].levelBelowMinimum =
            !menuCBRule_CheckPokemonRegulation(pokemon, rule, REGULATION_POKEMON_LEVEL_MIN);
        menuCBTaisenWork.entryFlags[entry][i].levelAboveMaximum =
            !menuCBRule_CheckPokemonRegulation(pokemon, rule, REGULATION_POKEMON_LEVEL_MAX);

        if (!menuCBRule_CheckPokemonRegulation(pokemon, rule, REGULATION_POKEMON_ITEM)) {
            switch (rule->mHeldItemMode) {
            case HELD_ITEMS_FORBIDDEN:
                menuCBTaisenWork.entryFlags[entry][i].heldItemForbidden = 1;
                break;

            case HELD_ITEMS_RESTRICTED:
                menuCBTaisenWork.entryFlags[entry][i].heldItemRestricted = 1;
                break;

            case HELD_ITEMS_ALLOWED:
                break;

            default:
                break;
            }
        }

        menuCBTaisenWork.entryFlags[entry][i].illegalPokemon = !menuCBRule_CheckPokemonKind(pokemon, POKEMON_CONDITION_ILLEGAL);
        menuCBTaisenWork.entryFlags[entry][i].shadowPokemon = !menuCBRule_CheckPokemonKind(pokemon, POKEMON_CONDITION_SHADOW);
        menuCBTaisenWork.entryFlags[entry][i].eggOrInvalidPokemon = !menuCBRule_CheckPokemonKind(pokemon, POKEMON_CONDITION_TAMAGO);
        menuCBTaisenWork.entryFlags[entry][i].invalidHeldItem = !menuCBRule_CheckPokemonKind(pokemon, POKEMON_CONDITION_HELD_ITEM);
        menuCBTaisenWork.entryFlags[entry][i].invalidMove = !menuCBRule_CheckPokemonKind(pokemon, POKEMON_CONDITION_ILLEGAL_MOVE);
        menuCBTaisenWork.entryFlags[entry][i].invalidPokemon = !menuCBRule_CheckPokemonKind(pokemon, POKEMON_CONDITION_VALID);
        menuCBTaisenWork.entryFlags[entry][i].partyLevelSumExceeded =
            !menuCBRule_CheckTrainerRegulation(party, rule, REGULATION_TRAINER_LEVEL_SUM);
        menuCBTaisenWork.entryFlags[entry][i].duplicateSpecies =
            !menuCBRule_CheckTrainerRegulationAt(party, pokemon, rule, REGULATION_TRAINER_SAME_SPECIES);
        menuCBTaisenWork.entryFlags[entry][i].duplicateHeldItem =
            !menuCBRule_CheckTrainerRegulationAt(party, pokemon, rule, REGULATION_TRAINER_SAME_ITEM);
        menuCBTaisenWork.entryFlags[entry][i].insufficientPokemon =
            !menuCBRule_CheckTrainerRegulation(party, rule, REGULATION_TRAINER_POKEMON_NUM);
        menuCBTaisenWork.entryFlags[entry][i].legendPokemon =
            !menuCBRule_CheckLegendPokemon(pokemon, rule);
    }
}

/* Mew and Deoxys may only be entered once the event released them. */
extern "C" u8 menuCBRule_CheckPokemonEventFlag(Pokemon *pokemon)
{
    u16 species = battlePokemonGetSpecies(pokemon);

    switch (species) {
    case POKEMON_SPECIES_DEOXYS:
    case POKEMON_SPECIES_MEW:
        if (!pokemonBiosGetEventGetFlag(pokemon)) {
            return 0;
        }
        break;

    default:
        break;
    }

    return 1;
}

extern "C" u8 menuCBRule_CheckErrorAll(Pokemon *party)
{
    u16 i;

    if (!menuCBRule_CheckTrainerErrorAll(party)) {
        return 0;
    }

    for (i = 0; i < 6; i++) {
        Pokemon *pokemon = heroBiosGetPokemonPtr(party, i);

        if (!menuCBRule_CheckPokemonErrorAll(pokemon)) {
            return 0;
        }
    }

    return 1;
}

extern "C" u8 menuCBRule_CheckTrainerErrorAll(Pokemon *party)
{
    s32 i;

    // There are no trainer-record checks in this version.
    for (i = 0; i < 0; i++) {
        if (!menuCBRule_HasTrainerError(party, i)) {
            return 0;
        }
    }

    return 1;
}

/* A trainer record carries no regulation of its own. */
extern "C" u8 menuCBRule_HasTrainerError(Pokemon *party, s32 index)
{
    return 0;
}

/* Every per-Pokemon condition of one Pokemon. */
extern "C" u8 menuCBRule_CheckPokemonErrorAll(Pokemon *pokemon)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        if (!menuCBRule_CheckPokemonKind(pokemon, i)) {
            return 0;
        }
    }

    return 1;
}

/* The single per-Pokemon condition `kind` asks for. */
extern "C" bool menuCBRule_CheckPokemonKind(Pokemon *pokemon, s32 kind)
{
    Pokemon basis;
    u32 total;
    s32 i;

    if (menuCBRule_IsBlankPokemon(pokemon)) {
        return 1;
    }

    switch (kind) {
    case POKEMON_CONDITION_ILLEGAL:
        if (!menuCBRule_CheckPokemonKind(pokemon, POKEMON_CONDITION_TAMAGO)) {
            return 1;
        }

        if (!menuCBRule_CheckPokemonEventFlag(pokemon)) {
            return 0;
        }

        /* Stored stats must not exceed the stats recalculated from experience
           and effort values. */
        pokemonBiosCopy(&basis, pokemon);
        pokemonResetBasisStatus(&basis);

        if (battlePokemonStatusGetMaxHp(&basis) < battlePokemonStatusGetMaxHp(pokemon) ||
            battlePokemonStatusGetAttack(&basis) < battlePokemonStatusGetAttack(pokemon) ||
            battlePokemonStatusGetDef(&basis) < battlePokemonStatusGetDef(pokemon) ||
            battlePokemonStatusGetSpAtk(&basis) < battlePokemonStatusGetSpAtk(pokemon) ||
            battlePokemonStatusGetSpDef(&basis) < battlePokemonStatusGetSpDef(pokemon) ||
            pokemonGetNimbleness(&basis) < pokemonGetNimbleness(pokemon)) {
            return 0;
        }

        total = pokemonGetMaxHpEffort(pokemon);
        total += pokemonGetPhyAtkEffort(pokemon);
        total += pokemonGetPhyDefEffort(pokemon);
        total += pokemonGetSpeAtkEffort(pokemon);
        total += pokemonGetSpeDefEffort(pokemon);
        total += pokemonGetSpeedEffort(pokemon);

        if (total > MAX_TOTAL_EFFORT) {
            return 0;
        }

        if (pokemonGetMaxHpEffort(pokemon) > MAX_STAT_EFFORT ||
            pokemonGetPhyAtkEffort(pokemon) > MAX_STAT_EFFORT ||
            pokemonGetPhyDefEffort(pokemon) > MAX_STAT_EFFORT ||
            pokemonGetSpeAtkEffort(pokemon) > MAX_STAT_EFFORT ||
            pokemonGetSpeDefEffort(pokemon) > MAX_STAT_EFFORT ||
            pokemonGetSpeedEffort(pokemon) > MAX_STAT_EFFORT) {
            return 0;
        }

        return 1;

    case POKEMON_CONDITION_SHADOW:
        return !pokemonIsShadowPokemon(pokemon);

    case POKEMON_CONDITION_TAMAGO:
        return !pokemon_GetTamagoFlag(pokemon);

    case POKEMON_CONDITION_HELD_ITEM:
        return menuCBRule_CheckValidItem(pokemonGetHeldItem(pokemon));

    case POKEMON_CONDITION_ILLEGAL_MOVE:
        for (i = 0; i < 4; i++) {
            u16 move = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_MOVE, i);

            if (move != 0 && wazaDataBiosGetPtr(move) == wazaDataBiosGetPtr(0)) {
                return 0;
            }
        }

        return 1;

    case POKEMON_CONDITION_VALID:
        pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIES, 0);
        return pokemonCheckValid(pokemon);

    default:
        break;
    }

    return 0;
}

extern "C" u8 menuCBRule_CheckRegulationAll(Pokemon *party, FightRuleData *rule)
{
    s32 i;
    s32 num = 0;

    if (!menuCBRule_CheckTrainerRegulationAll(party, rule)) {
        return 0;
    }

    for (i = 0; i < 6; i++) {
        Pokemon *pokemon = heroBiosGetPokemonPtr(party, i);

        if (pokemon == 0 || !pokemonCheckValid(pokemon)) {
            continue;
        }

        if (!menuCBRule_CheckPokemonRegulationAll(pokemon, rule)) {
            return 0;
        }

        if (!menuCBRule_CheckLegendPokemon(pokemon, rule)) {
            return 0;
        }

        num++;
    }

    return num > 0;
}

extern "C" u8 menuCBRule_CheckTrainerRegulationAll(Pokemon *party, FightRuleData *rule)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (!menuCBRule_CheckTrainerRegulation(party, rule, i)) {
            return 0;
        }
    }

    return 1;
}

/* Regulation `kind` of one slot against the party it is entered from. */
extern "C" bool menuCBRule_CheckTrainerRegulationAt(Pokemon *party, Pokemon *pokemon,
                                                   FightRuleData *rule, s32 kind)
{
    s32 i;

    switch (kind) {
    case REGULATION_TRAINER_LEVEL_SUM:
        return menuCBRule_CheckTrainerRegulation(party, rule, kind);

    case REGULATION_TRAINER_SAME_SPECIES:
        if (rule->mAllowSameSpecies != 0) {
            return 1;
        }

        if (menuCBRule_IsBlankPokemon(pokemon) || _menuCBRule_IsTamagoPokemon(pokemon)) {
            return 1;
        }

        for (i = 0; i < 6; i++) {
            Pokemon *other = heroBiosGetPokemonPtr(party, i);

            if (other == pokemon) {
                continue;
            }

            if (menuCBRule_IsBlankPokemon(other)) {
                continue;
            }

            if (_menuCBRule_IsTamagoPokemon(other)) {
                continue;
            }

            if (battlePokemonGetSpecies(other) == battlePokemonGetSpecies(pokemon)) {
                return 0;
            }
        }

        return 1;

    case REGULATION_TRAINER_SAME_ITEM:
        if (rule->mAllowSameItem != 0) {
            return 1;
        }

        if (menuCBRule_IsBlankPokemon(pokemon) || _menuCBRule_IsTamagoPokemon(pokemon)) {
            return 1;
        }

        if (pokemonGetHeldItem(pokemon) == 0) {
            return 1;
        }

        for (i = 0; i < 6; i++) {
            Pokemon *other = heroBiosGetPokemonPtr(party, i);

            if (other == pokemon) {
                continue;
            }

            if (menuCBRule_IsBlankPokemon(other)) {
                continue;
            }

            if (_menuCBRule_IsTamagoPokemon(other)) {
                continue;
            }

            if (pokemonGetHeldItem(other) == pokemonGetHeldItem(pokemon)) {
                return 0;
            }
        }

        return 1;

    case REGULATION_TRAINER_POKEMON_NUM:
        return menuCBRule_CheckTrainerRegulation(party, rule, REGULATION_TRAINER_POKEMON_NUM) ||
            menuCBRule_IsBlankPokemon(pokemon) == 0;

    default:
        break;
    }

    return 0;
}

/* The rule set party-wide regulation `kind`. */
extern "C" bool menuCBRule_CheckTrainerRegulation(Pokemon *party, FightRuleData *rule,
                                                s32 kind)
{
    s32 levelSum = 0;
    bool sameSpecies = true;
    bool sameItem = true;
    u32 i;
    Pokemon *pokemon;
    u32 j;
    Pokemon *other;

    for (i = 0; i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(party, i);

        if (menuCBRule_IsBlankPokemon(pokemon)) {
            continue;
        }

        if (_menuCBRule_IsTamagoPokemon(pokemon)) {
            continue;
        }

        levelSum += pokemon_GetLevel(pokemon);

        for (j = i + 1; j < 6; j++) {
            other = heroBiosGetPokemonPtr(party, j);

            if (menuCBRule_IsBlankPokemon(other)) {
                continue;
            }

            if (_menuCBRule_IsTamagoPokemon(other)) {
                continue;
            }

            if (pokemonGetHeldItem(pokemon) != 0) {
                sameItem &= pokemonGetHeldItem(pokemon) != pokemonGetHeldItem(other);
            }

            sameSpecies &= battlePokemonGetSpecies(pokemon) != battlePokemonGetSpecies(other);
        }
    }

    switch (kind) {
    case REGULATION_TRAINER_LEVEL_SUM:
        return levelSum <= rule->mPokemonLevelSumMax;

    case REGULATION_TRAINER_SAME_SPECIES:
        return rule->mAllowSameSpecies != 0 || sameSpecies;

    case REGULATION_TRAINER_SAME_ITEM:
        return rule->mAllowSameItem != 0 || sameItem;

    case REGULATION_TRAINER_POKEMON_NUM: {
        s32 num = 0;
        u16 slot;

        for (slot = 0; slot < 6; slot++) {
            if (!menuCBRule_IsBlankPokemon(heroBiosGetPokemonPtr(party, slot))) {
                num++;
            }
        }

        if (rule->mPokemonCountMode == 1) {
            return rule->mPokemonCountMode <= (u16)num;
        }

        return rule->mPokemonCount <= (u16)num;
    }

    default:
        break;
    }

    return 0;
}

extern "C" u8 menuCBRule_CheckPokemonRegulationAll(Pokemon *pokemon, FightRuleData *rule)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        if (!menuCBRule_CheckPokemonRegulation(pokemon, rule, i)) {
            return 0;
        }
    }

    return 1;
}

/* The Pokemon-only regulations of a rule set, for one Pokemon. */
extern "C" bool menuCBRule_CheckPokemonRegulation(Pokemon *pokemon, FightRuleData *rule,
                                                 s32 kind)
{
    if (menuCBRule_IsBlankPokemon(pokemon)) {
        return 1;
    }

    switch (kind) {
    case REGULATION_POKEMON_LEVEL_MIN:
        return pokemon_GetLevel(pokemon) >= rule->mPokemonLevelMin;

    case REGULATION_POKEMON_LEVEL_MAX:
        return pokemon_GetLevel(pokemon) <= rule->mPokemonLevelMax;

    case REGULATION_POKEMON_ITEM:
        return menuCBRule_CheckRegulationItem(pokemonGetHeldItem(pokemon));

    default:
        break;
    }

    return 0;
}

/* Whether the party may enter a battle under the current rule set. */
extern "C" s32 menuCBRule_CheckHero(Pokemon *party)
{
    s32 result = 0;

    if (menuCBRule_CheckErrorAll(party)) {
        if (menuCBRule_CheckRegulationAll(party, menuCBBios_CurrentRule())) {
            result = 1;
        }
    }

    return result;
}

/* Whether `pokemon` is still an egg, or an entry that never became one. */
u8 _menuCBRule_IsTamagoPokemon(Pokemon *pokemon)
{
    u8 result;

    if (menuCBRule_IsBlankPokemon(pokemon)) {
        result = 0;
    } else {
        result = 0;

        if (pokemon_GetTamagoFlag(pokemon) || !pokemonCheckValid(pokemon)) {
            result = 1;
        }
    }

    return result;
}

/* The fixed rule set the ore colosseum plays, while its rules are clean. */
extern "C" FightRuleData *menuCB_OreRule(void)
{
    if (menuCBRulesetGetDirty()) {
        return 0;
    }

    return (FightRuleData *)&oreRule;
}

/* Whether `pokemon` is an empty party slot. */
extern "C" u8 menuCBRule_IsBlankPokemon(Pokemon *pokemon)
{
    u8 result = 0;

    if (pokemon == 0 || pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIES, 0) == 0) {
        result = 1;
    }

    return result;
}

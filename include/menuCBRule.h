#ifndef MENU_CB_RULE_H
#define MENU_CB_RULE_H

#include <FightRuleData.h>
#include <Pokemon.h>
#include <types.h>

/* Species the colosseum holds back until its event is taken: Mew and the
   Deoxys forms. */
#define POKEMON_SPECIES_MEW 0x97
#define POKEMON_SPECIES_DEOXYS 0x19A

/* The per-Pokemon conditions the entry screens report slot by slot. */
enum PokemonCondition {
    /* Illegal flags, event availability and the effort values a freshly
       caught Pokemon may not carry. */
    POKEMON_CONDITION_ILLEGAL = 0,
    POKEMON_CONDITION_SHADOW = 1,
    POKEMON_CONDITION_TAMAGO = 2,
    POKEMON_CONDITION_HELD_ITEM = 3,
    POKEMON_CONDITION_ILLEGAL_MOVE = 4,
    POKEMON_CONDITION_VALID = 5
};

/* The Pokemon-only regulations of a rule set. */
enum PokemonRegulation {
    REGULATION_POKEMON_LEVEL_MIN = 0,
    REGULATION_POKEMON_LEVEL_MAX = 1,
    REGULATION_POKEMON_ITEM = 2
};

/* The party-wide regulations of a rule set. */
enum TrainerRegulation {
    REGULATION_TRAINER_LEVEL_SUM = 0,
    REGULATION_TRAINER_SAME_SPECIES = 1,
    REGULATION_TRAINER_SAME_ITEM = 2,
    REGULATION_TRAINER_POKEMON_NUM = 3
};

#ifdef __cplusplus
extern "C" {
#endif

bool menuCBRule_CheckPokemonKind(Pokemon *pokemon, s32 kind);
bool menuCBRule_CheckPokemonRegulation(Pokemon *pokemon, FightRuleData *rule, s32 kind);
bool menuCBRule_CheckTrainerRegulation(Pokemon *party, FightRuleData *rule, s32 kind);
bool menuCBRule_CheckTrainerRegulationAt(Pokemon *party, Pokemon *pokemon,
                                         FightRuleData *rule, s32 kind);
u8 menuCBRule_CheckLegendPokemon(Pokemon *pokemon, FightRuleData *rule);

#ifdef __cplusplus
}
#endif

#endif

#ifndef MENU_CB_RULE_H
#define MENU_CB_RULE_H

#include <FightRuleData.h>
#include <Pokemon.h>
#include <hero.h>
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

FightRuleData *menuCB_OreRule(void);
extern unsigned long menuCBRule_RegulationItemNum;
u16 menuCBRule_RegulationItemID(s32 index);

// Entry condition reports for the Hero's party.
void menuCBEntrySlotConditionsBuild(Hero *hero, Pokemon *pokemon, s32 slot);
void menuCBEntrySlotConditionsBuildAll(Hero *hero, s32 entry);
void menuCBEntrySlotConditionsSetTrainerRegulation(Hero *hero);
void menuCBEntrySlotConditionsSetItemRegulation(Hero *hero, Pokemon *pokemon, s32 slot);

u8 menuCBRule_IsBlankPokemon(Pokemon *pokemon);
u8 menuCBRule_CheckPokemonEventFlag(Pokemon *pokemon);
u8 menuCBRule_CheckPokemonErrorAll(Pokemon *pokemon);
u8 menuCBRule_CheckPokemonRegulationAll(Pokemon *pokemon, FightRuleData *rule);
u8 menuCBRule_CheckTrainerRegulationAll(Hero *hero, FightRuleData *rule);
u8 menuCBRule_CheckRegulationAll(Hero *hero, FightRuleData *rule);
u8 menuCBRule_CheckTrainerError(Hero *hero, s32 index);
u8 menuCBRule_CheckTrainerErrorAll(Hero *hero);
u8 menuCBRule_CheckErrorAll(Hero *hero);
s32 menuCBRule_CheckHero(Hero *hero);

u8 menuCBRule_EnableSkill_SonicBoom(void);
u8 menuCBRule_EnableSkill_Michidure(void);
u8 menuCBRule_ProhibitionSuicide(void);
u8 menuCBRule_EnableSkillSwap(void);
u8 menuCBRule_EnableSbToIceed(void);
u8 menuCBRule_EnableSbToSleep(void);
u8 menuCBRule_DEOKISISUCamouflage(void);
s32 menuCBRule_GetCommandTimeLimit(void);
s32 menuCBRule_GetBattleTimeLimit(void);
bool menuCBRule_CheckValidItem(u16 itemID);

bool menuCBRule_CheckPokemonKind(Pokemon *pokemon, s32 kind);
bool menuCBRule_CheckPokemonRegulation(Pokemon *pokemon, FightRuleData *rule, s32 kind);
bool menuCBRule_CheckTrainerRegulation(Hero *hero, FightRuleData *rule, s32 kind);
bool menuCBRule_CheckTrainerRegulationAt(Hero *hero, Pokemon *pokemon,
                                        FightRuleData *rule, s32 kind);
u8 menuCBRule_CheckLegendPokemon(Pokemon *pokemon, FightRuleData *rule);

#ifdef __cplusplus
}
#endif

#endif

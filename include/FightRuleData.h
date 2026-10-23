#ifndef FIGHT_RULE_DATA_H
#define FIGHT_RULE_DATA_H

#include <types.h>

typedef struct FightRuleData FightRuleData;

// FightRuleData -- one battle regulation record: the entry fees, time limits
// and restriction lists a link battle enforces, plus the message shown for
// it. The colosseum keeps a table of six of these in the save
// (savedataGetStatus(0, 0xE)), the _MENUCBWORK work area
// mirrors it, and the read-only defaults live in rodata. One record is 0x90
// bytes; the title and restriction list follow the battle settings.
struct FightRuleData {
    s16 mPokemonLevelMin; // 0x00: lowest level an entered Pokemon may have.
    s16 mPokemonLevelMax; // 0x02: highest level an entered Pokemon may have.
    s16 mPokemonLevelSumMax; // 0x04: highest level sum a party may total.
    s16 m06;
    s32 mHeldItemMode; // HeldItemMode policy for entered Pokemon.
    u8 mAllowSameSpecies; // 0x0C: lets a party hold the same species.
    u8 mAllowSameItem; // 0x0D: lets a party hold the same held item.
    u8 mEnableSbToSleep; // 0x0E: menuCBRule_EnableSbToSleep reads it.
    u8 mEnableSbToIceed; // 0x0F: menuCBRule_EnableSbToIceed reads it.
    u8 mEnableSkillSwap; // 0x10: menuCBRule_EnableSkillSwap reads it.
    u8 mProhibitionSuicide; // 0x11: menuCBRule_ProhibitionSuicide reads it.
    u8 mEnableSkillMichidure; // 0x12: menuCBRule_EnableSkill_Michidure reads it.
    u8 mEnableSkillSonicBoom; // 0x13: menuCBRule_EnableSkill_SonicBoom reads it.
    s16 m14; // 0x14: battle time limit in minutes (readers multiply by 60).
    s16 m16; // 0x16: command time limit.
    u8 mAllowLegendPokemon; // 0x18: lets legendary Pokemon be entered.
    u8 mDeoxysCamouflage; // 0x19: menuCBRule_DEOKISISUCamouflage reads it.
    s16 mPokemonCount; // 0x1A: normal number of Pokemon to enter.
    s16 mPokemonCountMode; // 0x1C: zero uses mPokemonCount; one permits a single Pokemon.
    u16 mTitle[21]; // 0x1E: custom rule title.
    u32 m48[2];
    u16 mMsgId; // 0x50: message ID of the rule set; also marks initialized slots.
    /* 0x52: the restriction positions of the rule set, one entry per list
       position. The rule menus stamp the entries of the items a rule set
       regulates; the entry screens test the positions one by one. */
    u8 m52[0x3E];
};

extern "C" {
const FightRuleData *menuCBRule_GetDefaultRuleData(s32 index);
u32 menuCBRule_ParticipationPokemonMax(void);
FightRuleData *menuCBRule_ConstantRule2(s32 ruleId);
}

#endif

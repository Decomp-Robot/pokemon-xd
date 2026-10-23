#include <FightRuleData.h>
#include <menuCBBios.h>
#include <types.h>
#include <savedata.h>

/*
 * menuCBBiosRule.cpp
 *
 * The colosseum rule data of the link (GBA) battle bios: the six rule slots
 * the rule menus edit (menuCBRule_*), mirrored from the save table the rules
 * are persisted in.
 */

/* -------------------------------------------------------------------------
 * External functions
 * ---------------------------------------------------------------------- */

extern "C" {
FightRuleData *menuCB_OreRule(void);
}

/* -------------------------------------------------------------------------
 * Functions of this module
 * ---------------------------------------------------------------------- */

extern "C" u32 menuCBBios_GetPokemonNumLimit(void);
extern "C" s32 menuCBBios_GetRuleId(void);
extern "C" FightRuleData *menuCBBios_CurrentRule(void);
extern "C" FightRuleData *menuCBBios_GetRulesetData(s32 ruleId);
extern "C" void menuCBBios_InitDefaultRules(void);

// Pokemon entry limit of the current rule set.
extern "C" u32 menuCBBios_GetPokemonNumLimit(void)
{
    return menuCBRule_ParticipationPokemonMax();
}

extern "C" s32 menuCBBios_GetRuleId(void)
{
    return menuCBTaisenWork.ruleId;
}

/*
 * Rule set in effect: the ore colosseum plays its fixed table, otherwise the
 * customizable rule sets 0-2 run on their working copy while the fixed rule
 * sets come straight out of the work-area table.
 */
extern "C" FightRuleData *menuCBBios_CurrentRule(void)
{
    FightRuleData *oreRule = menuCB_OreRule();
    FightRuleData *ruleData;
    s32 ruleId;

    if (oreRule != 0) {
        return oreRule;
    }

    ruleData = menuCBRule_ConstantRule2(menuCBBios_GetRuleId());
    ruleId = menuCBBios_GetRuleId();

    switch (ruleId) {
    case 0:
    case 1:
    case 2:
        if (ruleData != 0) {
            return ruleData;
        }
        break;

    case 3:
    case 4:
    case 5:
    default:
        break;
    }

    return menuCBBios_GetRulesetData(menuCBBios_GetRuleId());
}

extern "C" FightRuleData *menuCBBios_GetRulesetData(s32 ruleId)
{
    if (ruleId < 0 || (u32)ruleId >= 6) {
        return 0;
    }
    return &menuCBTaisenWork.mRulesetData[ruleId];
}

// Copy the three rodata default rule sets into the save table and stamp
// their message IDs, so the colosseum starts from the shipped rules.
extern "C" void menuCBBios_InitDefaultRules(void)
{
    FightRuleData *ruleData =
        (FightRuleData *)savedataGetStatus(0, TRAINER_DATA_FIGHT_RULES);

    ruleData[0] = *menuCBRule_GetDefaultRuleData(0);
    ruleData[1] = *menuCBRule_GetDefaultRuleData(1);
    ruleData[2] = *menuCBRule_GetDefaultRuleData(2);
    ruleData[0].mMsgId = 0x9124;
    ruleData[1].mMsgId = 0xA2D4;
    ruleData[2].mMsgId = 0xB51C;
}

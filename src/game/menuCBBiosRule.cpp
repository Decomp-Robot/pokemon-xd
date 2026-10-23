#include <FightRuleData.h>
#include <menuCBBios.h>
#include <types.h>

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
void *getTrainerDataStartOffset(s32 slot, s32 id);

// Rule menu module: the read-only default rule sets, the entry pokemon
// limit of the current rule set and the working copy of a customizable
// rule set.
FightRuleData *zz_004d314_(s32 index);
u32 zz_004d0b4_(void);
FightRuleData *zz_004d0d4_(s32 ruleId);
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

// Entry pokemon number limit of the current rule set, forwarded from the
// rule module's accessor (zz_004d0b4_, in the still-unrecovered unit at
// 0x8004D0B4): the entry screens cap their pokemon count with
// min(toolentryTaisenGetPokemonNum(), this).
extern "C" u32 menuCBBios_GetPokemonNumLimit(void)
{
    return zz_004d0b4_();
}

extern "C" s32 menuCBBios_GetRuleId(void)
{
    return lbl_804299F8.ruleId;
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

    ruleData = zz_004d0d4_(menuCBBios_GetRuleId());
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
    return &lbl_804299F8.mRulesetData[ruleId];
}

// Copy the three rodata default rule sets into the save table and stamp
// their message IDs, so the colosseum starts from the shipped rules.
extern "C" void menuCBBios_InitDefaultRules(void)
{
    FightRuleData *ruleData =
        (FightRuleData *)getTrainerDataStartOffset(0, 0xE);

    ruleData[0] = *zz_004d314_(0);
    ruleData[1] = *zz_004d314_(1);
    ruleData[2] = *zz_004d314_(2);
    ruleData[0].mMsgId = 0x9124;
    ruleData[1].mMsgId = 0xA2D4;
    ruleData[2].mMsgId = 0xB51C;
}

#ifndef FIGHT_RULE_DATA_H
#define FIGHT_RULE_DATA_H

#include <types.h>

typedef struct FightRuleData FightRuleData;

// FightRuleData -- one battle regulation record: the entry fees, time limits
// and restriction lists a link battle enforces, plus the message shown for
// it. The colosseum keeps a table of six of these in the save
// (getTrainerDataStartOffset(0, 0xE)), the MenuCBTaisenWork work area
// mirrors it, and the read-only defaults live in rodata. One record is 0x90
// bytes; the m1E/m52 entry lists sit at their recorded (even) offsets.
struct FightRuleData {
    s16 m00;
    s16 m02;
    s16 m04;
    s16 m06;
    u32 m08;
    u8 m0C; // 0x0C: unknown.
    u8 m0D; // 0x0D: unknown.
    u8 mEnableSbToSleep; // 0x0E: menuCBRule_EnableSbToSleep reads it.
    u8 mEnableSbToIceed; // 0x0F: menuCBRule_EnableSbToIceed reads it.
    u8 mEnableSkillSwap; // 0x10: menuCBRule_EnableSkillSwap reads it.
    u8 mProhibitionSuicide; // 0x11: menuCBRule_ProhibitionSuicide reads it.
    u8 mEnableSkillMichidure; // 0x12: menuCBRule_EnableSkill_Michidure reads it.
    u8 mEnableSkillSonicBoom; // 0x13: menuCBRule_EnableSkill_SonicBoom reads it.
    s16 m14; // 0x14: battle time limit in minutes (readers multiply by 60).
    s16 m16; // 0x16: command time limit.
    u8 m18; // 0x18: unknown.
    u8 mDeoxysCamouflage; // 0x19: menuCBRule_DEOKISISUCamouflage reads it.
    s16 m1A; // 0x1A: regulation kind the rule set answers to.
    s16 m1C; // 0x1C: entry pokemon limit while the rule set restricts.
    u16 m1E[20]; // 0x1E: rule entry list (item/entry IDs), read as u16.
    u16 m46;
    u32 m48[2];
    u16 mMsgId; // 0x50: message ID of the rule set; also marks initialized slots.
    /* 0x52: the restriction positions of the rule set, one entry per list
       position. The rule menus stamp the entries of the items a rule set
       regulates; the entry screens test the positions one by one. */
    u8 m52[0x3E];
};

#endif

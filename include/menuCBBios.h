#ifndef MENUCBBIOS_H
#define MENUCBBIOS_H

#include <FightRuleData.h>
#include <fightTrainer.h>
#include <types.h>

/*
 * Work area of the link (GBA) battles. The connect-battle menus (menuCB*)
 * keep their state here and address it directly; every member below sits at
 * the offset the corresponding menu reads or writes.
 */
typedef struct MenuCBTaisenWork {
    /* 0x0000 */ u8 _00[0x08];
    /* 0x0008 */ s32 battleMode; // battle mode the connect-battle menus run in.
    /* 0x000C */ s32 ruleId; // rule set the colosseum screens act on.
    /* 0x0010 */ u8 _10[0x18 - 0x10];
    /* 0x0018 */ FIGHT_TRAINER mTrainer[4]; // trainer record of each controller port.
    /* 0x4C98 */ FIGHT_TRAINER mTrainerImmediately[4]; // "fight immediately" records.
    /* 0x9918 */ FightRuleData mRulesetData[6]; // colosseum rule table, mirror of the save table.
    /* 0x9C78 */ u8 _9C78[0xAFE0 - 0x9C78];
    /* 0xAFE0 */ s32 selectResult; // result of the battle-result selection dialogue.
    /* 0xAFE4 */ u8 _AFE4[0xB060 - 0xAFE4];
    /* 0xB060 */ s32 unk_B060; // unknown; cleared alongside selectResult.
    /* 0xB064 */ u8 _B064[0xB0E0 - 0xB064];
    /* 0xB0E0 */ s32 nameEntryMode; // input path the name entry windows pick.
} MenuCBTaisenWork;

extern MenuCBTaisenWork lbl_804299F8;

// GBA link-encounter flag.
extern u32 staEncountFlag;

// Controller IDs of the four ports, in port order.
extern const s32 staControlerMap[4];

#endif

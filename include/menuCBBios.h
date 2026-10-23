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
    /* 0x0000 */ u8 _00[0x04];
    /* 0x0004 */ s32 connectMode; // GBA connect-battle flow variant; menuCBBios_InitContext
                                 // opens menu 0xA4 for 1 and 0xA9 otherwise.
    /* 0x0008 */ s32 battleMode; // battle mode the connect-battle menus run in.
    /* 0x000C */ s32 ruleId; // rule set the colosseum screens act on.
    /* 0x0010 */ s32 unk_010; // 0x10: written by the context setup only.
    /* 0x0014 */ s32 mStartMenuType; // 0x14: menu the battle starts in (0 or 4).
    /* 0x0018 */ FIGHT_TRAINER mTrainer[4]; // trainer record of each controller port.
    /* 0x4C98 */ FIGHT_TRAINER mTrainerImmediately[4]; // "fight immediately" records.
    /* 0x9918 */ FightRuleData mRulesetData[6]; // colosseum rule table, mirror of the save table.
    /* 0x9C78 */ u8 _9C78[0xAFE0 - 0x9C78];
    /* 0xAFE0 */ s32 selectResult; // result of the battle-result selection dialogue.
    /* 0xAFE4 */ u8 _AFE4[0xB060 - 0xAFE4];
    /* 0xB060 */ s32 unk_B060; // unknown; cleared alongside selectResult.
    /* 0xB064 */ u8 _B064[0xB0E0 - 0xB064];
    /* 0xB0E0 */ s32 nameEntryMode; // input path the name entry windows pick.
    /* 0xB0E4 */ u8 _B0E4[0xB104 - 0xB0E4];
    /* 0xB104 */ s32 mAbortRequest; // 0xB104: abort request of the entry screen.
    /* 0xB108 */ s32 mEntryState; // 0xB108: state of the entry screen.
    /* 0xB10C */ u8 _B10C[0xB110 - 0xB10C];
    /* 0xB110 */ s32 unk_B110;
    /* 0xB114 */ u8 _B114[0xB1A0 - 0xB114];
    /* 0xB1A0 */ s32 mPlayerLeftEntry; // 0xB1A0: set once a player left the entry.
    /* 0xB1A4 */ u8 _B1A4[0xB458 - 0xB1A4];
    /* 0xB458 */ s16 mPartySlotValue[4][6]; // 0xB458: party slot values of the four ports.
    /* 0xB488 */ s8 mPartySlotFlag[4][6]; // 0xB488-0xB49F: party slot flags of the
                                  // four ports, one byte per slot.
    /* 0xB4A1 */ u8 _B4A1[0xB4B0 - 0xB4A1];
} MenuCBTaisenWork;

extern MenuCBTaisenWork menuCBTaisenWork;

// GBA link-encounter flag.
extern u32 staEncountFlag;

// Controller IDs of the four ports, in port order.
extern const s32 staControlerMap[4];

#ifdef __cplusplus
extern "C" {
#endif

// Whether the battle of the link encounter was entered.
s32 menuCBBios_GetEncountFlag(void);

// Result the fight layer reported for the battle that is running
// (battleSetResult); 1 ends the link exchange.
u16 battleGetResult(void);

#ifdef __cplusplus
}
#endif

#endif

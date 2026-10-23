#ifndef MENUCBBIOS_H
#define MENUCBBIOS_H

#include <FightRuleData.h>
#include <Pokemon.h>
#include <fightTrainer.h>
#include <hero.h>
#include <types.h>

/*
 * Work area of the link (GBA) battles. The connect-battle menus (menuCB*)
 * keep their state here and address it directly; every member below sits at
 * the offset the corresponding menu reads or writes.
 */
/* Entry failures, in message priority order. Only the low two bytes carry
   flags. Egg and invalid entries are excluded from the level checks; an
   invalid held item takes precedence over a rule-restricted held item. */
typedef struct MenuCBEntrySlotFlags {
    u8 legendPokemon : 1;
    u8 duplicateSpecies : 1;
    u8 levelBelowMinimum : 1;
    u8 levelAboveMaximum : 1;
    u8 shadowPokemon : 1;
    u8 eggOrInvalidPokemon : 1;
    u8 invalidMove : 1;
    u8 illegalPokemon : 1;
    u8 heldItemForbidden : 1;
    u8 heldItemRestricted : 1;
    u8 duplicateHeldItem : 1;
    u8 invalidHeldItem : 1;
    u8 partyLevelSumExceeded : 1;
    u8 insufficientPokemon : 1;
    u8 invalidPokemon : 1;
    u8 unusedFlag : 1;
    u16 pad;
} MenuCBEntrySlotFlags;

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
    /* 0x9C78 */ FightRuleData *mEntryRule; // regulation record the entry screens show.
    /* 0x9C7C */ u8 _9C7C[0x9C84 - 0x9C7C];
    /* 0x9C84: per entry flags of its six Pokemon slots. The record count is not
       pinned down by the recovered modules; every access goes through the
       entry number below. */
    /* 0x9C84 */ MenuCBEntrySlotFlags entryFlags[70][6];
    /* 0xA314 */ u8 _A314[0xA65C - 0xA314];
    /* 0xA65C */ Hero mEntryHero; // hero copy the entry screens work on.
    /* 0xAFD4 */ u8 _AFD4[0xAFE0 - 0xAFD4];
    /* 0xAFE0 */ s32 selectResult; // result of the battle-result selection dialogue.
    /* 0xAFE4 */ s32 mEntryCursor; // entry row the entry screens' cursor is on.
    /* 0xAFE8 */ u8 _AFE8[0xAFF0 - 0xAFE8];
    /* 0xAFF0 */ s32 mSubmenuConfirm; // set while the entry submenu asks a yes/no question.
    /* 0xAFF4 */ u8 _AFF4[0xB004 - 0xAFF4];
    /* 0xB004 */ s32 mRuleNo; // rule record (mRulesetData) the entry screens work on.
    /* 0xB008 */ u8 _B008[0xB014 - 0xB008];
    /* 0xB014 */ s32 mEntrySelect; // entry the trainer rows are drawn for; -1 for the
                                  // rule row.
    /* 0xB018 */ u8 _B018[0xB028 - 0xB018];
    /* 0xB028 */ s32 listRow; // top row of the scrolled entry lists.
    /* 0xB02C */ u8 _B02C[0xB060 - 0xB02C];
    /* 0xB060 */ s32 unk_B060; // unknown; cleared alongside selectResult.
    /* 0xB064 */ u8 _B064[0xB0D8 - 0xB064];
    /* 0xB0D8 */ s32 entryNo; // entry the entry screens work on.
    /* 0xB0DC */ u8 _B0DC[0xB0E0 - 0xB0DC];
    /* 0xB0E0 */ s32 nameEntryMode; // input path the name entry windows pick.
    /* 0xB0E4 */ u8 _B0E4[0xB0E8 - 0xB0E4];
    /* 0xB0E8 */ s32 page2D; // 2D Pokemon page of battle mode 1.
    /* 0xB0EC */ s32 page2DSub; // 2D Pokemon page of battle mode 2.
    /* 0xB0F0 */ s32 battleMode2D; // battle mode the 2D Pokemon pages are laid out for.
    /* 0xB0F4 */ u8 _B0F4[0xB0F8 - 0xB0F4];
    /* 0xB0F8 */ s32 pokeSelect; // Pokemon list selection (item 0x1E..0x23).
    /* 0xB0FC */ s32 dispState; // display state handed to the menus.
    /* 0xB100 */ s32 selectCount; // number of flagged entry slots.
    /* 0xB104 */ s32 mAbortRequest; // abort request of the entry screen.
    /* 0xB108 */ s32 mEntryState; // state of the entry screen.
    /* 0xB10C */ u32 mRuleSelect; // regulation chosen in the rule submenu: 0 for its
                                  // first item, 1 for its last one.
    /* 0xB110 */ s32 unk_B110;
    /* 0xB114 */ u8 _B114[0xB118 - 0xB114];
    /* 0xB118 */ s32 mRuleCursor; // row the rule and port windows' cursor is on.
    /* 0xB11C */ u32 selectedValue; // entry menuCBSpriteColorIfMatch compares.
    /* 0xB120 */ u8 _B120[0xB124 - 0xB120];
    /* 0xB124 */ s32 mRuleCursorPrev; // row that cursor just left; -1 while no row
                                  // transition is heldItemRestricted.
    /* 0xB128 */ u32 mRuleCursorPrevColor; // colour that row is drawn in.
    /* 0xB12C */ u32 mRuleCursorColor; // colour the cursor row is drawn in.
    /* 0xB130 */ u8 _B130[0xB138 - 0xB130];
    /* 0xB138 */ u32 savedColor; // window colour menuCBWindowColorSave stored.
    /* 0xB13C */ u8 _B13C[0xB144 - 0xB13C];
    /* 0xB144 */ s32 mEntryCursorPrev; // entry row that cursor just left; -1 while no
                                  // row transition is heldItemRestricted.
    /* 0xB148 */ u32 mEntryCursorPrevColor; // colour that row is drawn in.
    /* 0xB14C */ u32 mEntryCursorColor; // colour the cursor row is drawn in.
    /* 0xB150 */ u8 _B150[0xB1A0 - 0xB150];
    /* 0xB1A0 */ s32 mPlayerLeftEntry; // set once a player left the entry.
    /* 0xB1A4 */ u8 _B1A4[0xB1D4 - 0xB1A4];
    /* 0xB1D4 */ s32 dstBank; // bank/index a party copy is taken from.
    /* 0xB1D8 */ s32 dstIndex;
    /* 0xB1DC */ u32 pokeMsgID; // message menuCBPokemonMsgCheck selected.
    /* 0xB1E0 */ u32 mSlotNo; // 1-based number of the entry slot a message reports.
    /* 0xB1E4 */ u8 _B1E4[0xB428 - 0xB1E4];
    /* 0xB428 */ s16 mPartySlotX[4][6]; // x the slot's face sprite was placed at, one
                                  // per port and slot.

    /* 0xB458 */ s16 mPartySlotValue[4][6]; // party slot values of the four ports.
    /* 0xB488 */ s8 mPartySlotFlag[4][6]; // party slot flags of the four ports,
                                  // one byte per slot.
    /* 0xB4A0 */ u8 pageWrap; // set while the 2D list wraps around.
    /* 0xB4A1 */ u8 _B4A1[0xB524 - 0xB4A1];
    /* 0xB524 */ Pokemon *reliveDancers[8][4]; // Relive hall stage dancers.
    /* 0xB5A4 */ u8 _B5A4[0xB5F8 - 0xB5A4];
} MenuCBTaisenWork;

extern MenuCBTaisenWork menuCBTaisenWork;

// GBA link-encounter flag.
extern u32 staEncountFlag;

// Controller IDs of the four ports, in port order.
extern const s32 staControlerMap[4];

#ifdef __cplusplus
extern "C" {
#endif

u16 menuCBTrainerBios_GetPlayerID(FIGHT_TRAINER *trainer);
s32 menuCBTrainerBios_GetControlerID(FIGHT_TRAINER *trainer);
void menuCBTrainerBios_SetControlerID(FIGHT_TRAINER *trainer, s32 controllerId);

// Whether the battle of the link encounter was entered.
s32 menuCBBios_GetEncountFlag(void);

// The controller type gbaCommandKeyRead cached for the SI port, which the
// exchanges of the link library test against SI_GBA.
u32 menuCBBios_PortIDtoControlerID(long portID);

// Result the fight layer reported for the battle that is running
// (battleSetResult); 1 ends the link exchange.
u16 battleGetResult(void);
u16 toolentryTaisenGetPokemonHomePlace(s32 port);

#ifdef __cplusplus
}
#endif

#endif

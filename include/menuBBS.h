#ifndef MENUBBS_H
#define MENUBBS_H

#include <FightRuleData.h>
#include <fightTrainer.h>
#include <types.h>
#include <menuSpriteBios.h>
#include <menuCBCommon.h>
#include <winSprite.h>

/* -------------------------------------------------------------------------
 * The rule set the connect-battle menus play with
 * ---------------------------------------------------------------------- */

/* The six colosseum rule sets a battle can be booked under. The table is
   kept in the save (savedataGetStatus(0, 0xE)) and mirrored into
   the work area; mDirty marks the copy that still has to be written back. */
typedef struct MenuCBRuleSet {
    /* 0x000 */ FightRuleData mRule[6];
    /* 0x360 */ u8 mDirty;
    /* 0x361 */ u8 _361[0x364 - 0x361];
} MenuCBRuleSet;

/* -------------------------------------------------------------------------
 * Work areas
 * ---------------------------------------------------------------------- */

/* The menu stack of the connect-battle menus. slot 0 holds the menu the
   battle was entered from, every menuCB_MenuPush adds one on top; the
   windowID is the id menuCB_MenuPop closes again. */
typedef struct MenuCBStackEntry {
    s32 menuID;
    s32 windowID;
} MenuCBStackEntry;

typedef struct MenuCBStack {
    MenuCBStackEntry mStack[8];
    s32 mCount;
} MenuCBStack;

#include <menuCBPokemonEntry.h>

#endif

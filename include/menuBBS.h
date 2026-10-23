#ifndef MENUBBS_H
#define MENUBBS_H

#include <FightRuleData.h>
#include <fightTrainer.h>
#include <types.h>

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

/* Per-player work of the Pokemon entry screen: mPos holds the on-screen
   position of every party slot (six x values followed by six y values),
   the remaining members are the state the GBA exchange is driven with. */
typedef struct MenuCBPokemonEntryPlayer {
    /* 0x00 */ f32 mPos[12];
} MenuCBPokemonEntryPlayer;

/* Face record of one party slot: mLoaded marks a slot the face texture was
   built for, mFaceID is the face the Game Boy reported for it. One record is
   0x0C bytes; the table starts at work offset 0x30. */
typedef struct MenuCBPokemonEntryFaceWork {
    /* 0x00 */ u8 mLoaded;
    /* 0x01 */ u8 _01;
    /* 0x02 */ u16 mFaceID;
    /* 0x04 */ s32 mX;
    /* 0x08 */ u8 _08[4];
} MenuCBPokemonEntryFaceWork;

typedef struct MenuCBPokemonEntryWork {
    /* 0x000 */ s32 mState;
    /* 0x004 */ bool mEntryFlag[4]; // per SI port: the player handed its party over.
    /* 0x008 */ s8 mVersusId[4]; // versus battle the port plays, -1 if none
    /* 0x00C */ s32 mTimer;
    /* 0x010 */ u8 _010[0x02C - 0x010];
    /* 0x02C */ s32 mCursor; // entry cursor; the face texture work counts the
                              // slots it has loaded in it.
    /* 0x030 */ MenuCBPokemonEntryFaceWork mFaceWork[4][6];
    /* 0x150 */ s32 mPlayerNum; // players the entry screen is laid out for.
    /* 0x154 */ s32 mScroll;
    /* 0x158 */ u8 mReadFlag; // set once the face textures are all loaded.
    /* 0x159 */ u8 _159[0x160 - 0x159];
    /* 0x160 */ MenuCBPokemonEntryPlayer mPlayer[4];
    /* 0x220 */ f32 mBaseX;
    /* 0x224 */ s32 mBaseIndex;
    /* 0x228 */ u8 _228[0x22C - 0x228];
    /* 0x22C */ u8 mDisconnected; // set while a player waits for its GBA
    /* 0x22D */ u8 _22D[0x230 - 0x22D];
    /* 0x230 */ s32 mDisconnectedPort;
} MenuCBPokemonEntryWork;

/* Trainer name plate the connect-battle screens show, filled in from the
   language entry table. */
typedef struct MenuCBTrainerName {
    u8 _00[0x54];
    s16 mFamilyName;
    s16 mGivenName;
    s32 mMoney;
    s16 mMoneyDigits;
    s16 mPlayTime;
} MenuCBTrainerName;

/* One SI port as the input layer hands it to the menu callbacks.  The
   menus only ever move the value around, so it stays a scalar. */
enum GSinputDevice {
    /* 0 */ INPUT_DEVICE_0,
    /* 1 */ INPUT_DEVICE_1,
    /* 2 */ INPUT_DEVICE_2,
    /* 3 */ INPUT_DEVICE_3,
};

/* One entry of the per-language trainer name table. */
typedef struct MenuCBLanguageEntry {
    /* 0x00 */ s16 mPlayTime;
    /* 0x02 */ s16 mMoneyDigits;
    /* 0x04 */ s16 mFamilyName;
    /* 0x06 */ s16 mGivenName;
    /* 0x08 */ s32 mMoney;
} MenuCBLanguageEntry;

#endif

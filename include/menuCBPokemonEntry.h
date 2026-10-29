#ifndef MENU_CB_POKEMON_ENTRY_H
#define MENU_CB_POKEMON_ENTRY_H

#include <GSinput.h>
#include <Pokemon.h>
#include <types.h>

/* Per-player work of the Pokemon entry screen: mPos holds the on-screen
   position of every party slot (six x values followed by six y values),
   the remaining members are the state the GBA exchange is driven with. */
typedef struct MenuCBPokemonEntryPlayer {
    /* 0x00 */ f32 mPos[12];
} MenuCBPokemonEntryPlayer;

/* Face record of one party slot: mLoaded marks a slot the face texture was
   built for, mFaceID is the face the Game Boy reported for it. */
typedef struct MenuCBPokemonEntryFaceWork {
    /* 0x00 */ u8 mLoaded;
    /* 0x01 */ u8 _01;
    /* 0x02 */ u16 mFaceID;
    /* 0x04 */ s32 mUnknown04;
    /* 0x08 */ u8 _08[4];
} MenuCBPokemonEntryFaceWork;

typedef struct MenuCBPokemonEntryWork {
    /* 0x000 */ s32 mState;
    /* 0x004 */ bool mEntryFlag[4]; // per SI port: the player handed its party over
    /* 0x008 */ s8 mVersusId[4]; // versus battle the port plays, -1 if none
    /* 0x00C */ s32 mTimer;
    /* 0x010 */ u8 _010[0x02C - 0x010];
    /* 0x02C */ s32 mCursor; // entry cursor; the face texture work counts the slots it has loaded here
    /* 0x030 */ MenuCBPokemonEntryFaceWork mFaceWork[4][6];
    /* 0x150 */ s32 mPlayerNum; // players the entry screen is laid out for
    /* 0x154 */ s32 mScroll;
    /* 0x158 */ u8 mReadFlag; // set once the face textures are all loaded
    /* 0x159 */ u8 _159[0x160 - 0x159];
    /* 0x160 */ MenuCBPokemonEntryPlayer mPlayer[4];
    /* 0x220 */ f32 mBaseX;
    /* 0x224 */ s32 mBaseIndex;
    /* 0x228 */ u8 _228[0x22C - 0x228];
    /* 0x22C */ u8 mConnectionOK; // cleared when entry is interrupted
    /* 0x22D */ u8 _22D[0x230 - 0x22D];
    /* 0x230 */ s32 mDisconnectedPort;
} MenuCBPokemonEntryWork;

extern MenuCBPokemonEntryWork _menuCBPokemonWork;

#ifdef __cplusplus
extern "C" {
#endif

s32 menuCB_PokemonEntry(void);
void menuCBPokemonEntryButtonTask_20041006(void);
u16 menuCBPokemonEntryPokemonNumMin(long port);
void menuCBPokemonEntryPokemonFaceFree(void);

#ifdef __cplusplus
}

void _menuCB_DeoxysParamCustom(Pokemon *pokemon, u16 port);
u8 _menuCB_pokemonGetDeoxysSel(u16 port, bool camouflage);
void _menuCBPokemonEntryEntCheckGBA(GSinputDevice device, long port);
void _menuCBPokemonEntryEntCheckDebug(long port);
void _menuCBPokemonEntryEntCheck(GSinputDevice device, long port);
s32 _menuCBPokemonEntryButtonCheck(unsigned long buttons, long port);
void _menuCBPokemonEntryWork(void);
u16 _menuCBPokemonEntryPokemonNum(long port);
void _menuCBPokemonEntryWorkInit(void);
void _menuCBPokemonEntryDispPosition(void);
u8 _menuCBPokemonEntryAllPlayer(void);
void _menuCBPokemonEntrySetEntryFlag(long port, bool flag);
u8 _menuCBPokemonEntryGetEntryFlag(long port);
#endif

#endif

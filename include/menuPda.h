#ifndef MENUPDA_H
#define MENUPDA_H

#include "GSgfx.h"
#include "types.h"
#include "winSprite.h"

// Ids of the PDA top menu entries. The rows currently shown are listed in
// menuPdaResTable; the entries open:
enum PDATOP_MENU_RES {
    PDATOP_MENU_RES_DP_MON = 0, // dark pokemon list (menuPdaDPMonList).
    PDATOP_MENU_RES_MEMO = 1,   // memo list (menuPdaMemoList).
    PDATOP_MENU_RES_MAIL = 2,   // XD mail list (menuPdaMailListXD).
    PDATOP_MENU_RES_EXTRA = 3,  // submenu of zz_0042c2c_ (menus 0x77/0x6D).
    PDATOP_MENU_RES_EXIT = 4,   // leave the PDA screen.
};

// Argument the PDA frame menu (0x6C) is opened with: three bytes the menu
// reads and the two s32 values it writes back.
struct PDA_FRAME_ARG {
    u8 mUnk00;   // 0x00: 0.
    u8 mUnk01;   // 0x01: 0xFF.
    u8 mUnk02;   // 0x02: 0.
    u8 _03;      // 0x03: padding, untouched.
    s32 *mUnk04; // 0x04: value the frame menu fills in.
    s32 *mUnk08; // 0x08: second value the frame menu fills in.
};

// Work area of the PDA menus: the selected top menu entry, the submenu state,
// the fade the PDA script animates with and the cursor bookkeeping.
struct MenuPdaWork {
    PDATOP_MENU_RES mRes;  // 0x00: top menu entry to open; script mode for the PDA script.
    s32 mSubmenuFlag;      // 0x04: set while a submenu is open.
    s32 mUnk08;            // 0x08: set with mRes by the PDA script.
    u8 _0C[0x34];          // 0x0C-0x3F: unknown here.
    u8 mFadeOutDone;       // 0x40: set once the fade-out wait is done.
    u8 mExitFlag;          // 0x41: set by the top menu to leave the PDA screen.
    u8 _42[0x1A];          // 0x42-0x5B: unknown here.
    f32 mFadeFrom;         // 0x5C: fade scale the PDA animates from.
    f32 mFadeTo;           // 0x60: fade scale the PDA animates towards.
    u8 _64[0x04];          // 0x64-0x67: unknown here.
    u8 mCursorMoved;       // 0x68: set while the cursor was moved by the pad.
    u8 _69[0x03];          // 0x69-0x6B: unknown here.
};

// Work area of the PDA menus, the item ids of its top menu rows and the fade
// state the PDA script animates.
extern MenuPdaWork menuPdaWork;
extern s16 menuPdaResTable[];
extern u8 menuPdaMode;
extern u8 menuPdaPageDisp;
extern s16 menuPdaResCount;
extern void *menuPdaModel;
extern s32 menuPdaFrameValue0;
extern s32 menuPdaFrameValue1;
extern f32 menuPdaScriptFade;
extern u8 menuPdaSoundEnable;
extern const f32 menuPdaScriptFadeLimit;

// Frame labels of the PDA open flash and the memo page flash (sdata2).
extern const char menuPdaFlashMemoStart[8]; // "1_start"
extern const char menuPdaFlashMemoEnd[6]; // "1_end"
extern const char menuPdaFlashDPMonStart[8]; // "2_start"
extern const char menuPdaFlashDPMonEnd[6]; // "2_end"
extern const char menuPdaPageEndFormat[7]; // "%d_end"

// Fade constants of the PDA script, shared with the PDA helpers.
extern const f32 menuPdaScriptFadeTime;
extern const f32 menuPdaZero;
extern const f32 menuPdaAlphaFull;
extern const char menuPdaPageStartFormat[];
extern GSgfxLayerID menuPdaEventLayer[2];

#endif

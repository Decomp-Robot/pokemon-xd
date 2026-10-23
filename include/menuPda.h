#ifndef MENUPDA_H
#define MENUPDA_H

#include "GSgfx.h"
#include "types.h"
#include "winSprite.h"

enum {
    MENU_ID_PDA_TOP = 0x6B,
    MENU_ID_PDA_FRAME = 0x6C,
    MENU_ID_PDA_BACKDROP = 0xA8,
    MENU_ID_PDA_SCRIPT_INPUT = 0x107,
};

// Ids of the PDA top menu entries. The rows currently shown are listed in
// _cursor_index; the entries open:
enum PDATOP_MENU_RES {
    PDATOP_MENU_RES_DP_MON = 0, // dark pokemon list (menuPdaDPMonList).
    PDATOP_MENU_RES_MEMO = 1,   // memo list (menuPdaMemoList).
    PDATOP_MENU_RES_MAIL = 2,   // XD mail list (menuPdaMailListXD).
    PDATOP_MENU_RES_SEARCHER = 3,  // submenu of menuPdaSearcher (menus 0x77/0x6D).
    PDATOP_MENU_RES_EXIT = 4,   // leave the PDA screen.
};

// Argument the PDA frame menu (0x6C) is opened with: three bytes the menu
// reads and the two s32 values it writes back.
struct PDA_FRAME_ARG {
    u8 mUnk00;   // 0x00: 0.
    u8 mUnk01;   // 0x01: 0xFF.
    u8 mUnk02;   // 0x02: 0.
    u8 _03;      // 0x03: padding, untouched.
    s32 *mTitleMessageID; // 0x04: title message id.
    s32 *mPreviousRes; // 0x08: second value the frame menu fills in.
};

// Work area of the PDA menus: the selected top menu entry, the submenu state,
// the fade the PDA script animates with and the cursor bookkeeping.
struct MenuPdaWork {
    void ResetTransform(f32 scale = 1.0f)
    {
        mPosition.x = mPosition.y = mPosition.z = 0.0f;
        mScaleX = mScaleY = scale;
    }


    PDATOP_MENU_RES mRes;  // 0x00: top menu entry to open; script mode for the PDA script.
    s32 mSubmenuFlag;      // 0x04: set while a submenu is open.
    s32 mPreviousRes;      // 0x08: previous top-menu entry.
    u8 _0C[8];
    GSvec mPosition;      // 0x14
    f32 mScaleX;          // 0x20
    f32 mScaleY;          // 0x24
    GSvec2 mMenuPosition; // 0x28
    GSvec mExitPosition;  // 0x30
    s32 mUnk3C;           // 0x3C
    u8 mFadeOutDone;       // 0x40: set once the fade-out wait is done.
    u8 mExitFlag;          // 0x41: set by the top menu to leave the PDA screen.
    u8 _42[2];
    f32 mOuterStartAngle; // 0x44
    f32 mOuterEndAngle;   // 0x48
    f32 mInnerStartAngle; // 0x4C
    f32 mInnerEndAngle;   // 0x50
    f32 mStartAngleDelta; // 0x54
    f32 mEndAngleDelta;   // 0x58
    f32 mCursorY;          // 0x5C: current cursor row height.
    f32 mCursorTargetY;    // 0x60: selected row height.
    f32 mCursorDistance;   // 0x64
    u8 mCursorMoved;       // 0x68: set while the cursor was moved by the pad.
    u8 _69[0x03];          // 0x69-0x6B: unknown here.
};

// Work area of the PDA menus, the item ids of its top menu rows and the fade
// state the PDA script animates.
extern MenuPdaWork _menuPdaStatus;
extern u8 menuPdaMode;
extern bool menuPdaPageDisp;
extern s16 _cursor_index_max;
extern void *_pModel;
extern s32 TitleMsgID;
extern s32 LineAnimTime;
extern f32 _pdaScriptAddFrame;
extern u8 menuPdaSoundEnable;
extern f32 _pdaScriptTime;
extern GSvec2 menuPdaCursorCenterOuter;
extern f32 _r1;
extern GSvec2 menuPdaCursorCenterInner;
extern f32 _r2;
extern f32 _next_height;
extern u8 menuPdaCursorInit;
extern void *_menuPdaFlashBuffer;
extern void *_menuPdaTextureBuffer;

extern GSgfxLayerID menuPdaEventLayer[2];

bool _checkAddFunc(tagWINDOW_WORK *window);
void _SelectCursorCalcAngle(s8 select);

u8 _menuPdaScriptGetAlpha(void);
void _menuPdaScriptCalcAlpha(f32 dt);
void _menuPdaScriptCalcAlphaInit(void);
void _menuPdaSetMenuItemScript(u8 mode);
void _menuPdaSelectAnimeSet(long page);

extern "C" {
void menuPdaDrawDirectModel(void);
void menuPdaReleaseModelPointer(void);
void menuPdaSetModelPointer(void);
s32 menuPdaGetRes(void);
s32 menuPdaGetSubres(void);
}

#endif

#ifndef MENU_FIGHT_H
#define MENU_FIGHT_H

#include <types.h>

// One slot of the target selection window, shared by the window's draw and
// button siblings; the four rows are the up/left/down/right panels. mNickname
// is the nickname of the pokemon the panel stands for.
struct MENU_FIGHT_TARGET_SLOT {
    u32 _00;
    s32 mItemID;
    u16 *mNickname;
};

// Sprite items of the four panels of the target window, one per stick
// direction. The button hook moves the cursor onto the panel whose item the
// pressed direction selects.
enum {
    TARGET_PANEL_LEFT = 55,
    TARGET_PANEL_UP = 56,
    TARGET_PANEL_DOWN = 64,
    TARGET_PANEL_RIGHT = 65,
};

#ifdef __cplusplus
extern "C" {
#endif

// Reports whether the ball panel may be opened for the given status menu.
s32 menuFightChkOpenBall(s32 menuId);

struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

// Draw hooks of the fight menu's turn display and the two timer windows.
void menuFightDrawTurn(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);
void menuFightDrawTimer(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);
// Per-frame update of the timer windows: refreshes the displayed remaining
// times and plays the countdown sounds.
s32 menuFightCtrlTimer(struct tagWINDOW_WORK *window);
// Flags the fight menu accepted and cancelled once the command countdown
// timed out.
void menuFightButtonTimeOutCheck(struct tagWINDOW_WORK *window);
// Open/close/check hooks for the battle time display window.
u8 menuFightCloseCheckTotalTimer(void);
void menuFightCloseTotalTimer(void);
void menuFightOpenTotalTimer(void);
// Open/close/check hooks for the per-turn countdown window.
u8 menuFightCloseCheckCountDown(void);
void menuFightCloseCountDown(void);
void menuFightOpenCountDown(void);
// Secret target-selection subsystem of the fight menu: draws the target
// marker over the selected panel, handles target selection input, and draws
// the cursor over the currently selected target.
void menuFightDrawTargetSecret(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);
void menuFightButtonTargetSecret(struct tagWINDOW_WORK *window);
void menuFightDrawTargetCursor(struct tagWINDOW_WORK *window, struct tagSPRITE_WORK *sprite);

#ifdef __cplusplus
}

// Right-stick trigger of the fight menu's command window, read by the control
// hooks; returns the direction the stick is held in.
s32 _menuFightGetRightStickInfo(s32 stickNo, bool trigger);
s32 _menuFightGetLeftKeyInfo(s32 stickNo);
// Reports whether the fight menu's command countdown has run out.
bool _menuFightCheckTimeOut(void);
#endif

#endif

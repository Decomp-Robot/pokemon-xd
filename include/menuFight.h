#ifndef MENU_FIGHT_H
#define MENU_FIGHT_H

#include <types.h>

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

#ifdef __cplusplus
}

// Right-stick trigger of the fight menu's command window, read by the control
// hooks; returns the direction the stick is held in.
s32 _menuFightGetRightStickInfo(s32 stickNo, bool trigger);
// Reports whether the fight menu's command countdown has run out.
bool _menuFightCheckTimeOut(void);
#endif

#endif

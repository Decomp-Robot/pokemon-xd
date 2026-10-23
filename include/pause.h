#ifndef PAUSE_H
#define PAUSE_H

#include <types.h>

// Pause state (sbss 0x804EA6D0): nonzero while pauseOn holds the game paused.
// Read all over the game through pauseIsPause to skip updates while paused.
extern u8 pauseState;
// Pause ready flag (sbss 0x804EA6D1): set by pauseInit; pauseOn needs it.
extern u8 pauseInitialized;
// BGM id held across the pause (sbss 0x804EA6D4); 0xFFFFFFFF when none.
extern u32 pauseBgmID;
// Pause app id returned by GSgappCreate (sbss 0x804EA6D8).
extern s32 pauseAppID;

#ifdef __cplusplus
extern "C" {
#endif

// Pause flag query used all over the game to skip updates while paused.
u8 pauseIsPause(void);
void pauseOff(void);
void pauseOn(void);
void pauseInit(void);

#ifdef __cplusplus
}

void _pauseDaemon(unsigned long appID, unsigned long userData);
#endif

#endif

#ifndef APP_MENU_MENUNINTENDOWARNING_H
#define APP_MENU_MENUNINTENDOWARNING_H

#include <types.h>
#include <winSprite.h>

// Steps of the warning screen sequence.
enum WarningSeqState {
    WARNING_SEQ_OPENING = 0,     // window opening; wait until its movement finishes
    WARNING_SEQ_INPUT_DELAY = 1, // timed delay before input is accepted
    WARNING_SEQ_ACCEPTING = 2,   // waiting for the player to accept
    WARNING_SEQ_AUTO_ACCEPT = 3, // accept automatically and close
};

// Warning screen sequence state. _warningSeqTimer accumulates elapsed
// time from the start of the sequence and carries across step changes;
// it is reset when the sequence is (re)started, both by
// _NintendoWarningInit and by the warning controller when its demo
// state restarts. _warningSeq is the step being played, and
// _warningTimer tracks time within the current step.
extern f32 _warningSeqTimer;
extern s32 _warningSeq;
extern f32 _warningTimer;

extern "C" void menuNintendoWarning(void);
extern "C" void menuNintendoWarningButton(tagWINDOW_WORK *window);
void _menuNintendoWarningButtonNormal(tagWINDOW_WORK *window);
void _NintendoWarningInit(void);
void _NintendoWarningMain(void);
void _NintendoWarningExit(void);

#endif

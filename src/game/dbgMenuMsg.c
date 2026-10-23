#include <msgctrl.h>
#include <fade.h>
#include <types.h>
#include <menuSub.h>

/* Debug message test/serial-message check menus. */
#pragma force_active on
extern f32 dbgMessageFadeDuration;

void winMsgOpenError(s32 a, s32 b, s32 c);
void winMsgClose(s32 id);
void winMsgCloseError(s32 id);
void winMsgOpenField(s32 msg, s32 a, s32 b);
void winMsgWaitForConfirm(s32 id);
void winMsgCloseFight(s32 id);
void GSthreadSwitch(void);
u32 GSinputButtonsPressed(s32 pad);
void menuSubKeyWait(void);

#pragma peephole off


s32 dbgMenuMsgTest(s32 menuID, s32 state)
{
    switch (state) {
    case 0:
        fadeSet(3, dbgMessageFadeDuration);
        fadeCheck(1);
        winMsgOpenError(1, 1, 0);
        winMsgClose(1);
        fadeSet(2, dbgMessageFadeDuration);
        fadeCheck(1);
        break;
    case 1:
        winMsgWaitForConfirm(1);
        winMsgCloseError(1);
        break;
    case 2:
        GSthreadSwitch();
        GSthreadSwitch();
        GSthreadSwitch();
        winMsgWaitForConfirm(0);
        GSthreadSwitch();
        GSthreadSwitch();
        break;
    case 3:
        msgctrlSetValue(0x31, 0x7DA);
        break;
    }
    return 0;
}

s32 dbgMenuMsgCheck(void)
{
    s32 msg;

    if (GSinputButtonsPressed(1) & 0x20) {
        if (!menuSubOpenNumberInputDigit(0, &msg)) {
            return -1;
        }
        winMsgOpenField(msg, 1, 0);
        menuSubKeyWait();
        winMsgCloseFight(1);
        return 0;
    }
    return 1;
}

#pragma peephole reset


#pragma force_active reset

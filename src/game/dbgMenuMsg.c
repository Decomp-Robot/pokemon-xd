#include <winMsg.h>
#include <fade.h>
#include <msgctrl.h>
#include <types.h>
#include <GSinput.h>
#include <GSthread.h>
#include <menuSub.h>

/* Debug message test/serial-message check menus. */
#pragma force_active on

#pragma peephole off

enum {
    /* R trigger starts the serial-message check. */
    MSG_PAD_TRIGGER_R = 0x20
};

s32 dbgMenuMsgTest(s32 menuID, s32 state)
{
    switch (state) {
    case 0:
        fadeSet(FADE_OUT_BLACK, 0.5f);
        fadeCheck(1);
        winMsgOpenError(1, 1, 0);
        winMsgClose(1);
        fadeSet(FADE_IN_BLACK, 0.5f);
        fadeCheck(1);
        break;
    case 1:
        winMsgCloseFight(1);
        winMsgCloseError(1);
        break;
    case 2:
        GSthreadSwitch();
        GSthreadSwitch();
        GSthreadSwitch();
        winMsgCloseFight(0);
        GSthreadSwitch();
        GSthreadSwitch();
        break;
    case 3:
        msgctrlSetValue(MSGCTRL_VALUE_MESSAGE_ID, 2010);
        break;
    }
    return 0;
}

s32 dbgMenuMsgCheck(void)
{
    s32 msg;

    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & MSG_PAD_TRIGGER_R) {
        if (!menuSubOpenNumberInputDigit(0, &msg)) {
            return -1;
        }
        winMsgOpenField(msg, 1, 0);
        menuSubKeyWait();
        winMsgCloseField(1);
        return 0;
    }
    return 1;
}

#pragma peephole reset

#pragma force_active reset

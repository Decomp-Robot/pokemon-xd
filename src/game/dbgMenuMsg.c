#include <types.h>
#include <menuSub.h>

/* Debug message test/serial-message check menus. */
/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on
extern f32 lbl_804EBDE8;

void fadeSet(s32 kind, f32 length);
void fadeCheck(s32 id);
void winMsgOpenError(s32 a, s32 b, s32 c);
void winMsgClose(s32 id);
void winMsgCloseError(s32 id);
void winMsgOpenField(s32 msg, s32 a, s32 b);
void zz_message_wait_input_a(s32 id);
void zz_message_wait_input_b(s32 id);
void GSthreadSwitch(void);
void msgctrlSetValue(s32 index, s32 value);
u32 GSinputButtonsPressed(s32 pad);
void menuSubKeyWait(void);

#pragma peephole off


s32 dbgMenuMsgTest(s32 menuID, s32 state)
{
    switch (state) {
    case 0:
        fadeSet(3, lbl_804EBDE8);
        fadeCheck(1);
        winMsgOpenError(1, 1, 0);
        winMsgClose(1);
        fadeSet(2, lbl_804EBDE8);
        fadeCheck(1);
        break;
    case 1:
        zz_message_wait_input_a(1);
        winMsgCloseError(1);
        break;
    case 2:
        GSthreadSwitch();
        GSthreadSwitch();
        GSthreadSwitch();
        zz_message_wait_input_a(0);
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
        zz_message_wait_input_b(1);
        return 0;
    }
    return 1;
}

#pragma peephole reset


#pragma force_active reset

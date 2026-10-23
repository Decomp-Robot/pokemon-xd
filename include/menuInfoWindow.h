#ifndef MENU_INFO_WINDOW_H
#define MENU_INFO_WINDOW_H

#include <types.h>

// One message control substitution to apply before the window message is drawn.
struct InfoWindowMsgCtrl {
    u32 mId;    // message control id; 0 terminates the list
    u32 mValue; // value handed to the message control
};

class CInfoWindow {
public:
    // Appends (id, value) to the message control list, in the first free slot.
    void setMsgCtrlValue(unsigned long id, unsigned long value);

private:
    s32 mWindowID;                 // 0x00: id looked up with windowSearchID
    u32 mItemID;                   // 0x04: sprite work item id
    u8 mVisible;                   // 0x08: draw the window when set
    u8 mStatefulMsg;               // 0x09: use the scrolling GSmsgMain message
    u8 mPad0A[2];                  // 0x0A
    u32 mMessageID;                // 0x0C: message printed into the window
    InfoWindowMsgCtrl mMsgCtrls[4]; // 0x10: id/value pairs, terminated by a zero id
    u16 mTriggeredKey;             // 0x30: key trigger from windowGetKeyTrg
};

#endif

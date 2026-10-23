#ifndef MENU_INFO_WINDOW_H
#define MENU_INFO_WINDOW_H

#include <types.h>

// Message-control slot for the info window. Each slot pairs the control id
// (+0x00) with the value to install (+0x04); see draw, which walks the table
// while the id is nonzero and hands each pair to msgctrlSetValue.
struct InfoWindowMsgCtrl {
    u32 mId;
    u32 mValue;
};

// Partial CInfoWindow layout: only the leading words and the control table
// at +0x10 are observed by this unit (init stores the window id at +0x0 and
// the message id at +0x4; open/close use the flags at +0x8/+0x9 and the
// message handle at +0xC; exec/draw use the key state at +0x30). The
// remainder of the object is unrecovered here.
class CInfoWindow {
public:
    void setMsgCtrlValue(unsigned long id, unsigned long value);

private:
    u8 _00[0x10]; // 0x00-0x0F: ids, flags and message handle (unknown here).
    InfoWindowMsgCtrl mMsgCtrls[4];
};

#endif

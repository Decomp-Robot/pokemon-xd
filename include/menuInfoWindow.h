#ifndef MENU_INFO_WINDOW_H
#define MENU_INFO_WINDOW_H

#include <types.h>

struct InfoWindowMsgCtrl {
    u32 mId;
    u32 mValue;
};

class CInfoWindow {
public:
    void setMsgCtrlValue(unsigned long id, unsigned long value);

private:
    u8 _00[0x10];
    InfoWindowMsgCtrl mMsgCtrls[4];
};

#endif

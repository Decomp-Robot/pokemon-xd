#include <types.h>

// Message-control slots for the info window (see draw, which walks the
// table while the id is nonzero and hands each value to msgctrlSetValue).
// Each slot pairs the control id (+0) with the value to install (+4).
struct InfoWindowMsgCtrl {
    u32 mId;
    u32 mValue;
};

// Partial CInfoWindow layout: only the leading words and the control table
// at +0x10 are observed by this unit (init stores the window id at +0x0
// and the message id at +0x4; open/close use the flags at +0x8/+0x9 and
// the message handle at +0xC; exec/draw use the key state at +0x30).
// The remainder of the object is unrecovered here.
class CInfoWindow {
public:
    void setMsgCtrlValue(unsigned long id, unsigned long value);

private:
    u8 _00[0x10];
    InfoWindowMsgCtrl mMsgCtrls[4];
};

// CInfoWindow::setMsgCtrlValue -- install a message-control value. The
// table is scanned for the first slot whose id is still zero and the new
// id/value pair is written there. Groups with the other info-window
// helpers sharing the control table (draw, open, close, init, isMsgEnd,
// exec), which will join this module as they are recovered.
//
// Note: this translation unit builds with the peephole optimizer disabled
// (menuInfoWindow, GC/2.6 -opt nopeephole). With peephole enabled the
// prologue copy of `this` is scheduled after the slot-index init; with
// nopeephole the compiler keeps the original mr/li order exactly.
void CInfoWindow::setMsgCtrlValue(unsigned long id, unsigned long value)
{
    unsigned long index = 0;

    while (mMsgCtrls[index].mId != 0) {
        index++;
    }
    mMsgCtrls[index].mId = id;
    mMsgCtrls[index].mValue = value;
}

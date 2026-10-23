#include <menuInfoWindow.h>

// CInfoWindow::setMsgCtrlValue -- queue a message-control value for the next
// message opened from this window. The slot table is scanned for the first
// entry whose id is still zero and the new id/value pair is written there;
// draw later walks the table and hands each pair to msgctrlSetValue. Groups
// with the other info-window helpers sharing the control table (draw, open,
// close, init, isMsgEnd, exec), which will join this module as they are
// recovered.
void CInfoWindow::setMsgCtrlValue(unsigned long id, unsigned long value)
{
    unsigned long index = 0;

    while (mMsgCtrls[index].mId != 0) {
        index++;
    }
    mMsgCtrls[index].mId = id;
    mMsgCtrls[index].mValue = value;
}

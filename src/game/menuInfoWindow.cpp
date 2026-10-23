#include <menuInfoWindow.h>

// Store a message control value in the first free slot.
#pragma peephole off
void CInfoWindow::setMsgCtrlValue(unsigned long id, unsigned long value)
{
    unsigned long index = 0;

    while (mMsgCtrls[index].mId != 0) {
        index++;
    }
    mMsgCtrls[index].mId = id;
    mMsgCtrls[index].mValue = value;
}
#pragma peephole reset

#ifndef MENU_INFO_WINDOW_H
#define MENU_INFO_WINDOW_H

#include <types.h>
class CMetaMenu;

struct InfoWindowMsgCtrl {
    u32 mId;
    u32 mValue;
};

class CInfoWindow {
public:
    void init(long menuID, unsigned long itemID);
    void initTutorialTalk();
    void open(unsigned long messageID, bool animated);
    void close();
    bool isMsgEnd();
    void exec(CMetaMenu &menu, unsigned long messageID, bool closeOnEnd);
    void draw();
    void setMsgCtrlValue(unsigned long id, unsigned long value);

private:
    long mMenuID;
    unsigned long mItemID;
    bool mOpen;
    bool mAnimated;
    unsigned long mMessageID;
    InfoWindowMsgCtrl mMsgCtrls[4];
    unsigned short mKeyTrigger;
};

#endif

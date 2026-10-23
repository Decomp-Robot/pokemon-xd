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
    void exec(CMetaMenu& menu, unsigned long messageID, bool closeOnEnd);
    void draw();
    void setMsgCtrlValue(unsigned long id, unsigned long value);
    bool isMsgEnd();
    void open(unsigned long messageID, bool animate);
    void close();
    void initTutorialTalk();
    void init(long windowID, unsigned long itemID);

private:
    long mWindowID;
    unsigned long mItemID;
    bool mDisplay;
    bool mAnimateMessage;
    unsigned long mMessageID;
    InfoWindowMsgCtrl mMsgCtrls[4];
    u16 mKeyTrigger;
};

#endif

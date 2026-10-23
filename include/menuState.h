#ifndef MENU_STATE_H
#define MENU_STATE_H
#include <types.h>
class CState {
public:
    enum { NO_STATE = -1 };
    long mState;
    long mNextState;
    long mPreviousState;
    long mFrame;
    float mTime;
    float mFrameTime;
    u8 mWork[0x40];
    u8 mPreviousWork[0x40];
    long mUnknown98;
    long mUnknown9C;
    long mUnknownA0;
    long mUnknownA4;
    void update();
    void addTime();
    void init();
};
#endif

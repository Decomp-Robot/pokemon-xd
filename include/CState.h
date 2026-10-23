#ifndef C_STATE_H
#define C_STATE_H

#include <types.h>

class CState {
public:
    void init();
    void update();
    void addTime();

    long mCurrent;
    long mNext;
    long mPrevious;
    long mFrameCount;
    float mTime;
    float mDeltaTime;
    long mWork[16];
    long mPreviousWork[16];
    long reserved98;
    long reserved9C;
    long reservedA0;
    long mTutorialFrameCount;
};

#endif

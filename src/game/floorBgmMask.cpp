#include <floorBgmMask.h>

void FloorBgmMask::setState(unsigned int state)
{
    mState = state;
}

u8 FloorBgmMask::isActive()
{
    return mActive;
}

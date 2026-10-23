#include <WorldMap.h>
extern s16 worldMapCursorTable[];
extern u8 worldMapTargetDebugEnabled;
extern const f32 worldMapFloatZero;
extern const f32 worldMapTargetCamOffsetX;
extern const f32 worldMapTargetCamOffsetY;
extern const f32 worldMapTargetCamOffsetZ;
extern const f32 worldMapFloatOne;
void WorldMap::TargetDebug(long index)
{
    s32 prev = mCamWork->mDebugTarget;
    s16 id = worldMapCursorTable[index];
    mCamWork->mPrevDebugTarget = prev;
    mCamWork->mDebugTarget = id;
    if (worldMapTargetDebugEnabled == 0) return;
    if (mCamWork->mPrevDebugTarget == mCamWork->mDebugTarget) return;
    SetCameraNextWork(&mIcons[id].pos, worldMapTargetCamOffsetX, worldMapTargetCamOffsetY, worldMapFloatZero, worldMapTargetCamOffsetZ);
    SetCameraSpeed(worldMapFloatOne);
}

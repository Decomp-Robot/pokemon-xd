#ifndef LEAFFX_H
#define LEAFFX_H

#include <GSeffect.h>
#include <GSmodel.h>
#include <dolphin/mtx.h>

struct leafData {
    GSvec points[4];
    Quaternion rotationStart;
    Quaternion rotationEnd;
    f32 scale;
    f32 time;
    GSmodel* model;
};

class leafEffect : public GSeffect {
public:
    virtual ~leafEffect();
    virtual void _stop();
    virtual bool _update(f32 delta);
    virtual bool _start();
    leafEffect(u8** data, f32 duration);
    void UpdateLeafData(leafData* leaf);
    bool GenerateLeafData(leafData* leaf);

    leafData* mLeaves;
    GSmodel* mModel;
    f32 mScale;
    f32 mLeafScale;
    f32 mLeafScaleRandom;
    f32 mDistance;
    f32 mDistanceRandom;
    f32 mControlDistance;
    f32 mControlDistanceRandom;
    f32 mFall;
    f32 mSpeed;
    f32 mInitialDistance;
    f32 mInitialDistanceRandom;
    f32 mFadeTime;
    f32 mTime;
    f32 mDuration;
    u32 mLeafCount;
    u32 mResourceID;
    GSvec mPosition;
};

#endif

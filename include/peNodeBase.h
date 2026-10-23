#ifndef PE_NODE_BASE_H
#define PE_NODE_BASE_H

#include <GSgfx.h>

class peEmitterNode;
struct _peEmitter;

struct peSRT {
    GSvec mPosition;
    f32 mRotation[4];
    GSvec mScale;
};


struct _EmitParams {
    _EmitParams() { mLife = 0.0f; mAccelerationScale = 0.0f; }
    f32 mLife;
    f32 mAccelerationScale;
    GSvec mPosition;
    GSvec mVelocity;
    peSRT mTransform;
};


class peNode {
public:
    void Start(peEmitterNode *parent, _EmitParams *params);
    bool Update(peEmitterNode *parent, f32 delta);

    f32 mLife;
    f32 mRemainingLife;
    GSvec mPosition;
    GSvec mVelocity;
    peSRT *mTransform;
    f32 mAccelerationScale;
    peNode *mNext;
    peNode *mPrevious;

private:
};


#endif

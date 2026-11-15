#ifndef PEMODEL_H
#define PEMODEL_H

#include <peContainer.h>
#include <peBankData.h>

class peModelContainer : public peContainer {
public:
    peModelContainer(peEmitterNode *parent, _peModel *model, u16 capacity);
    virtual ~peModelContainer();
    virtual void Stop();
    virtual void Render();
    virtual void Update(f32 delta);
    virtual u16 Emit(u16 count);

    bool RenderForLayer();

    _peModel *mModel;
};

enum peModelReflectionFlags {
    PE_MODEL_REFLECT_X = 0x1,
    PE_MODEL_REFLECT_Y = 0x2,
    PE_MODEL_REFLECT_Z = 0x4
};

class peModelNode : public peNode {
public:
    bool Update(peEmitterNode *parent, _peModel *model, f32 delta);
    void Start(peEmitterNode *parent, _peModel *model, _EmitParams *params);

    void Render(_peModel *model);

    peNodeTransform mWorldTransform;
    u16 mReflection;
    s16 mPrimitiveInstance;
    f32 mInitialRotation[4];
    f32 mRotation[4];
    GSvec mScale;
    GSvec mRotationVelocity;
    f32 mDepth;
};

#endif

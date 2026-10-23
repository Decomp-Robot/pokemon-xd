#ifndef PEEMITTER_H
#define PEEMITTER_H

#include <peNode.h>

class peContainer;
struct _peEmitter;
struct _particleData;

enum peEmitterFlags {
    PE_EMITTER_LOOP = 0x01,
    PE_EMITTER_LOCAL_VELOCITY = 0x40,
    PE_EMITTER_LOCAL_FORCE = 0x100,
    PE_EMITTER_COLLIDE_GROUND = 0x200,
    PE_EMITTER_ALIGN_TO_VELOCITY = 0x400,
    PE_EMITTER_ATTRACT_POINT = 0x10000,
    PE_EMITTER_USE_BOUND = 0x20000
};

struct peSpline;
struct peColorSpline;
struct _pePositionFunction;

struct _peEmitter {
    u32 flags;
    u32 mChildCount;
    f32 mLife;
    f32 mParticleCount;
    f32 mParticleCountRandom;
    peSpline *mParticleCountSpline;
    u8 mEmissionParameters[0x04];
    f32 mEmitInterval;
    u8 mParticleParameters[0x80];
    f32 mDrag;                 // 0xA0
    f32 mBounce;               // 0xA4
    f32 mAttraction;           // 0xA8
    GSvec mForceParameters;
    peSpline *mForceSplines[3];
    GSvec mPositionParameters;
    peSpline *mPositionSplines[3];
    GSvec mRotationParameters;
    peSpline *mRotationSplines[3];
    GSvec mRotationVelocityParameters;
    GSvec mRotationVelocityRandom;
    peSpline *mRotationVelocitySplines[3];
    GSvec mScaleParameters;
    peSpline *mScaleSplines[3];
    peColorSpline *mColorSpline;
    peSpline *mAlphaSpline;
    _particleData *mChildren;  // 0x138
};

class peEmitterNode : public peNode {
public:
    peEmitterNode() {
        transform = 0;
        next = 0;
        previous = 0;
        emitter = 0;
        children = 0;
    }
    void Init(_peEmitter *emitter);
    void Release();
    void Start(peEmitterNode *parent, _peEmitter *emitter, _EmitParams *params);
    bool Update(peEmitterNode *parent, _peEmitter *emitter, f32 delta);
    void Stop(_peEmitter *emitter);
    void Render(_peEmitter *emitter);
    void SetLastLoop();

    u8 reserved30[0x08];
    f32 colorScale[4];
    u8 reserved48[0x54];
    GSvec force;
    u8 reservedA8[0x54];
    _peEmitter *emitter;      // 0xFC
    peContainer *children; // 0x100
};

#endif

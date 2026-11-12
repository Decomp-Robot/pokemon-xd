#ifndef PEEMITTER_H
#define PEEMITTER_H

#include <peNode.h>
#include <peNodeMath.h>
#include <peContainer.h>

class peContainer;
struct _peEmitter;
struct _particleData;

enum peEmitterFlags {
    PE_EMITTER_LOOP = 0x01,
    PE_EMITTER_NO_ROTATION = 0x02,
    PE_EMITTER_RANDOM_ROTATION_X = 0x08,
    PE_EMITTER_RANDOM_ROTATION_Y = 0x10,
    PE_EMITTER_RANDOM_ROTATION_Z = 0x20,
    PE_EMITTER_LOCAL_VELOCITY = 0x40,
    PE_EMITTER_ROTATION_SPLINE = 0x80,
    PE_EMITTER_LOCAL_FORCE = 0x100,
    PE_EMITTER_COLLIDE_GROUND = 0x200,
    PE_EMITTER_ALIGN_TO_VELOCITY = 0x400,
    PE_EMITTER_ATTRACT_POINT = 0x10000,
    PE_EMITTER_USE_BOUND = 0x20000,
    PE_EMITTER_REVERSE_ROTATION_X = 0x100000,
    PE_EMITTER_REVERSE_ROTATION_Y = 0x200000,
    PE_EMITTER_REVERSE_ROTATION_Z = 0x400000
};

enum peEmitterNodeFlags {
    PE_EMITTER_NODE_REVERSE_X = 0x01,
    PE_EMITTER_NODE_REVERSE_Y = 0x02,
    PE_EMITTER_NODE_REVERSE_Z = 0x04,
    PE_EMITTER_NODE_EMISSION_STARTED = 0x08,
    PE_EMITTER_NODE_EMITTED = 0x10,
    PE_EMITTER_NODE_FIRST_LOOP = 0x20,
    PE_EMITTER_NODE_LAST_LOOP = 0x40
};

enum pePositionFunctionFlags {
    PE_POSITION_TYPE_MASK = 0x3,
    PE_POSITION_SEQUENTIAL_OFFSET = 0x10,
    PE_POSITION_SEQUENTIAL_RADIUS = 0x20,
    PE_POSITION_SEQUENTIAL_ANGLE = 0x40,
    PE_POSITION_SEQUENTIAL_SPIN = 0x80,
    PE_POSITION_LOCAL = 0x100
};

enum pePositionFunctionType {
    PE_POSITION_CONE = 0,
    PE_POSITION_ARC = 1,
    PE_POSITION_DISC = 3
};

enum peVelocityFunctionFlags {
    PE_VELOCITY_FROM_POSITION = 0x2,
    PE_VELOCITY_LOCAL = 0x10,
    PE_VELOCITY_SEQUENTIAL_SPEED = 0x20
};

struct peSpline;
struct peColorSpline;

struct _pePositionFunction {
    u16 flags;
    GSvec center;
    union {
        struct {
            GSvec offset;
            GSvec direction;
            GSvec base;
            f32 radius;
            f32 radiusRandom;
            f32 spinScale;
        } cone;
        struct {
            GSvec axis;
            f32 radius;
            f32 radiusRandom;
            f32 angle;
            f32 angleRandom;
            f32 angleScale;
            f32 spinScale;
        } arc;
        struct {
            GSvec axis;
            GSvec base;
            f32 radius;
            f32 radiusRandom;
            f32 spinScale;
        } disc;
    };
};

struct _peVelocityFunction {
    u16 flags;
    union {
        GSvec direction;
        struct {
            f32 speed;
            f32 speedRandom;
        } radial;
    };
    GSvec axis;
    f32 speed;
    f32 speedRandom;
    f32 angle;
    f32 angleRandom;
};

struct _peEmitter {
    u32 flags;
    u32 mChildCount;
    f32 mLife;
    f32 mParticleCount;
    f32 mParticleCountRandom;
    peSpline *mParticleCountSpline;
    f32 mEmissionStart;
    f32 mEmitInterval;
    f32 mParticleLife;
    f32 mParticleLifeRandom;
    f32 mInitialNodeLife;
    f32 mForceResponse;
    f32 mForceResponseRandom;
    _pePositionFunction mPositionFunction;
    _peVelocityFunction mVelocityFunction;
    f32 mDrag;
    f32 mBounce;
    f32 mAttraction;
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
    _particleData *mChildren;
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
    void CreateWorldParams();
    void CreateParameters(u16 count, f32 delta);
    static void GetPositionParameter(GSvec *position, GSvec *direction,
                                     _pePositionFunction *function, f32 time);

    f32 mEmissionTime;
    f32 mEmissionAccumulator;
    f32 colorScale[4];
    u16 mFlags;
    u16 mEmittedCount;
    GSvec mScale;
    GSvec mLocalScale;
    peQuaternion mRotation;
    GSvec mPositionOffset;
    peQuaternion mRandomRotation;
    GSvec mRotationVelocity;
    GSvec force;
    peNodeTransform mWorldTransform;
    peNodeTransform mPreviousWorldTransform;
    f32 mCameraDepth;
    _peEmitter *emitter;
    peContainer *children;
};

class peEmitterContainer : public peContainer {
public:
    peEmitterContainer(peEmitterNode *parent, _peEmitter *source, u16 capacity);

    virtual void Stop();
    virtual void Render();
    bool DoRender();
    virtual void Update(f32 delta);
    virtual u16 Emit(u16 count);
    virtual void PreUpdate();
    virtual ~peEmitterContainer();

    _peEmitter *mEmitter;
};

#endif

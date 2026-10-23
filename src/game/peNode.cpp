#include <peNodeMath.h>

extern "C" f32 (*GSgfxMatrixGetPtr(u32))[4];

static inline const peEmitterRenderParameters &RenderParameters(peEmitterNode *node) {
    return *(const peEmitterRenderParameters *)node->mEmitterState;
}

void peNodeTransformsConcat(peSRT *result, peSRT *parent, peSRT *local,
                             bool cameraRotation)
{
    result->mPosition = Product(local->mPosition, parent->mScale);
    Rotate(result->mPosition, parent->mRotation);
    result->mPosition += parent->mPosition;
    if (cameraRotation) {
        C_QUATMtx(SDKQuaternion(result->mRotation), GSgfxMatrixGetPtr(PE_CAMERA_INVERSE_MATRIX));
    } else {
        *(peQuaternion *)result->mRotation = Multiply(parent->mRotation, local->mRotation);
    }
    result->mScale = Product(local->mScale, parent->mScale);
}

bool peNode::Update(peEmitterNode *parent, f32 delta)
{
    _peEmitter *emitter;
    bool moving;
    GSvec displacement, acceleration;
    moving = Magnitude(mAccelerationScale) > 0.00001f;
    mRemainingLife -= delta;
    if (mRemainingLife <= 0.0f) return false;
    if (!parent) return true;
    emitter = parent->mEmitter;
    if (moving) {
        acceleration = RenderParameters(parent).mAcceleration;
        if (!(emitter->mFlags & PE_EMITTER_LOCAL_ACCELERATION)) {
            peQuaternion inverse = QuaternionInverse(mTransform->mRotation);
            Rotate(acceleration, &inverse.x);
        }
        if (emitter->mFlags & PE_EMITTER_ATTRACT_POINT) {
            displacement = acceleration - mPosition;
            f32 length = PSVECMag(SDKVector(&displacement));
            if (length > 0.00001f) {
                f32 strength = emitter->mAttraction;
                displacement *= delta * (strength * mAccelerationScale) / length;
            }
        } else {
            displacement = acceleration * (mAccelerationScale * delta);
        }
        mVelocity += displacement;
        f32 drag = emitter->mDrag;
        if (drag)
            mVelocity *= 1.0f - drag;
    }
    displacement = mVelocity * delta;
    mPosition += displacement;
    if (moving && (emitter->mFlags & PE_EMITTER_COLLIDE_GROUND)) {
        displacement = Product(mPosition, mTransform->mScale);
        Rotate(displacement, mTransform->mRotation);
        displacement += mTransform->mPosition;
        if (displacement.y < 0.0f) {
            peQuaternion positionInverse, velocityInverse;
            if (displacement.y < 0.0f) {
                displacement.y = 0.0f;
                displacement -= mTransform->mPosition;
                positionInverse = QuaternionInverse(mTransform->mRotation);
                Rotate(displacement, &positionInverse.x);
                RemoveScale(displacement, mTransform->mScale);
                mPosition = displacement;
                displacement = Product(mVelocity, mTransform->mScale);
                Rotate(displacement, mTransform->mRotation);
                displacement.y *= emitter->mBounce;
                velocityInverse = QuaternionInverse(mTransform->mRotation);
                Rotate(displacement, &velocityInverse.x);
                RemoveScale(displacement, mTransform->mScale);
                mVelocity = displacement;
            }
        }
    }
    return true;
}

void peNode::Start(peEmitterNode *parent, _EmitParams *params)
{
    mLife = mRemainingLife = params->mLife;
    mAccelerationScale = params->mAccelerationScale;
    mPosition = params->mPosition;
    mVelocity = params->mVelocity;
    if (parent && (parent->mEmitter->mFlags & PE_EMITTER_SAVE_TRANSFORM)) {
        mTransform->mPosition = params->mTransform.mPosition;
        *(peQuaternion *)mTransform->mRotation = *(peQuaternion *)params->mTransform.mRotation;
        mTransform->mScale = params->mTransform.mScale;
    }
}

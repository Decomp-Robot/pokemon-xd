#include <peParticle.h>
#include <peNodeMath.h>

extern "C" f32 (*GSgfxMatrixGetPtr(u32))[4];

void peNodeTransformsConcat(peNodeTransform *result, peNodeTransform *parent, peNodeTransform *local,
                             bool cameraRotation)
{
    result->position = Product(local->position, parent->scale);
    Rotate(result->position, parent->rotation);
    result->position += parent->position;
    if (cameraRotation) {
        C_QUATMtx(SDKQuaternion(result->rotation), GSgfxMatrixGetPtr(PE_CAMERA_INVERSE_MATRIX));
    } else {
        *(peQuaternion *)result->rotation = Multiply(parent->rotation, local->rotation);
    }
    result->scale = Product(local->scale, parent->scale);
}

bool peNode::Update(peEmitterNode *parent, f32 delta)
{
    _peEmitter *emitter;
    bool moving;
    GSvec displacement, force;
    moving = Magnitude(forceResponse) > 0.00001f;
    remainingLife -= delta;
    if (remainingLife <= 0.0f) return false;
    if (!parent) return true;
    emitter = parent->emitter;
    if (moving) {
        force = parent->force;
        if (!(emitter->flags & PE_EMITTER_LOCAL_FORCE)) {
            peQuaternion inverse = QuaternionInverse(transform->rotation);
            Rotate(force, &inverse.x);
        }
        if (emitter->flags & PE_EMITTER_ATTRACT_POINT) {
            displacement = force - position;
            f32 length = PSVECMag(SDKVector(&displacement));
            if (length > 0.00001f) {
                f32 strength = emitter->mAttraction;
                displacement *= delta * (strength * forceResponse) / length;
            }
        } else {
            displacement = force * (forceResponse * delta);
        }
        velocity += displacement;
        f32 drag = emitter->mDrag;
        if (drag)
            velocity *= 1.0f - drag;
    }
    displacement = velocity * delta;
    position += displacement;
    if (moving && (emitter->flags & PE_EMITTER_COLLIDE_GROUND)) {
        displacement = Product(position, transform->scale);
        Rotate(displacement, transform->rotation);
        displacement += transform->position;
        if (displacement.y < 0.0f) {
            peQuaternion positionInverse, velocityInverse;
            if (displacement.y < 0.0f) {
                displacement.y = 0.0f;
                displacement -= transform->position;
                positionInverse = QuaternionInverse(transform->rotation);
                Rotate(displacement, &positionInverse.x);
                RemoveScale(displacement, transform->scale);
                position = displacement;
                displacement = Product(velocity, transform->scale);
                Rotate(displacement, transform->rotation);
                displacement.y *= emitter->mBounce;
                velocityInverse = QuaternionInverse(transform->rotation);
                Rotate(displacement, &velocityInverse.x);
                RemoveScale(displacement, transform->scale);
                velocity = displacement;
            }
        }
    }
    return true;
}

void peNode::Start(peEmitterNode *parent, _EmitParams *params)
{
    life = remainingLife = params->life;
    forceResponse = params->forceResponse;
    position = params->position;
    velocity = params->velocity;
    if (parent && (parent->emitter->flags & PE_EMITTER_LOCAL_VELOCITY)) {
        transform->position = params->transform.position;
        *(peQuaternion *)transform->rotation = *(peQuaternion *)params->transform.rotation;
        transform->scale = params->transform.scale;
    }
}

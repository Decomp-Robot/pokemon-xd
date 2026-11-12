#include <peEmitter.h>
#include <particleEngine.h>
#include <peBank.h>
#include <peBankData.h>
#include <peParticle.h>
#include <peShape.h>
#include <peParticleStrip.h>
#include <peDistortion.h>
#include <peModel.h>
#include <HSD_Rand.h>
#include <GSAPI/GSsysdolphin_baselib/baselib/quatlib.h>
#include <peContainer.h>
#include <peSpline.h>
#include <peNodeMath.h>
#include <dolphin/os/OSFastCast.h>
#include <math.h>

/* These bits select the passes and layers on which an emitter may render. */
enum peEmitterContainerRenderFlags {
    PE_EMITTER_RENDER_OPAQUE = 0x00000800,
    PE_EMITTER_RENDER_TRANSLUCENT = 0x00001000,
    PE_EMITTER_RENDER_LAYER_0 = 0x00002000,
    PE_EMITTER_RENDER_LAYER_2 = 0x00004000,
    PE_EMITTER_RENDER_LAYER_4 = 0x00008000,
    PE_EMITTER_RENDER_LAYER_1 = 0x00800000,
    PE_EMITTER_RENDER_LAYER_3 = 0x01000000,
    PE_EMITTER_RENDER_LAYER_5 = 0x02000000,
    PE_EMITTER_RENDER_LAYER_MASK = 0x0380E000,
    PE_EMITTER_RENDER_HIDDEN = 0x80000000,
    PE_EMITTER_SORT_CHILDREN_BY_DEPTH = 0x00040000
};

inline peEmitterContainer::peEmitterContainer(peEmitterNode *parent, _peEmitter *source, u16 capacity)
    : peContainer(parent, sizeof(peEmitterNode), capacity), mEmitter(source)
{
    peEmitterNode *next;
    for (peEmitterNode *node = (peEmitterNode *)freeNodes; node; node = next) {
        next = (peEmitterNode *)node->next;
        node->Init(source);
    }
    g_particle_debug_stats.containerBytes += sizeof(peEmitterContainer);
    g_particle_debug_stats.totalBytes += sizeof(peEmitterContainer);
}

static inline f32 SplineValue(peSpline *spline, f32 time)
{
    if (!spline || !(spline->flags & PE_SPLINE_MODE_MASK))
        return 1.0f;
    f32 difference = spline->time - time;
    difference = difference < 0.0f ? -difference : difference;
    if (difference <= 0.00001f)
        return spline->value;
    return peSplineGetValue(spline, time);
}

static inline f32 Unsigned8ToFloat(u8 value) { f32 result; OSu8tof32(&value, &result); return result; }

static inline f32 Unsigned16ToFloat(u16 value) { f32 result; OSu16tof32(&value, &result); return result; }
static inline u16 FloatToUnsigned16(f32 value) { u16 result; OSf32tou16(&value, &result); return result; }

/* Color spline channels are half intensity: 128 is full color. */
static inline f32 ColorScale(u8 component)
{
    return 0.00390625f * (2.0f * Unsigned8ToFloat(component));
}

/* Number of particles this node emits this frame. Emission begins once the
 * emitter's start time is reached; an emitter with an interval keeps emitting
 * at its rate until the interval has passed, otherwise it emits one burst. */
static inline u16 EmitCount(peEmitterNode *node, f32 time, f32 delta)
{
    if (node->mFlags & PE_EMITTER_NODE_LAST_LOOP)
        return 0;
    if (!(node->mFlags & PE_EMITTER_NODE_EMISSION_STARTED) &&
        node->mEmissionTime < node->emitter->mEmissionStart)
        return 0;
    node->mFlags |= PE_EMITTER_NODE_EMISSION_STARTED;
    if ((node->mFlags & PE_EMITTER_NODE_EMITTED) &&
        node->mEmissionTime > node->emitter->mEmissionStart + node->emitter->mEmitInterval)
        return 0;
    node->mFlags |= PE_EMITTER_NODE_EMITTED;

    f32 random = HSD_Randf();
    f32 count = node->emitter->mParticleCount +
                node->emitter->mParticleCountRandom * random;
    count *= SplineValue(node->emitter->mParticleCountSpline, time);
    if (node->emitter->mEmitInterval <= 0.00001f) {
        node->mEmissionAccumulator = 0.0f;
        return FloatToUnsigned16(count);
    }

    count *= delta;
    count += node->mEmissionAccumulator;
    u16 emitCount = FloatToUnsigned16(count);
    node->mEmissionAccumulator = count - Unsigned16ToFloat(emitCount);
    if (!emitCount && !node->mEmittedCount) {
        node->mEmissionAccumulator = 0.0f;
        emitCount = 1;
    }
    return emitCount;
}

static inline GSvec Lerp(const GSvec &from, const GSvec &to, f32 t)
{
    GSvec result = to - from;
    result *= t;
    result += from;
    return result;
}

static inline peQuaternion Lerp(const f32 *from, const f32 *to, f32 t)
{
    peQuaternion result;
    C_QUATLerp(SDKQuaternion(from), SDKQuaternion(to), SDKQuaternion(&result.x), t);
    return result;
}

static inline void InverseRotate(GSvec &value, const f32 *rotation)
{
    peQuaternion inverse = QuaternionInverse(rotation);
    Rotate(value, &inverse.x);
}

static inline void RotateVector(GSvec &out, const GSvec &value, const f32 *rotation)
{
    Mtx matrix;
    PSMTXQuat(matrix, SDKQuaternion(rotation));
    PSMTXMultVecSR(matrix, SDKVector(&value), SDKVector(&out));
}

static inline void RotateBy(GSvec &value, const peQuaternion &rotation)
{
    RotateVector(value, value, &rotation.x);
}

static inline GSvec RotatedBy(const GSvec &value, const peQuaternion &rotation)
{
    GSvec result;
    RotateVector(result, value, &rotation.x);
    return result;
}

static inline void GetVelocityParameter(GSvec *velocity, GSvec *direction, _peVelocityFunction *function, f32 time)
{
    peQuaternion rotation;
    f32 speed;

    switch (function->flags & PE_VELOCITY_FROM_POSITION) {
    case 0: {
        C_QUATRotAxisRad(SDKQuaternion(&rotation.x), SDKVector(&function->axis),
                         function->angle + function->angleRandom * HSD_Randf());
        *velocity = RotatedBy(function->direction, rotation);
        f32 random = HSD_Randf();
        C_QUATRotAxisRad(SDKQuaternion(&rotation.x), SDKVector(&function->direction),
                         3.1415927f * (2.0f * random));
        RotateBy(*velocity, rotation);
        if (function->flags & PE_VELOCITY_SEQUENTIAL_SPEED)
            speed = function->speed + time * function->speedRandom;
        else
            speed = function->speed + function->speedRandom * HSD_Randf();
        *velocity *= speed;
        break;
    }
    case PE_VELOCITY_FROM_POSITION:
        if (function->flags & PE_VELOCITY_SEQUENTIAL_SPEED)
            speed = function->radial.speed + time * function->radial.speedRandom;
        else
            speed = function->radial.speed + function->radial.speedRandom * HSD_Randf();
        *velocity = *direction * speed;
        break;
    default:
        velocity->x = 0.0f;
        velocity->y = 0.0f;
        velocity->z = 0.0f;
        break;
    }

}

static inline GSvec Normalized(const GSvec &vector)
{
    GSvec result;
    PSVECNormalize(SDKVector(&vector), SDKVector(&result));
    return result;
}

static inline void RenderEmitter(peEmitterNode *node)
{
    peContainer *container = node->children;
    if (!node->mEmittedCount)
        return;
    for (; container; container = container->nextContainer)
        container->Render();
}

void peEmitterNode::GetPositionParameter(GSvec *position, GSvec *direction,
                                         _pePositionFunction *function, f32 time)
{
    GSvec offset;
    Quaternion rotation;
    f32 angle;
    f32 radius;

    switch (function->flags & PE_POSITION_TYPE_MASK) {
    case PE_POSITION_CONE: {
        *direction = function->cone.direction;
        if ((Magnitude(function->cone.radius) > 0.00001f ||
             Magnitude(function->cone.radiusRandom) > 0.00001f) &&
            PSVECSquareMag((Vec *)direction) > 0.00001f) {
            if (function->flags & PE_POSITION_SEQUENTIAL_SPIN)
                angle = 3.1415927f * (2.0f * (function->cone.spinScale * time));
            else {
                f32 random = HSD_Randf();
                angle = 3.1415927f * (2.0f * random);
            }
            C_QUATRotAxisRad(&rotation, (Vec *)direction, angle);
            *position = Rotated(function->cone.base, &rotation.x);
            if (function->flags & PE_POSITION_SEQUENTIAL_RADIUS)
                radius = time * function->cone.radiusRandom + function->cone.radius;
            else
                radius = function->cone.radius + function->cone.radiusRandom * HSD_Randf();
            PSVECScale((Vec *)position, (Vec *)position, radius);
        } else {
            position->x = 0.0f;
            position->y = 0.0f;
            position->z = 0.0f;
        }

        if (function->flags & PE_POSITION_SEQUENTIAL_OFFSET) {
            offset = function->cone.offset * time;
        } else {
            angle = HSD_Randf();
            offset = function->cone.offset * angle;
        }
        PSVECAdd((Vec *)&offset, (Vec *)&function->center, (Vec *)&offset);
        PSVECAdd((Vec *)position, (Vec *)&offset, (Vec *)position);
        if (PSVECSquareMag((Vec *)position) > 0.00001f) {
            *direction = Normalized(*position);
        } else {
            direction->x = 0.0f;
            direction->y = 0.0f;
            direction->z = 0.0f;
        }
        break;
    }

    case PE_POSITION_ARC: {
        if (function->flags & PE_POSITION_SEQUENTIAL_ANGLE)
            angle = function->arc.angle + function->arc.angleRandom * (function->arc.angleScale * time);
        else
            angle = function->arc.angle + function->arc.angleRandom * HSD_Randf();
        C_QUATRotAxisRad(&rotation, (Vec *)&function->arc.axis, angle);
        *direction = Rotated(function->center, &rotation.x);
        if (function->flags & PE_POSITION_SEQUENTIAL_SPIN)
            angle = 3.1415927f * (2.0f * (function->arc.spinScale * time));
        else {
            f32 random = HSD_Randf();
            angle = 3.1415927f * (2.0f * random);
        }
        C_QUATRotAxisRad(&rotation, (Vec *)&function->center, angle);
        Rotate(*direction, &rotation.x);
        if (function->flags & PE_POSITION_SEQUENTIAL_RADIUS)
            radius = time * function->arc.radiusRandom + function->arc.radius;
        else
            radius = function->arc.radius + function->arc.radiusRandom * HSD_Randf();
        *position = *direction * radius;
        break;
    }

    case PE_POSITION_DISC: {
        *direction = function->disc.axis;
        if (function->flags & PE_POSITION_SEQUENTIAL_SPIN)
            angle = 3.1415927f * (2.0f * (function->disc.spinScale * time));
        else {
            f32 random = HSD_Randf();
            angle = 3.1415927f * (2.0f * random);
        }
        C_QUATRotAxisRad(&rotation, (Vec *)direction, angle);
        *position = Rotated(function->disc.base, &rotation.x);
        if (function->flags & PE_POSITION_SEQUENTIAL_RADIUS)
            radius = time * function->disc.radiusRandom + function->disc.radius;
        else
            radius = function->disc.radius + function->disc.radiusRandom * HSD_Randf();
        PSVECScale((Vec *)position, (Vec *)position, radius);
        PSVECAdd((Vec *)position, (Vec *)&function->center, (Vec *)position);
        break;
    }

    default:
        position->x = 0.0f;
        position->y = 0.0f;
        position->z = 0.0f;
        direction->x = 0.0f;
        direction->y = 0.0f;
        direction->z = 0.0f;
        break;
    }
}

void peEmitterNode::CreateParameters(u16 count, f32 delta)
{
    _peEmitter *data = emitter;
    _EmitParams *params;
    bool localVelocity = (data->flags & PE_EMITTER_LOCAL_VELOCITY) != 0;
    _pePositionFunction *positionFunction;
    f32 sequenceStep = data->mLife;
    f32 sequenceTime = sequenceStep * Unsigned16ToFloat(mEmittedCount + count);
    f32 interpolationStep = 1.0f / Unsigned16ToFloat(count);
    positionFunction = &data->mPositionFunction;
    f32 interpolation = 1.0f;
    f32 age = 0.0f;
    params = g_emitParams;
    f32 ageStep = delta * interpolationStep;
    _peVelocityFunction *velocityFunction = &data->mVelocityFunction;
    GSvec direction;
    f32 random;

    while (count--) {
        GetPositionParameter(&params->position, &direction, positionFunction, sequenceTime);
        GetVelocityParameter(&params->velocity, &direction, velocityFunction, sequenceTime);

        direction = params->velocity * age;
        params->position += direction;
        random = HSD_Randf();
        params->life = emitter->mParticleLife + emitter->mParticleLifeRandom * random - age;
        random = HSD_Randf();
        params->forceResponse = emitter->mForceResponse + emitter->mForceResponseRandom * random;

        if (localVelocity) {
            params->transform.position = Lerp(mPreviousWorldTransform.position, mWorldTransform.position, interpolation);
            *(peQuaternion *)params->transform.rotation = Lerp(mPreviousWorldTransform.rotation, mWorldTransform.rotation, interpolation);
            params->transform.scale = Lerp(mPreviousWorldTransform.scale, mWorldTransform.scale, interpolation);
            if (positionFunction->flags & PE_POSITION_LOCAL) {
                params->position -= params->transform.position;
                InverseRotate(params->position, params->transform.rotation);
                RemoveScale(params->position, params->transform.scale);
            }
            if (velocityFunction->flags & PE_VELOCITY_LOCAL) {
                InverseRotate(params->velocity, params->transform.rotation);
            }
        } else {
            if (positionFunction->flags & PE_POSITION_LOCAL) {
                params->position -= mWorldTransform.position;
                InverseRotate(params->position, mWorldTransform.rotation);
                RemoveScale(params->position, mWorldTransform.scale);
            }
            if (velocityFunction->flags & PE_VELOCITY_LOCAL) {
                InverseRotate(params->velocity, mWorldTransform.rotation);
            }
        }

        sequenceTime -= sequenceStep;
        params++;
        interpolation -= interpolationStep;
        age += ageStep;
    }
}

void peEmitterNode::CreateWorldParams()
{
    u32 flags = emitter->flags;
    peNodeTransform local;

    mPreviousWorldTransform.position = mWorldTransform.position;
    *(peQuaternion *)mPreviousWorldTransform.rotation = *(peQuaternion *)mWorldTransform.rotation;
    mPreviousWorldTransform.scale = mWorldTransform.scale;

    if (mFlags & PE_EMITTER_NODE_REVERSE_X)
        mWorldTransform.rotation[0] = -mRotation.x;
    else
        mWorldTransform.rotation[0] = mRotation.x;
    if (mFlags & PE_EMITTER_NODE_REVERSE_Y)
        mWorldTransform.rotation[1] = -mRotation.y;
    else
        mWorldTransform.rotation[1] = mRotation.y;
    if (mFlags & PE_EMITTER_NODE_REVERSE_Z)
        mWorldTransform.rotation[2] = -mRotation.z;
    else
        mWorldTransform.rotation[2] = mRotation.z;
    mWorldTransform.rotation[3] = mRotation.w;

    local.position = position + mPositionOffset;
    peQuaternion rotation = Multiply(&mRandomRotation.x, mWorldTransform.rotation);
    local.rotation[0] = rotation.x;
    local.rotation[1] = rotation.y;
    local.rotation[2] = rotation.z;
    local.rotation[3] = rotation.w;
    local.scale = mLocalScale;
    peNodeTransformsConcat(&mWorldTransform, transform, &local, (flags & PE_EMITTER_NO_ROTATION) != 0);

    mCameraDepth = Dot(g_CameraLookDir, mWorldTransform.position - g_CameraPosition);

    if (!mEmittedCount) {
        mPreviousWorldTransform.position = mWorldTransform.position;
        *(peQuaternion *)mPreviousWorldTransform.rotation = *(peQuaternion *)mWorldTransform.rotation;
        mPreviousWorldTransform.scale = mWorldTransform.scale;
    }
}

void peEmitterNode::SetLastLoop()
{
    if (mFlags & PE_EMITTER_NODE_LAST_LOOP)
        return;
    if (!(emitter->flags & PE_EMITTER_LOOP))
        return;

    mFlags |= PE_EMITTER_NODE_LAST_LOOP;
    f32 loopEnd = emitter->mEmissionStart + emitter->mEmitInterval;
    if (remainingLife < emitter->mInitialNodeLife - loopEnd) {
        remainingLife = emitter->mInitialNodeLife - loopEnd;
        mEmissionTime = loopEnd;
    }
}

void peEmitterNode::Start(peEmitterNode *parent, _peEmitter *source,
                          _EmitParams *params)
{
    u32 flags = source->flags;
    peNode::Start(parent, params);

    if (!parent && (flags & PE_EMITTER_LOOP))
        remainingLife = source->mEmissionStart + source->mEmitInterval;
    else
        remainingLife = source->mInitialNodeLife;
    life = source->mInitialNodeLife;

    mEmissionTime = 0.0f;
    mEmissionAccumulator = 0.0f;
    colorScale[3] = 0.0f;
    mEmittedCount = 0;
    mFlags = PE_EMITTER_NODE_FIRST_LOOP;
    mRotation.x = 0.0f;
    mRotation.y = 0.0f;
    mRotation.z = 0.0f;
    mRotation.w = 1.0f;
    mRandomRotation.x = 0.0f;
    mRandomRotation.y = 0.0f;
    mRandomRotation.z = 0.0f;
    mRandomRotation.w = 1.0f;
    mPreviousWorldTransform.position.x = 0.0f;
    mPreviousWorldTransform.position.y = 0.0f;
    mPreviousWorldTransform.position.z = 0.0f;
    mPreviousWorldTransform.rotation[0] = 0.0f;
    mPreviousWorldTransform.rotation[1] = 0.0f;
    mPreviousWorldTransform.rotation[2] = 0.0f;
    mPreviousWorldTransform.rotation[3] = 1.0f;
    mPreviousWorldTransform.scale = unitScaleVector;
    mWorldTransform.position.x = 0.0f;
    mWorldTransform.position.y = 0.0f;
    mWorldTransform.position.z = 0.0f;
    mWorldTransform.rotation[0] = 0.0f;
    mWorldTransform.rotation[1] = 0.0f;
    mWorldTransform.rotation[2] = 0.0f;
    mWorldTransform.rotation[3] = 1.0f;
    mWorldTransform.scale = unitScaleVector;

    if (!(flags & PE_EMITTER_NO_ROTATION) &&
        !(parent && (parent->emitter->flags & PE_EMITTER_ALIGN_TO_VELOCITY) == PE_EMITTER_ALIGN_TO_VELOCITY)) {
        if ((flags & PE_EMITTER_ROTATION_SPLINE) != PE_EMITTER_ROTATION_SPLINE)
            EulerToQuat(&source->mRotationParameters, &mRotation.x);
        if (flags & (PE_EMITTER_RANDOM_ROTATION_X | PE_EMITTER_RANDOM_ROTATION_Y |
                     PE_EMITTER_RANDOM_ROTATION_Z | PE_EMITTER_REVERSE_ROTATION_X |
                     PE_EMITTER_REVERSE_ROTATION_Y | PE_EMITTER_REVERSE_ROTATION_Z)) {
            GSvec angles;
            if (flags & PE_EMITTER_RANDOM_ROTATION_X) {
                f32 angleX = 6.2831855f * HSD_Randf();
                angles.x = angleX;
            } else
                angles.x = 0.0f;
            if (flags & PE_EMITTER_RANDOM_ROTATION_Y)
                angles.y = 6.2831855f * HSD_Randf();
            else
                angles.y = 0.0f;
            if (flags & PE_EMITTER_RANDOM_ROTATION_Z)
                angles.z = 6.2831855f * HSD_Randf();
            else
                angles.z = 0.0f;
            if ((flags & PE_EMITTER_REVERSE_ROTATION_X) && HSD_Randf() < 0.5f) {
                mFlags |= PE_EMITTER_NODE_REVERSE_X;
                angles.x = -angles.x;
            }
            if ((flags & PE_EMITTER_REVERSE_ROTATION_Y) && HSD_Randf() < 0.5f) {
                mFlags |= PE_EMITTER_NODE_REVERSE_Y;
                angles.y = -angles.y;
            }
            if ((flags & PE_EMITTER_REVERSE_ROTATION_Z) && HSD_Randf() < 0.5f) {
                mFlags |= PE_EMITTER_NODE_REVERSE_Z;
                angles.z = -angles.z;
            }
            EulerToQuat(&angles, &mRandomRotation.x);
        }
    }

    if ((flags & PE_EMITTER_ROTATION_SPLINE) != PE_EMITTER_ROTATION_SPLINE) {
        mRotationVelocity.x = source->mRotationVelocityParameters.x +
                              source->mRotationVelocityRandom.x * HSD_Randf();
        mRotationVelocity.y = source->mRotationVelocityParameters.y +
                              source->mRotationVelocityRandom.y * HSD_Randf();
        mRotationVelocity.z = source->mRotationVelocityParameters.z +
                              source->mRotationVelocityRandom.z * HSD_Randf();
    }
    mScale = source->mScaleParameters;

    for (peContainer *container = children; container;
         container = container->nextContainer)
        container->Start();
}

bool peEmitterNode::Update(peEmitterNode *parent, _peEmitter *emitter, f32 delta)
{
    u32 flags = emitter->flags;
    f32 time;
    GScolor color;
    GSvec rotation;

    if (!peNode::Update(parent, delta)) {
        /* Only a looping root emitter outlives its node life: it rewinds to
         * the start of the next loop unless SetLastLoop stopped it. */
        if (!parent && (flags & PE_EMITTER_LOOP)) {
            if (mFlags & PE_EMITTER_NODE_LAST_LOOP)
                return false;
            remainingLife += emitter->mEmitInterval;
            mEmissionTime -= emitter->mInitialNodeLife - emitter->mEmissionStart;
            mEmittedCount = 0;
        } else {
            return false;
        }
        mFlags &= ~PE_EMITTER_NODE_FIRST_LOOP;
    }

    if (mFlags & PE_EMITTER_NODE_FIRST_LOOP)
        time = 1.0f - remainingLife / life;
    else
        time = 1.0f;

    peColorSplineGetValue(&color, emitter->mColorSpline, time);
    colorScale[0] = ColorScale(color.channels.r);
    colorScale[1] = ColorScale(color.channels.g);
    colorScale[2] = ColorScale(color.channels.b);
    colorScale[3] = SplineValue(emitter->mAlphaSpline, time);
    if (parent) {
        colorScale[0] *= parent->colorScale[0];
        colorScale[1] *= parent->colorScale[1];
        colorScale[2] *= parent->colorScale[2];
        colorScale[3] *= parent->colorScale[3];
    }

    if (!(flags & PE_EMITTER_NO_ROTATION)) {
        if (parent && (parent->emitter->flags & PE_EMITTER_ALIGN_TO_VELOCITY)) {
            GSvec direction = velocity;
            if (parent->emitter->flags & PE_EMITTER_LOCAL_VELOCITY)
                Rotate(direction, transform->rotation);
            rotation.x = atan2(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
            rotation.y = atan2(direction.x, direction.z);
            rotation.z = 0.0f;
            EulerToQuat(&rotation, &mRotation.x);
        } else if (flags & PE_EMITTER_ROTATION_SPLINE) {
            rotation.x = emitter->mRotationParameters.x * SplineValue(emitter->mRotationSplines[0], time);
            rotation.y = emitter->mRotationParameters.y * SplineValue(emitter->mRotationSplines[1], time);
            rotation.z = emitter->mRotationParameters.z * SplineValue(emitter->mRotationSplines[2], time);
            EulerToQuat(&rotation, &mRotation.x);
        } else {
            peQuaternion step;
            rotation.x = delta * (mRotationVelocity.x * SplineValue(emitter->mRotationVelocitySplines[0], time));
            rotation.y = delta * (mRotationVelocity.y * SplineValue(emitter->mRotationVelocitySplines[1], time));
            rotation.z = delta * (mRotationVelocity.z * SplineValue(emitter->mRotationVelocitySplines[2], time));
            EulerToQuat(&rotation, &step.x);
            PSQUATMultiply(SDKQuaternion(&mRotation.x), SDKQuaternion(&step.x),
                           SDKQuaternion(&mRotation.x));
        }
    }

    mLocalScale.x = mScale.x * SplineValue(emitter->mScaleSplines[0], time);
    mLocalScale.y = mScale.y * SplineValue(emitter->mScaleSplines[1], time);
    mLocalScale.z = mScale.z * SplineValue(emitter->mScaleSplines[2], time);
    mPositionOffset.x = emitter->mPositionParameters.x * SplineValue(emitter->mPositionSplines[0], time);
    mPositionOffset.y = emitter->mPositionParameters.y * SplineValue(emitter->mPositionSplines[1], time);
    mPositionOffset.z = emitter->mPositionParameters.z * SplineValue(emitter->mPositionSplines[2], time);
    force.x = emitter->mForceParameters.x * SplineValue(emitter->mForceSplines[0], time);
    force.y = emitter->mForceParameters.y * SplineValue(emitter->mForceSplines[1], time);
    force.z = emitter->mForceParameters.z * SplineValue(emitter->mForceSplines[2], time);

    CreateWorldParams();

    peContainer *container;
    for (container = children; container; container = container->nextContainer)
        container->PreUpdate();

    u16 count = EmitCount(this, time, delta);
    if (count) {
        if (count > g_maxEmitParams)
            count = g_maxEmitParams;
        CreateParameters(count, emitter->mEmitInterval > 0.00001f ? delta : 0.0f);
        for (container = children; container; container = container->nextContainer)
            mEmittedCount += container->Emit(count);
    }

    for (container = children; container; container = container->nextContainer)
        container->Update(delta);

    mEmissionTime += delta;
    return true;
}

void peEmitterNode::Render(_peEmitter *)
{
    RenderEmitter(this);
}

void peEmitterNode::Stop(_peEmitter *)
{
    for (peContainer *container = children; container;
         container = container->nextContainer)
        container->Stop();
}

void peEmitterNode::Init(_peEmitter *source)
{
    void *record;
    emitter = source;
    u16 capacity = source->mChildCount;
    for (_particleData *data = source->mChildren; data; data = data->mNext) {
        record = data->mData;
        if (!record)
            continue;
        peContainer *container = 0;
        switch (data->mKind) {
        case PE_DATA_EMITTER: {
            _peEmitter *childEmitter = data->mEmitter;
            if (childEmitter->mChildren)
                container = new peEmitterContainer(this, childEmitter, capacity);
            break;
        }
        case PE_DATA_PARTICLE:
            if (((_peParticle *)record)->mResourceID)
                container = new peParticleContainer(this, (_peParticle *)record, capacity);
            break;
        case PE_DATA_PARTICLE_STRIP:
            if (((_peParticleStrip *)record)->mResourceID)
                container = new peParticleStripContainer(this, (_peParticleStrip *)record, capacity);
            break;
        case PE_DATA_DISTORTION:
            if (((_peDistortion *)record)->mResourceID)
                container = new peDistortionContainer(this, (_peDistortion *)record, capacity);
            break;
        case PE_DATA_MODEL:
            if (((_peModel *)record)->mResourceID)
                container = new peModelContainer(this, (_peModel *)record, capacity);
            break;
        case PE_DATA_SHAPE:
            if (((_peShape *)record)->resourceID)
                container = new peShapeContainer(this, (_peShape *)record, capacity);
            break;
        }

        if (container) {
            if (!children) {
                children = container;
            } else {
                peContainer *tail = children;
                while (tail->nextContainer)
                    tail = tail->nextContainer;
                tail->nextContainer = container;
            }
        }
    }
}

void peEmitterNode::Release()
{
    peContainer *next;
    for (peContainer *container = children; container; container = next) {
        next = container->nextContainer;
        container->Stop();
        delete container;
    }
    children = 0;
}

peEmitterContainer::~peEmitterContainer()
{
    peContainer *child;
    peContainer *nextChild;
    peEmitterNode *node;
    peEmitterNode *next;

    node = (peEmitterNode *)freeNodes;
    while (node) {
        next = (peEmitterNode *)node->next;
        child = node->children;
        while (child) {
            nextChild = child->nextContainer;
            child->Stop();
            delete child;
            child = nextChild;
        }
        node->children = 0;
        node = next;
    }

    g_particle_debug_stats.containerBytes -= sizeof(peEmitterContainer);
    g_particle_debug_stats.totalBytes -= sizeof(peEmitterContainer);
}

void peEmitterContainer::PreUpdate()
{
    _particleData *child = mEmitter->mChildren;
    while (child)
        child = child->mNext;
}

u16 peEmitterContainer::Emit(u16 count)
{
    if (!started)
        return 0;

    u16 remaining;
    _EmitParams *params;
    peEmitterNode *next;
    u16 emitted;
    peEmitterNode *node;

    remaining = CreateNodes(count);
    emitted = remaining;
    params = g_emitParams;
    node = (peEmitterNode *)nodes;
    while (remaining) {
        next = (peEmitterNode *)node->next;
        node->Start(emitter, mEmitter, params);
        node = next;
        --remaining;
        ++params;
    }
    return emitted;
}

void peEmitterContainer::Update(f32 delta)
{
    peContainer *child;
    peEmitterNode *node;
    peEmitterNode *next;

    if (!started)
        return;

    node = (peEmitterNode *)nodes;
    while (node) {
        next = (peEmitterNode *)node->next;
        if (!node->Update(emitter, mEmitter, delta)) {
            child = node->children;
            while (child) {
                child->Stop();
                child = child->nextContainer;
            }
            RemoveNode(node);
        }
        node = next;
    }

    if (started && (mEmitter->flags & PE_EMITTER_SORT_CHILDREN_BY_DEPTH) && nodeCount >= 2) {
        node = (peEmitterNode *)nodes->next;
        while (node) {
            f32 depth = node->mCameraDepth;
            peEmitterNode *before = node;
            peEmitterNode *previous = (peEmitterNode *)node->previous;
            while (previous && previous->mCameraDepth < depth) {
                before = previous;
                previous = (peEmitterNode *)previous->previous;
            }
            next = (peEmitterNode *)node->next;
            if (before != node)
                MoveNode(node, before);
            node = next;
        }
    }
}

bool peEmitterContainer::DoRender()
{
    if (!started)
        return false;

    u32 flags = mEmitter->flags;
    if (!nodeCount)
        return false;
    if (flags & PE_EMITTER_RENDER_HIDDEN)
        return false;

    switch (g_render_pass) {
    case GSGFX_LAYER_PASS_OPAQUE:
        if (!(flags & PE_EMITTER_RENDER_OPAQUE))
            return false;
        break;
    case GSGFX_LAYER_PASS_XLU:
        if (!(flags & PE_EMITTER_RENDER_TRANSLUCENT))
            return false;
        break;
    default:
        return false;
    }

    switch (g_current_layer->mLayerID) {
    case 0:
        if (flags & PE_EMITTER_RENDER_LAYER_0)
            return true;
        break;
    case 2:
        if (flags & PE_EMITTER_RENDER_LAYER_2)
            return true;
        if (!(flags & PE_EMITTER_RENDER_LAYER_MASK))
            return true;
        break;
    case 4:
        if (flags & PE_EMITTER_RENDER_LAYER_4)
            return true;
        break;
    case 1:
        if (flags & PE_EMITTER_RENDER_LAYER_1)
            return true;
        break;
    case 3:
        if (flags & PE_EMITTER_RENDER_LAYER_3)
            return true;
        break;
    case 5:
        if (flags & PE_EMITTER_RENDER_LAYER_5)
            return true;
        break;
    default:
        return false;
    }
    return false;
}

void peEmitterContainer::Render()
{
    if (started && DoRender()) {
        peEmitterNode *node = (peEmitterNode *)nodes;
        while (node) {
            RenderEmitter(node);
            node = (peEmitterNode *)node->next;
        }
    }
}

void peEmitterContainer::Stop()
{
    if (!started)
        return;

    PreUpdate();
    started = false;

    peContainer *child;
    peEmitterNode *next;
    peEmitterNode *node = (peEmitterNode *)nodes;
    while (node) {
        next = (peEmitterNode *)node->next;
        child = node->children;
        while (child) {
            child->Stop();
            child = child->nextContainer;
        }
        node = next;
    }

    RemoveAll();
}

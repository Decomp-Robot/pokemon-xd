#include <math.h>
#include <peModel.h>
#include <peNodeMath.h>
#include <particleEngine.h>
#include <GSshape.h>
#include <GScamera.h>
#include <GSlight.h>
#include <GSbound.h>
#include <hsd/jobj.h>
#include <hsd/cobj.h>
#include <hsd/state.h>
#include <dolphin/gx/GXTransform.h>
#include <hsdRandom.h>


static u32 _primitive_instance_memory;

inline void UpdatePrimitiveMemory()
{
    g_particle_debug_stats.shapeBytes -= _primitive_instance_memory;
    g_particle_debug_stats.totalBytes -= _primitive_instance_memory;
    _primitive_instance_memory = shapeGetInstancedMemorySizeAll();
    g_particle_debug_stats.shapeBytes += _primitive_instance_memory;
    g_particle_debug_stats.totalBytes += _primitive_instance_memory;
}

void peModelContainer::Stop()
{
    if (!started)
        return;
    PreUpdate();
    started = false;
    if (mModel->mFlags & PE_MODEL_SHAPE_RESOURCE) {
        peModelNode *node = (peModelNode *)nodes;
        peModelNode *next;
        for (; node; node = next) {
            next = (peModelNode *)node->next;
            if (mModel->mFlags & PE_MODEL_SHAPE_RESOURCE) {
                shapeFreePrimitiveInstance(mModel->mShapeHandle - 1, node->mPrimitiveInstance);
                UpdatePrimitiveMemory();
            }
        }
    }
    RemoveAll();
}

inline bool peModelContainer::RenderForLayer()
{
    if (!started)
        return false;
    u32 flags = mModel->mFlags;
    if (!nodeCount)
        return false;
    if (g_render_pass != GSGFX_LAYER_PASS_XLU)
        return false;
    switch (g_current_layer->mLayerID) {
    case GSGFX_LAYER_DEFAULT:
        if (flags & PE_MODEL_SECONDARY_LAYER)
            return false;
        break;
    case GSGFX_LAYER_SECONDARY:
        if (!(flags & PE_MODEL_SECONDARY_LAYER))
            return false;
        break;
    default:
        return false;
    }
    return true;
}

inline void peModelNode::Render(_peModel *model)
{
    u16 shape;
    u32 flags;
    GSmtx rotation, matrix;
    PSMTXQuat(rotation.sdk(), SDKQuaternion(mWorldTransform.rotation));
    PSMTXScale(matrix.sdk(), mWorldTransform.scale.x, mWorldTransform.scale.y, mWorldTransform.scale.z);
    PSMTXConcat(rotation.sdk(), matrix.sdk(), matrix.sdk());
    PSMTXTransApply(matrix.sdk(), matrix.sdk(), mWorldTransform.position.x, mWorldTransform.position.y, mWorldTransform.position.z);
    if (model->mFlags & PE_MODEL_SHAPE_RESOURCE) {
        shape = model->mShapeHandle - 1;
        shapeUpdate(shape, mPrimitiveInstance, life - remainingLife);
        g_current_layer->mMatrixStack->pushMult(&matrix);
        shapeRender(shape, mPrimitiveInstance, g_current_layer->mLayerID);
        g_current_layer->mMatrixStack->pop();
    } else {
        HSD_JObj *joint = model->mJoint;
        if (joint) {
            switch (g_render_pass) {
            case GSGFX_LAYER_PASS_OPAQUE: flags = HSD_TRSP_OPA; break;
            case GSGFX_LAYER_PASS_XLU: flags = HSD_TRSP_XLU | HSD_TRSP_TEXEDGE; break;
            default: return;
            }
            HSD_JObjSetMatrix(joint, (f32 (*)[4])&matrix);
            if (model->mAnimationLength > 0.00001f) {
                f32 frame = (life - remainingLife) * GSgfxVideoGetVsyncRate();
                HSD_JObjReqAnimAll(joint, fmod(frame, model->mAnimationLength));
                HSD_JObjAnimAll(joint);
                HSD_JObjDispAll(joint, 0, flags, 0);
            }
        }
    }
}

void peModelContainer::Render()
{
    if (!started)
        return;
    if (!RenderForLayer())
        return;
    if (!(mModel->mFlags & PE_MODEL_SHAPE_RESOURCE)) {
        GScamera *camera = GScameraGetActiveCamera();
        if (!camera)
            return;
        GXSetClipMode(GX_CLIP_ENABLE);
        HSD_StateInvalidate(0xFFFFFFFF);
        HSD_CObjSetCurrent(camera->mCObj);
        GSlightSetupLights(camera->mCObj);
    }
    for (peModelNode *node = (peModelNode *)nodes; node; node = (peModelNode *)node->next) {
        node->Render(mModel);
    }
    if (!(mModel->mFlags & PE_MODEL_SHAPE_RESOURCE))
        g_current_layer->InvalidateState();
    particleEngineSetDefaultRenderState(true);
}

void peModelContainer::Update(f32 delta)
{
    if (!started)
        return;
    peModelNode *node = (peModelNode *)nodes;
    peModelNode *next;
    for (; node; node = next) {
        next = (peModelNode *)node->next;
        if (!node->Update(emitter, mModel, delta)) {
            if (mModel->mFlags & PE_MODEL_SHAPE_RESOURCE) {
                shapeFreePrimitiveInstance(mModel->mShapeHandle - 1, node->mPrimitiveInstance);
                UpdatePrimitiveMemory();
            }
            RemoveNode(node);
        }
    }
    if (started && (mModel->mFlags & PE_MODEL_DEPTH_SORT) && nodeCount >= 2) {
        node = (peModelNode *)nodes->next;
        for (; node; node = next) {
            f32 depth = node->mDepth;
            peModelNode *before = node;
            peModelNode *previous = (peModelNode *)node->previous;
            while (previous && previous->mDepth < depth) {
                before = previous;
                previous = (peModelNode *)previous->previous;
            }
            next = (peModelNode *)node->next;
            if (before != node)
                MoveNode(node, before);
        }
    }
    if (g_currentBound && nodes)
        GSboundAlwaysInView(g_currentBound, true);
}

u16 peModelContainer::Emit(u16 count)
{
    if (!started)
        return 0;
    u16 remaining = CreateNodes(count);
    _EmitParams *params = g_emitParams;
    peModelNode *node = (peModelNode *)nodes;
    peModelNode *next;
    u16 emitted = remaining;
    for (; remaining; --remaining, ++params) {
        next = (peModelNode *)node->next;
        node->Start(emitter, mModel, params);
        node = next;
    }
    return emitted;
}

peModelContainer::~peModelContainer()
{
    g_particle_debug_stats.containerBytes -= sizeof(peModelContainer);
    g_particle_debug_stats.totalBytes -= sizeof(peModelContainer);
}

peModelContainer::peModelContainer(peEmitterNode *parent, _peModel *model, u16 capacity)
    : peContainer(parent, sizeof(peModelNode), capacity), mModel(model)
{
    g_particle_debug_stats.containerBytes += sizeof(peModelContainer);
    g_particle_debug_stats.totalBytes += sizeof(peModelContainer);
}

bool peModelNode::Update(peEmitterNode *parent, _peModel *model, f32 delta)
{
    u32 flags = model->mFlags;
    if (!peNode::Update(parent, delta))
        return false;
    if ((flags & PE_MODEL_ANIMATION_LIFETIME) && life - remainingLife >= model->mAnimationLength)
        return false;
    f32 time = 1.0f - remainingLife / life;
    GSvec workVector;
    f32 reflected[4];
    if (parent->emitter->flags & PE_EMITTER_ALIGN_TO_VELOCITY) {
        GSvec direction = velocity;
        if (parent->emitter->flags & PE_EMITTER_LOCAL_VELOCITY)
            Rotate(direction, transform->rotation);
        workVector.x = atan2(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
        workVector.y = atan2(direction.x, direction.z);
        workVector.z = 0.0f;
        EulerToQuat(&workVector, mRotation);
    } else if (flags & PE_MODEL_ROTATION_SPLINE) {
        workVector.x = model->mRotation.x * peParticleNode::SplineValue(model->mRotationSplines[0], time);
        workVector.y = model->mRotation.y * peParticleNode::SplineValue(model->mRotationSplines[1], time);
        workVector.z = model->mRotation.z * peParticleNode::SplineValue(model->mRotationSplines[2], time);
        EulerToQuat(&workVector, mRotation);
    } else {
        workVector = mRotationVelocity * delta;
        f32 increment[4];
        EulerToQuat(&workVector, increment);
        PSQUATMultiply(SDKQuaternion(mRotation), SDKQuaternion(increment), SDKQuaternion(mRotation));
    }
    reflected[0] = mReflection & PE_MODEL_REFLECT_X ? -mRotation[0] : mRotation[0];
    if (mReflection & PE_MODEL_REFLECT_Y)
        reflected[1] = -mRotation[1];
    else
        reflected[1] = mRotation[1];
    if (mReflection & PE_MODEL_REFLECT_Z)
        reflected[2] = -mRotation[2];
    else
        reflected[2] = mRotation[2];
    reflected[3] = mRotation[3];
    workVector.x = mScale.x * peParticleNode::SplineValue(model->mScaleSplines[0], time);
    workVector.y = mScale.y * peParticleNode::SplineValue(model->mScaleSplines[1], time);
    workVector.z = mScale.z * peParticleNode::SplineValue(model->mScaleSplines[2], time);
    peNodeTransform local;
    local.position = position;
    peQuaternion combined = Multiply(mInitialRotation, reflected);
    local.rotation[0] = combined.x;
    local.rotation[1] = combined.y;
    local.rotation[2] = combined.z;
    local.rotation[3] = combined.w;
    local.scale = workVector;
    peNodeTransformsConcat(&mWorldTransform, transform, &local, false);
    mDepth = Dot(g_CameraLookDir, mWorldTransform.position - g_CameraPosition);
    return true;
}

void peModelNode::Start(peEmitterNode *parent, _peModel *model, _EmitParams *params)
{
    u32 flags = model->mFlags;
    peNode::Start(parent, params);
    mReflection = 0;
    mScale = model->mScaleRandom * HSD_Randf();
    mScale += model->mScale;
    mInitialRotation[0] = 0.0f;
    mInitialRotation[1] = 0.0f;
    mInitialRotation[2] = 0.0f;
    mInitialRotation[3] = 1.0f;
    mWorldTransform.position = GSvec(0.0f, 0.0f, 0.0f);
    mWorldTransform.rotation[0] = 0.0f;
    mWorldTransform.rotation[1] = 0.0f;
    mWorldTransform.rotation[2] = 0.0f;
    mWorldTransform.rotation[3] = 1.0f;
    mWorldTransform.scale = unitScaleVector;
    if (!(parent->emitter->flags & PE_EMITTER_ALIGN_TO_VELOCITY)) {
        EulerToQuat(&model->mRotation, mRotation);
        if (flags & PE_MODEL_RANDOM_ORIENTATION_MASK) {
            GSvec rotation;
            if (flags & PE_MODEL_RANDOM_ROTATION_X) {
                f32 random = HSD_Randf();
                rotation.x = 6.2831855f * random;
            } else {
                rotation.x = 0.0f;
            }
            if (flags & PE_MODEL_RANDOM_ROTATION_Y) {
                f32 random = HSD_Randf();
                rotation.y = 6.2831855f * random;
            } else {
                rotation.y = 0.0f;
            }
            if (flags & PE_MODEL_RANDOM_ROTATION_Z) {
                f32 random = HSD_Randf();
                rotation.z = 6.2831855f * random;
            } else {
                rotation.z = 0.0f;
            }
            if ((flags & PE_MODEL_RANDOM_REFLECT_X) && HSD_Randf() < 0.5f)
                mReflection |= PE_MODEL_REFLECT_X;
            if ((flags & PE_MODEL_RANDOM_REFLECT_Y) && HSD_Randf() < 0.5f)
                mReflection |= PE_MODEL_REFLECT_Y;
            if ((flags & PE_MODEL_RANDOM_REFLECT_Z) && HSD_Randf() < 0.5f)
                mReflection |= PE_MODEL_REFLECT_Z;
            EulerToQuat(&rotation, mInitialRotation);
        }
    }
    mRotationVelocity = model->mRotationVelocity;
    if (flags & PE_MODEL_RANDOM_VELOCITY_X)
        mRotationVelocity.x *= HSD_Randf();
    if (flags & PE_MODEL_RANDOM_VELOCITY_Y)
        mRotationVelocity.y *= HSD_Randf();
    if (flags & PE_MODEL_RANDOM_VELOCITY_Z)
        mRotationVelocity.z *= HSD_Randf();
    if (model->mFlags & PE_MODEL_SHAPE_RESOURCE) {
        mPrimitiveInstance = shapeCreatePrimitiveInstance(model->mShapeHandle - 1);
        UpdatePrimitiveMemory();
    }
}

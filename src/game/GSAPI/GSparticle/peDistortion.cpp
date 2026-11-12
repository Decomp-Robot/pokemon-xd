#include <math.h>
#include <GSbound.h>
#include <peNodeMath.h>
#include <GSAPI/GSsysdolphin_baselib/baselib/quatlib.h>
#include <dolphin/os/OSFastCast.h>
#include <particleEngine.h>
#include <peDistortion.h>
#include <hsdRandom.h>

extern "C" void DCFlushRange(void *data, u32 size);

namespace {
enum peDistortionVertexAttribute {
    PE_DISTORTION_POSITION = 9,
    PE_DISTORTION_COLOR = 11,
    PE_DISTORTION_TEXCOORD = 13,
    PE_DISTORTION_INDEX_16 = 3,
    PE_DISTORTION_VERTEX_WORD_BYTES = 4,
};
}

void peDistortionContainer::Stop()
{
    if (!started)
        return;

    PreUpdate();
    started = false;
    peDistortionNode *node = (peDistortionNode *)nodes;
    u32 flags = emitter->emitter->flags;
    if (flags & PE_DISTORTION_ENABLE_LAYER_1)
        SetLayerVisibility((GSgfxLayerID)1, false);
    if (flags & PE_DISTORTION_ENABLE_LAYER_3)
        SetLayerVisibility((GSgfxLayerID)3, false);
    if (flags & PE_DISTORTION_ENABLE_LAYER_5)
        SetLayerVisibility((GSgfxLayerID)5, false);

    for (; node; node = (peDistortionNode *)node->next)
        node->Stop(&mDisplayLists[mBufferIndex]);

    RemoveAll();
    StopSwap();
}

void peDistortionContainer::Render()
{
    if (!started)
        return;
    if (!RenderForLayer())
        return;

    ResolveMultipleSwap();
    u32 flags = mDistortion->mFlags;
    if (flags & PE_DISTORTION_BILLBOARD)
        g_current_layer->cullMode(GSGFX_CULL_BACK);
    else
        g_current_layer->cullMode(GSGFX_CULL_NONE);
    g_current_layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    g_current_layer->pixelMode(true, false, false);
    g_current_layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0,
                                  GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);

    particleEngineSetupBlendMode(0);
    particleEngineSetupTevStages(0, false, false);

    u32 magFilter = PE_TEXTURE_NEAREST;
    if (flags & PE_DISTORTION_MAG_LINEAR)
        magFilter = PE_TEXTURE_LINEAR;
    u32 wrapS = flags & PE_DISTORTION_WRAP_S_MASK;
    u32 wrapT = (flags >> PE_DISTORTION_WRAP_T_SHIFT) & PE_DISTORTION_WRAP_T_MASK;
    particleEngineSetupTexture(0, mDistortion->mTexture, wrapS, wrapT,
                               flags & PE_DISTORTION_MIN_LINEAR ? PE_TEXTURE_LINEAR : PE_TEXTURE_NEAREST, magFilter);

    char *renderData = mRenderData[mBufferIndex];
    GSgfxDL *displayList = &mDisplayLists[mBufferIndex];
    GSgfxVF *format = displayList->VertexFormat();
    GSgfxVFSetArray(format, PE_DISTORTION_POSITION, PE_DISTORTION_INDEX_16,
                    renderData, PE_DISTORTION_VERTEX_WORD_BYTES, mRenderDataSize);
    GSgfxVFSetArray(format, PE_DISTORTION_COLOR, PE_DISTORTION_INDEX_16,
                    &((peParticleRenderData *)renderData)->mColor,
                    PE_DISTORTION_VERTEX_WORD_BYTES, mRenderDataSize);
    GSgfxVFSetArray(format, PE_DISTORTION_TEXCOORD, PE_DISTORTION_INDEX_16,
                    mTexCoords, PE_DISTORTION_VERTEX_WORD_BYTES, sizeof(mTexCoords));
    g_current_layer->dlDraw(&mDisplayLists[mBufferIndex]);
    MarkForSwap();
}

void peDistortionContainer::Update(f32 delta)
{
    if (!started)
        return;

    peDistortionNode *node = (peDistortionNode *)nodes;
    peDistortionNode *next;
    for (; node; node = next) {
        next = (peDistortionNode *)node->next;
        if (!node->Update(emitter, mDistortion, &mDisplayLists[mBufferIndex], mRenderData[mBufferIndex], delta)) {
            node->Stop(&mDisplayLists[mBufferIndex]);
            RemoveNode(node);
        }
    }
}

u16 peDistortionContainer::Emit(u16 count)
{
    if (!started)
        return 0;

    u16 remaining = CreateNodes(count);
    _EmitParams *params = g_emitParams;
    peDistortionNode *node = (peDistortionNode *)nodes;
    peDistortionNode *next;
    u16 emitted = remaining;
    for (; remaining; --remaining, ++params) {
        next = (peDistortionNode *)node->next;
        node->Start(emitter, mDistortion, &mDisplayLists[mBufferIndex], params);
        node = next;
    }
    return emitted;
}

void peDistortionContainer::Start()
{
    if (started)
        return;

    started = true;
    u32 flags = emitter->emitter->flags;
    InitSwap();
    if (flags & PE_DISTORTION_ENABLE_LAYER_1)
        SetLayerVisibility((GSgfxLayerID)1, true);
    if (flags & PE_DISTORTION_ENABLE_LAYER_3)
        SetLayerVisibility((GSgfxLayerID)3, true);
    if (flags & PE_DISTORTION_ENABLE_LAYER_5)
        SetLayerVisibility((GSgfxLayerID)5, true);
}

peDistortionContainer::~peDistortionContainer()
{
    g_particle_debug_stats.containerBytes -= sizeof(*this);
    g_particle_debug_stats.totalBytes -= sizeof(*this);
}

peDistortionContainer::peDistortionContainer(peEmitterNode *parent,
                                             _peDistortion *distortion,
                                             u16 maxNodes)
    : peRenderContainer(parent, sizeof(peDistortionNode), maxNodes,
                        &g_vertex_format, GSGFX_QUADS, PE_PARTICLE_CORNERS,
                        sizeof(peParticleRenderData),
                        (distortion->mFlags & PE_DISTORTION_DOUBLE_BUFFER) != 0)
{
    mDistortion = distortion;
    _peDistortion *parameters = mDistortion;
    f32 *texCoords = mTexCoords;
    texCoords[0] = parameters->mTexCoordRight;
    texCoords[1] = parameters->mTexCoordBottom;
    texCoords[2] = parameters->mTexCoordLeft;
    texCoords[3] = parameters->mTexCoordBottom;
    texCoords[4] = parameters->mTexCoordLeft;
    texCoords[5] = parameters->mTexCoordTop;
    texCoords[6] = parameters->mTexCoordRight;
    texCoords[7] = parameters->mTexCoordTop;
    DCFlushRange(texCoords, sizeof(mTexCoords));

    peDistortionNode *node = (peDistortionNode *)freeNodes;
    u16 index = 0;
    for (; node;
         node = (peDistortionNode *)node->next)
        node->mRenderIndex = index++;

    g_particle_debug_stats.containerBytes += sizeof(*this);
    g_particle_debug_stats.totalBytes += sizeof(*this);
}

static inline u8 FloatToUnsigned8(f32 value)
{
    u8 result;
    OSf32tou8(&value, &result);
    return result;
}

void peDistortionNode::UpdateRenderData(peEmitterNode *parent,
                                       _peDistortion *distortion,
                                       char *renderData,
                                       f32 time)
{
    u32 flags = distortion->mFlags;
    GSvec normal, vertical, corner, offset, horizontal, worldPosition;
    peQuaternion orientation;
    GSvec euler;
    peQuaternion rotationBeforeScale, rotationAfterScale, inverse, verticalInverse;
    worldPosition = Product(position, transform->scale);
    Rotate(worldPosition, transform->rotation);
    worldPosition += transform->position;

    if (flags & PE_DISTORTION_BILLBOARD) {
        if (flags & PE_DISTORTION_CAMERA_FACING) {
            horizontal = g_CameraPosition - worldPosition;
            PSVECNormalize(SDKVector(&horizontal), SDKVector(&horizontal));
            normal = Cross(horizontal, g_CameraUpDir);
            PSVECNormalize(SDKVector(&normal), SDKVector(&normal));
            Cross(horizontal, normal);
        } else {
            vertical = g_CameraUpDir;
            normal = g_CameraSideDir;
        }
    } else {
        normal = xAxisVector;
        vertical = yAxisVector;

        if (parent->emitter->flags & PE_EMITTER_ALIGN_TO_VELOCITY) {
            GSvec localVelocity = velocity;
            f32 vx = localVelocity.x, vz = localVelocity.z, vy = localVelocity.y;
            euler.x = atan2(-vy, sqrtf(vx * vx + vz * vz));
            euler.y = atan2(vx, vz);
            euler.z = 0.0f;
            EulerToQuat(&euler, &orientation.x);
            Rotate(normal, &orientation.x);
            Rotate(vertical, &orientation.x);
        }

        Rotate(normal, transform->rotation);
        Rotate(vertical, transform->rotation);
    }

    horizontal = Cross(vertical, normal);

    f32 angle = 0.0f;
    if (mOrientationFlags & PE_NODE_REVERSE_ROTATION)
        angle -= mInitialRotation + mRotation;
    else
        angle += mInitialRotation + mRotation;

    u32 beforeScale = flags & PE_DISTORTION_ROTATE_BEFORE_SCALE;
    if (beforeScale && Magnitude(angle) > 0.00001f) {
        C_QUATRotAxisRad(SDKQuaternion(&rotationBeforeScale.x), SDKVector(&horizontal), angle);
        Rotate(vertical, &rotationBeforeScale.x);
        Rotate(normal, &rotationBeforeScale.x);
    }

    inverse = Inverse(transform->rotation);
    corner = Rotated(normal, &inverse.x);
    ApplyScale(corner, transform->scale);
    Rotate(corner, transform->rotation);
    normal *= PSVECDotProduct(SDKVector(&normal), SDKVector(&corner));

    verticalInverse = Inverse(transform->rotation);
    corner = Rotated(vertical, &verticalInverse.x);
    ApplyScale(corner, transform->scale);
    Rotate(corner, transform->rotation);
    vertical *= PSVECDotProduct(SDKVector(&vertical), SDKVector(&corner));

    normal *= mWidth * peParticleNode::SplineValue(distortion->mWidthSpline, time);
    vertical *= mHeight * peParticleNode::SplineValue(distortion->mHeightSpline, time);

    if (!beforeScale && Magnitude(angle) > 0.00001f) {
        C_QUATRotAxisRad(SDKQuaternion(&rotationAfterScale.x), SDKVector(&horizontal), angle);
        Rotate(vertical, &rotationAfterScale.x);
        Rotate(normal, &rotationAfterScale.x);
    }

    bool useOffset = (flags & PE_DISTORTION_USE_OFFSET) != 0;
    if (useOffset) {
        offset = normal * distortion->mParameters.mOffsetScaleX;
        ApplyScale(offset, vertical * distortion->mParameters.mOffsetScaleY);
    }

    peParticleRenderData *data =
        reinterpret_cast<peParticleRenderData *>(renderData) + mRenderIndex;

    corner = -normal;
    corner -= vertical;
    if (useOffset)
        corner += offset;
    corner += worldPosition;
    data->mCorners[0] = corner;
    if (g_currentBound)
        GSboundAddPoint(g_currentBound, corner.x, corner.y, corner.z);

    corner = normal - vertical;
    if (useOffset)
        corner += offset;
    corner += worldPosition;
    data->mCorners[1] = corner;
    if (g_currentBound)
        GSboundAddPoint(g_currentBound, corner.x, corner.y, corner.z);

    corner = normal + vertical;
    if (useOffset)
        corner += offset;
    corner += worldPosition;
    data->mCorners[2] = corner;
    if (g_currentBound)
        GSboundAddPoint(g_currentBound, corner.x, corner.y, corner.z);

    corner = vertical - normal;
    if (useOffset)
        corner += offset;
    corner += worldPosition;
    data->mCorners[3] = corner;
    if (g_currentBound)
        GSboundAddPoint(g_currentBound, corner.x, corner.y, corner.z);

    GScolor color;
    color.channels.r = color.channels.g = color.channels.b = 255;
    color.channels.a = FloatToUnsigned8(255.0f * parent->colorScale[3] *
        peParticleNode::SplineValue(distortion->mAlphaSpline, time));
    data->mColor = color.rgba;
}

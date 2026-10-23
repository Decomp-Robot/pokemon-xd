#include <particleEngine.h>
#include <peParticle.h>
#include <GSAPI/GSsysdolphin_baselib/baselib/quatlib.h>
#include <dolphin/os/OSFastCast.h>

extern GSgfxVF g_vertex_format;
extern GSgfxVF g_ext_vertex_format;

extern "C" {
void DCFlushRange(void *data, u32 size);
}

enum peVertexArrayAttribute {
    PE_VERTEX_POSITION = 9,
    PE_VERTEX_COLOR = 11,
    PE_VERTEX_SECOND_COLOR = 12,
    PE_VERTEX_TEXCOORD = 13,
    PE_VERTEX_INDEX_16 = 3,
    PE_VERTEX_ELEMENT_BYTES = 4,
};

inline bool peParticleContainer::RenderForLayer()
{
    if (!started)
        return false;
    if (!nodeCount)
        return false;
    if (g_render_pass != GSGFX_LAYER_PASS_XLU)
        return false;
    u32 flags = mParticle->mFlags;
    switch (g_current_layer->mLayerID) {
    case 0:
        if (flags & PE_PARTICLE_LAYER_0)
            return true;
        break;
    case 2:
        if (flags & PE_PARTICLE_LAYER_2)
            return true;
        if (!(flags & PE_PARTICLE_LAYER_MASK))
            return true;
        break;
    case 4:
        if (flags & PE_PARTICLE_LAYER_4)
            return true;
        break;
    case 1:
        if (flags & PE_PARTICLE_LAYER_1)
            return true;
        break;
    case 3:
        if (flags & PE_PARTICLE_LAYER_3)
            return true;
        break;
    case 5:
        if (flags & PE_PARTICLE_LAYER_5)
            return true;
        break;
    default:
        return false;
    }
    return false;
}

inline bool ParticleDepthOutOfOrder(peParticleNode *first, peParticleNode *second)
{
    if (first->mDepth == second->mDepth)
        return false;
    if (first->mDisplayIndex < second->mDisplayIndex)
        return first->mDepth < second->mDepth;
    return first->mDepth > second->mDepth;
}

inline void peParticleNode::SetDisplayIndex(GSgfxDL *displayList, u16 displayIndex,
                                            u16 renderFloats, bool extended)
{
    mDisplayIndex = displayIndex;
    u16 corner;
    u16 renderIndex = mRenderIndex * renderFloats;
    u16 *indices = (u16 *)displayList->VertexData(displayIndex * PE_PARTICLE_CORNERS);
    corner = 0;
    for (; corner < PE_PARTICLE_CORNERS; ++corner) {
        *indices++ = renderIndex + corner * PE_PARTICLE_POSITION_COMPONENTS;
        *indices++ = renderIndex;
        if (extended)
            *indices++ = renderIndex;
        *indices++ = corner * PE_PARTICLE_TEXCOORD_COMPONENTS;
    }
}

void peParticleContainer::DepthSort()
{
    if (started && (mParticle->mFlags & PE_PARTICLE_DEPTH_SORT) &&
        nodeCount >= 2 && mSortedNodes) {
        peParticleNode *node;
        int index;
        int position;
        bool changed;
        int previous;
        changed = false;
        for (index = 1; index < capacity; ++index) {
            node = mSortedNodes[index];
            if (!node->mActive)
                continue;
            position = index;
            while ((previous = PreviousActive(position)) >= 0) {
                peParticleNode *before = mSortedNodes[previous];
                if (!ParticleDepthOutOfOrder(before, node))
                    break;
                mSortedNodes[position] = before;
                position = previous;
                changed = true;
            }
            mSortedNodes[position] = node;
        }
        if (!changed)
            return;
        u32 flags;
        GSgfxDL *displayList;
        bool extended;
        flags = mParticle->mFlags;
        displayList = &mDisplayLists[mBufferIndex];
        extended = (flags & PE_PARTICLE_DUAL_COLOR) != 0;
        u16 renderFloats = PE_PARTICLE_RENDER_WORDS + (extended != 0);
        for (index = 0; index < capacity; ++index) {
            peParticleNode *node = mSortedNodes[index];
            if (!node->mActive || node->mDisplayIndex == (u16)index)
                continue;
            node->SetDisplayIndex(displayList, index, renderFloats, extended);
        }
        displayList->Invalidate();
        this->changed = true;
    }
}

void peParticleContainer::Stop()
{
    if (!started)
        return;
    PreUpdate();
    started = false;
    u32 flags = mParticle->mFlags;
    if (flags & PE_PARTICLE_LAYER_1)
        SetLayerVisibility((GSgfxLayerID)1, false);
    if (flags & PE_PARTICLE_LAYER_3)
        SetLayerVisibility((GSgfxLayerID)3, false);
    if (flags & PE_PARTICLE_LAYER_5)
        SetLayerVisibility((GSgfxLayerID)5, false);
    for (peParticleNode *node = (peParticleNode *)nodes; node;
         node = (peParticleNode *)node->next)
        node->Stop(mParticle, &mDisplayLists[mBufferIndex]);
    RemoveAll();
    StopSwap();
}

void peParticleContainer::Render()
{
    if (!started)
        return;
    if (!RenderForLayer())
        return;
    ResolveMultipleSwap();
    u32 flags = mParticle->mFlags;
    bool extended = (flags & PE_PARTICLE_DUAL_COLOR) != 0;
    if (flags & PE_PARTICLE_CULL_BACK)
        g_current_layer->cullMode(GSGFX_CULL_BACK);
    else
        g_current_layer->cullMode(GSGFX_CULL_NONE);
    particleEngineSetupBlendMode(flags & PE_PARTICLE_BLEND_MASK);
    char *renderData = mRenderData[mBufferIndex];
    GSgfxVF *format = mDisplayLists[mBufferIndex].VertexFormat();
    if (extended) {
        GSgfxVFSetArray(format, PE_VERTEX_POSITION, PE_VERTEX_INDEX_16, renderData, PE_VERTEX_ELEMENT_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, PE_VERTEX_COLOR, PE_VERTEX_INDEX_16, &((peParticleRenderData *)renderData)->mColor, PE_VERTEX_ELEMENT_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, PE_VERTEX_SECOND_COLOR, PE_VERTEX_INDEX_16, &((peParticleDualColorRenderData *)renderData)->mSecondColor, PE_VERTEX_ELEMENT_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, PE_VERTEX_TEXCOORD, PE_VERTEX_INDEX_16, mTexCoords, PE_VERTEX_ELEMENT_BYTES, sizeof(mTexCoords));
    } else {
        GSgfxVFSetArray(format, PE_VERTEX_POSITION, PE_VERTEX_INDEX_16, renderData, PE_VERTEX_ELEMENT_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, PE_VERTEX_COLOR, PE_VERTEX_INDEX_16, &((peParticleRenderData *)renderData)->mColor, PE_VERTEX_ELEMENT_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, PE_VERTEX_TEXCOORD, PE_VERTEX_INDEX_16, mTexCoords, PE_VERTEX_ELEMENT_BYTES, sizeof(mTexCoords));
    }
    if (flags & PE_PARTICLE_TEXTURE_ANIMATION) {
        particleEngineSetupTevStages(0, extended,
                                    (flags & PE_PARTICLE_TEV_MASK) == PE_PARTICLE_TEV_MASK);
        g_current_layer->mPrimitiveType = GSGFX_QUADS;
        g_current_layer->mVertexFormat = format;
        for (peParticleNode *node = (peParticleNode *)nodes; node;
             node = (peParticleNode *)node->next)
            node->Render(mParticle, &mDisplayLists[mBufferIndex], mRenderData[mBufferIndex]);
    } else {
        particleEngineSetupTevStages(0, extended, false);
        u32 magFilter = PE_TEXTURE_NEAREST;
        if (flags & PE_PARTICLE_MAG_LINEAR)
            magFilter = PE_TEXTURE_LINEAR;
        GStexture *texture = mParticle->mTexture;
        u32 wrapS = (flags >> 12) & 3;
        u32 wrapT = (flags >> 14) & 3;
        particleEngineSetupTexture(0, texture, wrapS, wrapT,
                                   flags & PE_PARTICLE_MIN_LINEAR ? PE_TEXTURE_LINEAR : PE_TEXTURE_NEAREST,
                                   magFilter);
        g_current_layer->dlDraw(&mDisplayLists[mBufferIndex]);
    }
    MarkForSwap();
}

void peParticleContainer::Update(f32 delta)
{
    if (!started)
        return;
    peParticleNode *node = (peParticleNode *)nodes;
    peParticleNode *next;
    for (; node; node = next) {
        next = (peParticleNode *)node->next;
        if (!node->Update(emitter, mParticle, mRenderData[mBufferIndex], delta)) {
            node->Stop(mParticle, &mDisplayLists[mBufferIndex]);
            RemoveNode(node);
        }
    }
    DepthSort();
}

u16 peParticleContainer::Emit(u16 count)
{
    if (!started)
        return 0;
    u16 remaining = CreateNodes(count);
    _EmitParams *params = g_emitParams;
    peParticleNode *next;
    u16 emitted = remaining;
    peParticleNode *node = (peParticleNode *)nodes;
    for (; remaining; --remaining, ++params) {
        next = (peParticleNode *)node->next;
        node->Start(emitter, mParticle, &mDisplayLists[mBufferIndex], params);
        node = next;
    }
    return emitted;
}

void peParticleContainer::Start()
{
    if (started)
        return;
    started = true;
    u32 flags = mParticle->mFlags;
    InitSwap();
    if (flags & PE_PARTICLE_LAYER_1)
        SetLayerVisibility((GSgfxLayerID)1, true);
    if (flags & PE_PARTICLE_LAYER_3)
        SetLayerVisibility((GSgfxLayerID)3, true);
    if (flags & PE_PARTICLE_LAYER_5)
        SetLayerVisibility((GSgfxLayerID)5, true);
}

peParticleContainer::~peParticleContainer()
{
    if (mSortedNodes)
        delete[] mSortedNodes;
    g_particle_debug_stats.containerBytes -= sizeof(*this);
    g_particle_debug_stats.totalBytes -= sizeof(*this);
}

peParticleContainer::peParticleContainer(peEmitterNode *parent, _peParticle *particle,
                                         u16 maxNodes)
    : peRenderContainer(parent, sizeof(peParticleNode), maxNodes,
                        particle->mFlags & PE_PARTICLE_DUAL_COLOR ?
                            &g_ext_vertex_format : &g_vertex_format,
                        GSGFX_QUADS, PE_PARTICLE_CORNERS,
                        particle->mFlags & PE_PARTICLE_DUAL_COLOR ?
                            sizeof(peParticleDualColorRenderData) : sizeof(peParticleRenderData),
                        (particle->mFlags & PE_PARTICLE_DOUBLE_BUFFER) != 0)
{
    mSortedNodes = 0;
    mParticle = particle;
    f32 *texCoords = mTexCoords;
    s16 leftFixed = mParticle->mTexCoordLeft;
    f32 left;
    OSs16tof32(&leftFixed, &left);
    left *= 1.0f / 1024.0f;
    s16 topFixed = mParticle->mTexCoordTop;
    f32 top;
    OSs16tof32(&topFixed, &top);
    top *= 1.0f / 1024.0f;
    s16 rightFixed = mParticle->mTexCoordRight;
    f32 right;
    OSs16tof32(&rightFixed, &right);
    right *= 1.0f / 1024.0f;
    s16 bottomFixed = mParticle->mTexCoordBottom;
    f32 bottom;
    OSs16tof32(&bottomFixed, &bottom);
    bottom *= 1.0f / 1024.0f;
    texCoords[0] = right;
    texCoords[1] = bottom;
    texCoords[2] = left;
    texCoords[3] = bottom;
    texCoords[4] = left;
    texCoords[5] = top;
    texCoords[6] = right;
    texCoords[7] = top;
    DCFlushRange(texCoords, sizeof(mTexCoords));
    if (mParticle->mFlags & PE_PARTICLE_DEPTH_SORT)
        mSortedNodes = new peParticleNode *[maxNodes];
    peParticleNode *node = (peParticleNode *)freeNodes;
    u16 index = 0;
    for (; node; node = (peParticleNode *)node->next, ++index) {
        node->mRenderIndex = index;
        node->mDisplayIndex = index;
        if (mSortedNodes)
            mSortedNodes[index] = node;
    }
    g_particle_debug_stats.containerBytes += sizeof(*this);
    g_particle_debug_stats.totalBytes += sizeof(*this);
}

#include <peNodeMath.h>
#include <math.h>
#include <dolphin/os/OSFastCast.h>

extern "C" {
f32 HSD_Randf();
double fmod(double, double);
double atan2(double, double);
}

extern GSvec xAxisVector, yAxisVector, zAxisVector;
extern GSvec g_CameraPosition, g_CameraLookDir, g_CameraUpDir, g_CameraSideDir;

static inline f32 Unsigned8ToFloat(u8 value) { f32 result; OSu8tof32(&value, &result); return result; }
static inline f32 Unsigned16ToFloat(u16 value) { f32 result; OSu16tof32(&value, &result); return result; }
static inline u8 FloatToUnsigned8(f32 value) { u8 result; OSf32tou8(&value, &result); return result; }
static inline u16 FloatToUnsigned16(f32 value) { u16 result; OSf32tou16(&value, &result); return result; }

static inline void SetBlendColor(GScolor color, u8 slot) {
    g_current_layer->tunitKColor(slot, color);
}
static inline peTextureFilter MinimumFilter(u32 flags)
{
    if (flags & PE_PARTICLE_MIN_LINEAR)
        return PE_TEXTURE_LINEAR;
    return PE_TEXTURE_NEAREST;
}
static inline void SetupTexture(u32 slot, GStexture *texture, u32 flags)
{
    u32 magFilter = PE_TEXTURE_NEAREST;
    if (flags & PE_PARTICLE_MAG_LINEAR)
        magFilter = PE_TEXTURE_LINEAR;
    u32 wrapS = (flags >> PE_PARTICLE_WRAP_S_SHIFT) & PE_PARTICLE_WRAP_MASK;
    u32 wrapT = (flags >> PE_PARTICLE_WRAP_T_SHIFT) & PE_PARTICLE_WRAP_MASK;
    particleEngineSetupTexture(slot, texture, wrapS, wrapT, MinimumFilter(flags), magFilter);
}

void peParticleNode::Render(_peParticle *particle, GSgfxDL *displayList,
                            char *renderData)
{
    u32 flags;
    peTextureList *textures;
    bool extended;
    u16 textureIndex;
    f32 fraction, frame;
    flags = particle->mFlags;
    textures = particle->mTextureList;
    textureIndex = mTextureIndex;
    extended = (flags & PE_PARTICLE_DUAL_COLOR) != 0;
    if (!renderData) return;
    if ((displayList->mFlags & GS_DISPLAY_LIST_ALLOCATED) != GS_DISPLAY_LIST_ALLOCATED) return;
    if (flags & PE_PARTICLE_ANIMATE_TEXTURE) {
        f32 count = Unsigned16ToFloat(textures->mCount);
        bool interpolate = (flags & PE_PARTICLE_INTERPOLATE_TEXTURE) != 0;
        f32 frames = count;
        if (interpolate) frames = count - 1.0f;
        f32 phase;
        if (flags & PE_PARTICLE_TEXTURE_TIME_SECONDS)
            phase = frames * (life - remainingLife) / particle->mTextureAnimationRate;
        else
            phase = particle->mTextureAnimationRate * (frames * (1.0f - remainingLife / life));
        frame = fmod(phase + Unsigned16ToFloat(textureIndex), frames);
        textureIndex = FloatToUnsigned16(frame) % (u16)textures->mCount;
        if (interpolate) {
            f32 textureFraction = frame;
            textureFraction -= Unsigned16ToFloat(textureIndex);
            fraction = 255.0f * textureFraction;
            GScolor blend;
            blend.channels.r = blend.channels.g = blend.channels.b = blend.channels.a = FloatToUnsigned8(fraction);
            SetBlendColor(blend, 0);
            if (extended) {
                peParticleDualColorRenderData *data = (peParticleDualColorRenderData *)renderData;
                data += mRenderIndex;
                u32 packed = data->mSecondColor;
                const u8 *components = (const u8 *)&packed;
                blend.channels.r = components[0];
                blend.channels.g = components[1];
                blend.channels.b = components[2];
                blend.channels.a = components[3];
                SetBlendColor(blend, 1);
            }
            SetupTexture(1, textures->mTextures[textureIndex + 1], flags);
        }
    }
    SetupTexture(0, textures->mTextures[textureIndex], flags);
    g_current_layer->beginSurface(PE_PARTICLE_CORNERS);
    u16 *indices = (u16 *)displayList->VertexData(mDisplayIndex * PE_PARTICLE_CORNERS);
    u16 corners = PE_PARTICLE_CORNERS;
    register volatile PPCWGPipe *fifo = &GXWGFifo;
    while (corners--) {
        fifo->u16 = *indices++;
        fifo->u16 = *indices++;
        if (extended) fifo->u16 = *indices++;
        fifo->u16 = *indices++;
    }
    g_current_layer->endSurface();
}

void peParticleNode::Start(peEmitterNode *parent, _peParticle *particle,
                           GSgfxDL *displayList, _EmitParams *params)
{
    u8 words;
    u16 *indices;
    u16 firstIndex;
    u16 corner;
    u32 flags = particle->mFlags;
    bool extended = (flags & PE_PARTICLE_DUAL_COLOR) != 0;
    if ((displayList->mFlags & GS_DISPLAY_LIST_ALLOCATED) != GS_DISPLAY_LIST_ALLOCATED) return;
    peNode::Start(parent, params);
    mOrientationFlags = 0;
    mActive = true;
    f32 random = HSD_Randf();
    mWidth = particle->mWidthRandom * random;
    mHeight = particle->mHeightRandom * random;
    mWidth += particle->mWidth;
    mHeight += particle->mHeight;
    if (flags & PE_PARTICLE_RANDOM_ROTATION)
        mInitialRotation = 6.2831855f * HSD_Randf();
    else
        mInitialRotation = 0.0f;
    if ((flags & PE_PARTICLE_RANDOM_REVERSE) && HSD_Randf() < 0.5f)
        mOrientationFlags |= PE_NODE_REVERSE_ROTATION;
    mRotation = particle->mRotation;
    mRotationVelocity = particle->mRotationVelocityRandom * HSD_Randf() + particle->mRotationVelocity;
    if ((flags & PE_PARTICLE_RANDOM_TEXTURE) == PE_PARTICLE_RANDOM_TEXTURE)
        mTextureIndex = FloatToUnsigned8(Unsigned16ToFloat(particle->mTextureList->mCount) * HSD_Randf());
    else
        mTextureIndex = 0;
    words = PE_PARTICLE_RENDER_WORDS + (extended != 0);
    firstIndex = mRenderIndex * words;
    indices = (u16 *)displayList->VertexData(mDisplayIndex * PE_PARTICLE_CORNERS);
    corner = 0;
    for (; corner < PE_PARTICLE_CORNERS; ++corner) {
        *indices++ = firstIndex + corner * PE_PARTICLE_POSITION_COMPONENTS;
        *indices++ = firstIndex;
        if (extended) *indices++ = firstIndex;
        *indices++ = corner * PE_PARTICLE_TEXCOORD_COMPONENTS;
    }
    displayList->Invalidate();
}

void peParticleNode::UpdateRenderData(peEmitterNode *parent, _peParticle *particle,
                                      char *renderData, const f32 time)
{
    u32 flags = particle->mFlags;
    bool extended = (flags & PE_PARTICLE_DUAL_COLOR) != 0;
    GSvec normal, vertical, horizontal, pivot, corner, worldPosition;
    peQuaternion rotation, orientation;
    GSvec euler;
    peQuaternion inverse, verticalInverse;
    GSvec offset, viewOffset;
    f32 angle;
    worldPosition = Product(position, transform->scale);
    Rotate(worldPosition, transform->rotation);
    worldPosition += transform->position;
    if (flags & PE_PARTICLE_BILLBOARD) {
        if (flags & PE_PARTICLE_CAMERA_FACING) {
            horizontal = g_CameraPosition - worldPosition;
            PSVECNormalize(SDKVector(&horizontal), SDKVector(&horizontal));
            normal = Cross(horizontal, g_CameraUpDir);
            PSVECNormalize(SDKVector(&normal), SDKVector(&normal));
            if (flags & PE_PARTICLE_UPRIGHT) {
                horizontal.y = 0.0f;
                PSVECNormalize(SDKVector(&horizontal), SDKVector(&horizontal));
                vertical.x = 0.0f; vertical.y = 1.0f; vertical.z = 0.0f;
            } else {
                vertical = Cross(normal, horizontal);
            }
        } else {
            if (flags & PE_PARTICLE_UPRIGHT)
                vertical = yAxisVector;
            else
                vertical = g_CameraUpDir;
            normal = g_CameraSideDir;
        }
        angle = particle->mOrientationY;
    } else {
        C_QUATRotAxisRad(SDKQuaternion(&rotation.x), SDKVector(&yAxisVector), particle->mOrientationY);
        C_QUATRotAxisRad(SDKQuaternion(&orientation.x), SDKVector(&zAxisVector), particle->mOrientationZ);
        PSQUATMultiply(SDKQuaternion(&orientation.x), SDKQuaternion(&rotation.x), SDKQuaternion(&orientation.x));
        normal = Rotated(xAxisVector, &orientation.x);
        vertical = Rotated(yAxisVector, &orientation.x);
        if (parent->emitter->flags & PE_EMITTER_ALIGN_TO_VELOCITY) {
            GSvec localVelocity = velocity;
            f32 vx = localVelocity.x, vz = localVelocity.z, vy = localVelocity.y;
            euler.x = atan2(-vy, sqrtf(vx * vx + vz * vz));
            euler.y = atan2(vx, vz);
            euler.z = 0.0f;
            EulerToQuat(&euler, &rotation.x);
            Rotate(normal, &rotation.x);
            Rotate(vertical, &rotation.x);
        }
        Rotate(normal, transform->rotation);
        Rotate(vertical, transform->rotation);
        angle = 0.0f;
    }
    horizontal = Cross(vertical, normal);
    if (mOrientationFlags & PE_NODE_REVERSE_ROTATION)
        angle -= mInitialRotation + mRotation;
    else
        angle += mInitialRotation + mRotation;
    u32 beforeScale = flags & PE_PARTICLE_ROTATE_BEFORE_SCALE;
    if (beforeScale && Magnitude(angle) > 0.00001f) {
        C_QUATRotAxisRad(SDKQuaternion(&rotation.x), SDKVector(&horizontal), angle);
        Rotate(vertical, &rotation.x);
        Rotate(normal, &rotation.x);
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
    normal *= mWidth * SplineValue(particle->mWidthSpline, time);
    vertical *= mHeight * SplineValue(particle->mHeightSpline, time);
    if (!beforeScale && Magnitude(angle) > 0.00001f) {
        C_QUATRotAxisRad(SDKQuaternion(&rotation.x), SDKVector(&horizontal), angle);
        Rotate(vertical, &rotation.x);
        Rotate(normal, &rotation.x);
    }
    bool offsetPivot = (flags & PE_PARTICLE_PIVOT) != 0;
    if (offsetPivot) {
        pivot = normal * particle->mPivotX;
        offset = vertical * particle->mPivotY;
        pivot += offset;
    }
    peParticleRenderData *data;
    if (extended)
        data = &((peParticleDualColorRenderData *)renderData)[mRenderIndex];
    else
        data = &((peParticleRenderData *)renderData)[mRenderIndex];
    mDepth = Dot(g_CameraLookDir, worldPosition - g_CameraPosition);
    corner = -normal;
    corner -= vertical;
    if (offsetPivot) corner += pivot;
    corner += worldPosition;
    data->mCorners[0] = corner;
    if (g_currentBound) GSboundAddPoint(g_currentBound, corner.x, corner.y, corner.z);
    corner = normal - vertical;
    if (offsetPivot) corner += pivot;
    corner += worldPosition;
    data->mCorners[1] = corner;
    if (g_currentBound) GSboundAddPoint(g_currentBound, corner.x, corner.y, corner.z);
    corner = normal + vertical;
    if (offsetPivot) corner += pivot;
    corner += worldPosition;
    data->mCorners[2] = corner;
    if (g_currentBound) GSboundAddPoint(g_currentBound, corner.x, corner.y, corner.z);
    corner = vertical - normal;
    if (offsetPivot) corner += pivot;
    corner += worldPosition;
    data->mCorners[3] = corner;
    if (g_currentBound) GSboundAddPoint(g_currentBound, corner.x, corner.y, corner.z);
    GScolor color;
    peColorSplineGetValue(&color, particle->mColorSpline, time);
    color.channels.r = FloatToUnsigned8(parent->colorScale[0] * Unsigned8ToFloat(color.channels.r));
    color.channels.g = FloatToUnsigned8(parent->colorScale[1] * Unsigned8ToFloat(color.channels.g));
    color.channels.b = FloatToUnsigned8(parent->colorScale[2] * Unsigned8ToFloat(color.channels.b));
    color.channels.a = FloatToUnsigned8((255.0f * parent->colorScale[3]) * SplineValue(particle->mAlphaSpline, time));
    data->mColor = color.rgba;
    if (extended) {
        peColorSplineGetValue(&color, particle->mSecondColorSpline, time);
        ((peParticleDualColorRenderData *)data)->mSecondColor = color.rgba;
    }
}

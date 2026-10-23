#include <peParticleContainer.h>
#include <dolphin/os/OSFastCast.h>

extern GSgfxVF g_vertex_format;
extern GSgfxVF g_ext_vertex_format;

extern "C" {
void DCFlushRange(void *data, u32 size);
void peSetLayerVisibility(GSgfxLayerID layer, bool visible);
void particleEngineSetupBlendMode(u32 mode);
void particleEngineSetupTevStages(u32 textureCount, bool extended, bool modulate);
void particleEngineSetupTexture(u32 slot, GStexture *texture, u32 wrapS, u32 wrapT,
                                u32 minFilter, u32 magFilter);
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
    if (!mStarted)
        return false;
    if (!mNodeCount)
        return false;
    if (peCurrentPass != GSGFX_LAYER_PASS_XLU)
        return false;
    u32 flags = mParticle->mFlags;
    switch (peCurrentLayer->mLayerID) {
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
    if (mStarted && (mParticle->mFlags & PE_PARTICLE_DEPTH_SORT) &&
        mNodeCount >= 2 && mSortedNodes) {
        peParticleNode *node;
        int index;
        int position;
        bool changed;
        int previous;
        changed = false;
        for (index = 1; index < mMaxNodes; ++index) {
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
        for (index = 0; index < mMaxNodes; ++index) {
            peParticleNode *node = mSortedNodes[index];
            if (!node->mActive || node->mDisplayIndex == (u16)index)
                continue;
            node->SetDisplayIndex(displayList, index, renderFloats, extended);
        }
        displayList->Invalidate();
        mChanged = true;
    }
}

void peParticleContainer::Stop()
{
    if (!mStarted)
        return;
    PreUpdate();
    mStarted = false;
    u32 flags = mParticle->mFlags;
    if (flags & PE_PARTICLE_LAYER_1)
        peSetLayerVisibility((GSgfxLayerID)1, false);
    if (flags & PE_PARTICLE_LAYER_3)
        peSetLayerVisibility((GSgfxLayerID)3, false);
    if (flags & PE_PARTICLE_LAYER_5)
        peSetLayerVisibility((GSgfxLayerID)5, false);
    for (peParticleNode *node = (peParticleNode *)mNodes; node;
         node = (peParticleNode *)node->mNext)
        node->Stop(mParticle, &mDisplayLists[mBufferIndex]);
    RemoveAll();
    StopSwap();
}

void peParticleContainer::Render()
{
    if (!mStarted)
        return;
    if (!RenderForLayer())
        return;
    ResolveMultipleSwap();
    u32 flags = mParticle->mFlags;
    bool extended = (flags & PE_PARTICLE_DUAL_COLOR) != 0;
    if (flags & PE_PARTICLE_CULL_BACK)
        ((GSgfxLayer *)peCurrentLayer)->cullMode(GSGFX_CULL_BACK);
    else
        ((GSgfxLayer *)peCurrentLayer)->cullMode(GSGFX_CULL_NONE);
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
        peCurrentLayer->mPrimitiveType = GSGFX_QUADS;
        ((GSgfxLayer *)peCurrentLayer)->mVertexFormat = format;
        for (peParticleNode *node = (peParticleNode *)mNodes; node;
             node = (peParticleNode *)node->mNext)
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
        ((GSgfxLayer *)peCurrentLayer)->dlDraw(&mDisplayLists[mBufferIndex]);
    }
    MarkForSwap();
}

void peParticleContainer::Update(f32 delta)
{
    if (!mStarted)
        return;
    peParticleNode *node = (peParticleNode *)mNodes;
    peParticleNode *next;
    for (; node; node = next) {
        next = (peParticleNode *)node->mNext;
        if (!node->Update(mParent, mParticle, mRenderData[mBufferIndex], delta)) {
            node->Stop(mParticle, &mDisplayLists[mBufferIndex]);
            RemoveNode(node);
        }
    }
    DepthSort();
}

u16 peParticleContainer::Emit(u16 count)
{
    if (!mStarted)
        return 0;
    u16 remaining = CreateNodes(count);
    _EmitParams *params = peEmitParams;
    peParticleNode *next;
    u16 emitted = remaining;
    peParticleNode *node = (peParticleNode *)mNodes;
    for (; remaining; --remaining, ++params) {
        next = (peParticleNode *)node->mNext;
        node->Start(mParent, mParticle, &mDisplayLists[mBufferIndex], params);
        node = next;
    }
    return emitted;
}

void peParticleContainer::Start()
{
    if (mStarted)
        return;
    mStarted = true;
    u32 flags = mParticle->mFlags;
    InitSwap();
    if (flags & PE_PARTICLE_LAYER_1)
        peSetLayerVisibility((GSgfxLayerID)1, true);
    if (flags & PE_PARTICLE_LAYER_3)
        peSetLayerVisibility((GSgfxLayerID)3, true);
    if (flags & PE_PARTICLE_LAYER_5)
        peSetLayerVisibility((GSgfxLayerID)5, true);
}

peParticleContainer::~peParticleContainer()
{
    if (mSortedNodes)
        delete[] mSortedNodes;
    g_particle_debug_stats.mBankData -= sizeof(*this);
    g_particle_debug_stats.mTotal -= sizeof(*this);
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
    peParticleNode *node = (peParticleNode *)mFreeNodes;
    u16 index = 0;
    for (; node; node = (peParticleNode *)node->mNext, ++index) {
        node->mRenderIndex = index;
        node->mDisplayIndex = index;
        if (mSortedNodes)
            mSortedNodes[index] = node;
    }
    g_particle_debug_stats.mBankData += sizeof(*this);
    g_particle_debug_stats.mTotal += sizeof(*this);
}

#include <peNodeMath.h>
#include <math.h>
#include <dolphin/os/OSFastCast.h>

extern "C" {
void EulerToQuat(const GSvec *, f32 *);
f32 HSD_Randf();
double fmod(double, double);
double atan2(double, double);
void GSgfxRenderDrawEnd(GSgfxLayer *);
void particleEngineSetupTexture(u32, GStexture *, u32, u32, u32, u32);
}

extern GSvec xAxisVector, yAxisVector, zAxisVector;
extern GSvec g_CameraPosition, g_CameraLookDir, g_CameraUpDir, g_CameraSideDir;
struct peColor {
    u8 r, g, b, a;
    peColor &operator=(const u32 &packed) {
        const u8 *components = (const u8 *)&packed;
        r = components[0]; g = components[1];
        b = components[2]; a = components[3];
        return *this;
    }
};
extern "C" void peColorSplineGetValue(peColor *, peColorSpline *, f32);

static inline f32 Unsigned8ToFloat(u8 value) { f32 result; OSu8tof32(&value, &result); return result; }
static inline f32 Unsigned16ToFloat(u16 value) { f32 result; OSu16tof32(&value, &result); return result; }
static inline u8 FloatToUnsigned8(f32 value) { u8 result; OSf32tou8(&value, &result); return result; }
static inline u16 FloatToUnsigned16(f32 value) { u16 result; OSf32tou16(&value, &result); return result; }

static inline const peEmitterRenderParameters &RenderParameters(peEmitterNode *node) {
    return *(const peEmitterRenderParameters *)node->mEmitterState;
}
static inline void SetLayerKonstColor(GSgfxLayer *layer, peColor color, u32 slot)
{
    layer->mKonstColors[slot] = *(u32 *)&color;
    layer->mTevKonstColorsDirtyA |= 1 << slot;
    layer->mDirtyStateA |= PE_PASS_KONST_COLORS_CHANGED;
    layer->mTevKonstColorsDirtyB |= 1 << slot;
    layer->mDirtyStateB |= PE_PASS_KONST_COLORS_CHANGED;
}
static inline void SetBlendColor(peColor color, u32 slot) {
    SetLayerKonstColor(peCurrentLayer, color, slot);
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
            phase = frames * (mLife - mRemainingLife) / particle->mTextureAnimationRate;
        else
            phase = particle->mTextureAnimationRate * (frames * (1.0f - mRemainingLife / mLife));
        frame = fmod(phase + Unsigned16ToFloat(textureIndex), frames);
        textureIndex = FloatToUnsigned16(frame) % (u16)textures->mCount;
        if (interpolate) {
            f32 textureFraction = frame;
            textureFraction -= Unsigned16ToFloat(textureIndex);
            fraction = 255.0f * textureFraction;
            peColor blend;
            blend.r = blend.g = blend.b = blend.a = FloatToUnsigned8(fraction);
            SetBlendColor(blend, 0);
            if (extended) {
                peParticleDualColorRenderData *data = (peParticleDualColorRenderData *)renderData;
                data += mRenderIndex;
                u32 packed = data->mSecondColor;
                blend = packed;
                SetBlendColor(blend, 1);
            }
            SetupTexture(1, textures->mTextures[textureIndex + 1], flags);
        }
    }
    SetupTexture(0, textures->mTextures[textureIndex], flags);
    peCurrentLayer->beginSurface(PE_PARTICLE_CORNERS);
    u16 *indices = (u16 *)displayList->VertexData(mDisplayIndex * PE_PARTICLE_CORNERS);
    u16 corners = PE_PARTICLE_CORNERS;
    register volatile PPCWGPipe *fifo = &GXWGFifo;
    while (corners--) {
        fifo->u16 = *indices++;
        fifo->u16 = *indices++;
        if (extended) fifo->u16 = *indices++;
        fifo->u16 = *indices++;
    }
    GSgfxRenderDrawEnd(peCurrentLayer);
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
    GSvec normal, vertical, horizontal, pivot, corner, position;
    peQuaternion rotation, orientation;
    GSvec euler;
    peQuaternion inverse, verticalInverse;
    GSvec offset, viewOffset;
    f32 angle;
    position = Product(mPosition, mTransform->mScale);
    Rotate(position, mTransform->mRotation);
    position += mTransform->mPosition;
    if (flags & PE_PARTICLE_BILLBOARD) {
        if (flags & PE_PARTICLE_CAMERA_FACING) {
            horizontal = g_CameraPosition - position;
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
        if (parent->mEmitter->mFlags & PE_EMITTER_ALIGN_VELOCITY) {
            GSvec velocity = mVelocity;
            f32 vx = velocity.x, vz = velocity.z, vy = velocity.y;
            euler.x = atan2(-vy, sqrtf(vx * vx + vz * vz));
            euler.y = atan2(vx, vz);
            euler.z = 0.0f;
            EulerToQuat(&euler, &rotation.x);
            Rotate(normal, &rotation.x);
            Rotate(vertical, &rotation.x);
        }
        Rotate(normal, mTransform->mRotation);
        Rotate(vertical, mTransform->mRotation);
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
    inverse = Inverse(mTransform->mRotation);
    corner = Rotated(normal, &inverse.x);
    ApplyScale(corner, mTransform->mScale);
    Rotate(corner, mTransform->mRotation);
    normal *= PSVECDotProduct(SDKVector(&normal), SDKVector(&corner));
    verticalInverse = Inverse(mTransform->mRotation);
    corner = Rotated(vertical, &verticalInverse.x);
    ApplyScale(corner, mTransform->mScale);
    Rotate(corner, mTransform->mRotation);
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
    mDepth = Dot(g_CameraLookDir, position - g_CameraPosition);
    corner = -normal;
    corner -= vertical;
    if (offsetPivot) corner += pivot;
    corner += position;
    data->mCorners[0] = corner;
    if (peCurrentBound) GSboundAddPoint(peCurrentBound, corner.x, corner.y, corner.z);
    corner = normal - vertical;
    if (offsetPivot) corner += pivot;
    corner += position;
    data->mCorners[1] = corner;
    if (peCurrentBound) GSboundAddPoint(peCurrentBound, corner.x, corner.y, corner.z);
    corner = normal + vertical;
    if (offsetPivot) corner += pivot;
    corner += position;
    data->mCorners[2] = corner;
    if (peCurrentBound) GSboundAddPoint(peCurrentBound, corner.x, corner.y, corner.z);
    corner = vertical - normal;
    if (offsetPivot) corner += pivot;
    corner += position;
    data->mCorners[3] = corner;
    if (peCurrentBound) GSboundAddPoint(peCurrentBound, corner.x, corner.y, corner.z);
    peColor color;
    peColorSplineGetValue(&color, particle->mColorSpline, time);
    color.r = FloatToUnsigned8(RenderParameters(parent).mRed * Unsigned8ToFloat(color.r));
    color.g = FloatToUnsigned8(RenderParameters(parent).mGreen * Unsigned8ToFloat(color.g));
    color.b = FloatToUnsigned8(RenderParameters(parent).mBlue * Unsigned8ToFloat(color.b));
    color.a = FloatToUnsigned8((255.0f * RenderParameters(parent).mAlpha) * SplineValue(particle->mAlphaSpline, time));
    data->mColor = *(u32 *)&color;
    if (extended) {
        peColorSplineGetValue(&color, particle->mSecondColorSpline, time);
        ((peParticleDualColorRenderData *)data)->mSecondColor = *(u32 *)&color;
    }
}

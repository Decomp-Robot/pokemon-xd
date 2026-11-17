#include <math.h>
#include <GStexture.h>
#include <GSbound.h>
#include <peNodeMath.h>
#include <dolphin/os/OSFastCast.h>
#include <dolphin/gx/GXEnum.h>
#include <particleEngine.h>
#include <peParticleStrip.h>
#include <hsdRandom.h>

enum peStripVertexWordSize { PE_STRIP_VERTEX_WORD_BYTES = 4 };

static inline f32 Unsigned8ToFloat(u8 value) { f32 result; OSu8tof32(&value, &result); return result; }
static inline f32 Unsigned16ToFloat(u16 value) { f32 result; OSu16tof32(&value, &result); return result; }
static inline u8 FloatToUnsigned8(f32 value) { u8 result; OSf32tou8(&value, &result); return result; }
static inline u16 FloatToUnsigned16(f32 value) { u16 result; OSf32tou16(&value, &result); return result; }

static inline void SetBlendColor(GScolor color, u8 slot) {
    g_current_layer->tunitKColor(slot, color);
}
static inline peTextureFilter MinimumFilter(u32 flags)
{
    if (flags & PE_STRIP_MIN_LINEAR)
        return PE_TEXTURE_LINEAR;
    return PE_TEXTURE_NEAREST;
}
static inline void SetupTexture(u32 slot, GStexture *texture, u32 flags)
{
    peTextureFilter magFilter = PE_TEXTURE_NEAREST;
    if (flags & PE_STRIP_MAG_LINEAR)
        magFilter = PE_TEXTURE_LINEAR;
    GStextureWrapMode wrapT = (GStextureWrapMode)((flags >> PE_STRIP_WRAP_T_SHIFT) & PE_STRIP_WRAP_MASK);
    particleEngineSetupTexture(slot, texture, GS_TEXTURE_WRAP_CLAMP, wrapT, MinimumFilter(flags), magFilter);
}

void peParticleStripContainer::RearrangeIndices()
{
    if (!started)
        return;

    u16 *indices;
    peParticleStripNode *node;
    u16 renderIndex;
    u16 displayIndex;
    u16 corner;
    u16 index;
    peStripRenderData *data;
    bool flip;
    u8 renderStride;
    f32 step;
    u32 flags;
    bool reverse;
    _peEmitter *emitterData;
    u8 displayWords;
    u8 renderWords;
    bool extended;
    char *renderData;
    GSgfxDL *displayList;


    displayIndex = 0;
    index = 0;
    flags = mStrip->mFlags;
    extended = (flags & PE_STRIP_SECOND_COLOR_SPLINE) != 0;
    if (nodeCount < 2)
        return;
    emitterData = emitter->emitter;
    if (flags & PE_STRIP_STRETCH_TEXTURE)
        step = 1.0f / Unsigned16ToFloat(nodeCount - 1);
    else
        step = emitterData->mLife;
    flip = (flags & PE_STRIP_FLIP_U) != 0;
    reverse = (flags & PE_STRIP_REVERSE_TEXTURE) != 0;
    if (extended) {
        renderWords = PE_STRIP_RENDER_WORDS + 1;
        displayWords = PE_STRIP_CORNERS * 4;
        renderStride = sizeof(peStripDualColorRenderData);
    } else {
        renderWords = PE_STRIP_RENDER_WORDS;
        displayWords = PE_STRIP_CORNERS * 3;
        renderStride = sizeof(peStripRenderData);
    }
    renderData = mRenderData[mBufferIndex];
    displayList = &mDisplayLists[mBufferIndex];
    node = (peParticleStripNode *)nodes;

    for (; node; node = (peParticleStripNode *)node->next) {
        data = (peStripRenderData *)(renderData + node->mRenderIndex * renderStride);
        node->mDisplayIndex = displayIndex;
        renderIndex = node->mRenderIndex * renderWords;
        indices = (u16 *)displayList->VertexData(0);
        indices += displayIndex;
        for (corner = 0; corner < PE_STRIP_CORNERS; ++corner) {
            *indices++ = renderIndex + corner * PE_STRIP_POSITION_COMPONENTS;
            *indices++ = renderIndex;
            if (extended)
                *indices++ = renderIndex;
            *indices++ = renderIndex + corner * PE_STRIP_TEXCOORD_COMPONENTS;
        }

        f32 position;
        if (reverse)
            position = Unsigned16ToFloat(nodeCount - index - 1);
        else
            position = Unsigned16ToFloat(index);
        if (flip)
            data->mTexCoords[0][0] = data->mTexCoords[1][0] = 1.0f - position * step;
        else
            data->mTexCoords[0][0] = data->mTexCoords[1][0] = position * step;
        data->mTexCoords[0][1] = mStrip->mTexCoordTop;
        data->mTexCoords[1][1] = mStrip->mTexCoordBottom;
        displayIndex += displayWords;
        ++index;
    }
    displayList->Invalidate();
}

void peParticleStripContainer::Stop()
{
    if (!started)
        return;

    PreUpdate();
    started = false;
    u32 flags = mStrip->mFlags;
    if (flags & PE_STRIP_LAYER_1)
        SetLayerVisibility(GSGFX_LAYER_BACKGROUND_DISTORTION, false);
    if (flags & PE_STRIP_LAYER_3)
        SetLayerVisibility(GSGFX_LAYER_MAIN_DISTORTION, false);
    if (flags & PE_STRIP_LAYER_5)
        SetLayerVisibility(GSGFX_LAYER_SECONDARY_DISTORTION, false);

    peParticleStripNode *node = (peParticleStripNode *)nodes;
    for (; node; node = (peParticleStripNode *)node->next)
        node->Stop(mStrip, &mDisplayLists[mBufferIndex]);

    RemoveAll();
    ResetSwap();
}

void peParticleStripContainer::Render()
{
    if (!started)
        return;
    if (!RenderForLayer())
        return;

    ResolveMultipleSwap();
    u32 flags = mStrip->mFlags;
    bool extended = (flags & PE_STRIP_SECOND_COLOR_SPLINE) != 0;
    g_current_layer->cullMode(GSGFX_CULL_NONE);
    particleEngineSetupBlendMode(flags & PE_STRIP_BLEND_MASK);

    char *renderData = mRenderData[mBufferIndex];
    GSgfxVF *format = mDisplayLists[mBufferIndex].VertexFormat();
    if (extended) {
        GSgfxVFSetArray(format, GX_VA_POS, GX_INDEX16, renderData,
                        PE_STRIP_VERTEX_WORD_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, GX_VA_CLR0, GX_INDEX16,
                        &((peStripRenderData *)renderData)->mColor,
                        PE_STRIP_VERTEX_WORD_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, GX_VA_CLR1, GX_INDEX16,
                        &((peStripDualColorRenderData *)renderData)->mSecondColor,
                        PE_STRIP_VERTEX_WORD_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, GX_VA_TEX0, GX_INDEX16,
                        ((peStripRenderData *)renderData)->mTexCoords,
                        PE_STRIP_VERTEX_WORD_BYTES, mRenderDataSize);
    } else {
        GSgfxVFSetArray(format, GX_VA_POS, GX_INDEX16, renderData,
                        PE_STRIP_VERTEX_WORD_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, GX_VA_CLR0, GX_INDEX16,
                        &((peStripRenderData *)renderData)->mColor,
                        PE_STRIP_VERTEX_WORD_BYTES, mRenderDataSize);
        GSgfxVFSetArray(format, GX_VA_TEX0, GX_INDEX16,
                        ((peStripRenderData *)renderData)->mTexCoords,
                        PE_STRIP_VERTEX_WORD_BYTES, mRenderDataSize);
    }

    if (flags & PE_STRIP_TEXTURE_LIST) {
        peTextureList *textures = mStrip->mTextureList;
        u16 textureIndex = mTextureFrame;
        if (flags & PE_STRIP_ANIMATE_TEXTURE) {
            f32 fraction;
            f32 frames = Unsigned16ToFloat(textures->mCount);
            bool interpolate = (flags & PE_STRIP_DUAL_COLOR) != 0;
            particleEngineSetupTevStages(0, extended, interpolate);
            if (interpolate)
                frames -= 1.0f;
            f32 phase;
            if (flags & PE_STRIP_TEXTURE_TIME_SECONDS)
                phase = frames * (emitter->emitter->mInitialNodeLife - mRemainingLife) /
                        mStrip->mTextureAnimationRate;
            else
                phase = mStrip->mTextureAnimationRate *
                        (frames * (1.0f - mRemainingLife / emitter->emitter->mInitialNodeLife));
            f32 frame = fmod(phase + Unsigned16ToFloat(textureIndex), frames);
            textureIndex = FloatToUnsigned16(frame) % (u16)textures->mCount;
            if (interpolate) {
                f32 textureFraction = frame;
                textureFraction -= Unsigned16ToFloat(textureIndex);
                fraction = 255.0f * textureFraction;
                GScolor blend;
                blend.channels.r = blend.channels.g = blend.channels.b = blend.channels.a =
                    FloatToUnsigned8(fraction);
                SetBlendColor(blend, 0);
                SetupTexture(1, textures->mTextures[textureIndex + 1], flags);
            }
        } else {
            particleEngineSetupTevStages(0, extended, false);
        }
        SetupTexture(0, textures->mTextures[textureIndex], flags);
    } else {
        particleEngineSetupTevStages(0, extended, false);
        peTextureFilter magFilter = PE_TEXTURE_NEAREST;
        if (flags & PE_STRIP_MAG_LINEAR)
            magFilter = PE_TEXTURE_LINEAR;
        GStextureWrapMode wrapT = (GStextureWrapMode)((flags >> PE_STRIP_WRAP_T_SHIFT) & PE_STRIP_WRAP_MASK);
        particleEngineSetupTexture(0, mStrip->mTexture, GS_TEXTURE_WRAP_CLAMP, wrapT, MinimumFilter(flags), magFilter);
    }

    g_current_layer->dlDraw(&mDisplayLists[mBufferIndex]);
    MarkForSwap();
}

void peParticleStripContainer::Update(f32 delta)
{
    if (!started)
        return;

    peParticleStripNode *node = (peParticleStripNode *)nodes;
    peParticleStripNode *next;
    for (; node; node = next) {
        next = (peParticleStripNode *)node->next;
        if (!node->Update(emitter, mStrip, &mDisplayLists[mBufferIndex],
                          mRenderData[mBufferIndex], delta)) {
            node->Stop(mStrip, &mDisplayLists[mBufferIndex]);
            RemoveNode(node);
        }
    }
    mRemainingLife -= delta;
    RearrangeIndices();
}

u16 peParticleStripContainer::Emit(u16 count)
{
    if (!started)
        return 0;

    u16 remaining = CreateNodes(count);
    _EmitParams *params = g_emitParams;
    peParticleStripNode *node = (peParticleStripNode *)nodes;
    peParticleStripNode *next;
    u16 emitted = remaining;
    for (; remaining; --remaining, ++params) {
        next = (peParticleStripNode *)node->next;
        f32 sequence = emitter->emitter->mLife *
                       Unsigned16ToFloat(emitter->mEmittedCount + emitted - remaining);
        node->Start(emitter, mStrip, &mDisplayLists[mBufferIndex], params, sequence);
        node = next;
    }
    return emitted;
}

void peParticleStripContainer::Start()
{
    if (started)
        return;

    started = true;
    u32 flags = mStrip->mFlags;
    InitSwap();
    if (flags & PE_STRIP_LAYER_1)
        SetLayerVisibility(GSGFX_LAYER_BACKGROUND_DISTORTION, true);
    if (flags & PE_STRIP_LAYER_3)
        SetLayerVisibility(GSGFX_LAYER_MAIN_DISTORTION, true);
    if (flags & PE_STRIP_LAYER_5)
        SetLayerVisibility(GSGFX_LAYER_SECONDARY_DISTORTION, true);

    if ((flags & (PE_STRIP_TEXTURE_LIST | PE_STRIP_RANDOM_TEXTURE)) ==
        (PE_STRIP_TEXTURE_LIST | PE_STRIP_RANDOM_TEXTURE))
        mTextureFrame = FloatToUnsigned8(Unsigned16ToFloat(mStrip->mTextureList->mCount) *
                                         HSD_Randf());
    else
        mTextureFrame = 0;
    mRemainingLife = emitter->emitter->mInitialNodeLife;
}

peParticleStripContainer::~peParticleStripContainer()
{
    g_particle_debug_stats.containerBytes -= sizeof(*this);
    g_particle_debug_stats.totalBytes -= sizeof(*this);
}

peParticleStripContainer::peParticleStripContainer(peEmitterNode *parent,
                                                   _peParticleStrip *strip,
                                                   u16 capacity)
    : peRenderContainer(parent, sizeof(peParticleStripNode), capacity,
                        (strip->mFlags & PE_STRIP_SECOND_COLOR_SPLINE) ? &g_ext_vertex_format
                                                              : &g_vertex_format,
                        GS_GFX_PRIM_TRIANGLE_STRIP, PE_STRIP_CORNERS,
                        (strip->mFlags & PE_STRIP_SECOND_COLOR_SPLINE) ? sizeof(peStripDualColorRenderData)
                                                              : sizeof(peStripRenderData),
                        (strip->mFlags & PE_STRIP_DOUBLE_BUFFER) != 0)
{
    mStrip = strip;
    mTextureFrame = 0xFF;

    peParticleStripNode *node = (peParticleStripNode *)freeNodes;
    u16 index = 0;
    for (; node; node = (peParticleStripNode *)node->next)
        node->mDisplayIndex = node->mRenderIndex = index++;

    g_particle_debug_stats.containerBytes += sizeof(*this);
    g_particle_debug_stats.totalBytes += sizeof(*this);
}

void peParticleStripNode::UpdateRenderData(peEmitterNode *parent,
                                           _peParticleStrip *strip,
                                           char *renderData,
                                           f32 time)
{
    peStripRenderData *data;
    u32 flags = strip->mFlags;
    bool extended = (flags & PE_STRIP_SECOND_COLOR_SPLINE) != 0;
    GSvec worldPosition = Product(position, transform->scale);
    Rotate(worldPosition, transform->rotation);
    worldPosition += transform->position;

    if (extended)
        data = (peStripRenderData *)(renderData + mRenderIndex * sizeof(peStripDualColorRenderData));
    else
        data = (peStripRenderData *)(renderData + mRenderIndex * sizeof(peStripRenderData));

    GSvec extent = Product(mExtent, transform->scale);
    Rotate(extent, transform->rotation);
    if (flags & PE_STRIP_CENTERED)
        worldPosition -= extent * 0.5f;

    data->mCorners[0] = worldPosition;
    if (g_currentBound)
        GSboundAddPoint(g_currentBound, worldPosition.x, worldPosition.y, worldPosition.z);
    worldPosition += extent;
    data->mCorners[1] = worldPosition;
    if (g_currentBound)
        GSboundAddPoint(g_currentBound, worldPosition.x, worldPosition.y, worldPosition.z);

    GScolor color;
    peColorSplineGetValue(&color, strip->mColorSpline, time);
    color.channels.r = FloatToUnsigned8(parent->colorScale[0] * Unsigned8ToFloat(color.channels.r));
    color.channels.g = FloatToUnsigned8(parent->colorScale[1] * Unsigned8ToFloat(color.channels.g));
    color.channels.b = FloatToUnsigned8(parent->colorScale[2] * Unsigned8ToFloat(color.channels.b));
    color.channels.a = FloatToUnsigned8((255.0f * parent->colorScale[3]) *
                                        peParticleNode::SplineValue(strip->mAlphaSpline, time));
    data->mColor = color.rgba;
    if (extended) {
        peColorSplineGetValue(&color, strip->mSecondColorSpline, time);
        ((peStripDualColorRenderData *)data)->mSecondColor = color.rgba;
    }
}

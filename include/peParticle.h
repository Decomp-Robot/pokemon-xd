#ifndef PE_PARTICLE_H
#define PE_PARTICLE_H

#include <peBank.h>
#include <peContainer.h>
#include <peSpline.h>


enum peParticleFlags {
    PE_PARTICLE_BLEND_MASK = 0x7,
    PE_PARTICLE_MIN_LINEAR = 0x80,
    PE_PARTICLE_MAG_LINEAR = 0x100,
    PE_PARTICLE_CULL_BACK = 0x200,
    PE_PARTICLE_LAYER_4 = 0x800,
    PE_PARTICLE_ROTATION_SPLINE = 0x10000,
    PE_PARTICLE_DEPTH_SORT = 0x80000,
    PE_PARTICLE_TEXTURE_ANIMATION = 0x100000,
    PE_PARTICLE_TEV_MASK = 0xC00000,
    PE_PARTICLE_DUAL_COLOR = 0x02000000,
    PE_PARTICLE_DOUBLE_BUFFER = 0x04000000,
    PE_PARTICLE_LAYER_0 = 0x08000000,
    PE_PARTICLE_LAYER_2 = 0x10000000,
    PE_PARTICLE_LAYER_1 = 0x20000000,
    PE_PARTICLE_LAYER_3 = 0x40000000,
    PE_PARTICLE_LAYER_5 = 0x80000000,
    PE_PARTICLE_LAYER_MASK = 0xF8000800,
};

enum peParticleGeometry {
    PE_PARTICLE_CORNERS = 4,
    PE_PARTICLE_POSITION_COMPONENTS = 3,
    PE_PARTICLE_TEXCOORD_COMPONENTS = 2,
    PE_PARTICLE_RENDER_WORDS = 13,
    PE_PARTICLE_INVALID_INDEX = 0xFFFF,
};

enum peTextureFilter {
    PE_TEXTURE_NEAREST = 0,
    PE_TEXTURE_LINEAR = 2,
};

enum peRenderFlags {
    PE_PASS_KONST_COLORS_CHANGED = 0x10,
    PE_PARTICLE_WRAP_S_SHIFT = 12,
    PE_PARTICLE_WRAP_T_SHIFT = 14,
    PE_PARTICLE_WRAP_MASK = 3,
    PE_CAMERA_INVERSE_MATRIX = 3,
};

struct peParticleRenderData {
    GSvec mCorners[PE_PARTICLE_CORNERS];
    u32 mColor;
};

struct peParticleDualColorRenderData : public peParticleRenderData {
    u32 mSecondColor;
};

struct peTextureList {
    u32 mCount;
    GStexture *mTextures[1];
};

struct _peParticle {
    u32 mFlags;
    f32 mWidth;
    f32 mHeight;
    f32 mWidthRandom;
    f32 mHeightRandom;
    peSpline *mWidthSpline;
    peSpline *mHeightSpline;
    f32 mPivotX;
    f32 mPivotY;
    f32 mRotation;
    f32 mRotationVelocity;
    f32 mRotationVelocityRandom;
    f32 mTextureAnimationRate;
    peSpline *mRotationSpline;
    struct peColorSpline *mColorSpline;
    struct peColorSpline *mSecondColorSpline;
    peSpline *mAlphaSpline;
    s16 mTexCoordLeft;
    s16 mTexCoordTop;
    s16 mTexCoordRight;
    s16 mTexCoordBottom;
    f32 mOrientationY;
    f32 mOrientationZ;
    union {
        s32 mResourceID;
        GStexture *mTexture;
        peTextureList *mTextureList;
    };
};


class peParticleNode : public peNode {
public:
    void SetDisplayIndex(GSgfxDL *displayList, u16 displayIndex,
                         u16 renderFloats, bool extended);
    void Start(peEmitterNode *parent, _peParticle *particle, GSgfxDL *displayList,
               _EmitParams *params);
    bool Update(peEmitterNode *parent, _peParticle *particle, char *renderData,
                f32 delta) {
        if (!renderData)
            return false;
        if (!peNode::Update(parent, delta))
            return false;
        f32 time = 1.0f - remainingLife / life;
        if (particle->mFlags & PE_PARTICLE_ROTATION_SPLINE) {
            mRotation = particle->mRotation * SplineValue(particle->mRotationSpline, time);
        }
        mRotation += mRotationVelocity * delta;
        UpdateRenderData(parent, particle, renderData, time);
        return true;
    }
    void UpdateRenderData(peEmitterNode *parent, _peParticle *particle,
                          char *renderData, f32 time);
    void Render(_peParticle *particle, GSgfxDL *displayList, char *renderData);

    void Stop(_peParticle *particle, GSgfxDL *displayList) {
        u32 flags;
        u16 *indices;
        bool extended;
        flags = particle->mFlags;
        extended = (flags & PE_PARTICLE_DUAL_COLOR) != 0;
        mActive = false;
        indices = (u16 *)displayList->VertexData(mDisplayIndex * PE_PARTICLE_CORNERS);
        for (u16 corners = PE_PARTICLE_CORNERS; corners--; ) {
            *indices++ = PE_PARTICLE_INVALID_INDEX;
            *indices++ = PE_PARTICLE_INVALID_INDEX;
            *indices++ = PE_PARTICLE_INVALID_INDEX;
            if (extended)
                *indices++ = PE_PARTICLE_INVALID_INDEX;
        }
        displayList->Invalidate();
    }
    static f32 SplineValue(peSpline *spline, f32 time) {
        if (!spline || !(spline->flags & PE_SPLINE_MODE_MASK))
            return 1.0f;
        f32 difference = spline->time - time;
        difference = difference < 0.0f ? -difference : difference;
        if (difference <= 0.00001f)
            return spline->value;
        return peSplineGetValue(spline, time);
    }

    u16 mRenderIndex;   // 0x30: fixed slot in the render-data buffer.
    u16 mDisplayIndex;  // 0x32: sorted slot in the display list.
    u8 mTextureIndex;
    u8 mOrientationFlags;
    bool mActive;
    u8 mReserved;
    f32 mWidth;
    f32 mHeight;
    f32 mInitialRotation;
    f32 mRotation;      // 0x44
    f32 mRotationVelocity;
    f32 mDepth;
};


enum peNodeFlags {
    PE_PARTICLE_ROTATE_BEFORE_SCALE = 0x10,
    PE_PARTICLE_RANDOM_REVERSE = 0x20,
    PE_PARTICLE_RANDOM_ROTATION = 0x40,
    PE_PARTICLE_BILLBOARD = 0x200,
    PE_PARTICLE_UPRIGHT = 0x400,
    PE_PARTICLE_CAMERA_FACING = 0x20000,
    PE_PARTICLE_PIVOT = 0x40000,
    PE_PARTICLE_RANDOM_TEXTURE = 0x300000,
    PE_PARTICLE_ANIMATE_TEXTURE = 0x400000,
    PE_PARTICLE_INTERPOLATE_TEXTURE = 0x800000,
    PE_PARTICLE_TEXTURE_TIME_SECONDS = 0x1000000,
    PE_NODE_REVERSE_ROTATION = 1,
};

class peRenderContainer : public peContainer {
public:
    peRenderContainer(peEmitterNode *parent, unsigned long nodeSize, u16 maxNodes,
                      GSgfxVF *format, GSgfxPrimType primitive, u8 vertices,
                      u8 renderStride, bool doubleBuffer);
    virtual ~peRenderContainer();
    void InitSwap();
    void ResolveMultipleSwap();
    void MarkForSwap();
    void StopSwap();
    virtual void PreUpdate();
    virtual void Start() = 0;

    u8 mSwapState;
    u8 mBufferIndex;
    u16 mReserved;
    u32 mRenderDataSize;
    GSgfxDL mDisplayLists[2];
    char *mRenderData[2];
    u16 mRenderTokens[2];
};

class peParticleContainer : public peRenderContainer {
public:
    peParticleContainer(peEmitterNode *parent, _peParticle *particle, u16 maxNodes);
    virtual ~peParticleContainer();
    void DepthSort();
    virtual void Stop();
    virtual void Render();
    virtual void Update(f32 delta);
    virtual u16 Emit(u16 count);
    virtual void Start();

    int PreviousActive(int index) {
        if (!mSortedNodes)
            return -1;
        while (index > 0) {
            --index;
            if (mSortedNodes[index]->mActive)
                return index;
        }
        return -1;
    }
    bool RenderForLayer();

    peParticleNode **mSortedNodes;
    _peParticle *mParticle;
    f32 mTexCoords[8];
};

#endif

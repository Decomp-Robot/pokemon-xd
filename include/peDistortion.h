#ifndef PE_DISTORTION_H
#define PE_DISTORTION_H

#include <peBankData.h>
#include <peParticle.h>
#include <hsdRandom.h>

/* Flags shared by the packed distortion bank record and its render path. */
enum peDistortionFlags {
    PE_DISTORTION_RANDOM_ROTATION = 0x80,
    PE_DISTORTION_BILLBOARD = 0x100,
    PE_DISTORTION_ROTATION_SPLINE = 0x200,
    PE_DISTORTION_CAMERA_FACING = 0x400,
    PE_DISTORTION_USE_OFFSET = 0x800,
    PE_DISTORTION_RANDOM_REVERSE = 0x1000,
    PE_DISTORTION_RENDER_LAYER_5 = 0x10,
    PE_DISTORTION_RENDER_LAYER_1 = 0x2000,
    PE_DISTORTION_ENABLE_LAYER_5 = 0x2000000,
    PE_DISTORTION_ENABLE_LAYER_3 = 0x1000000,
    PE_DISTORTION_ENABLE_LAYER_1 = 0x800000,
    PE_DISTORTION_DOUBLE_BUFFER = 0x4000,
    PE_DISTORTION_ROTATE_BEFORE_SCALE = 0x8000,
    PE_DISTORTION_MIN_LINEAR = 0x20,
    PE_DISTORTION_MAG_LINEAR = 0x40,
    PE_DISTORTION_WRAP_S_MASK = 3,
    PE_DISTORTION_WRAP_T_SHIFT = 2,
    PE_DISTORTION_WRAP_T_MASK = 3,
};

class peDistortionNode : public peNode {
public:
    void Start(peEmitterNode *parent, _peDistortion *distortion,
               GSgfxDL *displayList, _EmitParams *params) {
        u32 flags = distortion->mFlags;
        if ((displayList->mFlags & GS_DISPLAY_LIST_ALLOCATED) != GS_DISPLAY_LIST_ALLOCATED)
            return;
        peNode::Start(parent, params);

        f32 random = HSD_Randf();
        mWidth = distortion->mMotion.mWidthRandom * random;
        mHeight = distortion->mMotion.mHeightRandom * random;
        mWidth += distortion->mMotion.mWidth;
        mHeight += distortion->mMotion.mHeight;

        if (flags & PE_DISTORTION_RANDOM_ROTATION)
            mInitialRotation = 6.2831855f * HSD_Randf();
        else
            mInitialRotation = 0.0f;

        if ((flags & PE_DISTORTION_RANDOM_REVERSE) &&
            HSD_Randf() < 0.5f)
            mOrientationFlags |= PE_NODE_REVERSE_ROTATION;

        mRotation = distortion->mParameters.mRotation;
        mRotationVelocity = distortion->mParameters.mRotationVelocityRandom * HSD_Randf() +
                            distortion->mParameters.mRotationVelocity;
        u16 *indices;
        u16 renderIndex = mRenderIndex * PE_PARTICLE_RENDER_WORDS;
        u16 vertexIndex = mRenderIndex * PE_PARTICLE_CORNERS;
        indices = (u16 *)displayList->VertexData(vertexIndex);
        for (u16 corner = 0; corner < PE_PARTICLE_CORNERS; ++corner) {
            *indices++ = renderIndex + corner * PE_PARTICLE_POSITION_COMPONENTS;
            *indices++ = renderIndex;
            *indices++ = corner * PE_PARTICLE_TEXCOORD_COMPONENTS;
        }
        displayList->Invalidate();
    }

    bool Update(peEmitterNode *parent, _peDistortion *distortion,
                GSgfxDL *displayList, char *renderData, f32 delta) {
        if (!renderData) return false;
        if ((displayList->mFlags & GS_DISPLAY_LIST_ALLOCATED) != GS_DISPLAY_LIST_ALLOCATED) return false;
        if (!peNode::Update(parent, delta)) return false;

        f32 time = 1.0f - remainingLife / life;
        if (distortion->mFlags & PE_DISTORTION_ROTATION_SPLINE)
            mRotation = distortion->mParameters.mRotation *
                        peParticleNode::SplineValue(distortion->mRotationSpline, time);
        mRotation += mRotationVelocity * delta;
        UpdateRenderData(parent, distortion, renderData, time);
        return true;
    }

    void UpdateRenderData(peEmitterNode *parent, _peDistortion *distortion,
                          char *renderData, f32 time);

    void Stop(GSgfxDL *displayList) {
        u16 *indices = reinterpret_cast<u16 *>(
            displayList->VertexData(mRenderIndex * PE_PARTICLE_CORNERS));
        for (u16 corner = PE_PARTICLE_CORNERS; corner--; ) {
            *indices++ = PE_PARTICLE_INVALID_INDEX;
            *indices++ = PE_PARTICLE_INVALID_INDEX;
            *indices++ = PE_PARTICLE_INVALID_INDEX;
        }
        displayList->Invalidate();
    }

    u16 mRenderIndex;
    u16 mOrientationFlags;
    f32 mWidth;
    f32 mHeight;
    f32 mInitialRotation;
    f32 mRotation;
    f32 mRotationVelocity;
};

class peDistortionContainer : public peRenderContainer {
public:
    peDistortionContainer(peEmitterNode *parent, _peDistortion *distortion,
                          u16 maxNodes);
    virtual ~peDistortionContainer();
    virtual void Stop();
    virtual void Render();
    virtual void Update(f32 delta);
    virtual u16 Emit(u16 count);
    virtual void Start();

    bool RenderForLayer() const {
        if (!started) return false;
        u32 flags = mDistortion->mFlags;
        if (!nodeCount) return false;
        if (g_render_pass != GSGFX_LAYER_PASS_XLU) return false;
        switch (g_current_layer->mLayerID) {
        case 1:
            if (!(flags & PE_DISTORTION_RENDER_LAYER_1)) return false;
            break;
        case 3:
            if (flags & PE_DISTORTION_RENDER_LAYER_5) return false;
            break;
        case 5:
            if (!(flags & PE_DISTORTION_RENDER_LAYER_5)) return false;
            break;
        default:
            return false;
        }
        return true;
    }

    _peDistortion *mDistortion;
    f32 mTexCoords[8];
};

#endif

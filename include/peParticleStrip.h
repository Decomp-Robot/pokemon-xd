#ifndef PEPARTICLESTRIP_H
#define PEPARTICLESTRIP_H

#include <peParticle.h>
#include <peBankData.h>
#include <hsdRandom.h>

enum peParticleStripGeometry {
    PE_STRIP_CORNERS = 2,
    PE_STRIP_POSITION_COMPONENTS = 3,
    PE_STRIP_TEXCOORD_COMPONENTS = 2,
    PE_STRIP_RENDER_WORDS = 11,
    PE_STRIP_INVALID_INDEX = 0xFFFF,
};

struct peStripRenderData {
    GSvec mCorners[PE_STRIP_CORNERS];
    u32 mColor;
    f32 mTexCoords[PE_STRIP_CORNERS][PE_STRIP_TEXCOORD_COMPONENTS];
};

struct peStripDualColorRenderData : public peStripRenderData {
    u32 mSecondColor;
};

class peParticleStripNode : public peNode {
public:
    void Start(peEmitterNode *parent, _peParticleStrip *strip, GSgfxDL *displayList,
               _EmitParams *params, f32 sequence) {
        if ((displayList->mFlags & GS_DISPLAY_LIST_ALLOCATED) != GS_DISPLAY_LIST_ALLOCATED)
            return;
        peNode::Start(parent, params);
        if (strip->mFlags & PE_STRIP_SEQUENTIAL_WIDTH)
            mExtent = strip->mWidthDirection *
                      (10.0f * (sequence * strip->mWidthRandom + strip->mWidth));
        else {
            f32 random = HSD_Randf();
            mExtent = strip->mWidthDirection *
                      (10.0f * (strip->mWidthRandom * random + strip->mWidth));
        }
        displayList->Invalidate();
    }

    bool Update(peEmitterNode *parent, _peParticleStrip *strip, GSgfxDL *displayList,
                char *renderData, f32 delta) {
        if (!renderData)
            return false;
        if ((displayList->mFlags & GS_DISPLAY_LIST_ALLOCATED) != GS_DISPLAY_LIST_ALLOCATED)
            return false;
        if (!peNode::Update(parent, delta))
            return false;
        UpdateRenderData(parent, strip, renderData, 1.0f - remainingLife / life);
        return true;
    }

    void UpdateRenderData(peEmitterNode *parent, _peParticleStrip *strip,
                          char *renderData, f32 time);

    void Stop(_peParticleStrip *strip, GSgfxDL *displayList) {
        bool extended = static_cast<bool>(strip->mFlags & PE_STRIP_SECOND_COLOR_SPLINE);
        u16 *indices = (u16 *)displayList->VertexData(0);
        indices += mDisplayIndex;
        for (u16 corners = PE_STRIP_CORNERS; corners--; ) {
            *indices++ = PE_STRIP_INVALID_INDEX;
            *indices++ = PE_STRIP_INVALID_INDEX;
            *indices++ = PE_STRIP_INVALID_INDEX;
            if (extended)
                *indices++ = PE_STRIP_INVALID_INDEX;
        }
        displayList->Invalidate();
    }

    u16 mRenderIndex;
    u16 mDisplayIndex;
    GSvec mExtent;
};

class peParticleStripContainer : public peRenderContainer {
public:
    peParticleStripContainer(peEmitterNode *parent, _peParticleStrip *strip, u16 capacity);
    virtual ~peParticleStripContainer();
    void RearrangeIndices();
    virtual void Stop();
    virtual void Render();
    virtual void Update(f32 delta);
    virtual u16 Emit(u16 count);
    virtual void Start();

    bool RenderForLayer() {
        if (!started)
            return false;
        u32 flags = mStrip->mFlags;
        if (nodeCount < 2)
            return false;
        if (g_render_pass != GSGFX_LAYER_PASS_XLU)
            return false;
        switch (g_current_layer->mLayerID) {
        case GSGFX_LAYER_BASE:
            if (flags & PE_STRIP_LAYER_0)
                return true;
            break;
        case GSGFX_LAYER_DEFAULT:
            if (flags & PE_STRIP_LAYER_2)
                return true;
            if (!(flags & PE_STRIP_LAYER_MASK))
                return true;
            break;
        case GSGFX_LAYER_SECONDARY:
            if (flags & PE_STRIP_LAYER_4)
                return true;
            break;
        case GSGFX_LAYER_BACKGROUND_DISTORTION:
            if (flags & PE_STRIP_LAYER_1)
                return true;
            break;
        case GSGFX_LAYER_MAIN_DISTORTION:
            if (flags & PE_STRIP_LAYER_3)
                return true;
            break;
        case GSGFX_LAYER_SECONDARY_DISTORTION:
            if (flags & PE_STRIP_LAYER_5)
                return true;
            break;
        default:
            return false;
        }
        return false;
    }

    _peParticleStrip *mStrip;
    f32 mRemainingLife;
    u8 mTextureFrame;
};

#endif

#ifndef GLOW_H
#define GLOW_H

#include <GSeffect.h>

class glowEffect : public GSeffect {
public:
    enum GlowRenderMode {
        GLOW_RENDER_OFFSET,
        GLOW_RENDER_SCALE
    };

    glowEffect();
    virtual ~glowEffect();
    virtual void _render(GSgfxLayer* layer, GSgfxLayerPass pass);
    void _renderOffset(GSgfxLayer* layer);
    void _renderScale(GSgfxLayer* layer);

public:

    f32 mIntensity;

    u8 mBlurRadius;
    u8 mAlphaBlend;
    u8 mUseAlphaPass;
    u8 mRenderMode;
    u8 mBlurSteps;

    static GSgfxLayerID texture_layer_id;
    static u16 token;
    static u8 refCount;
};

#endif

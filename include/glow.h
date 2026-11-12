#ifndef GLOW_H
#define GLOW_H

#include <GSeffect.h>

class glowEffect : public GSeffect {
public:
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
};

#endif

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

    u8 mParam0;
    u8 mParam1;
    u8 mParam2;
    u8 mParam3;
    u8 mParam4;
};

#endif

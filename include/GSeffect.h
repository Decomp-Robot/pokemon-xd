#ifndef GSEFFECT_H
#define GSEFFECT_H

#include <GSgfx.h>

class GSeffectBase {
protected:
    u8 mActive;
    u8 mVisible;
    u8 mPadding02[0x02];

    GSeffectBase* mNext;
};

class GSeffect : public GSeffectBase {
public:
    GSeffect();
    virtual int _start();
    virtual void _stop();
    virtual int _update(f32 timestep);
    virtual void _render(GSgfxLayer* layer, GSgfxLayerPass pass);
    virtual void _fastRender();
    virtual ~GSeffect();
    void Start();
    void Stop();
};

class glowEffect : public GSeffect {
public:
    glowEffect();
    virtual ~glowEffect();
    virtual void _render(GSgfxLayer* layer, GSgfxLayerPass pass);

public:
    f32 mIntensity;
    u8 mSpread;
    u8 mAlphaBlend;
    u8 mParam2;
    u8 mRenderMode;
    u8 mSubdivisions;
};

#endif

#ifndef GS_EFFECT_H
#define GS_EFFECT_H

#include <GSgfx.h>
#include <types.h>

class GSeffect {
protected:
    bool mActive;
    bool mVisible;
    GSeffect* mNext;

    friend void _effectFastRender(void);
    friend void _effectRender(GSgfxLayer* layer, GSgfxLayerPass pass);
    friend void _effectUpdate(f32 delta);

public:
    GSeffect();
    virtual bool _start();
    virtual void _stop();
    virtual bool _update(f32 delta);
    virtual void _render(GSgfxLayer* layer, GSgfxLayerPass pass);
    virtual void _fastRender();
    virtual ~GSeffect();
    bool Start();
    void Stop();
};

extern "C" void GSeffectInitSystem(void);

#endif

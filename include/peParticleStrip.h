#ifndef PEPARTICLESTRIP_H
#define PEPARTICLESTRIP_H

#include <peParticle.h>
#include <peBankData.h>

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

    _peParticleStrip *mStrip;
    f32 mRemainingLife;
    u8 mTextureFrame;
};

#endif

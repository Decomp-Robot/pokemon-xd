#ifndef WAZA_SEQUENCE_BILLBOARD_H
#define WAZA_SEQUENCE_BILLBOARD_H

#include <GSeffect.h>
#include <GSmodel.h>
#include <GSparticle.h>

class billboardEffect : public GSeffect {
    unsigned long mModelResourceID;
    unsigned long mParticleResourceID;
    GSmodel* mModel;
    _GSparticleBank* mParticleBank;
    GSmodelAnimType mAnimationType;
    u16 mAnimationIndex;
    u16 mGeneratorIndex;
    _GSparticleGen* mGenerator;

public:
    billboardEffect(u8** stream);
    virtual bool _start();
    virtual void _stop();
    virtual bool _update(f32 delta);
    virtual ~billboardEffect();
};

#endif

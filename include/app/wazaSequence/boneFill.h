#ifndef WAZA_SEQUENCE_BONEFILL_H
#define WAZA_SEQUENCE_BONEFILL_H

#include <GSeffect.h>
#include <GSparticle.h>

struct ModelSequence;

class boneFillEffect : public GSeffect {
public:
    _GSparticleBank* mParticleBank;
    _GSparticleGen* mGenerator;
    ModelSequence* mModelSequence;
    s32 mStartJoint;
    s32 mEndJoint;
    s32 mStartPart;
    s32 mEndPart;
    f32 mBaseLength;

    virtual ~boneFillEffect();
    virtual bool _start();
    virtual void _stop();
    virtual bool _update(f32 delta);
};

#endif

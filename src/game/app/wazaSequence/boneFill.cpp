#include <app/wazaSequence/boneFill.h>

boneFillEffect::~boneFillEffect()
{
    Stop();
}

void boneFillEffect::_stop()
{
    mModelSequence = 0;
    if (mGenerator) {
        GSparticleDestroyGenerator(mGenerator, true);
        mGenerator = 0;
    }
}

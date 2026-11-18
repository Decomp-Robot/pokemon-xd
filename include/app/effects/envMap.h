#ifndef APP_EFFECTS_ENVMAP_H
#define APP_EFFECTS_ENVMAP_H

#include <GSeffect.h>

struct _GSmodel;

// Environment-mapping effect. Enables a model's environment map with the
// stored mode/type/blend when started, keeps the blend value refreshed while
// active, and disables the map when stopped.
class envMapEffect : public GSeffect {
public:
    envMapEffect(s32 mode, s32 type)
        : mModel(NULL), mEnvMapMode(mode), mEnvMapType(type), mBlend(0.0f)
    {
    }

    virtual bool _start();
    virtual void _stop();
    virtual bool _update(f32 delta);
    virtual ~envMapEffect();

    _GSmodel *mModel;
    s32 mEnvMapMode;
    s32 mEnvMapType;
    f32 mBlend;
};

#endif

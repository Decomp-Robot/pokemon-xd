#include <app/effects/envMap.h>
#include <GSmodel.h>

envMapEffect::~envMapEffect()
{
    Stop();
}

void envMapEffect::_stop()
{
    if (mModel != NULL) {
        GSmodelDisableEnvMap(mModel);
    }
}

bool envMapEffect::_update(f32)
{
    if (mModel == NULL) {
        return false;
    }
    if (!GSmodelIsEnvMapEnabled(mModel)) {
        GSmodelEnableEnvMap(mModel, mEnvMapMode, mEnvMapType, mBlend);
    } else {
        GSmodelSetEnvMapBlendValue(mModel, mBlend);
    }
    return true;
}

bool envMapEffect::_start()
{
    if (mModel == NULL) {
        return false;
    }
    GSmodelEnableEnvMap(mModel, mEnvMapMode, mEnvMapType, mBlend);
    return true;
}

#include <app/effects/filter.h>
#include <GSfilter.h>
#include <GSmodel.h>
#include <GSres.h>
#include <GSlog.h>
#include <floor.h>
#include <floorBios.h>
#include <scene.h>

filterEffect::~filterEffect()
{
    Stop();
}

bool filterEffect::_update(f32)
{
    if (mMode != SCREEN_FILTER) {
        UpdateModulation();
    } else if (mFilter != NULL) {
        GSfilterSetColor(mFilter, (GScolor *)&mColor);
    } else {
        return false;
    }
    return true;
}

inline void filterEffect::StopModulation()
{
    if (mModel != NULL) {
        if (mRestoreModulation) {
            GSmodelSetModulationColor(mModel, &mSavedColor);
        } else {
            GSmodelDisableModulation(mModel);
        }
        if (mOverridePE) GSmodelResetPEdescr(mModel);
    } else {
        u32 group = floorGetCurrentGroupID();
        u32 resource = floorDataBiosGetMapResID(floorDataBiosGetCurrentPtr());
        u32 index = 0;
        void *archive = GSresGetResource(group, resource);
        if (archive != NULL) {
            _HSD_SceneDesc *scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, "scene_data");
            if (scene != NULL && scene->mModels != NULL) {
                u32 modelResource = floorReadMakeModelResID(resource);
                for (; scene->mModels[index] != NULL; index++) {
                    _GSmodel *model = (_GSmodel *)GSresGetResource(group, modelResource | index);
                    if (model != NULL) {
                        GSmodelDisableModulation(model);
                        if (mOverridePE) GSmodelResetPEdescr(model);
                    }
                }
            }
        }
    }
}

void filterEffect::_stop()
{
    if (mMode != SCREEN_FILTER) {
        StopModulation();
    } else if (mFilter != NULL) {
        GSfilterFree(mFilter);
    }
}

bool filterEffect::_start()
{
    if (mMode != SCREEN_FILTER) {
        InitModulation();
    } else {
        mFilter = GSfilterCreate((GScolor *)&mColor);
        if (mFilter == NULL) {
            GSlogWrite("filterStart: Could not start filter effect - invalid filter!\n");
            return false;
        }
    }
    return true;
}

static inline u8 CombineChannel(u8 base, u8 color)
{
    f32 value = (f32)base + (f32)color - 128.0f;
    if (value < 0.0f) value = 0.0f;
    else if (value > 255.0f) value = 255.0f;
    return (u8)value;
}

void filterEffect::UpdateModulation()
{
    GScolor color;
    if (mRestoreModulation) {
        color.channels.r = CombineChannel(mSavedColor.channels.r, mColor.r);
        color.channels.g = CombineChannel(mSavedColor.channels.g, mColor.g);
        color.channels.b = CombineChannel(mSavedColor.channels.b, mColor.b);
        color.channels.a = (u8)((f32)mSavedColor.channels.a * (f32)mColor.a / 255.0f);
    } else {
        color.channels.r = mColor.r;
        color.channels.g = mColor.g;
        color.channels.b = mColor.b;
        color.channels.a = mColor.a;
    }
    if (mModel != NULL) {
        GSmodelSetModulationColor(mModel, &color);
    } else {
        u32 group = floorGetCurrentGroupID();
        u32 resource = floorDataBiosGetMapResID(floorDataBiosGetCurrentPtr());
        u32 index = 0;
        void *archive = GSresGetResource(group, resource);
        if (archive != NULL) {
            _HSD_SceneDesc *scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, "scene_data");
            if (scene != NULL && scene->mModels != NULL) {
                u32 modelResource = floorReadMakeModelResID(resource);
                for (; scene->mModels[index] != NULL; index++) {
                    _GSmodel *model = (_GSmodel *)GSresGetResource(group, modelResource | index);
                    if (model != NULL) GSmodelSetModulationColor(model, &color);
                }
            }
        }
    }
}

void filterEffect::InitModulation()
{
    static HSD_PEDesc pedesc = {
        HSD_PE_COLOR_UPDATE | HSD_PE_ALPHA_UPDATE | HSD_PE_Z_COMP | HSD_PE_Z_UPDATE,
        0, 0, 255,
        GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET,
        GX_LESS, GX_GREATER, GX_AOP_OR, GX_NEVER
    };
    mRestoreModulation = false;
    if (mModel != NULL) {
        if (mOverridePE) GSmodelSetPEdescr(mModel, &pedesc);
        if (!GSmodelIsModulationEnabled(mModel)) {
            GSmodelEnableModulation(mModel, (GScolor *)&mColor);
        } else if (mCombineModulation) {
            mRestoreModulation = true;
            GSmodelGetModulationColor(mModel, &mSavedColor);
        } else {
            GSmodelSetModulationColor(mModel, (GScolor *)&mColor);
        }
    } else {
        u32 group = floorGetCurrentGroupID();
        u32 resource = floorDataBiosGetMapResID(floorDataBiosGetCurrentPtr());
        u32 index = 0;
        void *archive = GSresGetResource(group, resource);
        if (archive != NULL) {
            _HSD_SceneDesc *scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, "scene_data");
            if (scene != NULL && scene->mModels != NULL) {
                u32 modelResource = floorReadMakeModelResID(resource);
                for (; scene->mModels[index] != NULL; index++) {
                    _GSmodel *model = (_GSmodel *)GSresGetResource(group, modelResource | index);
                    if (model != NULL) {
                        if (mOverridePE) GSmodelSetPEdescr(model, &pedesc);
                        GSmodelEnableModulation(model, (GScolor *)&mColor);
                    }
                }
            }
        }
    }
}

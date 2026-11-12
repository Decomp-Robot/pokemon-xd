#ifndef SCENE_H
#define SCENE_H

#include <hsd/scene.h>
#include <HSDArchive.h>

struct GScameraDesc;

// Resource descriptor exported as scene_data by scene archives.
struct _HSD_SceneDesc {
    _HSD_SceneModelSet** mModels;
    GScameraDesc* mCamera;
    void** mLights;
    void** mFogs;
};

#endif

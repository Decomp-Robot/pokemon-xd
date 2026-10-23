#ifndef SCENE_H
#define SCENE_H

#include <hsd/scene.h>

// Resource descriptor exported as scene_data by scene archives.
struct _HSD_SceneDesc {
    _HSD_SceneModelSet** mModels;
    void* mCamera;
    void** mLights;
};

#ifdef __cplusplus
extern "C" {
#endif

// Archive lookup the map loader uses to fetch the scene_data header.
void* HSD_ArchiveGetPublicAddress(void* archive, const char* name);

#ifdef __cplusplus
}
#endif

#endif

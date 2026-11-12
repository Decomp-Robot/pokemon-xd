#include <types.h>
#include <GSres.h>
#include <GScamera.h>
#include <scene.h>
#include <floor.h>

extern "C" char floorSceneDataName[];
bool _unloadCamera(void *camera, u32 group, u32 res);

void _registerCamera(u32 group, u32 res, _HSD_SceneDesc *scene)
{
    if (scene != 0) {
        _GScamera *camera = GScameraLoad(scene->mCamera);
        if (camera != 0) {
            GSresRegisterResource(camera, group, res, _unloadCamera);
        }
    }
}

_HSD_SceneDesc *_makeArchiveHeader(u32 group, u32 res, u32 size)
{
    GSresUnlockResource(group, res);
    u8 *data = (u8 *)GSresGetResource(group, res);
    HSD_Archive *archive = (HSD_Archive *)data;
    HSD_ArchiveParse(archive,
                     data + ((sizeof(HSD_Archive) + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1)),
                     size);
    return (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, floorSceneDataName);
}

bool _unloadObject(void *data, u32 group, u32 res)
{
    u32 index = 0;
    HSD_Archive *archive = (HSD_Archive *)GSresGetResource(group, res);
    if (archive == 0) {
        return false;
    }
    _HSD_SceneDesc *scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, floorSceneDataName);
    u32 modelRes = floorReadMakeModelResID(res);
    if (scene->mModels != 0) {
        while (scene->mModels[index] != 0) {
            GSresFreeResource(group, modelRes | index);
            index++;
        }
    }
    return scene != 0;
}

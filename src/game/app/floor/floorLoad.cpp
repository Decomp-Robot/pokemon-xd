#include <app/floor/floorLoad.h>
#include <GScamera.h>
#include <GSmodel.h>
#include <GSres.h>
#include <HSDArchive.h>
#include <scene.h>

static bool _unloadCamera(void *camera, unsigned long group, unsigned long resource);
static bool _unloadModel(void *model, unsigned long group, unsigned long resource);

extern "C" void loadCamera(HSD_Archive *archive, unsigned long group, unsigned long resource)
{
    if (archive != 0) {
        _HSD_SceneDesc *scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, "scene_data");
        if (scene != 0) {
            GScamera *camera = GScameraLoad(scene->mCamera);
            GSresRegisterResource(camera, group, resource, _unloadCamera);
        }
    }
}

extern "C" void loadModel(void *archiveData, unsigned long group,
                          unsigned long modelIndex, unsigned long resource)
{
    HSD_Archive *archive = (HSD_Archive *)archiveData;
    if (archive == 0) {
        return;
    }
    _HSD_SceneDesc *scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, "scene_data");
    if (scene == 0) {
        return;
    }
    GSboundAnimHeader *bounds = modelIndex == 0
        ? (GSboundAnimHeader *)HSD_ArchiveGetPublicAddress(archive, "bound_box") : 0;
    for (unsigned long index = 0; scene->mModels[index] != 0; index++) {
        if (index == modelIndex) {
            GSmodel *model = GSmodelLoad(scene->mModels[index], bounds);
            GSresRegisterResource(model, group, resource, _unloadModel);
            break;
        }
    }
}

static bool _unloadCamera(void *camera, unsigned long, unsigned long)
{
    GScameraFree((GScamera *)camera);
    return true;
}

static bool _unloadModel(void *model, unsigned long, unsigned long)
{
    GSmodelFree((GSmodel *)model);
    return true;
}

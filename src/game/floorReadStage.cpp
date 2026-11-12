#include <floor.h>
#include <floorBios.h>
#include <floorBgmMask.h>
#include <GSres.h>
#include <GSlight.h>
#include <GSmsg.h>
#include <scene.h>
#include <sound.h>

_HSD_SceneDesc *_makeArchiveHeader(u32 group, u32 res, u32 size);
void _registerCamera(u32 group, u32 res, _HSD_SceneDesc *scene);
bool _unloadObject(void *data, u32 group, u32 res);
bool _unloadLight(void *data, u32 group, u32 res);
bool _unloadScript(void *data, u32 group, u32 res);

extern "C" void *floorReadCameraPostFunc(u32 group, u32 res, u32 size)
{
    _HSD_SceneDesc *scene = _makeArchiveHeader(group, floorReadMakeObjResID(res), size);
    _registerCamera(group, res, scene);
    return scene;
}

extern "C" void *floorReadCameraPreFunc(u32 group, u32 res, u32 size)
{
    u8 *data = (u8 *)GSresAllocResourceAlign(
        ((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1)) +
            ((sizeof(HSD_Archive) + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1)),
        FLOOR_READ_ALIGN, group, floorReadMakeObjResID(res), 0);
    if (data == 0) {
        return 0;
    }
    GSresLockResource(group, res);
    return data + ((sizeof(HSD_Archive) + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1));
}

extern "C" void *floorReadObjPostFunc(u32 group, u32 res, u32 size)
{
    _HSD_SceneDesc *scene;
    u32 index = 0;
    scene = _makeArchiveHeader(group, res, size);
    if (scene == 0) {
        return 0;
    }
    u32 modelRes = floorReadMakeModelResID(res);
    if (scene->mModels != 0) {
        while (scene->mModels[index] != 0) {
            GSresRegisterResource(scene->mModels[index], group, modelRes | index, 0);
            index++;
        }
    }
    return scene;
}

extern "C" void *floorReadObjPreFunc(u32 group, u32 res, u32 size)
{
    u8 *data = (u8 *)GSresAllocResourceAlign(
        ((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1)) +
            ((sizeof(HSD_Archive) + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1)),
        FLOOR_READ_ALIGN, group, res, _unloadObject);
    if (data == 0) {
        return 0;
    }
    GSresLockResource(group, res);
    return data + ((sizeof(HSD_Archive) + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1));
}

extern "C" void *floorReadMapPostFunc(u32 group, u32 res, u32 size)
{
    _HSD_SceneDesc *scene;
    u32 modelIndex = 0;
    u32 modelRes;
    u32 lightRes;
    u32 lightIndex = 0;
    GSlight *light;
    scene = _makeArchiveHeader(group, res, size);
    if (scene == 0) {
        return 0;
    }
    if (scene->mModels != 0) {
        modelRes = floorReadMakeModelResID(res);
        while (scene->mModels[modelIndex] != 0) {
            GSresRegisterResource(scene->mModels[modelIndex], group, modelRes | modelIndex, 0);
            modelIndex++;
        }
    }
    if (scene->mLights != 0) {
        lightRes = floorReadMakeLightResID(res);
        while (scene->mLights[lightIndex] != 0) {
            light = GSlightLoad(scene->mLights[lightIndex]);
            if (light != 0) {
                GSresRegisterResource(light, group, lightRes | lightIndex, _unloadLight);
            }
            lightIndex++;
        }
        floorSetLightInfo(lightRes, lightIndex);
    } else {
        floorSetLightInfo(0, 0);
    }
    _registerCamera(group, floorReadMakeCameraResID(res), scene);
    if (scene->mFogs != 0 && scene->mFogs[0] != 0) {
        GSresRegisterResource(scene->mFogs[0], group, floorReadMakeFogResID(res), 0);
    }
    floorInitMap(group, res);
    return scene;
}

extern "C" void *floorReadMapPreFunc(u32 group, u32 res, u32 size)
{
    u8 *data = (u8 *)GSresAllocResourceAlign(
        ((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1)) +
            ((sizeof(HSD_Archive) + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1)),
        FLOOR_READ_ALIGN, group, res, 0);
    if (data == 0) {
        return 0;
    }
    floorDataBiosSetMapResID(floorDataBiosGetCurrentPtr(), res);
    GSresLockResource(group, res);
    return data + ((sizeof(HSD_Archive) + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1));
}

extern "C" void *floorReadScriptPostFunc(u32 group, u32 res, u32 size)
{
    GSresUnlockResource(group, res);
    char *data = (char *)GSresGetResource(group, res);
    if (!floorIsPop() && data != 0) {
        floorMappingScript(data);
        if (!floorSound_playEnvSound(0)) {
            soundStopEnv(0);
        }
        floorExecSoundScript();
    }
    return data;
}

extern "C" void *floorReadScriptPreFunc(u32 group, u32 res, u32 size)
{
    if (floorIsPop()) {
        return 0;
    }
    void *data = GSresAllocResourceAlign((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1),
                                        FLOOR_READ_ALIGN, group, res, _unloadScript);
    GSresLockResource(group, res);
    return data;
}

extern "C" void *floorReadFontPostFunc(u32 group, u32 res, u32 size)
{
    if (floorIsPop()) {
        return 0;
    }
    GSresUnlockResource(group, res);
    void *data = GSresGetResource(group, res);
    if (data != 0) {
        GSmsgFontOpen(data);
    }
    return data;
}

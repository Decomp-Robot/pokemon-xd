#include <floorPostCall.h>
#include <floor.h>
#include <floorBios.h>
#include <GSres.h>
#include <GSparticle.h>
#include <GSmodel.h>
#include <scene.h>
#include <wazaSequence.h>
#include <ModelSequence.h>

extern FloorPostCallManager _postFuncManager;

extern "C" char floorSceneDataName[];
bool _unloadParticles(void *data, u32 group, u32 res);
bool _unloadWZX(void *data, u32 group, u32 res);
bool _unloadPKX(void *data, u32 group, u32 res);

extern "C" void *floorReadNotLinkedParticlePostFunc(u32 group, u32 res, u32 size)
{
    GSresUnlockResource(group, res);
    void *data = GSresGetResource(group, floorReadMakeObjResID(res));
    _GSparticleBank *bank = GSparticleLoad(data);
    if (bank != 0) {
        GSresRegisterResource(bank, group, res, _unloadParticles);
    }
    return data;
}

extern "C" void *floorReadNotLinkedParticlePreFunc(u32 group, u32 res, u32 size)
{
    void *data = GSresAllocResourceAlign((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1),
                                        FLOOR_READ_ALIGN, group, floorReadMakeObjResID(res), 0);
    GSresLockResource(group, res);
    return data;
}

extern "C" void *floorReadParticlePostFunc(u32 group, u32 res, u32 size)
{
    void *data;
    _GSparticleBank *bank;
    u32 index = 0;
    RoomData *floorData = floorDataBiosGetCurrentPtr();
    HSD_Archive *archive = (HSD_Archive *)GSresGetResource(floorGetCurrentGroupID(), floorData->mapResID);
    if (archive == 0) {
        _postFuncManager.add(group, res, size, floorReadParticlePostFunc);
        return 0;
    }

    GSresUnlockResource(group, res);
    data = GSresGetResource(group, floorReadMakeObjResID(res));
    bank = GSparticleLoad(data);
    if (bank != 0) {
        GSresRegisterResource(bank, group, res, _unloadParticles);
        RoomData *floorData = floorDataBiosGetCurrentPtr();
        HSD_Archive *archive = (HSD_Archive *)GSresGetResource(floorGetCurrentGroupID(), floorData->mapResID);
        if (archive == 0) {
            return 0;
        }
        _HSD_SceneDesc *scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, floorSceneDataName);
        if (scene == 0) {
            return 0;
        }
        if (scene->mModels != 0) {
            u32 modelRes = floorReadMakeModelResID(floorData->mapResID);
            while (scene->mModels[index] != 0) {
                _GSmodel *model = (_GSmodel *)GSresGetResource(floorGetCurrentGroupID(), modelRes | index);
                if (model != 0) {
                    GSmodelLinkToGSparticleBank(model, bank);
                }
                index++;
            }
        }
    }
    return data;
}

extern "C" void *floorReadParticlePreFunc(u32 group, u32 res, u32 size)
{
    void *data = GSresAllocResourceAlign((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1),
                                        FLOOR_READ_ALIGN, group, floorReadMakeObjResID(res), 0);
    GSresLockResource(group, res);
    return data;
}

extern "C" void *floorReadWZXPostFunc(u32 group, u32 res, u32 size)
{
    GSresUnlockResource(group, res);
    u8 *data = (u8 *)GSresGetResource(group, res);
    if (data == 0) {
        return 0;
    }
    if (WazaSequence::Load(data, group, res)) {
        return data;
    }
    return 0;
}

extern "C" void *floorReadWZXPreFunc(u32 group, u32 res, u32 size)
{
    void *data = GSresAllocResourceAlign((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1),
                                        FLOOR_READ_ALIGN, group, res, _unloadWZX);
    GSresLockResource(group, res);
    return data;
}

extern "C" void *floorReadPKXPostFunc(u32 group, u32 res, u32 size)
{
    GSresUnlockResource(group, res);
    u8 *data = (u8 *)GSresGetResource(group, res);
    if (data == 0) {
        return 0;
    }
    if (ModelSequence_Load(data, group, res)) {
        return data;
    }
    return 0;
}

extern "C" void *floorReadPKXPreFunc(u32 group, u32 res, u32 size)
{
    void *data = GSresAllocResourceAlign((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1),
                                        FLOOR_READ_ALIGN, group, res, _unloadPKX);
    if (data == 0) {
        return 0;
    }
    GSresIsUniqueID(group, res, false);
    GSresLockResource(group, res);
    return data;
}

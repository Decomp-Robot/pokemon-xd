// SDK header for the alarm and thread types the loader of the stage archives
// runs on.
#include <dolphin/os.h>

#include <types.h>
#include <GSres.h>
#include <GSfsys.h>
#include <GSlight.h>
#include <floorPostCall.h>
#include <ant.h>

// Floor/map resource loading. Format-specific pre/post callbacks allocate
// resource memory and initialize DECK, ANT, GSW and relative-link data.
// The floor-script update loop runs callbacks collected in _postFuncManager.
// Reads wait for any other thread loading the same archive.

// Alignment every archive of a stage is loaded with, and the unit its length
// is rounded up to.
#define FLOOR_READ_ALIGN 0x20

// The pending-callback manager of the floor subsystem.
extern FloorPostCallManager _postFuncManager;

// Scene description the public data of a stage archive carries, with the
// table of the lights of the stage at 0x08.
struct _HSD_SceneDesc {
    u8 sceneHeader[0x08];
    void **lights; // table of light pointers, the list ending in a null pointer
};

// Camera and ant helpers of the stage modules the reader hands resources to;
// both are C++ functions of the game.
void _registerCamera(unsigned long group, unsigned long res, _HSD_SceneDesc *scene);
void *antGetGStexture(void *resource);
bool _unloadLight(void *light, unsigned long group, unsigned long res);

extern "C" {

// Light and filesystem subsystems the reader hands resources over to.
extern "C" void floorAddResource(u32 group, u32 res);


// Public data of an archive of a stage; the scene description it starts with
// lists the lights of the stage.
extern "C" void *HSD_ArchiveGetPublicAddress(void *archive, const char *symbol);
// Name the public data of a stage archive carries its scene description under;
// the camera and the light registration both look their part up in it.
extern char floorSceneDataName[];
extern "C" u32 floorReadMakeCameraResID(u32 res);
extern "C" u32 floorReadMakeLightResID(u32 res);

// Archive formats a stage reads in: the DECK data of the stage floor, a
// texture of its ANT Pokemon and a GSW file.
extern "C" void deckRegister(u32 group, u32 res);
extern "C" void deckUnRegister(u32 group, u32 res);
extern "C" void GStextureLoad(void *texture);
extern "C" void GStextureFree(void *texture);
extern "C" void FlashSystemInit(u32 res, void *resource);

// The stage archive the fight runs in.
extern "C" u32 getCurrentMapGroupId(void);
extern "C" void GSthreadSwitch(void);

// Set while the reader of this module is loading a resource, so that a second
// thread asking for the same stage waits for the read to run out.
extern "C" void floorDelayPostFunc(void);

}

// The callback the IDEX read of floorReadResourceID hands over; it is named
// before the read that passes it.
void _fsysLoadCallbak(long requestType, void *context, long callbackArgument);

// The busy flag of the reader, set while a resource of the stage archive is
// loading.
struct FloorReadBusy {
    u8 busy;
    u8 padding[0x7];
};

extern FloorReadBusy floorReadBusyFlag;

// Release callbacks of the resource system: an archive of a stage, its DECK
// data, its ANT textures and its GSW file give themselves back
// through these once the fight is done with them. The reader hands them the
// resource together with the group id and resource id it is registered under.
bool _unloadDeckData(void *data, u32 group, u32 res);
bool _unloadTextureForAnt(void *data, u32 group, u32 res);
bool _unloadGSW(void *data, u32 group, u32 res);

// floorDelayPostFunc(void): runs the callbacks the floor subsystem collected
// during the frame; the fight loop calls it once a frame.
extern "C" void floorDelayPostFunc(void)
{
    _postFuncManager.update();
}

// floorReadResourceID(u32, u32): pulls one resource of the stage archive into
// memory and gives it back, so that the fight can run out of an archive that
// has not been read yet. A group id of 0 means the stage the fight is running
// in.
extern "C" void *floorReadResourceID(u32 group, u32 res)
{
    if (group == 0) {
        group = getCurrentMapGroupId();
    }
    // A resource that is already in memory needs no read; only a missing one
    // goes through the wait below.
    void *resource = GSresGetResource(group, res);
    if (resource != 0) {
        return resource;
    }
    if (floorReadBusyFlag.busy != 0) {
        while (floorReadBusyFlag.busy != 0) {
            GSthreadSwitch();
        }
    }
    floorReadBusyFlag.busy = 1;
    // Retry unavailable or failed reads, yielding between attempts.
    while (!GSfsysReadIDEXWait(group, res, _fsysLoadCallbak, 0, res)) {
        GSthreadSwitch();
    }
    GSfsysFree(group);
    floorAddResource(group, res);
    resource = GSresGetResource(group, res);
    floorReadBusyFlag.busy = 0;
    return resource;
}

// An archive of a stage needs no additional work when its read completes.
void _fsysLoadCallbak(long requestType, void *context, long callbackArgument)
{
}

// floorRegisterCamera(u32, u32): registers the camera of a stage out of the
// scene description its archive carries.
extern "C" void *floorRegisterCamera(u32 group, u32 res)
{
    void *archive;
    _HSD_SceneDesc *scene;

    archive = GSresGetResource(group, res);
    if (archive == 0) {
        return 0;
    }
    scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, floorSceneDataName);
    if (scene == 0) {
        return 0;
    }
    _registerCamera(group, floorReadMakeCameraResID(res), scene);
    return scene;
}

// floorRegisterLight(u32, u32): registers every light the scene description of a
// stage lists, one resource id per light.
extern "C" void *floorRegisterLight(u32 group, u32 res)
{
    void *archive;
    _HSD_SceneDesc *scene;
    void *light;
    u32 index = 0;
    u32 baseRes;

    archive = GSresGetResource(group, res);
    if (archive == 0) {
        return 0;
    }
    scene = (_HSD_SceneDesc *)HSD_ArchiveGetPublicAddress(archive, floorSceneDataName);
    if (scene == 0) {
        return 0;
    }
    if (scene->lights != 0) {
        baseRes = floorReadMakeLightResID(res);
        while ((light = scene->lights[index]) != 0) {
            void *loaded = GSlightLoad(light);

            if (loaded != 0) {
                GSresRegisterResource(loaded, group, baseRes | index, _unloadLight);
            }
            index++;
        }
    }
    return scene;
}

// floorReadDeckPostFunc(u32, u32): hands the DECK data that was read into
// memory over to the deck subsystem.
extern "C" void *floorReadDeckPostFunc(u32 group, u32 res)
{
    void *resource;

    GSresUnlockResource(group, res);
    resource = GSresGetResource(group, res);
    if (resource != 0) {
        deckRegister(group, res);
    }
    return resource;
}

// floorReadDeckPreFunc(u32, u32, u32): hands the resource system the memory
// DECK data is read into, together with the callback that gives it back.
extern "C" void *floorReadDeckPreFunc(u32 group, u32 res, u32 size)
{
    void *buffer;

    buffer = GSresAllocResourceAlign((size + (FLOOR_READ_ALIGN - 1)) & ~(FLOOR_READ_ALIGN - 1),
                                     FLOOR_READ_ALIGN, group, res,
                                     (GSresFreeFunc)_unloadDeckData);
    GSresLockResource(group, res);
    return buffer;
}

// _unloadDeckData(void *, u32, u32): releases the DECK data of the stage
// archive.
bool _unloadDeckData(void *data, u32 group, u32 res)
{
    deckUnRegister(group, res);
    return true;
}

// floorReadAntPostFunc(u32, u32, u32): turns the ANT resource that was read
// in into the texture the Pokemon renderer draws with.
extern "C" void *floorReadAntPostFunc(u32 group, u32 res, u32 size)
{
    void *resource;

    GSresUnlockResource(group, res);
    resource = GSresGetResource(group, res);
    antReadPostFunc(resource, size);
    GStextureLoad(antGetGStexture(resource));
    return resource;
}

// floorReadAntPreFunc(u32, u32, u32): hands the resource system the memory an
// ANT texture is read into, and the callback that gives it back.
extern "C" void *floorReadAntPreFunc(u32 group, u32 res, u32 size)
{
    void *buffer;

    buffer = GSresAllocResourceAlign((size + (FLOOR_READ_ALIGN - 1)) & ~(FLOOR_READ_ALIGN - 1),
                                     FLOOR_READ_ALIGN, group, res,
                                     (GSresFreeFunc)_unloadTextureForAnt);
    GSresLockResource(group, res);
    return buffer;
}

// _unloadTextureForAnt(void *, u32, u32): releases the ANT texture the stage
// archive was read into.
bool _unloadTextureForAnt(void *data, u32 group, u32 res)
{
    GStextureFree(antGetGStexture(data));
    return true;
}

// floorReadGSWPostFunc(u32, u32): hands a GSW file that was read into memory
// over to the flash filesystem.
extern "C" void *floorReadGSWPostFunc(u32 group, u32 res)
{
    void *resource;

    GSresUnlockResource(group, res);
    resource = GSresGetResource(group, res);
    FlashSystemInit(res, resource);
    return resource;
}

// floorReadGSWPreFunc(u32, u32, u32): hands the resource system the memory a GSW
// file is read into, and the callback that gives it back.
extern "C" void *floorReadGSWPreFunc(u32 group, u32 res, u32 size)
{
    void *buffer;

    buffer = GSresAllocResourceAlign((size + (FLOOR_READ_ALIGN - 1)) & ~(FLOOR_READ_ALIGN - 1),
                                     FLOOR_READ_ALIGN, group, res,
                                     (GSresFreeFunc)_unloadGSW);
    GSresLockResource(group, res);
    return buffer;
}

// floorReadEncodeRelPostFunc(u32, u32, u32): turns the relative links of a
// stage into the absolute ones the resource system works with.
extern "C" void *floorReadEncodeRelPostFunc(u32 group, u32 res, u32 size)
{
    void *resource;

    GSresUnlockResource(group, res);
    resource = GSresGetResource(group, res);
    GSfsysOSLinkPostFunc(group, res, size);
    return resource;
}

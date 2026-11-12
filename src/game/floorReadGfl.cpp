#include <floor.h>
#include <GSres.h>
#include <GSlensFlare.h>

bool _unloadFlare(void *data, u32 group, u32 res);

extern "C" void *floorReadGFLPostFunc(u32 group, u32 res, u32 size)
{
    GSresUnlockResource(group, res);
    GSresGetResource(group, res);
    void *data = GSresGetResource(group, floorReadMakeObjResID(res));
    u32 flare = GSlensFlareLoad(data);
    if (flare != 0) {
        GSresRegisterResource((void *)flare, group, res, _unloadFlare);
    }
    return data;
}

extern "C" void *floorReadGFLPreFunc(u32 group, u32 res, u32 size)
{
    u32 dataRes = floorReadMakeObjResID(res);
    void *data = GSresAllocResourceAlign(
        (size + (FLOOR_READ_ALIGN - 1)) & ~(FLOOR_READ_ALIGN - 1),
        FLOOR_READ_ALIGN, group, dataRes, 0);
    GSresLockResource(group, res);
    return data;
}

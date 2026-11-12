#include <types.h>
#include <GSres.h>
#include <GSmsg.h>
#include <floor.h>
#include <pokecolo.h>

bool _unloadFont(void *data, u32 group, u32 res);
bool _unloadMsg(void *data, u32 group, u32 res);

extern "C" void *floorReadFontPreFunc(u32 group, u32 res, u32 size)
{
    if (floorIsPop()) {
        return 0;
    }
    void *data = GSresAllocResourceAlign((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1),
                                         FLOOR_READ_ALIGN, group, res, _unloadFont);
    GSresLockResource(group, res);
    return data;
}

extern "C" void *floorReadMsgPostFunc(u32 group, u32 res, u32 size)
{
    if (floorIsPop()) {
        return 0;
    }
    GSresUnlockResource(group, res);
    void *data = GSresGetResource(group, res);
    if (GSmsgGetDataLanguage(data) != pokecoloGetLanguage()) {
        GSresFreeResource(group, res);
        return data;
    }
    if (data != 0) {
        GSmsgOpen(data);
    }
    return data;
}

extern "C" void *floorReadMsgPreFunc(u32 group, u32 res, u32 size)
{
    if (floorIsPop()) {
        return 0;
    }
    void *data = GSresAllocResourceAlign((size + FLOOR_READ_ALIGN - 1) & ~(FLOOR_READ_ALIGN - 1),
                                         FLOOR_READ_ALIGN, group, res, _unloadMsg);
    GSresLockResource(group, res);
    return data;
}

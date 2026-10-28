// The texture and the collision data of a stage are read like any other
// archive of a stage. The texture system and the collision system each get the
// memory their file is read into, and the file itself once the read completed.

#include <GScolsys2.h>
#include <GSres.h>
#include <GStexture.h>
#include <floor.h>
#include <types.h>

// Release callbacks the reader hands the resource system along with the
// texture and the collision file of a stage.
bool _unloadTexture(void *data, u32 group, u32 res);
bool _unloadColsys(void *data, u32 group, u32 res);

// floorReadTexPostFunc(u32, u32, u32): hands the texture that was read to the
// texture system and returns the resource the read was registered under.
extern "C" void *floorReadTexPostFunc(u32 group, u32 res, u32 size)
{
    void *resource;

    GSresUnlockResource(group, res);
    resource = GSresGetResource(group, res);
    GStextureLoad(resource);
    return resource;
}

// floorReadTexPreFunc(u32, u32, u32): hands the resource system the memory a
// texture is read into, and the callback that gives it back.
extern "C" void *floorReadTexPreFunc(u32 group, u32 res, u32 size)
{
    void *buffer;

    buffer = GSresAllocResourceAlign((size + (FLOOR_READ_ALIGN - 1)) & ~(FLOOR_READ_ALIGN - 1),
                                     FLOOR_READ_ALIGN, group, res, _unloadTexture);
    GSresLockResource(group, res);
    return buffer;
}

// floorReadColPostFunc(u32, u32, u32): hands the collision file that was read
// to the collision system and returns the resource the read was registered
// under.
extern "C" void *floorReadColPostFunc(u32 group, u32 res, u32 size)
{
    void *resource;

    GSresUnlockResource(group, res);
    resource = GSresGetResource(group, res);
    GScolsys2LoadCCD(resource);
    return resource;
}

// floorReadColPreFunc(u32, u32, u32): hands the resource system the memory the
// collision file is read into, and the callback that gives it back.
extern "C" void *floorReadColPreFunc(u32 group, u32 res, u32 size)
{
    void *buffer;

    buffer = GSresAllocResourceAlign((size + (FLOOR_READ_ALIGN - 1)) & ~(FLOOR_READ_ALIGN - 1),
                                     FLOOR_READ_ALIGN, group, res, _unloadColsys);
    GSresLockResource(group, res);
    return buffer;
}

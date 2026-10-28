// The release callbacks that give the textures and the collision file of a
// stage back to the system that owns them when their resource is released,
// and the ids the reader derives for the fixed resources of a stage.

#include <bool.h>
#include <floor.h>
#include <GScolsys2.h>
#include <GStexture.h>
#include <types.h>

// _unloadColsys(void *, u32, u32): gives the collision file of a stage back to
// the collision system when the resource is released.
bool _unloadColsys(void *data, u32 group, u32 res)
{
    GScolsys2UnloadCCD();
    return true;
}

// _unloadTexture(void *, u32, u32): gives a texture of a stage back to the
// texture system when the resource is released.
bool _unloadTexture(void *data, u32 group, u32 res)
{
    GStextureFree((struct GStexture *)data);
    return true;
}

// The resource ids of the fixed resources of a stage: the id the stage lists
// them under keeps the group it belongs to in its upper half and the resource
// number in its lower one.

u32 floorReadMakeFogResID(u32 resourceID)
{
    return (resourceID & 0x7FFF0000) | 0x1A00;
}

u32 floorReadMakeCameraResID(u32 resourceID)
{
    return (resourceID & 0x7FFF0000) | 0x1800;
}

u32 floorReadMakeLightResID(u32 resourceID)
{
    return (resourceID & 0x7FFF0000) | 0x1600;
}

u32 floorReadMakeModelResID(u32 resourceID)
{
    return (resourceID & 0x7FFF0000) | 0x1000;
}

u32 floorReadMakeObjResID(u32 resourceID)
{
    return (resourceID & 0x7FFF0000) | 0x0400;
}

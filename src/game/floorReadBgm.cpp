// The music of a stage is read through the plain read of the stage reader,
// and needs no work of its own once it is in memory.

#include <types.h>

// The plain read of the stage reader, which every archive of a stage that
// needs no work of its own when it is in memory is read with.
extern "C" void *floorReadNormalPreFunc(u32 group, u32 res, u32 size);

// floorReadBGMPostFunc(u32, u32, u32): the music of a stage needs no work
// once its read completes, and the resource is not touched.
extern "C" void *floorReadBGMPostFunc(u32 group, u32 res, u32 size)
{
    return 0;
}

// floorReadBGMPreFunc(u32, u32, u32): the music is read like any other
// archive of a stage, so the plain read of the reader allocates it.
extern "C" void *floorReadBGMPreFunc(u32 group, u32 res, u32 size)
{
    return floorReadNormalPreFunc(group, res, size);
}

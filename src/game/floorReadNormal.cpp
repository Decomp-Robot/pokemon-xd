// The plain read of the stage reader, used by every archive of a stage that
// needs no work of its own once it is in memory.

#include <GSres.h>
#include <floor.h>
#include <types.h>

// floorReadNormalPreFunc(u32, u32, u32): hands the resource system the memory
// an archive of a stage is read into, rounded up to the alignment every
// archive of a stage is read in with. Such an archive needs no work when the
// read completes, so no release callback is handed over and the resource is
// not locked here.
extern "C" void *floorReadNormalPreFunc(u32 group, u32 res, u32 size)
{
    return GSresAllocResourceAlign((size + (FLOOR_READ_ALIGN - 1)) & ~(FLOOR_READ_ALIGN - 1),
                                   FLOOR_READ_ALIGN, group, res, 0);
}

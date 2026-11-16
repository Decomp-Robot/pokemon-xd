#include <hsd/assert.h>
#include <hsd/memory.h>

/* Allocation callbacks in use. HSD_OSInit installs the defaults, a caller
 * may replace them with _HSD_MemSetCallbacks. */
HSD_MemCallbacks __mem_cb;

void *HSD_Alloc(u32 size)
{
    return __mem_cb.alloc(size, 32, 0);
}

void HSD_Free(void *ptr)
{
    __mem_cb.free(ptr);
}

s32 _HSD_MemCheckOwn(void *memory)
{
    return __mem_cb.check_own(memory);
}

void _HSD_MemSetCallbacks(HSD_MemCallbacks *callbacks, u32 size)
{
    HSD_ASSERTLINE(19, size == sizeof(__mem_cb));
    __mem_cb = *callbacks;
}

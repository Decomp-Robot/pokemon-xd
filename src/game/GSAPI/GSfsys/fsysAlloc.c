#include <GSAPI/GSfsys/fsys.h>
#include <GSAPI/GSmemman/GSmemman.h>
#include <GSres.h>
#include <string.h>

#define FSYS_ALLOC_ALIGNMENT 32
#define FSYS_CACHE_FREE_MARGIN 524288
#define FSYS_LOCAL_HEAP_SIZE 327680

s32 _fsysLocalHeap;

void* _fsysGetResource(u32 size, u32 groupID, u32 resourceID)
{
    return GSresAllocResourceAlign(size, FSYS_ALLOC_ALIGNMENT, groupID, resourceID, NULL);
}

u8 _fsysMemFreeCacheGroup(u32 groupID)
{
    FsysCacheManage* cache = _fsysGetCacheManageGroup(groupID);
    if (cache == NULL) {
        return FALSE;
    }
    return _fsysFreeCache(cache);
}

void _fsysMemFreeCache(u32 cacheAddress)
{
    _fsysFreeCacheAddr(cacheAddress);
}

void* _fsysMemAllocCache(FsysFile* file, u32 cacheIndex, u32 size, u32 priority)
{
    void* address;
    u32 alignedSize = (size + FSYS_ALLOC_ALIGNMENT - 1) & ~(FSYS_ALLOC_ALIGNMENT - 1);
    _fsysFreeFreshCache(file->groupId, alignedSize);
    for (;;) {
        address = _fsysAllocCache(file, cacheIndex, alignedSize, priority);
        if (address != NULL) {
            break;
        }
        if (!_fsysFreeFreshCacheGroup(file->groupId)) {
            return NULL;
        }
    }
    return address;
}

u8 _fsysFreeFreshCacheGroup(u32 groupID)
{
    u32 priority;
    u8 freed = FALSE;
    FsysCacheManage* cache;
    for (;;) {
        cache = _fsysGetFreshCacheManage(groupID, TRUE);
        if (cache == NULL) {
            return freed;
        }
        if (freed == TRUE && priority != cache->priority) {
            break;
        }
        priority = cache->priority;
        freed = TRUE;
        if (!_fsysFreeCache(cache)) {
            return FALSE;
        }
    }
    return freed;
}

u8 _fsysFreeFreshCache(u32 groupID, u32 size)
{
    FsysCacheManage* cache;
    for (;;) {
        if (size < _fsysGetFreeCacheHeapSize() - FSYS_CACHE_FREE_MARGIN) {
            break;
        }
        cache = _fsysGetFreshCacheManage(groupID, FALSE);
        if (cache == NULL) {
            return FALSE;
        }
        if (!_fsysFreeCache(cache)) {
            return FALSE;
        }
    }
    return TRUE;
}

u8 _fsysFreeCacheAddr(u32 cacheAddress)
{
    FsysCacheManage* cache = _fsysGetCacheManageAddr(cacheAddress);
    if (cache == NULL) {
        return FALSE;
    }
    return _fsysFreeCache(cache);
}

u8 _fsysFreeCache(FsysCacheManage* cache)
{
    u32 address = cache->aramAddress;
    u32 size = cache->size;
    _fsysDelCacheManage(cache);
    _fsysFreeCacheFromHeap((void*)address, size);
    return TRUE;
}

void* _fsysAllocCache(FsysFile* file, u32 cacheIndex, u32 size, u32 priority)
{
    FsysCacheManage* cache = _fsysGetCacheManage(file->groupId, cacheIndex);
    if (cache == NULL) {
        u8 keepCache = file->keepCache;
        FsysHeader* header = _fsysGetHeader(file);
        if (header != NULL && (header->flags & FSYS_HEADER_KEEP_CACHE)) {
            keepCache = TRUE;
        }
        cache = _fsysGetNewCacheManage(file->groupId, cacheIndex, keepCache, TRUE, priority);
        if (cache == NULL) {
            return NULL;
        }
        cache->size = size;
        cache->aramAddress = (u32)_fsysAllocCacheFromHeap(cache->size);
        if (cache->aramAddress == 0) {
            _fsysDelCacheManage(cache);
            return NULL;
        }
    }
    return (void*)cache->aramAddress;
}

void _fsysMemFreeHeader(FsysFile* file)
{
    FsysHeader* header;
    s32 interrupts = OSDisableInterrupts();
    header = file->header;
    file->header = NULL;
    file->present = FALSE;
    OSRestoreInterrupts(interrupts);
    _fsysFree(header);
}

u8 _fsysMemAllocHeader(FsysFile* file)
{
    if (file->header != NULL) {
        return FALSE;
    }
    file->present = FALSE;
    file->header = _fsysAlloc((file->dvdEntry->tableOffset + FSYS_ALLOC_ALIGNMENT - 1) &
                              ~(FSYS_ALLOC_ALIGNMENT - 1));
    if (file->header == NULL) {
        return FALSE;
    }
    memset(file->header, 0, sizeof(FsysHeader));
    memcpy(file->header, "INVA", 4);
    DCFlushRange(file->header, sizeof(FsysHeader));
    return TRUE;
}

u8 GSfsysCreateLocalHeap(void)
{
    void* start = OSGetArenaLo();
    void* end = (u8*)start + FSYS_LOCAL_HEAP_SIZE;
    _fsysLocalHeap = GSmemCreateHeap(start, end);
    if (_fsysLocalHeap == -1) {
        return FALSE;
    }
    OSSetArenaLo(end);
    return TRUE;
}

u8 _fsysFree(void* memory)
{
    GSmemFreeHeap(_fsysLocalHeap, memory);
    return TRUE;
}

void* _fsysAlloc(u32 size)
{
    return GSmemAllocHeap(_fsysLocalHeap, size);
}

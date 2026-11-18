/*
 * fsysCache.c
 *
 * The ARAM cache of the file system: the per-file cache descriptors, the
 * cache heap they are allocated from and the ARQ transfers that fill them.
 */

#include <GSAPI/GSfsys/fsys.h>
#include <GSAPI/GSmemman/GSmemman.h>

/* Cache heap sizes, cursors and tables, with the ARAM range and heap they
 * are allocated from. */
u32 _fsysCACHESize;
u32 _fsysCACHEManageSize;
u32 _fsysCACHEManageIndex;
u32 _fsysCACHEuniqueGroupID;
u32 _fsysCACHEuniqueID;
u32 _fsysCACHEFreeBytes;
u32 _fsysARQTransSize;
u32 _fsysARQTransIndex;
s32 _fsysAmemHeap;
u32 _fsysAmemTopAddr;
FsysArq* _fsysARQTransRoot;
FsysArq* _fsysARQTrans;
FsysCacheManage* _fsysCACHEManageRoot;
FsysCacheManage* _fsysCACHEManage;

/* Cell table the external cache heap is created with: FSYS_HEAP_TABLE_ENTRIES
 * GSmemCell entries. */
u8 _fsysAmemCell[0x2800];

/*! Queue a write of `size` bytes from `buffer` into ARAM. */
void _fsysWriteCache(FsysFile* file, FsysResHeader* header, void* buffer, u32 offset, u32 size,
                     u32 offsetAdjust, FsysArqCallback callback)
{
    FsysArq* arq = _fsysGetNewArq();
    u32 priority;

    if (!(arq != 0)) { FSYS_ASSERT(arq != 0); return; }
    arq->file = file;
    arq->header = header;
    arq->callback = callback;

    priority = (file->fileMode == FSYS_FILE_TYPE_STREAM) ? 1 : 0;
    DCFlushRange(buffer, size);
    ARQPostRequest(&arq->request, file->groupId, ARQ_TYPE_MRAM_TO_ARAM, priority, (u32)buffer,
                   offset + offsetAdjust, size, (ARQCallback)_fsysARQCallbackWrite);
}

/*! Read a resource, telling the caller when the data has landed. */
u8 _fsysReadCacheEX(FsysFile* file, FsysResHeader* header, void* buffer, u32 size, u32 offset,
                    FsysArqCallback callback, FsysStreamCallback userCallback, void* context, long argument)
{
    return _fsysReadCacheExec(file, header, buffer, size, offset, callback, userCallback, context, argument);
}

/*! Read a resource without a caller supplied completion handler. */
u8 _fsysReadCache(FsysFile* file, FsysResHeader* header, void* buffer, u32 size, u32 offset,
                  FsysArqCallback callback)
{
    return _fsysReadCacheExec(file, header, buffer, size, offset, callback, 0, 0, 0);
}

/*! Queue a read of `size` bytes from ARAM into `buffer`. */
u8 _fsysReadCacheExec(FsysFile* file, FsysResHeader* header, void* buffer, u32 size, u32 offset,
                      FsysArqCallback callback, FsysStreamCallback userCallback, void* context, long argument)
{
    FsysCacheManage* cache = _fsysGetCacheManage(file->groupId, file->cacheIndex);
    FsysArq* arq;
    u32 priority;

    if (!(cache != 0)) { FSYS_ASSERT(cache != 0); return 0; }
    arq = _fsysGetNewArq();
    if (!(arq != 0)) { FSYS_ASSERT(arq != 0); return 0; }

    arq->file = file;
    arq->header = header;
    arq->callback = callback;
    arq->userCallback = userCallback;
    arq->context = context;
    arq->argument = argument;

    priority = (file->fileMode == FSYS_FILE_TYPE_STREAM) ? 1 : 0;
    if (offset + size > cache->size) {
        size = cache->size - offset;
    }
    /* DMA buffers and lengths must be aligned. */
    if ((((u32)buffer & (ARQ_DMA_ALIGNMENT - 1)) == 0) && ((size & (ARQ_DMA_ALIGNMENT - 1)) == 0)) {
        DCInvalidateRange(buffer, size);
        arq->buffer = buffer;
        arq->length = size;
        ARQPostRequest(&arq->request, file->groupId, ARQ_TYPE_ARAM_TO_MRAM, priority,
                       cache->aramAddress + offset, (u32)buffer, size,
                       (ARQCallback)_fsysARQCallbackRead);
        return 1;
    }
}

/*! ARQ completion handler of a write. */
void _fsysARQCallbackWrite(ARQRequest* request)
{
    FsysArq* arq = _fsysArq2ArqTrans(request);

    if (!(arq != 0)) { FSYS_ASSERT(arq != 0); return; }
    if (arq->callback != 0) {
        arq->callback(request);
    }
    _fsysDelARQTrans(arq);
}

/*! ARQ completion handler of a read. */
void _fsysARQCallbackRead(ARQRequest* request)
{
    FsysArq* arq = _fsysArq2ArqTrans(request);

    if (!(arq != 0)) { FSYS_ASSERT(arq != 0); return; }
    DCInvalidateRange(arq->buffer, arq->length);
    if (arq->callback != 0) {
        arq->callback(request);
    }
    _fsysDelARQTrans(arq);
}

/*! Does this resource already have a cache? */
u8 _fsysCheckCache(u32 groupId, u32 cacheIndex)
{
    return _fsysGetCacheManage(groupId, cacheIndex) != 0;
}

/*! Hand out the next cache unique group ID.  The counter is cleared with
 *  the rest of the module state at startup. */
u32 _fsysIncCacheUniqueGroupID(void)
{
    return _fsysCACHEuniqueGroupID++;
}

/*! Set the caches up.  `heapSize` is the ARAM the caches may live in and
 *  `cacheCount` how many files may be cached at once. */
u8 _fsysInitCache(u32 heapSize, u32 cacheCount)
{
    u32 i;

    _fsysCACHEManageRoot = 0;
    _fsysCACHEManageIndex = 0;
    _fsysCACHEuniqueGroupID = 0;
    _fsysCACHEuniqueID = 0;
    _fsysCACHEFreeBytes = 0;
    _fsysARQTransRoot = 0;
    _fsysARQTransIndex = 0;

    _fsysCACHESize = heapSize;
    if (heapSize == 0) {
        _fsysCACHESize = FSYS_CACHE_RESERVE;
    }
    _fsysCACHEManageSize = cacheCount;
    _fsysCACHESize = (_fsysCACHESize + ARQ_DMA_ALIGNMENT - 1) & ~(ARQ_DMA_ALIGNMENT - 1);
    if (cacheCount == 0) {
        _fsysCACHEManageSize = FSYS_DEFAULT_CACHE_COUNT;
    }
    _fsysARQTransSize = FSYS_MAX_ARQ;

    if (!_fsysInitCacheHeap(_fsysCACHESize)) {
        return 0;
    }

    _fsysCACHEManage = (FsysCacheManage*)_fsysDoMalloc(_fsysCACHEManageSize * sizeof(FsysCacheManage));
    if (_fsysCACHEManage == 0) {
        return 0;
    }
    _fsysARQTrans = (FsysArq*)_fsysDoMalloc(_fsysARQTransSize * sizeof(FsysArq));
    if (_fsysARQTrans == 0) {
        return 0;
    }

    for (i = 0; i < _fsysCACHEManageSize; i++) {
        _fsysCACHEManage[i].allocated = 0;
        _fsysCACHEManage[i].inUse = 0;
        _fsysCACHEManage[i].lockFlag = 0;
    }
    for (i = 0; i < _fsysARQTransSize; i++) {
        _fsysARQTrans[i].inUse = 0;
    }
    return 1;
}

/*! How much of the cache heap is still free. */
u32 _fsysGetFreeCacheHeapSize(void)
{
    return FSYS_CACHE_RESERVE - _fsysCACHEFreeBytes;
}

/*! Hand `block` back to the cache heap, releasing `bytes` of accounted use. */
void _fsysFreeCacheFromHeap(void* block, u32 bytes)
{
    GSmemFreeHeap(_fsysAmemHeap, block);
    _fsysCACHEFreeBytes -= bytes;
}

/*! Take memory out of the cache heap. */
void* _fsysAllocCacheFromHeap(u32 size)
{
    void* ptr = GSmemAllocHeap(_fsysAmemHeap, size);

    if (ptr != 0) {
        _fsysCACHEFreeBytes += size;
    }
    return ptr;
}

/*! Check that the requested cache size fits in the fixed ARAM cache range. */
u8 _fsysInitCacheHeap(u32 size)
{
    return size <= FSYS_CACHE_RESERVE;
}

/*! Create the heap the caches are allocated from, spanning the ARAM range the
 *  module reserves for them. */
unsigned long GSfsysCreateCacheHeap(void)
{
    _fsysAmemTopAddr = FSYS_ARAM_HEAP_START;
    _fsysAmemHeap = GSmemCreateExternalHeap((void*)FSYS_ARAM_HEAP_START, (void*)FSYS_ARAM_HEAP_END,
                                            (GSmemCell*)_fsysAmemCell, FSYS_HEAP_TABLE_ENTRIES);
    return _fsysAmemHeap != -1;
}

/*! Lock or unlock every cache of a group. */
void _fsysSetLockCacheFlagGroup(u32 groupId, u8 lockFlag)
{
    s32 intr = OSDisableInterrupts();
    FsysCacheManage* cache = _fsysCACHEManageRoot;

    while (cache != 0) {
        if (cache->groupId == groupId) {
            cache->lockFlag = lockFlag;
        }
        cache = cache->next;
    }
    OSRestoreInterrupts(intr);
}

/*! Lock or unlock the cache of one resource. */
void _fsysSetLockCacheFlag(u32 groupId, u32 cacheIndex, u8 lockFlag)
{
    s32 intr = OSDisableInterrupts();
    FsysCacheManage* cache = _fsysGetCacheManage(groupId, cacheIndex);

    if (cache != 0) {
        cache->lockFlag = lockFlag;
    }
    OSRestoreInterrupts(intr);
}

/* Select an unlocked cache of another group, ordered by priority and creation ID. */
FsysCacheManage* _fsysGetFreshCacheManage(u32 groupId, u8 preferTail)
{
    FsysCacheManage* cache = _fsysCACHEManageRoot;
    FsysCacheManage* best = 0;

    while (cache != 0) {
        if (cache->inUse != 1 && cache->lockFlag != 1 && cache->groupId != groupId) {
            if (best == 0) {
                best = cache;
            } else if (best->priority >= cache->priority) {
                if (!preferTail) {
                    if (best->creationID < cache->creationID) {
                        best = cache;
                    }
                } else {
                    best = cache;
                }
            }
        }
        cache = cache->next;
    }
    return best;
}

/*! Find the cache at an ARAM address. */
FsysCacheManage* _fsysGetCacheManageAddr(u32 aramAddress)
{
    s32 intr = OSDisableInterrupts();
    FsysCacheManage* cache = _fsysCACHEManageRoot;

    while (cache != 0) {
        if (cache->aramAddress == aramAddress) {
            break;
        }
        cache = cache->next;
    }
    OSRestoreInterrupts(intr);
    return cache;
}

/*! Find the first cache belonging to a group. */
FsysCacheManage* _fsysGetCacheManageGroup(u32 groupId)
{
    s32 intr = OSDisableInterrupts();
    FsysCacheManage* cache = _fsysCACHEManageRoot;

    while (cache != 0) {
        if (cache->groupId == groupId) {
            break;
        }
        cache = cache->next;
    }
    OSRestoreInterrupts(intr);
    return cache;
}

/*! Find the cache of one resource in a group. */
FsysCacheManage* _fsysGetCacheManage(u32 groupId, u32 cacheIndex)
{
    s32 intr = OSDisableInterrupts();
    FsysCacheManage* cache = _fsysCACHEManageRoot;

    while (cache != 0) {
        if (cache->groupId == groupId && cache->cacheIndex == cacheIndex) {
            break;
        }
        cache = cache->next;
    }
    OSRestoreInterrupts(intr);
    return cache;
}

/*! Remove a cache descriptor from the live list. */
void _fsysDelCacheManage(FsysCacheManage* cache)
{
    _fsysDelTAG(cache, &_fsysCACHEManageRoot);
}

/*! Hand out a free cache descriptor and put it on the live list. */
FsysCacheManage* _fsysGetNewCacheManage(u32 groupID, u32 cacheIndex, u8 inUse,
                                       u8 lockFlag, u32 priority)
{
    BOOL intr;
    FsysCacheManage* cache;
    FsysCacheManage* found = 0;
    u32 index;

    intr = OSDisableInterrupts();
    index = _fsysCACHEManageIndex;

    while (found == 0) {
        cache = &_fsysCACHEManage[index];

        if (cache->allocated == 0) {
            cache->prev = 0;
            cache->next = 0;
            cache->allocated = 1;
            cache->inUse = inUse;
            cache->lockFlag = lockFlag;
            cache->groupId = groupID;
            cache->cacheIndex = cacheIndex;
            cache->aramAddress = 0;
            cache->size = 0;
            cache->priority = priority;
            cache->creationID = _fsysCACHEuniqueID;
            _fsysCACHEuniqueID++;
            found = cache;
        }
        index++;
        if (index >= _fsysCACHEManageSize) {
            index = 0;
        }
        if (index == _fsysCACHEManageIndex) {
            FSYS_RETURN_IF_FALSE(index != _fsysCACHEManageIndex);
        }
    }

    _fsysCACHEManageIndex = index;
    _fsysAddCacheManage(found);
    OSRestoreInterrupts(intr);
    return found;
}

/*! Link a cache descriptor into the live list. */
void _fsysAddCacheManage(FsysCacheManage* cache)
{
    _fsysAddTAG(cache, &_fsysCACHEManageRoot);
}

/*! Find the transfer that owns an ARQ request. */
FsysArq* _fsysArq2ArqTrans(ARQRequest* request)
{
    FsysArq* arq = _fsysARQTransRoot;

    while (arq != 0) {
        if (&arq->request == request) {
            return arq;
        }
        arq = arq->next;
    }
    return 0;
}

/*! Unlink a completed transfer from the live list. */
void _fsysDelARQTrans(FsysArq* trans)
{
    _fsysDelTAG(trans, &_fsysARQTransRoot);
}

/*! Hand out a free ARQ transfer descriptor and put it on the live list. */
FsysArq* _fsysGetNewArq(void)
{
    BOOL intr;
    FsysArq* found = 0;
    FsysArq* arq;
    u32 index;
    intr = OSDisableInterrupts();
    index = _fsysARQTransIndex;

    while (found == 0) {
        arq = &_fsysARQTrans[index];

        if (arq->inUse == 0) {
            arq->prev = 0;
            arq->next = 0;
            arq->inUse = 1;
            arq->callback = 0;
            arq->file = 0;
            arq->header = 0;
            arq->userCallback = 0;
            arq->context = 0;
            arq->argument = 0;
            arq->buffer = 0;
            arq->length = 0;
            found = arq;
        }
        index++;
        if (index >= _fsysARQTransSize) {
            index = 0;
        }
        if (index == _fsysARQTransIndex) {
            break;
        }
    }
    _fsysARQTransIndex = index;
    _fsysAddArq(found);
    OSRestoreInterrupts(intr);
    return found;
}

/*! Link a transfer descriptor into the live list. */
void _fsysAddArq(FsysArq* arq)
{
    _fsysAddTAG(arq, &_fsysARQTransRoot);
}

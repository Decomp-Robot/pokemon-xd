/*
 * fsysStream.c
 *
 * The streaming reads of the file system: the entry points that pull a
 * resource out of its ARAM cache and hand the data to the caller.
 */

#include <GSAPI/GSfsys/fsys.h>

/*! The read-block flag of the ARAM chunk an offset falls in. */
u32 _fsysStreamGetReadBlockFlag(u32 offset)
{
    return 1 << (offset / _fsysBlockByteSize);
}

/*! Pull `size` bytes at `offset` out of a cached file.  A file without an
 *  ARAM cache for that chunk reads back as zeroes. */
u8 _fsysStreamGetData(FsysFile* file, u32 id, void* buffer, u32 size, u32 offset,
                       FsysStreamCallback userCallback, void* context, long argument)
{
    s32 intr;
    FsysResHeader* header;

    intr = OSDisableInterrupts();
    if (id == FSYS_STREAM_ID) {
        header = _fsysGetResHeader(file, FSYS_STREAM_RES_INDEX);
    } else {
        header = _fsysGetResHeader(file, _fsysID2Index(file, id));
    }

    if (header != 0 && (header->readBlockFlags & _fsysStreamGetReadBlockFlag(offset)) != 0) {
        header->status &= ~FSYS_RES_STATUS_CACHED;
        if (_fsysReadCacheEX(file, header, buffer, size, offset, _fsysStreamGetDataCallback, userCallback,
                             context, argument) == 1) {
            if ((header->status & FSYS_RES_STATUS_END) != 0) {
                header->readRemain = header->dataSize;
            } else {
                header->readRemain -= size;
                header->readBlockFlags &= ~_fsysStreamGetReadBlockFlag(offset);
            }
            OSRestoreInterrupts(intr);
            return 1;
        }
    }

    OSRestoreInterrupts(intr);
    _fsysMemSet(buffer, 0, size);
    return 0;
}

/*! ARQ completion handler of a stream read: flags the resource as cached and
 *  hands the request to whoever asked for the data. */
void _fsysStreamGetDataCallback(ARQRequest* request)
{
    FsysArq* arq = _fsysArq2ArqTrans(request);
    FsysResHeader* header;
    FsysFile* file;

    if (!(arq != 0)) { FSYS_ASSERT(arq != 0); return; }
    header = arq->header;
    file = arq->file;

    header->status |= FSYS_RES_STATUS_CACHED;
    if (arq->userCallback != 0) {
        arq->userCallback(file->fileMode, arq->context, arq->argument);
    }
    arq->userCallback = 0;
}

/*! Drop every cache a file owns. */
void _fsysStreamFreeCacheAll(FsysFile* file)
{
    FsysHeader* header;
    FsysResHeader* res;
    s32 intr;
    s32 i;

    if (file == 0) {
        return;
    }
    if (file->fileMode != FSYS_FILE_TYPE_STREAM) {
        return;
    }

    intr = OSDisableInterrupts();
    header = _fsysGetHeader(file);
    if (header != 0) {
        for (i = 0; i < header->resCount; i++) {
            res = _fsysGetResHeader(file, i);

            if (res != 0 && res->cacheAddress != 0) {
                _fsysMemFreeCache(res->cacheAddress);
                res->cacheAddress = 0;
            }
        }
    }
    OSRestoreInterrupts(intr);
}

/*! Smallest cache the streamer asks for: a single transfer chunk. */
u32 _fsysStreamGetMinimumCacheSize(void)
{
    return _fsysBlockByteSize;
}

/*! ARAM a plain stream read wants. */
u32 _fsysStreamGetNormalCacheSize(void)
{
    return _fsysBlockByteSize * FSYS_STREAM_NORMAL_CHUNKS;
}

/*! ARAM a cached read wants. */
u32 _fsysStreamGetCacheSize(void)
{
    return _fsysBlockByteSize * FSYS_STREAM_CACHE_CHUNKS;
}

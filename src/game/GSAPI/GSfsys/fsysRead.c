#include <dolphin/os.h>

#include <GSAPI/GSfsys/fsys.h>

#include <stdio.h>

#define FSYS_READ_PATH_MAX 128

#define FSYS_RES_STATUS_READ_RESET                                                      \
    (FSYS_RES_STATUS_END | FSYS_RES_STATUS_STREAM_HEADER | FSYS_RES_STATUS_PREPARED |   \
     FSYS_RES_STATUS_IN_CACHE | FSYS_RES_STATUS_CACHE_ADDRESS | FSYS_RES_STATUS_EXPANDED)

u8 _fsysReadData(FsysFile* file, FsysResHeader* header, u8 cache, u32 cacheIndex, void* buffer,
                 u32 size, u32 offset, GSdvdReadCompletionFunc dvdCallback,
                 FsysArqCallback cacheCallback);
void _fsysReadHeaderCacheCallback(ARQRequest* request);
void _fsysReadHeaderDvdCallback(s32 result, GSdvdFile* dvdFile);
void _fsysReadHeaderCacheWriteCallback(ARQRequest* request);
void _fsysReadHeaderCallbackSub(FsysFile* file);
u8 _fsysReadFile(FsysFile* file);
void _fsysReadBlockCacheCallback(ARQRequest* request);
void _fsysReadBlockDvdCallback(s32 result, GSdvdFile* dvdFile);
void _fsysReadBlockCacheWriteCallback(ARQRequest* request);
void _fsysReadBlockHandling(FsysFile* file, FsysResHeader* header);
void _fsysReadDataNext(FsysFile* file, FsysResHeader* header);
u8 _fsysAdjustReadFile(FsysFile* file);
u8 _fsysExpandFile(FsysFile* file);
void _fsysSeekCallback(s32 result, GSdvdFile* dvdFile);

void _fsysSeekRequest(FsysFile* work)
{
    if (!GSdvdSeekAsync(work->dvdHandle, 0, _fsysSeekCallback)) {
        _fsysWorkSetTaskStatus(work, FSYS_TASK_ERROR_SEEK);
    }
}

void _fsysReadRequest(FsysFile* work)
{
    FsysFile* idle;
    s32 interrupt;
    u8 lockCache;

    if (work->header != NULL && work->present == 1) {
        _fsysReadHeaderCallbackSub(work);
        return;
    }
    while (1) {
        if (_fsysMemAllocHeader(work) == 1) {
            break;
        }
        interrupt = OSDisableInterrupts();
        idle = _fsysGetNewWork();
        if (idle == NULL) {
            OSRestoreInterrupts(interrupt);
            FSYS_ASSERT(idle != NULL);
            _fsysWorkSetTaskStatus(work, FSYS_TASK_ERROR_READ);
            return;
        }
        idle->fileState = FSYS_TASK_NONE;
        OSRestoreInterrupts(interrupt);
    }
    interrupt = OSDisableInterrupts();
    lockCache = 0;
    if (_fsysCheckCache(work->groupId, FSYS_INVALID_ID) == 1) {
        _fsysSetLockCacheFlag(work->groupId, FSYS_INVALID_ID, 1);
        lockCache = 1;
    }
    OSRestoreInterrupts(interrupt);
    _fsysReadData(work, NULL, lockCache, FSYS_INVALID_ID, work->header, work->dvdEntry->tableOffset,
                  0, _fsysReadHeaderDvdCallback, _fsysReadHeaderCacheCallback);
}

u8 _fsysReadData(FsysFile* file, FsysResHeader* header, u8 cache, u32 cacheIndex, void* buffer,
                 u32 size, u32 offset, GSdvdReadCompletionFunc dvdCallback,
                 FsysArqCallback cacheCallback)
{
    file->cacheIndex = cacheIndex;
    if (cache == 1) {
        if (!_fsysReadCache(file, header, buffer, size, offset, cacheCallback)) {
            _fsysWorkSetTaskStatus(file, FSYS_TASK_ERROR_READ);
            return 0;
        }
    } else if (header != NULL && header->dvdHandle != NULL) {
        if (!GSdvdReadAsync(header->dvdHandle, buffer, size, offset, dvdCallback)) {
            _fsysWorkSetTaskStatus(file, FSYS_TASK_ERROR_READ);
            return 0;
        }
    } else {
        if (header != NULL) {
            offset += header->dataOffset;
        }
        if (!GSdvdReadAsync(file->dvdHandle, buffer, size, offset, dvdCallback)) {
            _fsysWorkSetTaskStatus(file, FSYS_TASK_ERROR_READ);
            return 0;
        }
    }
    return 1;
}

void _fsysReadHeaderCacheCallback(ARQRequest* request)
{
    FsysArq* arq = _fsysArq2ArqTrans(request);

    if (arq != NULL) {
        _fsysReadHeaderCallbackSub(arq->file);
    }
}

void _fsysReadHeaderDvdCallback(s32 result, GSdvdFile* dvdFile)
{
    FsysFile* file = _fsysInfo2Work(dvdFile);
    void* cache;

    if (file == NULL) {
        return;
    }
    if (result < 0) {
        _fsysWorkSetTaskStatus(file, FSYS_TASK_ERROR_READ);
        return;
    }
    cache = _fsysMemAllocCache(file, FSYS_INVALID_ID, file->dvdEntry->tableOffset,
                               file->cacheUniqueGroupID);
    if (cache != NULL) {
        _fsysWriteCache(file, NULL, file->header, (u32)cache, file->dvdEntry->tableOffset, 0,
                        _fsysReadHeaderCacheWriteCallback);
    }
    _fsysReadHeaderCallbackSub(file);
}

void _fsysReadHeaderCacheWriteCallback(ARQRequest* request)
{
}

void _fsysReadHeaderCallbackSub(FsysFile* file)
{
    s32 interrupt = OSDisableInterrupts();

    file->present = 1;
    switch (file->fileMode) {
    case FSYS_FILE_TYPE_SEEK:
        break;
    case FSYS_FILE_TYPE_NORMAL:
    case FSYS_FILE_TYPE_STREAM:
        if (_fsysID2Index(file, file->resourceId) != FSYS_INVALID_ID) {
            file->selectedIndex = _fsysID2Index(file, file->resourceId);
            file->expandIndex = file->selectedIndex;
        }
        _fsysReadFile(file);
        break;
    case FSYS_FILE_TYPE_PREREAD:
        if (_fsysID2Index(file, file->resourceId) != FSYS_INVALID_ID) {
            if (_fsysCheckCache(file->groupId, _fsysID2Index(file, file->resourceId)) == 1) {
                _fsysWorkSetTaskStatus(file, FSYS_TASK_READ_COMPLETE);
                break;
            }
            file->selectedIndex = _fsysID2Index(file, file->resourceId);
            file->expandIndex = file->selectedIndex;
            _fsysReadFile(file);
        } else if (_fsysAdjustReadFile(file) == 1) {
            _fsysReadFile(file);
        } else {
            _fsysWorkSetTaskStatus(file, FSYS_TASK_READ_COMPLETE);
        }
        break;
    }
    OSRestoreInterrupts(interrupt);
}

u8 _fsysReadFile(FsysFile* file)
{
    FsysResHeader* header;
    char path[FSYS_READ_PATH_MAX];
    char* name;

    if (file->cancelRequest == 1) {
        _fsysWorkSetTaskStatus(file, FSYS_TASK_READ_COMPLETE);
        return 0;
    }
    header = _fsysGetResHeader(file, file->selectedIndex);
    if (header != NULL) {
        name = (char*)file->header + header->entryOffset;
        header->blockOffset = 0;
        header->cacheAddress = 0;
        header->blockList = NULL;
        header->spareBlockList = NULL;
        header->dvdHandle = NULL;
        header->taskStatus = FSYS_RES_TASK_NONE;
        header->requestedTaskStatus = FSYS_INVALID_ID;
        header->status &= ~FSYS_RES_STATUS_READ_RESET;
        header->status |= FSYS_RES_STATUS_NO_LOOP;
        header->readRemain = 0;
        header->readBlockFlags = 0;
        header->blockByteOffset = 0;
        header->loopStart = 0;
        header->loopEnd = header->dataSize;
        if (file->fileMode == FSYS_FILE_TYPE_STREAM) {
            if (header->dataType == FSYS_DATA_TYPE_SOUND_STREAM_HEADER ||
                header->dataType == FSYS_DATA_TYPE_MOVIE_STREAM_HEADER) {
                header->status |= FSYS_RES_STATUS_STREAM_HEADER;
            }
        }
        if (_fsysGetHeader(file)->flags & FSYS_HEADER_DEBUG_OVERRIDE) {
            sprintf(path, "debug/%s", name);
            if (GSdvdOpenTest(path) == 1) {
                header->dvdHandle = GSdvdOpen(path);
                header->status &= ~FSYS_RES_STATUS_COMPRESSED;
                header->dataSize = GSdvdGetLength(header->dvdHandle);
                header->dvdLength = header->dataSize;
            }
        }
        if (file->fileMode != FSYS_FILE_TYPE_STREAM &&
            _fsysCheckCache(file->groupId, file->selectedIndex) == 1) {
            header->status |= FSYS_RES_STATUS_IN_CACHE;
            _fsysSetLockCacheFlag(file->groupId, file->selectedIndex, 1);
        }
        if (file->fileMode == FSYS_FILE_TYPE_NORMAL ||
            (file->fileMode == FSYS_FILE_TYPE_STREAM && (header->status & FSYS_RES_STATUS_STREAM_HEADER))) {
            if (!_fsysExpandFile(file)) {
                return 0;
            }
        }
        return _fsysReadDataExec(file, header);
    }
}

u8 _fsysReadDataExec(FsysFile* file, FsysResHeader* header)
{
    FsysDataType* dataType;
    FsysBlock* block;
    FsysResHeader* first;
    u32 blockOffset;
    u32 cacheFlag;
    u8 result;
    s32 interrupt;

    interrupt = OSDisableInterrupts();
    if (file->fileMode == FSYS_FILE_TYPE_STREAM &&
        !(header->status & FSYS_RES_STATUS_PREPARED) &&
        header->dataType != FSYS_DATA_TYPE_SOUND_STREAM_HEADER &&
        header->dataType != FSYS_DATA_TYPE_MOVIE_STREAM_HEADER) {
        if (_fsysID2Index(file, file->resourceId) != FSYS_INVALID_ID) {
            first = _fsysGetResHeader(file, _fsysID2Index(file, file->resourceId));
        } else {
            first = _fsysGetResHeader(file, 0);
        }
        if (first == NULL) {
            _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_READ_DATA);
            OSRestoreInterrupts(interrupt);
            return 0;
        }
        if (!(first->status & FSYS_RES_STATUS_EXPANDED)) {
            _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_READ_DATA);
            OSRestoreInterrupts(interrupt);
            return 0;
        }
        dataType = _fsysGetDataType(header->dataType);
        if (dataType->preFunc != NULL) {
            dataType->preFunc(file->groupId, header->resourceId, header->dataSize);
        }
        header->status |= FSYS_RES_STATUS_PREPARED;
    }
    block = _fsysAllocBlock();
    if (block == NULL) {
        _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_READ_DATA);
        OSRestoreInterrupts(interrupt);
        return 0;
    }
    _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_READY);
    block->offset = header->blockOffset;
    _fsysAddBlock(&header->blockList, block);
    blockOffset = header->blockOffset;
    cacheFlag = (header->status & FSYS_RES_STATUS_IN_CACHE) != 0;
    result = _fsysReadData(file, header, cacheFlag, file->selectedIndex,
                           header->blockList->data,
                           _fsysGetTransLength(header->dataSize, blockOffset),
                           blockOffset,
                           _fsysReadBlockDvdCallback, _fsysReadBlockCacheCallback);
    OSRestoreInterrupts(interrupt);
    return result;
}

void _fsysReadBlockCacheCallback(ARQRequest* request)
{
    FsysArq* arq = _fsysArq2ArqTrans(request);
    FsysFile* file;
    FsysResHeader* header;

    if (arq != NULL) {
        file = arq->file;
        header = arq->header;
        _fsysReadBlockHandling(file, header);
        _fsysReadDataNext(file, header);
    }
}

void _fsysReadBlockDvdCallback(s32 result, GSdvdFile* dvdFile)
{
    u32 offset;
    FsysFile* file;
    FsysResHeader* header;

    file = _fsysInfo2Work(dvdFile);

    if (file == NULL) {
        return;
    }
    if (result < 0) {
        _fsysWorkSetTaskStatus(file, FSYS_TASK_ERROR_READ);
        return;
    }
    header = _fsysGetResHeader(file, file->selectedIndex);
    if (header == NULL) {
        return;
    }
    if (!(header->status & FSYS_RES_STATUS_CACHE_ADDRESS)) {
        if (file->fileMode == FSYS_FILE_TYPE_STREAM) {
            if (header->status & FSYS_RES_STATUS_STREAM_HEADER) {
                header->cacheAddress = 0;
            } else {
                header->cacheAddress = (u32)_fsysMemAllocCache(file, file->selectedIndex,
                                                               _fsysStreamGetCacheSize(),
                                                               file->cacheUniqueGroupID);
            }
        } else {
            header->cacheAddress = (u32)_fsysMemAllocCache(file, file->selectedIndex,
                                                           header->dataSize,
                                                           file->cacheUniqueGroupID);
        }
    }
    header->status |= FSYS_RES_STATUS_CACHE_ADDRESS;
    if (header->cacheAddress != 0) {
        if (file->fileMode == FSYS_FILE_TYPE_STREAM) {
            offset = header->blockByteOffset;
        } else {
            offset = header->blockOffset;
        }
        _fsysWriteCache(file, header, header->blockList->data, header->cacheAddress,
                        _fsysGetTransLength(header->dataSize, header->blockOffset), offset,
                        _fsysReadBlockCacheWriteCallback);
    } else {
        _fsysReadBlockHandling(file, header);
    }
    _fsysReadDataNext(file, header);
}

u32 _fsysGetTransLength(u32 total, u32 offset)
{
    u32 length = total - offset;

    if (length > _fsysBlockByteSize) {
        length = _fsysBlockByteSize;
    }
    return OSRoundUp32B(length);
}

void _fsysReadBlockCacheWriteCallback(ARQRequest* request)
{
    FsysArq* arq = _fsysArq2ArqTrans(request);

    if (arq != NULL) {
        _fsysReadBlockHandling(arq->file, arq->header);
    }
}

void _fsysReadBlockHandling(FsysFile* file, FsysResHeader* header)
{
    if (file->fileMode == FSYS_FILE_TYPE_NORMAL ||
        (file->fileMode == FSYS_FILE_TYPE_STREAM && (header->status & FSYS_RES_STATUS_STREAM_HEADER))) {
        if (file->cancelRequest != 1) {
            _fsysMoveBlock(&header->blockList, &header->spareBlockList);
            return;
        }
    }
    _fsysFreeBlock(&header->blockList, 0);
}

void _fsysReadDataNext(FsysFile* file, FsysResHeader* header)
{
    GSdvdFile* dvdHandle;
    FsysHeader* group;

    header->blockOffset += _fsysBlockByteSize;
    if (file->fileMode == FSYS_FILE_TYPE_STREAM && !(header->status & FSYS_RES_STATUS_STREAM_HEADER)) {
        header->readRemain += _fsysBlockByteSize;
        header->readBlockFlags |= _fsysStreamGetReadBlockFlag(header->blockByteOffset);
        header->blockByteOffset += _fsysBlockByteSize;
        if (header->blockByteOffset >= _fsysStreamGetCacheSize()) {
            header->blockByteOffset = 0;
        }
        if (file->cancelRequest == 1) {
            _fsysWorkSetTaskStatus(file, FSYS_TASK_READ_COMPLETE);
            return;
        }
        if (header->blockOffset >= header->loopEnd) {
            if ((header->status & FSYS_RES_STATUS_NO_LOOP) != 0) {
                _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_STREAMING_COMPLETE);
                return;
            }
            if (header->blockOffset < _fsysStreamGetCacheSize()) {
                header->status |= FSYS_RES_STATUS_END;
                _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_STREAMING_COMPLETE);
                header->readBlockFlags = -1;
                return;
            }
            header->blockOffset = header->loopStart;
        }
        if (header->readRemain >= _fsysStreamGetCacheSize()) {
            _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_READ_CACHE_GATED);
            return;
        }
        if (_fsysGetActiveTask(FSYS_FILE_TYPE_NORMAL) +
            _fsysGetActiveTask(FSYS_FILE_TYPE_PREREAD) != 0) {
            _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_READ_CACHE_GATED);
            return;
        }
    } else {
        if (header->blockOffset >= header->dataSize) {
            dvdHandle = header->dvdHandle;
            if (dvdHandle != NULL) {
                header->dvdHandle = NULL;
                GSdvdClose(dvdHandle);
            }
            if (file->fileMode == FSYS_FILE_TYPE_STREAM && file->doneArgument != 0) {
                file->selectedIndex = _fsysID2Index(file, file->doneArgument);
            } else if (_fsysID2Index(file, file->resourceId) != FSYS_INVALID_ID) {
                _fsysWorkSetTaskStatus(file, FSYS_TASK_READ_COMPLETE);
                return;
            } else {
                file->selectedIndex++;
            }
            group = _fsysGetHeader(file);
            if (file->selectedIndex >= group->resCount) {
                _fsysWorkSetTaskStatus(file, FSYS_TASK_READ_COMPLETE);
                return;
            }
            _fsysReadFile(file);
            return;
        }
    }
    _fsysReadDataExec(file, header);
}

FsysResHeader* _fsysGetResHeader(FsysFile* file, u32 index)
{
    FsysHeader* header = _fsysGetHeader(file);
    const u32* offsets;

    if (header == NULL) {
        return NULL;
    }
    if (index >= header->resCount) {
        return NULL;
    }
    offsets = (const u32*)((u8*)header + header->tableOffset);
    return (FsysResHeader*)((u8*)header + offsets[index]);
}

FsysHeader* _fsysGetHeader(FsysFile* file)
{
    if (file->header == NULL) {
        return NULL;
    }
    if (file->present == 0) {
        return NULL;
    }
    return file->header;
}

u8 _fsysAdjustReadFile(FsysFile* file)
{
    FsysHeader* header = _fsysGetHeader(file);
    u16 index;

    for (index = 0; index < header->resCount; index++) {
        if (_fsysCheckCache(file->groupId, index) == 0) {
            break;
        }
    }
    if (index >= header->resCount) {
        return 0;
    }
    file->selectedIndex = index;
    return 1;
}

u8 _fsysExpandFile(FsysFile* file)
{
    FsysResHeader* header;
    FsysHeader* table = _fsysGetHeader(file);

    if (file->expandIndex >= table->resCount) {
        return 0;
    }
    header = _fsysGetResHeader(file, file->expandIndex);
    if (header == NULL) {
        return 0;
    }
    file->expandIndex++;
    return _fsysExpandFileExec(file, header);
}

u8 _fsysExpandFileExec(FsysFile* file, FsysResHeader* header)
{
    s32 interrupt = OSDisableInterrupts();
    FsysBGWork* work;

    work = _fsysGetNewBGWork();
    if (work == NULL) {
        _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_EXPAND_DATA);
        OSRestoreInterrupts(interrupt);
        return 0;
    }
    _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_READY);
    work->header = header;
    work->file = file;
    if ((header->status & FSYS_RES_STATUS_COMPRESSED) != 0) {
        work->compressed = 1;
    } else {
        work->compressed = 0;
    }
    _fsysAddBGWork(work);
    OSRestoreInterrupts(interrupt);
    return 1;
}

FsysDataType* _fsysGetDataType(u32 type)
{
    FsysDataType* dataType = fsys_datatype;
    u32 count = fsys_datatype_number;
    u32 i;

    for (i = 0; i < count; i++) {
        if (dataType->typeID == type) {
            return dataType;
        }
        dataType++;
    }
    return NULL;
}

void _fsysSeekCallback(s32 result, GSdvdFile* dvdFile)
{
    FsysFile* file = _fsysInfo2Work(dvdFile);

    if (file != NULL) {
        if (result < 0) {
            _fsysWorkSetTaskStatus(file, FSYS_TASK_ERROR_SEEK);
        } else {
            _fsysWorkSetTaskStatus(file, FSYS_TASK_COMPLETE);
        }
    }
}

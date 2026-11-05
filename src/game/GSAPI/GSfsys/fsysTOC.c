/*
 * fsysTOC.c
 *
 * The catalog side of the file system: the gsfsys.toc catalog that maps
 * group ids to the groups of the disc image, and the per-group expand
 * tracking the daemons poll while their reads run.
 */

#include <dolphin/os.h>

#include <GSAPI/GSfsys/fsys.h>

/*! Task states a work slot files on itself.  The slots in one of these states
 *  carry no running task; the pending states a running read or expand pass
 *  files are the ones fsys.h names. */

/*! Task states the expand pass stages a resource through; the task field of
 *  a resource says which pass runs the next time its group is polled. */
#define FSYS_RES_TASK_NONE 0 /* no task queued for the resource */
#define FSYS_RES_TASK_EXPAND_BLOCKS 1 /* the expand pass was started for it */
#define FSYS_RES_TASK_CHECK_CANCEL 2
#define FSYS_RES_TASK_READ_DATA 3
#define FSYS_RES_TASK_EXPAND_DATA 4
#define FSYS_RES_TASK_READ_CACHE_GATED 5

/*! Finish a header-size read: file the table offset the disc header carried
 *  with the group entry the file was opened for, and drop the staging buffer
 *  when the read went wrong. */
void _fsysHeaderSizeReadDvdCallback(s32 result, GSdvdFile* file)
{
    FsysFile* work = _fsysInfo2Work(file);

    if (work == NULL) {
        _headerTop = NULL;
        _fsysWorkSetTaskStatus(work, FSYS_WORK_TASK_READ_HEADER_SIZE);
        FSYS_ASSERT(work != NULL);
        return;
    }
    if (result < 0) {
        _headerTop = NULL;
        _fsysWorkSetTaskStatus(work, FSYS_WORK_TASK_READ_HEADER_SIZE);
        FSYS_ASSERT(work != NULL);
        return;
    }
    work->dvdEntry->tableOffset = _headerTop->tableOffset;
    _headerTop = NULL;
    _fsysWorkSetTaskStatus(work, FSYS_WORK_TASK_READ_HEADER);
}

/*! Clear the expand request flags of one group of the catalog. */
void _fsysOffTOCFlag(u32 groupID, u32 flags)
{
    FsysGroup* group = _fsysGetGROUP(groupID);

    if (group != NULL) {
        group->expandFlags &= ~flags;
    }
}

/*! Set the expand request flags of one group of the catalog. */
void _fsysOnTOCFlag(u32 groupID, u32 flags)
{
    FsysGroup* group = _fsysGetGROUP(groupID);

    if (group != NULL) {
        group->expandFlags |= flags;
    }
}

/*! Answer whether every resource of a file that belongs to a group with
 *  expand requests has its data fully in place. */
u8 _fsysCheckExpandCompleteAll(FsysFile* file)
{
    FsysHeader* header = _fsysGetHeader(file);
    u8 complete;
    s32 index;

    if (header == NULL) {
        return 0;
    }
    complete = 1;
    if (_fsysID2Index(file, file->resourceId) != FSYS_INVALID_ID) {
        if (file->fileMode == FSYS_FILE_TYPE_STREAM) {
            if (!_fsysCheckExpandComplete(file, _fsysID2Index(file, file->resourceId))) {
                complete = 0;
            }
            if (!_fsysCheckExpandComplete(file, _fsysID2Index(file, file->doneArgument))) {
                complete = 0;
            }
        } else {
            return _fsysCheckExpandComplete(file, _fsysID2Index(file, file->resourceId));
        }
    } else {
        for (index = 0; index < header->resCount; index++) {
            if (!_fsysCheckExpandComplete(file, index)) {
                complete = 0;
            }
        }
    }
    return complete;
}

/*! Find the table index a resource id sits at, or FSYS_INVALID_ID when the
 *  file does not list it. */
u32 _fsysID2Index(FsysFile* file, u32 id)
{
    FsysHeader* header;
    FsysResHeader* res;
    u32 index;

    if (id == FSYS_INVALID_ID) {
        return FSYS_INVALID_ID;
    }
    header = _fsysGetHeader(file);
    if (header != NULL) {
        for (index = 0; index < header->resCount; index++) {
            res = _fsysGetResHeader(file, index);
            if (res != NULL && res->resourceId == id) {
                return index;
            }
        }
    }
    return FSYS_INVALID_ID;
}

/*! Run one expand pass for a resource of a file: hand the resource's pending
 *  task to the readers and answer whether its data is fully in place. */
u8 _fsysCheckExpandComplete(FsysFile* file, u32 index)
{
    FsysResHeader* header = _fsysGetResHeader(file, index);
    u8 complete;
    s32 intr;
    u32 activeCount;

    if (header == NULL) {
        return 0;
    }

    intr = OSDisableInterrupts();
    if (header->requestedTaskStatus != FSYS_INVALID_ID) {
        header->taskStatus = header->requestedTaskStatus;
        header->requestedTaskStatus = FSYS_INVALID_ID;
    }
    OSRestoreInterrupts(intr);

    if (!(header->status & FSYS_RES_STATUS_EXPANDED)) {
        switch (header->taskStatus) {
        default:
        case FSYS_RES_TASK_NONE:
        case FSYS_RES_TASK_EXPAND_BLOCKS:
            break;
        case FSYS_RES_TASK_READ_DATA:
            _fsysReadDataExec(file, header);
            break;
        case FSYS_RES_TASK_EXPAND_DATA:
            if (_fsysExpandFileExec(file, header) == 1) {
                _fsysReadDataExec(file, header);
            }
            break;
        case FSYS_RES_TASK_READ_CACHE_GATED:
            if (file->cancelRequest == 1) {
                _fsysWorkSetTaskStatus(file, FSYS_WORK_TASK_CANCELLED);
                _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_EXPAND_BLOCKS);
                break;
            }
            /* wait behind the tasks the two queue roots carry */
            activeCount = _fsysGetActiveTask(FSYS_FILE_TYPE_NORMAL) +
                          _fsysGetActiveTask(FSYS_FILE_TYPE_PREREAD);
            if (activeCount == 0) {
                if (header->readRemain <= _fsysStreamGetNormalCacheSize()) {
                    _fsysReadDataExec(file, header);
                }
            } else {
                if (header->readRemain <= _fsysStreamGetMinimumCacheSize()) {
                    _fsysReadDataExec(file, header);
                }
            }
            break;
        case FSYS_RES_TASK_CHECK_CANCEL:
            if (file->cancelRequest == 1) {
                _fsysWorkSetTaskStatus(file, FSYS_WORK_TASK_CANCELLED);
                _fsysResHeaderSetTaskStatus(header, FSYS_RES_TASK_EXPAND_BLOCKS);
            }
            break;
        }
        complete = 0;
    } else {
        complete = 1;
    }

    intr = OSDisableInterrupts();
    if (header->requestedTaskStatus != FSYS_INVALID_ID) {
        header->taskStatus = header->requestedTaskStatus;
        header->requestedTaskStatus = FSYS_INVALID_ID;
    }
    OSRestoreInterrupts(intr);
    return complete;
}



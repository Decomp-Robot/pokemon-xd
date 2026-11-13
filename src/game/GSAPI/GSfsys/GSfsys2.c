/*
 * GSfsys2.c
 *
 * The read and wait entry points of the GS file system: the calls that file a
 * read or a pre-read for a group with the queue engine and the calls that wait
 * for the work filed on a group to run.  Behind them sit the queue engine
 * itself, the start-up of the file system and the two daemons that carry the
 * work out: the foreground daemon runs the task of each open file and the
 * background daemon moves and expands the resources it has read.
 */

#include <GSAPI/GSfsys/fsys.h>
#include <GSAPI/GSgapp/GSgapp.h>
#include <GSthread.h>
#include <string.h>

/*! Middle of the three queue roots the request engine keeps work under.
 *  The reads below file their requests on it. */
#define FSYS_QUEUE_ROOT_MIDDLE 1
/*! Number of queue roots, highest priority first. */
#define FSYS_QUEUE_ROOT_MAX 3
#define FSYS_QUEUE_CANCEL 5
#define FSYS_QUEUE_OPERATION_MAX 6

/*! Request option: keep the cache a pre-read fills instead of releasing it. */
#define FSYS_REQUEST_KEEP_CACHE 1

/*! How a wait treats a group that has no work left in the file system. */
#define FSYS_WAIT_REQUIRE_WORK 0 /* wait for a work entry to show up */
#define FSYS_WAIT_ACCEPT_IDLE 1 /* nothing in flight already counts as done */

/*! Sizes of the work tables GSfsysInit2 allocates. */
#define FSYS_WORK_MAX 4
#define FSYS_BGWORK_MAX 16
#define FSYS_QUEMANAGE_MAX 24

/*! LZSS packing of the compressed resources. */
#define FSYS_LZ_WINDOW_SIZE 4096
#define FSYS_LZ_MATCH_MAX_INDEX 17
#define FSYS_LZ_WINDOW_START (FSYS_LZ_WINDOW_SIZE - FSYS_LZ_MATCH_MAX_INDEX - 1)
#define FSYS_LZ_HEAD_SIZE 16
#define FSYS_LZ_MATCH_LENGTH_BASE 2
#define FSYS_LZ_STATE_FLAGS 0
#define FSYS_LZ_STATE_FLAG_BYTE 1
#define FSYS_LZ_STATE_LITERAL 2
#define FSYS_LZ_STATE_MATCH_LOW 3
#define FSYS_LZ_STATE_MATCH_HIGH 4

/*! Task states of a resource, as the work status report names them. */
#define FSYS_RES_TASK_ERROR_READDATAEXEC (-3)
#define FSYS_RES_TASK_ERROR_EXPANDFILE (-2)
#define FSYS_RES_TASK_ERROR_STREAMING (-1)
#define FSYS_RES_TASK_STREAMING_COMPLETE 2
#define FSYS_RES_TASK_READY 1
#define FSYS_RES_TASK_NONE 0

/*! A request filed with the queue engine. */
typedef struct FsysQueManage FsysQueManage;
struct FsysQueManage {
    FsysQueManage* prev;
    FsysQueManage* next;
    s32 operation;
    unsigned long groupID;
    unsigned long resourceID;
    GSfsysReadCallback callback;
    void* context;
    long argument;
    u8 options;
    u8 started;
};


extern FsysQueManage* _fsysQueManageRoot[FSYS_QUEUE_ROOT_MAX];
FsysDiscHeader headerTop;
u32 _fsysWorkMax;
u32 _fsysWorkIndex;
u32 _fsysBGWorkMax;
u32 _fsysBGWorkIndex;
u32 _fsysQueManageMax;
FsysDiscHeader* _headerTop;
s32 _fsysDaemonBackgroundHDL;
s32 _fsysDaemonForegroundHDL;
FsysQueManage* _fsysQueManageExec;
FsysQueManage* _fsysQueManage;
FsysBGWork* _fsysBGWorkRoot;
FsysBGWork* _fsysBGWork;
FsysFile* _fsysWork;
FsysTOC* _fsysTOC;
u8 _fsysInitialized;


FsysQueManage* _fsysGetQueManage(unsigned long groupID, unsigned long resourceID);
FsysQueManage* _fsysGetNewQueManage(s32 operation, unsigned long groupID,
                                    unsigned long resourceID, u8 options,
                                    GSfsysReadCallback callback, void* context, long argument);
u8 _fsysAddQueManage(FsysQueManage* queue, s32 queueRoot);
void _fsysDelQueManage(FsysQueManage* queue);
u8 _fsysQueExecSub(FsysQueManage* queue);
void _fsysQueExec(void);
void _fsysFreeStatus(unsigned long groupID);
void _fsysBackgroundTaskNormalType(FsysBGWork* task, u8 copy);
void _fsysBackgroundTaskCompressType(FsysBGWork* task);
u8 _fsysBGTaskCancel(FsysBGWork* task);
void _fsysForegroundTask(FsysFile* work);

void GSfsysDaemonBackground(unsigned long appID, unsigned long userData);
void GSfsysDaemonForeground(unsigned long appID, unsigned long userData);


/*! File a pre-read request for a group. */
u8 _fsysPreReadExec(unsigned long groupID, u8 options, GSfsysReadCallback callback,
                    void* context, long argument, s32 queueRoot);
/*! File a request with the queue engine. */
u8 _fsysRequest(s32 operation, unsigned long groupID, unsigned long resourceID, u8 options,
                GSfsysReadCallback callback, void* context, long argument, s32 queueRoot);
/*! Hand a queue entry to the file system. */
u8 _fsysExec(unsigned long groupID, unsigned long resourceID, s32 fileMode, u8 keepCache,
             FsysStreamCallback callback, void* context, long argument);
/*! File every read of a group as a stream. */
u8 _fsysReadStreamingExec(unsigned long groupID, unsigned long resourceID, long argument);
/*! Wait for the work filed on a group to run. */
u8 _fsysWait(unsigned long groupID, u8 waitPolicy);

/*! Read one resource of a group, handing the callback, context and argument of the
 *  caller to the file system so it can run them when the read completes. */
u8 GSfsysReadIDEX(unsigned long groupID, unsigned long resourceID,
                  GSfsysReadCallback callback, void* context, long argument)
{
    return _fsysRequest(FSYS_FILE_TYPE_NORMAL, groupID, resourceID, 0, callback, context,
                        argument, FSYS_QUEUE_ROOT_MIDDLE);
}

/*! Read one resource of a group without a completion callback. */
u8 GSfsysReadID(unsigned long groupID, unsigned long resourceID)
{
    return _fsysRequest(FSYS_FILE_TYPE_NORMAL, groupID, resourceID, 0, NULL, NULL, 0,
                        FSYS_QUEUE_ROOT_MIDDLE);
}

/*! Cancel the work a group has in flight and wait for the cancel to land. */
u8 GSfsysCancelWait(unsigned long groupID)
{
    if (GSfsysCancel(groupID) == 0) {
        return 0;
    }
    return _fsysWait(groupID, FSYS_WAIT_REQUIRE_WORK);
}

/*! Ask the file system to drop the work it has in flight for a group. */
u8 GSfsysCancel(unsigned long groupID)
{
    FsysFile* work;

    if (!_fsysInitialized) {
        return 0;
    }
    if (GSfsysGetStatus(groupID) <= GSFSYS_STATUS_DONE) {
        return 0;
    }
    work = _fsysGetWork(groupID, 1);
    if (work == NULL) {
        return 0;
    }
    work->cancelRequest = TRUE;
    return 1;
}

/*! Pre-read a group and wait for the pre-read to run. */
u8 GSfsysPreReadWait(unsigned long groupID)
{
    if (!_fsysInitialized) {
        return 0;
    }
    for (;;) {
        if (GSfsysPreRead(groupID) == 1) {
            break;
        }
        GSthreadSwitch();
    }
    return _fsysWait(groupID, FSYS_WAIT_REQUIRE_WORK);
}

/*! Pre-read a group, keeping the cache the read fills. */
u8 GSfsysPreReadKeepCache(unsigned long groupID)
{
    return _fsysPreReadExec(groupID, FSYS_REQUEST_KEEP_CACHE, NULL, NULL, 0,
                            FSYS_QUEUE_ROOT_MIDDLE);
}

/*! Pre-read a group. */
u8 GSfsysPreRead(unsigned long groupID)
{
    return _fsysPreReadExec(groupID, 0, NULL, NULL, 0, FSYS_QUEUE_ROOT_MIDDLE);
}

/*! File the pre-read of a whole group with the queue engine. */
u8 _fsysPreReadExec(unsigned long groupID, u8 options, GSfsysReadCallback callback,
                    void* context, long argument, s32 queueRoot)
{
    return _fsysRequest(FSYS_FILE_TYPE_PREREAD, groupID, FSYS_INVALID_ID, options,
                        callback, context, argument, queueRoot);
}

/*! Read the resources of a group and wait for the reads to run. */
u8 GSfsysReadWait(unsigned long groupID)
{
    FsysFile* work;

    if (!_fsysInitialized) {
        return 0;
    }
    work = _fsysGetWork(groupID, 1);
    if (work != NULL && work->fileMode == FSYS_FILE_TYPE_PREREAD) {
        GSfsysCancelWait(groupID);
    }
    for (;;) {
        if (GSfsysRead(groupID) == 1) {
            break;
        }
        GSthreadSwitch();
    }
    return _fsysWait(groupID, FSYS_WAIT_REQUIRE_WORK);
}

/*! Wait until the reads queued on a group are done. */
u8 GSfsysWait(unsigned long groupID)
{
    return _fsysWait(groupID, FSYS_WAIT_ACCEPT_IDLE);
}

/*! Wait for the work filed on a group to run. */
u8 _fsysWait(unsigned long groupID, u8 waitPolicy)
{
    s32 status;

    if (!_fsysInitialized) {
        return 0;
    }
    for (;;) {
        status = GSfsysGetStatus(groupID);
        if (status == GSFSYS_STATUS_DONE) {
            return 1;
        }
        if (status == GSFSYS_STATUS_IDLE) {
            if (waitPolicy == FSYS_WAIT_ACCEPT_IDLE) {
                return 1;
            }
        } else if (status <= GSFSYS_STATUS_ERROR) {
            return 0;
        }
        GSthreadSwitch();
    }
}

/*! Stream the resources of a group, handing the argument of the caller to the
 *  callback the streamed reads run. */
u8 GSfsysReadStreamingEX(unsigned long groupID, unsigned long resourceID, long argument)
{
    return _fsysReadStreamingExec(groupID, resourceID, argument);
}

/*! Stream every resource of a group. */
u8 GSfsysReadStreaming(unsigned long groupID)
{
    return _fsysReadStreamingExec(groupID, FSYS_INVALID_ID, 0);
}

/*! Start the streamed read of a group.  A group that already has work in flight
 *  is cancelled and waited out first, so the stream starts from a clean entry. */
u8 _fsysReadStreamingExec(unsigned long groupID, unsigned long resourceID, long argument)
{
    if (!_fsysInitialized) {
        return 0;
    }
    if (GSfsysGetStatus(groupID) <= GSFSYS_STATUS_DONE) {
        return _fsysExec(groupID, resourceID, FSYS_FILE_TYPE_STREAM, 0, NULL, NULL, argument);
    }
    GSfsysCancelWait(groupID);
    return _fsysExec(groupID, resourceID, FSYS_FILE_TYPE_STREAM, 0, NULL, NULL, argument);
}

/*! Read the resources of a group without a completion callback. */
u8 GSfsysRead(unsigned long groupID)
{
    return _fsysRequest(FSYS_FILE_TYPE_NORMAL, groupID, FSYS_INVALID_ID, 0, NULL, NULL, 0,
                        FSYS_QUEUE_ROOT_MIDDLE);
}


/*! Report the task state of every open file on the debug log. */
void _fsysPrintWorkStatus(void)
{
    char* modeName[3] = { "READ", "PREREAD", "STREAMING" };
    FsysFile* work;
    FsysResHeader* header;
    char* name;
    u32 i;
    u32 j;

    for (i = 0; i < _fsysWorkMax; i++) {
        work = &_fsysWork[i];
        switch (work->fileState) {
        case FSYS_TASK_HEADERSIZE_READ:
            OSReport("[%d]HEADERSIZE_READ", i);
            break;
        case FSYS_TASK_HEADERSIZE_READ_WAIT:
            OSReport("[%d]HEADERSIZE_READ_WAIT", i);
            break;
        case FSYS_TASK_START:
            OSReport("[%d]START", i);
            break;
        case FSYS_TASK_READ:
        case FSYS_TASK_PREREAD:
        case FSYS_TASK_STREAMING:
            name = modeName[work->fileMode >> 1];
            if (work->header == NULL) {
                OSReport("[%d]%s:---", i, name);
                break;
            }
            for (j = 0; j < work->header->resCount; j++) {
                header = _fsysGetResHeader(work, j);
                switch (header->taskStatus) {
                case FSYS_RES_TASK_ERROR_READDATAEXEC:
                    OSReport("[%d]%s:[%d]ERROR_READDATAEXEC", i, name, j);
                    break;
                case FSYS_RES_TASK_ERROR_EXPANDFILE:
                    OSReport("[%d]%s:[%d]ERROR_EXPANDFILE", i, name, j);
                    break;
                case FSYS_RES_TASK_ERROR_STREAMING:
                    OSReport("[%d]%s:[%d]ERROR_STREAMING", i, name, j);
                    break;
                case FSYS_RES_TASK_STREAMING_COMPLETE:
                    OSReport("[%d]%s:[%d]STREAMING_COMPLETE", i, name, j);
                    break;
                case FSYS_RES_TASK_READY:
                    OSReport("[%d]%s:[%d]READY", i, name, j);
                    break;
                case FSYS_RES_TASK_NONE:
                    OSReport("[%d]%s:[%d]NONE", i, name, j);
                    break;
                default:
                    OSReport("[%d]%s:[%d]default", i, name, j);
                    break;
                }
            }
            break;
        case FSYS_TASK_SEEK:
            OSReport("[%d]SEEK", i);
            break;
        case FSYS_TASK_READ_COMPLETE:
            OSReport("[%d]READ_COMPLETE", i);
            break;
        case FSYS_TASK_ERROR:
            OSReport("[%d]ERROR", i);
            break;
        case FSYS_TASK_ERROR_READ:
            OSReport("[%d]ERROR_READ", i);
            break;
        case FSYS_TASK_ERROR_PREREAD:
            OSReport("[%d]ERROR_PREREAD", i);
            break;
        case FSYS_TASK_ERROR_SEEK:
            OSReport("[%d]ERROR_SEEK", i);
            break;
        case FSYS_TASK_ERROR_FILEOPEN:
            OSReport("[%d]ERROR_FILEOPEN", i);
            break;
        case FSYS_TASK_ERROR_FILECLOSE:
            OSReport("[%d]ERROR_FILECLOSE", i);
            break;
        case FSYS_TASK_COMPLETE:
            OSReport("[%d]COMPLETE", i);
            break;
        case FSYS_TASK_ERROR_WAIT:
            OSReport("[%d]ERROR_WAIT", i);
            break;
        case FSYS_TASK_COMPLETE_WAIT:
            OSReport("[%d]COMPLETE_WAIT", i);
            break;
        case FSYS_TASK_READY:
            OSReport("[%d]READY", i);
            break;
        case FSYS_TASK_READY_WAIT:
            OSReport("[%d]READY_WAIT", i);
            break;
        case FSYS_TASK_NONE:
            OSReport("[%d]NONE", i);
            break;
        default:
            OSReport("[%d]default", i);
            break;
        }
    }
}

/*! File a request with the queue engine.  A pre-read or a seek replaces the
 *  request of the same group that has not been started yet. */
u8 _fsysRequest(s32 operation, unsigned long groupID, unsigned long resourceID, u8 options,
                GSfsysReadCallback callback, void* context, long argument, s32 queueRoot)
{
    FsysQueManage* queue;

    if (!_fsysInitialized) {
        return 0;
    }
    if (operation < 0 || queueRoot >= FSYS_QUEUE_OPERATION_MAX) {
        return 0;
    }
    if (queueRoot < 0 || queueRoot >= FSYS_QUEUE_ROOT_MAX) {
        return 0;
    }
    queue = _fsysGetQueManage(groupID, resourceID);
    if (queue != NULL && !queue->started &&
        (operation == FSYS_FILE_TYPE_PREREAD || operation == FSYS_FILE_TYPE_SEEK)) {
        _fsysDelQueManage(queue);
    }
    queue = _fsysGetNewQueManage(operation, groupID, resourceID, options, callback, context,
                                 argument);
    if (queue == NULL) {
        return 0;
    }
    if (_fsysAddQueManage(queue, queueRoot) == 1) {
        _fsysQueExec();
    }
    return 1;
}

/*! Run the queued requests: finish the one in flight, then start the queued
 *  ones root by root until one of them has to wait. */
void _fsysQueExec(void)
{
    FsysQueManage* queue;
    FsysQueManage* next;
    s32 i;

    if (_fsysQueManageExec != NULL) {
        if (!_fsysQueExecSub(_fsysQueManageExec)) {
            return;
        }
        _fsysQueManageExec = NULL;
    }
    for (i = 0; i < FSYS_QUEUE_ROOT_MAX; i++) {
        for (queue = _fsysQueManageRoot[i]; queue != NULL; queue = next) {
            next = queue->next;
            if (!_fsysQueExecSub(queue)) {
                _fsysQueManageExec = queue;
                return;
            }
        }
    }
}

/*! Start a queued request, or check on one already started.  Returns 1 once
 *  the request is done with and has left the queue. */
u8 _fsysQueExecSub(FsysQueManage* queue)
{
    if (!queue->started) {
        queue->started = 1;
        switch (queue->operation) {
        case FSYS_QUEUE_CANCEL:
            GSfsysCancel(queue->groupID);
            return 0;
        default:
            _fsysDelQueManage(queue);
            return 1;
        case FSYS_FILE_TYPE_NORMAL:
        case FSYS_FILE_TYPE_PREREAD:
        case FSYS_FILE_TYPE_SEEK:
        case FSYS_FILE_TYPE_STREAM:
            _fsysExec(queue->groupID, queue->resourceID, queue->operation, queue->options,
                      queue->callback, queue->context, queue->argument);
            return 0;
        }
    }
    if (GSfsysGetStatus(queue->groupID) <= GSFSYS_STATUS_DONE) {
        _fsysDelQueManage(queue);
        return 1;
    }
    return 0;
}

/*! Take a request off its queue and free its entry. */
void _fsysDelQueManage(FsysQueManage* queue)
{
    s32 i;

    if (queue->prev != NULL) {
        queue->prev->next = queue->next;
    }
    if (queue->next != NULL) {
        queue->next->prev = queue->prev;
    }
    for (i = 0; i < FSYS_QUEUE_ROOT_MAX; i++) {
        if (queue == _fsysQueManageRoot[i]) {
            _fsysQueManageRoot[i] = queue->next;
            break;
        }
    }
    queue->groupID = 0;
}

/*! Put a request at the end of a queue.  Returns 1 when the queue was empty. */
u8 _fsysAddQueManage(FsysQueManage* queue, s32 queueRoot)
{
    FsysQueManage* last;
    u8 wasEmpty;

    if (_fsysQueManageRoot[queueRoot] == NULL) {
        _fsysQueManageRoot[queueRoot] = queue;
        wasEmpty = 1;
    } else {
        for (last = _fsysQueManageRoot[queueRoot]; last->next != NULL; last = last->next) {
        }
        last->next = queue;
        wasEmpty = 0;
        queue->prev = last;
    }
    return wasEmpty;
}

/*! Queued request of a group. */
FsysQueManage* _fsysGetQueManage(unsigned long groupID, unsigned long resourceID)
{
    FsysQueManage* queue;
    s32 i;

    for (i = 0; i < FSYS_QUEUE_ROOT_MAX; i++) {
        for (queue = _fsysQueManageRoot[i]; queue != NULL; queue = queue->next) {
            if (queue->groupID == groupID) {
                if (resourceID != FSYS_INVALID_ID && queue->resourceID == resourceID) {
                    return queue;
                }
                return queue;
            }
        }
    }
    return NULL;
}

/*! Fill a free queue entry with a request. */
FsysQueManage* _fsysGetNewQueManage(s32 operation, unsigned long groupID,
                                    unsigned long resourceID, u8 options,
                                    GSfsysReadCallback callback, void* context, long argument)
{
    FsysQueManage* queue = NULL;
    u32 i;

    for (i = 0; i < _fsysQueManageMax; i++) {
        if (_fsysQueManage[i].groupID == 0) {
            queue = &_fsysQueManage[i];
            queue->prev = NULL;
            queue->next = NULL;
            queue->operation = operation;
            queue->groupID = groupID;
            queue->resourceID = resourceID;
            queue->callback = callback;
            queue->context = context;
            queue->argument = argument;
            queue->options = options;
            queue->started = 0;
            break;
        }
    }
    return queue;
}

/*! Hand a request to the file system: set up the work entry of the group and
 *  open its file. */
u8 _fsysExec(unsigned long groupID, unsigned long resourceID, s32 fileMode, u8 keepCache,
             FsysStreamCallback callback, void* context, long argument)
{
    FsysFile* work;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    _fsysFreeStatus(groupID);
    work = _fsysGetWork(groupID, 0);
    if (work == NULL) {
        work = _fsysGetNewWork();
        if (!(work != NULL)) {
            OSRestoreInterrupts(enabled);
            FSYS_ASSERT(work != NULL);
            return 0;
        }
        work->header = NULL;
        work->cacheUniqueGroupID = _fsysIncCacheUniqueGroupID();
    }
    work->groupId = groupID;
    work->resourceId = resourceID;
    work->dvdHandle = NULL;
    work->fileMode = fileMode;
    work->fileState = FSYS_TASK_READY;
    work->pendingState = FSYS_PENDING_STATE_NONE;
    work->selectedIndex = 0;
    work->expandIndex = 0;
    work->cancelRequest = 0;
    work->keepCache = keepCache;
    work->doneCallback = callback;
    work->doneContext = context;
    work->doneArgument = argument;
    work->dvdEntry = _fsysGetGROUP(groupID);
    _fsysOpenFile(work);
    OSRestoreInterrupts(enabled);
    return 1;
}

/*! Forget the result of the last work of a group. */
void _fsysFreeStatus(unsigned long groupID)
{
    FsysFile* work;

    _fsysOffTOCFlag(groupID, FSYS_GROUP_COMPLETE);
    work = _fsysGetWork(groupID, 0);
    if (work != NULL) {
        work->fileState = FSYS_TASK_READY_WAIT;
    }
}

/*! Where the work filed on a group stands. */
long GSfsysGetStatus(unsigned long groupID)
{
    FsysGroup* group;
    FsysFile* work;

    if (!_fsysInitialized) {
        return GSFSYS_STATUS_ERROR;
    }
    group = _fsysGetGROUP(groupID);
    if (group == NULL) {
        return GSFSYS_STATUS_IDLE;
    }
    if (group->expandFlags & FSYS_GROUP_COMPLETE) {
        return GSFSYS_STATUS_DONE;
    }
    work = _fsysGetWork(groupID, 0);
    if (work == NULL) {
        return GSFSYS_STATUS_IDLE;
    }
    switch (work->fileState) {
    case FSYS_TASK_COMPLETE_WAIT:
        return GSFSYS_STATUS_DONE;
    case FSYS_TASK_READY_WAIT:
        return GSFSYS_STATUS_IDLE;
    case FSYS_TASK_ERROR_WAIT:
        return GSFSYS_STATUS_ERROR;
    default:
        return GSFSYS_STATUS_BUSY;
    }
}

/*! Bring the file system up: the block pool, the ARAM cache, the work tables,
 *  the catalog and the two daemons that run the work. */
u8 GSfsysInit2(u32 blockPoolSize, u32 cacheHeapSize, u32 cacheCount)
{
    u32 i;

    if (_fsysInitialized == 1) {
        return 0;
    }
    _fsysQueManageRoot[0] = NULL;
    _fsysWorkIndex = 0;
    _fsysBGWorkIndex = 0;
    _fsysQueManageRoot[1] = NULL;
    _fsysQueManageRoot[2] = NULL;
    _fsysQueManageExec = NULL;
    if (!_fsysInitBlock(blockPoolSize)) {
        return 0;
    }
    if (!_fsysInitCache(cacheHeapSize, cacheCount)) {
        return 0;
    }
    _fsysWorkMax = FSYS_WORK_MAX;
    _fsysBGWorkMax = FSYS_BGWORK_MAX;
    _fsysQueManageMax = FSYS_QUEMANAGE_MAX;
    _fsysWork = _fsysDoMalloc(FSYS_WORK_MAX * sizeof(FsysFile));
    if (_fsysWork == NULL) {
        return 0;
    }
    _fsysBGWork = _fsysDoMalloc(_fsysBGWorkMax * sizeof(FsysBGWork));
    if (_fsysBGWork == NULL) {
        return 0;
    }
    _fsysQueManage = _fsysDoMalloc(_fsysQueManageMax * sizeof(FsysQueManage));
    if (_fsysQueManage == NULL) {
        return 0;
    }
    for (i = 0; i < _fsysWorkMax; i++) {
        _fsysWork[i].fileState = FSYS_TASK_NONE;
        _fsysWork[i].pendingState = FSYS_PENDING_STATE_NONE;
        _fsysWork[i].header = NULL;
        _fsysWork[i].present = 0;
    }
    for (i = 0; i < _fsysBGWorkMax; i++) {
        _fsysBGWork[i].inUse = 0;
    }
    for (i = 0; i < _fsysQueManageMax; i++) {
        _fsysQueManage[i].groupID = 0;
    }
    if (!_fsysInitTOC()) {
        return 0;
    }
    _fsysDaemonForegroundHDL = GSgappCreate(1, 20, 0, GSfsysDaemonForeground);
    GSgappSetName(_fsysDaemonForegroundHDL, "GSfsysDaemonForeground");
    _fsysDaemonBackgroundHDL = GSgappCreate(1, 19, 0, GSfsysDaemonBackground);
    GSgappSetName(_fsysDaemonBackgroundHDL, "GSfsysDaemonBackground");
    _fsysInitialized = 1;
    return 1;
}

/*! Background daemon: give each expander task its destination, then move the
 *  data of the resource into it, expanding it when it is compressed. */
void GSfsysDaemonBackground(unsigned long appID, unsigned long userData)
{
    FsysBGWork* task;
    FsysDataType* dataType;
    void* output;

    for (task = _fsysBGWorkRoot; task != NULL; task = task->next) {
        if (!task->started) {
            task->started = 1;
            dataType = _fsysGetDataType(task->header->dataType);
            if (dataType->preFunc != NULL) {
                output = dataType->preFunc(task->file->groupId, task->header->resourceId,
                                           task->header->dvdLength);
            } else {
                output = _fsysGetResource((task->header->dvdLength + 31) & ~31,
                                          task->file->groupId, task->header->resourceId);
            }
            task->header->outputAddress = (u32)output;
        }
        if (task->header->outputAddress == 0) {
            _fsysBackgroundTaskNormalType(task, 0);
        } else if (task->compressed == 1) {
            _fsysBackgroundTaskCompressType(task);
        } else {
            _fsysBackgroundTaskNormalType(task, 1);
        }
    }
}

/*! Move the plain data of a resource out of its staging blocks; without a
 *  destination the blocks are only drained. */
void _fsysBackgroundTaskNormalType(FsysBGWork* task, u8 copy)
{
    FsysBlock* block;
    void* output;
    u32 length;

    if (task->finished == 1) {
        return;
    }
    do {
        if (_fsysBGTaskCancel(task) == 1) {
            return;
        }
        block = task->header->spareBlockList;
        if (block == NULL) {
            return;
        }
        if (copy == 1) {
            output = (void*)(task->header->outputAddress + task->inputOffset);
            length = _fsysGetTransLength(task->header->dataSize, task->inputOffset);
            memcpy(output, block->data, length);
        }
        _fsysFreeBlock(&task->header->spareBlockList, 1);
        task->inputOffset += _fsysBlockByteSize;
    } while (task->inputOffset < task->header->dataSize);
    DCFlushRange((void*)task->header->outputAddress, task->header->dataSize);
    task->finished = 1;
}

/*! Expand the LZSS data of a resource out of its staging blocks.  The
 *  expander stops at the end of each block and picks up where it left off
 *  when the next block arrives. */
void _fsysBackgroundTaskCompressType(FsysBGWork* task)
{
    FsysBlock* block;
    u32 match;
    u32 position;
    u32 flags;
    u32 codeCount;
    u32 input;
    u32 output;
    u8* dest;
    u8* window;
    u32 packedSize;
    u32 blockSize;
    u32 state;
    u32 c;
    s32 length;
    u32 src;
    s32 j;
    u8* data;

    if (task->finished == 1) {
        return;
    }
    if (_fsysBGTaskCancel(task) == 1) {
        return;
    }
    block = task->header->spareBlockList;
    if (block == NULL) {
        return;
    }
    if (task->window == NULL) {
        task->window = _fsysAlloc(FSYS_LZ_WINDOW_SIZE + FSYS_LZ_MATCH_MAX_INDEX);
        if (task->window == NULL) {
            return;
        }
        memset(task->window, 0, FSYS_LZ_WINDOW_START);
        memcpy(&task->headType, block->data, FSYS_LZ_HEAD_SIZE);
        task->windowPosition = FSYS_LZ_WINDOW_START;
        task->windowFlags = 0;
        task->headPackedRemain = task->headPackedSize - FSYS_LZ_HEAD_SIZE;
        task->state = FSYS_LZ_STATE_FLAGS;
        src = FSYS_LZ_HEAD_SIZE;
    } else {
        src = 0;
    }
    codeCount = task->codeCount;
    flags = task->windowFlags;
    position = task->windowPosition;
    match = task->windowMatch;
    input = task->inputOffset;
    output = task->outputOffset;
    dest = (u8*)task->header->outputAddress;
    packedSize = task->headPackedRemain;
    blockSize = _fsysBlockByteSize;
    window = task->window;
    state = task->state;

    for (;;) {
        data = block->data;
        switch (state) {
        default:
        case FSYS_LZ_STATE_FLAGS:
            for (;;) {
                flags >>= 1;
                if (!(flags & 0x100)) {
        case FSYS_LZ_STATE_FLAG_BYTE:
                    input++;
                    c = data[src++];
                    if (input > packedSize) {
                        goto blockDone;
                    }
                    if (src > blockSize) {
                        state = FSYS_LZ_STATE_FLAG_BYTE;
                        input--;
                        goto blockDone;
                    }
                    flags = c | 0xFF00;
                }
                if (flags & 1) {
        case FSYS_LZ_STATE_LITERAL:
                    input++;
                    c = data[src++];
                    if (input > packedSize) {
                        goto blockDone;
                    }
                    if (src > blockSize) {
                        state = FSYS_LZ_STATE_LITERAL;
                        input--;
                        goto blockDone;
                    }
                    dest[output++] = c;
                    window[position++] = c;
                    position &= FSYS_LZ_WINDOW_SIZE - 1;
                } else {
        case FSYS_LZ_STATE_MATCH_LOW:
                    input++;
                    match = data[src++];
                    if (input > packedSize) {
                        goto blockDone;
                    }
                    if (src > blockSize) {
                        state = FSYS_LZ_STATE_MATCH_LOW;
                        input--;
                        goto blockDone;
                    }
        case FSYS_LZ_STATE_MATCH_HIGH:
                    input++;
                    c = data[src++];
                    if (input > packedSize) {
                        goto blockDone;
                    }
                    if (src > blockSize) {
                        state = FSYS_LZ_STATE_MATCH_HIGH;
                        input--;
                        goto blockDone;
                    }
                    match |= (c & 0xF0) << 4;
                    length = (c & 0xF) + FSYS_LZ_MATCH_LENGTH_BASE;
                    for (j = 0; j <= length; j++) {
                        c = window[(match + j) & (FSYS_LZ_WINDOW_SIZE - 1)];
                        dest[output++] = c;
                        window[position++] = c;
                        position &= FSYS_LZ_WINDOW_SIZE - 1;
                    }
                }
                codeCount++;
            }
        }
    blockDone:
        _fsysFreeBlock(&task->header->spareBlockList, 1);
        if (input > packedSize) {
            DCFlushRange((void*)task->header->outputAddress, task->headLength);
            task->finished = 1;
            return;
        }
        if (_fsysBGTaskCancel(task) == 1) {
            return;
        }
        block = task->header->spareBlockList;
        if (block == NULL) {
            break;
        }
        src = 0;
    }
    task->codeCount = codeCount;
    task->windowFlags = flags;
    task->windowMatch = match;
    task->windowPosition = position;
    task->inputOffset = input;
    task->outputOffset = output;
    task->state = state;
}

/*! Fill a buffer and flush it out to memory. */
void _fsysMemSet(void* buffer, int value, u32 size)
{
    if (buffer != NULL) {
        memset(buffer, value, size);
        DCFlushRange(buffer, size);
    }
}

/*! Drop the staging blocks of a task whose file was cancelled.  Returns 1 when
 *  the task was cancelled. */
u8 _fsysBGTaskCancel(FsysBGWork* task)
{
    if (task->file->cancelRequest == 1) {
        _fsysFreeBlockAll(&task->header->blockList);
        _fsysFreeBlockAll(&task->header->spareBlockList);
        task->finished = 1;
        return 1;
    }
    return 0;
}

/*! Foreground daemon: hand the expanded resources to their data types in
 *  table order, then run the task of every open file and the request queue. */
void GSfsysDaemonForeground(unsigned long appID, unsigned long userData)
{
    FsysBGWork* task;
    FsysBGWork* next;
    FsysResHeader* header;
    FsysResHeader* prevHeader;
    FsysFile* file;
    FsysDataType* dataType;
    u32 index;
    u32 i;

    for (task = _fsysBGWorkRoot; task != NULL; task = next) {
        next = task->next;
        if (task->finished == 1) {
            header = task->header;
            file = task->file;
            index = _fsysID2Index(file, header->resourceId);
            if (index != 0) {
                prevHeader = _fsysGetResHeader(file, index - 1);
            } else {
                prevHeader = NULL;
            }
            if (_fsysID2Index(file, file->resourceId) != FSYS_INVALID_ID || prevHeader == NULL ||
                (prevHeader->status & FSYS_RES_STATUS_EXPANDED)) {
                dataType = _fsysGetDataType(header->dataType);
                if (!file->cancelRequest && dataType->postFunc != NULL) {
                    if (header->status & FSYS_RES_STATUS_COMPRESSED) {
                        dataType->postFunc(file->groupId, header->resourceId, header->dvdLength);
                    } else {
                        dataType->postFunc(file->groupId, header->resourceId, header->dataSize);
                    }
                }
                if (task->window != NULL) {
                    _fsysFree(task->window);
                    task->window = NULL;
                }
                _fsysDelBGWork(task);
                header->status |= FSYS_RES_STATUS_EXPANDED;
                _fsysSetLockCacheFlag(file->groupId, _fsysID2Index(file, header->resourceId), 0);
            } else if (prevHeader != NULL && !(prevHeader->status & FSYS_RES_STATUS_EXPANDED)) {
                OSReport("<1>-----fsysBGwork:Wait Prev ResFinish Index=%d:%d\n", file->groupId,
                         _fsysID2Index(file, header->resourceId));
            } else {
                OSReport("<2>-----fsysBGwork:Wait Prev ResFinish Index=%d:%d\n", file->groupId,
                         _fsysID2Index(file, header->resourceId));
            }
        }
    }
    for (i = 0; i < _fsysWorkMax; i++) {
        _fsysForegroundTask(&_fsysWork[i]);
    }
    _fsysQueExec();
}

/*! Run one step of the task of an open file. */
void _fsysForegroundTask(FsysFile* work)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (work->pendingState != FSYS_PENDING_STATE_NONE) {
        work->fileState = work->pendingState;
        work->pendingState = FSYS_PENDING_STATE_NONE;
    }
    OSRestoreInterrupts(enabled);

    switch (work->fileState) {
    case FSYS_TASK_HEADERSIZE_READ:
        if (_headerTop == NULL) {
            _headerTop = &headerTop;
            _fsysWorkSetTaskStatus(work, FSYS_TASK_HEADERSIZE_READ_WAIT);
            if (!GSdvdReadAsync(work->dvdHandle, _headerTop, sizeof(FsysDiscHeader), 0,
                                _fsysHeaderSizeReadDvdCallback)) {
                _headerTop = NULL;
                _fsysWorkSetTaskStatus(work, FSYS_TASK_HEADERSIZE_READ);
            }
        }
        break;
    case FSYS_TASK_START:
        switch (work->fileMode) {
        case FSYS_FILE_TYPE_NORMAL:
            _fsysWorkSetTaskStatus(work, FSYS_TASK_READ);
            _fsysReadRequest(work);
            break;
        case FSYS_FILE_TYPE_STREAM:
            _fsysWorkSetTaskStatus(work, FSYS_TASK_STREAMING);
            _fsysReadRequest(work);
            break;
        case FSYS_FILE_TYPE_PREREAD:
            _fsysWorkSetTaskStatus(work, FSYS_TASK_PREREAD);
            _fsysReadRequest(work);
            break;
        case FSYS_FILE_TYPE_SEEK:
            _fsysWorkSetTaskStatus(work, FSYS_TASK_SEEK);
            _fsysSeekRequest(work);
            break;
        default:
            _fsysWorkSetTaskStatus(work, FSYS_TASK_ERROR);
            break;
        }
        break;
    case FSYS_TASK_READ:
    case FSYS_TASK_PREREAD:
    case FSYS_TASK_STREAMING:
        _fsysCheckExpandCompleteAll(work);
        break;
    case FSYS_TASK_READ_COMPLETE:
        switch (work->fileMode) {
        case FSYS_FILE_TYPE_NORMAL:
            if (!_fsysCheckExpandCompleteAll(work)) {
                break;
            }
        case FSYS_FILE_TYPE_PREREAD:
        case FSYS_FILE_TYPE_STREAM:
            _fsysWorkSetTaskStatus(work, FSYS_TASK_COMPLETE);
            break;
        default:
            _fsysWorkSetTaskStatus(work, FSYS_TASK_ERROR);
            break;
        }
        break;
    case FSYS_TASK_ERROR:
    case FSYS_TASK_ERROR_READ:
    case FSYS_TASK_ERROR_PREREAD:
    case FSYS_TASK_ERROR_SEEK:
        OSReport("_fsysForegroundTask:ERROR...:%d\n", work->groupId);
        if (!_fsysCloseFile(work, 1)) {
            _fsysWorkSetTaskStatus(work, FSYS_TASK_ERROR_FILECLOSE);
            break;
        }
    case FSYS_TASK_ERROR_FILEOPEN:
        OSReport("_fsysForegroundTask:ERROR_FILEOPEN:%d\n", work->groupId);
        _fsysOpenFile(work);
        break;
    case FSYS_TASK_ERROR_FILECLOSE:
        OSReport("_fsysForegroundTask:ERROR_FILECLOSE:%d\n", work->groupId);
        _fsysWorkSetTaskStatus(work, FSYS_TASK_ERROR_WAIT);
        break;
    case FSYS_TASK_COMPLETE:
        if (!_fsysCloseFile(work, 0)) {
            _fsysWorkSetTaskStatus(work, FSYS_TASK_ERROR_FILECLOSE);
            break;
        }
        switch (work->fileMode) {
        case FSYS_FILE_TYPE_SEEK:
        case FSYS_FILE_TYPE_STREAM:
            break;
        default:
            _fsysOnTOCFlag(work->groupId, FSYS_GROUP_COMPLETE);
            break;
        }
        _fsysWorkSetTaskStatus(work, FSYS_TASK_COMPLETE_WAIT);
        break;
    case FSYS_TASK_NONE:
    case FSYS_TASK_READY_WAIT:
    case FSYS_TASK_COMPLETE_WAIT:
    case FSYS_TASK_ERROR_WAIT:
        break;
    }

    enabled = OSDisableInterrupts();
    if (work->pendingState != FSYS_PENDING_STATE_NONE) {
        work->fileState = work->pendingState;
        work->pendingState = FSYS_PENDING_STATE_NONE;
    }
    OSRestoreInterrupts(enabled);
}

/*! The three queue roots, highest priority first. */
FsysQueManage* _fsysQueManageRoot[FSYS_QUEUE_ROOT_MAX];

/*! Name of the catalog file on the disc. */
char const fsysTOCFilename[] = "gsfsys.toc";

/*
 * GSfsys2.c
 *
 * The read and wait entry points of the GS file system: the calls that file a
 * read or a pre-read for a group with the queue engine and the calls that wait
 * for the work filed on a group to run.
 */

#include <GSAPI/GSfsys/fsys.h>
#include <GSthread.h>

extern u8 _fsysInitialized;

/*! Middle of the three queue roots the request engine keeps work under.
 *  The reads below file their requests on it. */
#define FSYS_QUEUE_ROOT_MIDDLE 1

/*! Request option: keep the cache a pre-read fills instead of releasing it. */
#define FSYS_REQUEST_KEEP_CACHE 1

/*! The invalid resource id, the one GSfsysGetInvalidID() hands back: filing a
 *  read with it makes the queue engine read every resource of the group. */
#define FSYS_INVALID_ID 0xFFFF

/*! How a wait treats a group that has no work left in the file system. */
#define FSYS_WAIT_REQUIRE_WORK 0 /* wait for a work entry to show up */
#define FSYS_WAIT_ACCEPT_IDLE 1 /* nothing in flight already counts as done */

/*! File a pre-read request for a group. */
u8 _fsysPreReadExec(unsigned long groupID, u8 options, GSfsysReadCallback callback,
                    void* context, long argument, s32 queueRoot);
/*! File a request with the queue engine. */
u8 _fsysRequest(s32 operation, unsigned long groupID, unsigned long resourceID, u8 options,
                GSfsysReadCallback callback, void* context, long argument, s32 queueRoot);
/*! Hand a queue entry to the file system. */
u8 _fsysExec(unsigned long groupID, unsigned long resourceID, s32 fileMode, u8 keepCache,
             FsysStreamCallback callback, void* context, long argument);
/*! Open file the file system keeps for a group; `busyOnly` skips the failed
 *  and completed entries as well. */
FsysFile* _fsysGetWork(unsigned long groupID, u8 busyOnly);
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

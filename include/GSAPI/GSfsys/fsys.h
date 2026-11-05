#ifndef FSYS_H
#define FSYS_H

#include <dolphin/ar.h>
#include <dolphin/os.h>
#include <GSAPI/GSfsys/GSfsys.h>
#include <GSdvd.h>

/*
 * fsys.h
 *
 * The streaming half of the GS file system: the pool of ARAM staging blocks,
 * the per-file cache descriptors and the ARQ transfers that move data between
 * main memory and ARAM.
 */

/*! One ARAM staging block.  A file owns a doubly linked list of them, the
 *  owner holding the head. */
typedef struct FsysBlock FsysBlock;
typedef struct FsysFile FsysFile;
typedef struct FsysResHeader FsysResHeader;
typedef struct FsysCacheManage FsysCacheManage;
typedef struct FsysArq FsysArq;
typedef struct FsysHeader FsysHeader;
typedef struct FsysGroup FsysGroup;
typedef struct FsysTOC FsysTOC;
typedef struct FsysDiscHeader FsysDiscHeader;
typedef struct FsysBGWork FsysBGWork;
typedef struct FsysTag FsysTag;

struct FsysBlock {
    FsysBlock* prev;
    FsysBlock* next;
    void* data;
    u32 size;
};

struct FsysFile;
struct FsysResHeader;

/*! Runs when one of the transfers of the module completes, with the ARQ
 *  request that finished. */
typedef void (*FsysArqCallback)(ARQRequest* request);

/*! Runs when a read completes, with the mode of the file that was read and
 *  the context and argument the caller handed to the read. */
typedef GSfsysReadCallback FsysStreamCallback;

/*! Cache descriptor of one file. */
struct FsysCacheManage {
    FsysCacheManage* prev;
    FsysCacheManage* next;
    u8 allocated;
    u8 inUse;
    u8 lockFlag;
    u8 pad; /* aligns the ARAM address below */
    u32 aramAddress;
    u32 size;
    u32 priority;
    u32 creationID;
    u32 groupId;    /* group the cache belongs to */
    u32 cacheIndex; /* resource of that group the cache holds */
};

/*! One in-flight ARQ transfer. */
struct FsysArq {
    FsysArq* prev;
    FsysArq* next;
    u8 inUse;
    u8 pad[3]; /* aligns the request below */
    ARQRequest request;
    FsysArqCallback callback;
    FsysFile* file;
    FsysResHeader* header;
    FsysStreamCallback userCallback;
    void* context; /* handed back to userCallback when the read completes */
    long argument;
    void* buffer;
    u32 length;
};

/*! An open file.  A file belongs to a group, and that group together with the
 *  cache index below keys the ARAM cache its reads go through.  The table is
 *  an array of 64-byte slots. */
struct FsysFile {
    u32 groupId; /* group the file belongs to */
    u32 cacheIndex; /* index the reads of the file look their cache up with */
    u32 resourceId; /* resource being opened or read */
    GSdvdFile* dvdHandle; /* disc file while the file is open */
    s32 fileMode; /* FSYS_FILE_TYPE_STREAM streams through the ARAM cache,
                     FSYS_FILE_TYPE_NORMAL takes the plain handling */
    s32 fileState; /* task state the scheduler runs */
    u32 pendingState; /* staged state copied into fileState */
    FsysGroup* dvdEntry; /* group entry the length is read from */
    FsysHeader* header; /* resource table */
    u32 selectedIndex; /* table index the current request was filed with */
    u32 expandIndex; /* table index the expander has reached */
    u8 cancelRequest; /* set while the task runs to ask for it to be cancelled */
    u8 keepCache; /* request to retain the cache entry */
    u8 present; /* nonzero once the header callback has run */
    u8 pad2F;
    FsysStreamCallback doneCallback; /* completion handler run at close */
    void* doneContext;
    long doneArgument; /* second resource id a stream file reads; handed to doneCallback when the close completes */
    u32 cacheUniqueGroupID; /* unique group ID the cache allocator keys the file's cache with */
};

/*! The fixed header of an open file; resource offsets are header-relative.
 *  The header spans 96 bytes in memory. */
struct FsysHeader {
    u32 magic;
    u32 pad04;
    u32 pad08;
    u32 resCount; /* number of resource entries */
    u32 flags;
    u8 pad14[44];
    u32 tableOffset; /* header-relative offset of the u32 offset table */
    u8 pad44[28];
};

/*! One resource of an open file. */
struct FsysResHeader {
    u32 resourceId;
    u32 dataOffset; /* offset of resource data in the archive */
    u32 dvdLength;
    u32 status;
    u32 pad10;
    u32 dataSize;
    u32 outputAddress; /* destination address for normal resource reads */
    u32 entryOffset; /* header-relative offset of the entry */
    u32 dataType;
    u32 pad24;
    u32 blockOffset; /* byte offset the block reader has reached */
    u32 cacheAddress; /* ARAM address key of the cache this resource uses */
    FsysBlock* blockList; /* staging blocks the readers keep */
    FsysBlock* spareBlockList; /* staging blocks queued behind the list above */
    GSdvdFile* dvdHandle;
    s32 taskStatus;          /* task the file system is carrying out for the resource */
    s32 requestedTaskStatus; /* task a caller has asked for; 0xFFFF while none is pending */
    u32 readRemain;
    u32 blockByteOffset; /* byte offset advanced by each cache block */
    u32 readBlockFlags;
    u32 loopStart;
    u32 loopEnd;
};

/*! One entry of the gsfsys.toc catalog: the group it describes, the disc path
 *  of its archive and the offset of the resource table the reads of the group
 *  go through. */
struct FsysGroup {
    u32 groupID;
    char* path; /* archive path on the disc, catalog-relative until the catalog loads */
    u32 tableOffset; /* disc offset of the resource table */
    u32 expandFlags; /* expand request flags of the group */
};

/*! The fixed 32-byte header at the start of a group archive; the header-size
 *  read takes the resource table offset out of it. */
struct FsysDiscHeader {
    u32 pad00[7];
    u32 tableOffset; /* disc offset of the resource table */
};

/*! The loaded gsfsys.toc catalog image. */
struct FsysTOC {
    u32 pad00;
    u32 pad04;
    u32 groupCount; /* number of group entries the catalog keeps */
    u32 pad0C;
    u32 groupTableOffset; /* catalog-relative offset of the group table */
};

/*! Head of a record the file system threads onto one of its lists.  The
 *  expander tasks and the cache descriptors both start with it, so the same
 *  pair of helpers links and unlinks either kind of record. */
struct FsysTag {
    FsysTag* prev;
    FsysTag* next;
    u8 inUse; /* the record has been handed out; a cache calls it allocated */
};

/*! One task of the expander.  The tasks wait in a list the file system daemons
 *  walk; a task carries the resource it fills and the window the expander
 *  streams the data of the resource through.  It starts with the FsysTag head
 *  the list helpers link it by. */
struct FsysBGWork {
    FsysBGWork* prev; /* list the daemons walk; the root holds the first task */
    FsysBGWork* next;
    u8 inUse; /* the slot has been handed out */
    u8 finished; /* the data of the resource has been expanded */
    u8 compressed; /* the data of the resource is compressed */
    u8 started; /* a daemon has taken the task up */
    FsysFile* file;
    FsysResHeader* header;
    u8 headType; /* head of the block the task reads, taken from its data */
    u8 headSize0;
    u8 headSize1;
    u8 headSize2;
    u32 headLength;
    u32 headPackedSize;
    u32 headPackedRemain; /* packed bytes left once the head is skipped */
    u8* window; /* LZ77 window of the expander */
    u32 windowMatch; /* window index the current match reads from */
    u32 windowPosition; /* window index the expander writes at */
    u32 windowFlags; /* bits of compressed and plain bytes of the last codes */
    u32 codeCount;
    u32 inputOffset;
    u32 outputOffset;
    u32 state;
};

/* Leave the current function when a required condition is false. */
#define FSYS_RETURN_IF_FALSE(condition) \
    do {                                \
        if (!(condition)) {             \
            return;                     \
        }                               \
    } while (0)
/* Report an invalid file-system state before abandoning the operation. */
#define FSYS_ASSERT(condition)       \
    do {                             \
        if (!(condition)) {          \
            ASSERT(condition);       \
            return;                  \
        }                            \
    } while (0)

/* How the file is read.  Streaming files go through the ARAM cache with
 * high ARQ priority and own their caches at teardown; other files may still
 * read through cached data.  A pre-read fills the cache ahead of use and
 * releases it unless the caller asks to keep it. */
#define FSYS_FILE_TYPE_NORMAL 1
#define FSYS_FILE_TYPE_PREREAD 2
#define FSYS_FILE_TYPE_STREAM 4

/*! Set when data is read out of the cache into main memory. */
#define FSYS_RES_STATUS_CACHED 0x8000000

/*! Set in a resource header when the chunk was read to its very end. */
#define FSYS_RES_STATUS_END 0x800000

/*! Set in a resource header once its data is in place; the expand pass takes
 *  it as expanded and stops polling the resource. */
#define FSYS_RES_STATUS_EXPANDED 0x8

/*! Tasks a work slot files on itself while it opens its file: the slot still
 *  has to read the header size of its group, its group table offset is known
 *  so it can go on and read its resource header, or the group was cancelled
 *  while it expanded.  A header-size read that comes back wrong files the
 *  first task again. */
#define FSYS_WORK_TASK_READ_HEADER_SIZE 3
#define FSYS_WORK_TASK_READ_HEADER 5
#define FSYS_WORK_TASK_CANCELLED 10

/*! ARAM the file system keeps for itself. */
#define FSYS_CACHE_RESERVE 0xA00000

/* ARAM range the cache heap spans: start and end passed to the external heap
 * creator, leaving a heap of FSYS_CACHE_RESERVE bytes. */
#define FSYS_ARAM_HEAP_START 0x600000
#define FSYS_ARAM_HEAP_END 0x1000000

/*! Entries of the registration table the cache heap is created with. */
#define FSYS_HEAP_TABLE_ENTRIES 0x200

/*! Transfers the file system can have in flight at once. */
#define FSYS_MAX_ARQ 8

/*! Cache descriptors a file system gets when it asks for none. */
#define FSYS_DEFAULT_CACHE_COUNT 0x200

/*! Staging memory the block pool takes when its caller asks for none. */
#define FSYS_DEFAULT_BLOCK_POOL_SIZE 0x80000

/*! The invalid resource id, the one GSfsysGetInvalidID() hands back: filing a
 *  read with it makes the queue engine read every resource of the group. */
#define FSYS_INVALID_ID 0xFFFF

/*! Id that stands for the streaming descriptor of a file.  It is not listed in
 *  the resource table, so the reads that use it take the descriptor from the
 *  fixed index below instead of looking the id up. */
#define FSYS_STREAM_ID 0

/*! Index the streaming descriptor of a file sits at. */
#define FSYS_STREAM_RES_INDEX 1

/*! ARQ chunks a plain stream read wants in ARAM. */
#define FSYS_STREAM_NORMAL_CHUNKS 4

/*! ARQ chunks a streamed file wants in ARAM. */
#define FSYS_STREAM_CACHE_CHUNKS 8

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Module state.  The fsys module keeps its state in one .sbss block that the
 * units below read and write through these externs.
 */

extern u8* _fsysBlock;             /* base of the staging memory */
extern FsysBlock* _fsysBlockManage;       /* the staging block descriptors */
extern u32 _fsysBlockIndex;             /* round-robin cursor over them */
extern u32 _fsysBlockSize;              /* number of staging blocks */
extern u32 _fsysBlockBufferSize;           /* staging memory the pool asked for */
extern u32 _fsysBlockByteSize;            /* bytes per block (the ARQ chunk size) */

extern FsysCacheManage* _fsysCACHEManage; /* the cache descriptor array */
extern FsysCacheManage* _fsysCACHEManageRoot;  /* the live cache descriptors */
extern FsysArq* _fsysARQTrans;           /* the transfer array */
extern FsysArq* _fsysARQTransRoot;            /* the live transfers */
extern u32 _fsysAmemTopAddr;           /* start of the ARAM range the cache heap spans */
extern s32 _fsysAmemHeap;               /* index of the heap the caches are allocated from */
extern u32 _fsysARQTransIndex;               /* round-robin cursor over the transfers */
extern u32 _fsysARQTransSize;                /* number of transfers */
extern u32 _fsysCACHEFreeBytes;      /* bytes handed out of the cache heap */
extern u32 _fsysCACHEuniqueID;       /* unique id stamped on new caches */
extern u32 _fsysCACHEuniqueGroupID;      /* next cache unique group ID */
extern u32 _fsysCACHEManageIndex;             /* round-robin cursor over the caches */
extern u32 _fsysCACHEManageSize;              /* number of cache descriptors */
extern u32 _fsysCACHESize;           /* size of the cache heap */

extern u8 _fsysAmemCell[0x2800]; /* ARAM cell table the external cache heap
    is created with: FSYS_HEAP_TABLE_ENTRIES GSmemCell entries */

/* fsysTOC */
extern char const fsysTOCFormat[]; /* format for a catalog count on the debug log */
extern char const fsysTOCFilename[]; /* name of the catalog file on the disc */
extern FsysTOC* _fsysTOC; /* the loaded gsfsys.toc catalog image */
extern FsysDiscHeader* _headerTop; /* disc header the pending header-size read is staged into */

/* fsysStream */
u32 _fsysStreamGetReadBlockFlag(u32 offset);
u8 _fsysStreamGetData(FsysFile* file, u32 id, void* buffer, u32 size, u32 offset,
                       FsysStreamCallback userCallback, void* context, long argument);
void _fsysStreamGetDataCallback(ARQRequest* request);
void _fsysStreamFreeCacheAll(FsysFile* file);
u32 _fsysStreamGetMinimumCacheSize(void);
u32 _fsysStreamGetNormalCacheSize(void);
u32 _fsysStreamGetCacheSize(void);

/* fsysBlock */
void _fsysMoveBlock(FsysBlock** from, FsysBlock** to);
void _fsysAddBlock(FsysBlock** list, FsysBlock* block);
void _fsysFreeBlockAll(FsysBlock** list);
u8 _fsysFreeBlock(FsysBlock** list, u8 freeHead);
FsysBlock* _fsysAllocBlock(void);
u8 _fsysInitBlock(u32 size);

/* fsysCache */
void _fsysWriteCache(FsysFile* file, FsysResHeader* header, void* buffer, u32 offset, u32 size,
                     u32 offsetAdjust, FsysArqCallback callback);
u8 _fsysReadCacheEX(FsysFile* file, FsysResHeader* header, void* buffer, u32 size, u32 offset,
                    FsysArqCallback callback, FsysStreamCallback userCallback, void* context, long argument);
u8 _fsysReadCache(FsysFile* file, FsysResHeader* header, void* buffer, u32 size, u32 offset,
                  FsysArqCallback callback);
u8 _fsysReadCacheExec(FsysFile* file, FsysResHeader* header, void* buffer, u32 size, u32 offset,
                      FsysArqCallback callback, FsysStreamCallback userCallback, void* context, long argument);
void _fsysARQCallbackWrite(ARQRequest* request);
void _fsysARQCallbackRead(ARQRequest* request);
u8 _fsysCheckCache(u32 groupId, u32 cacheIndex);
u32 _fsysIncCacheUniqueGroupID(void);
u8 _fsysInitCache(u32 heapSize, u32 cacheCount);
u32 _fsysGetFreeCacheHeapSize(void);
void _fsysFreeCacheFromHeap(void* block, u32 bytes);
void* _fsysAllocCacheFromHeap(u32 size);
u8 _fsysInitCacheHeap(u32 size);
void _fsysSetLockCacheFlagGroup(u32 groupId, u8 lockFlag);
void _fsysSetLockCacheFlag(u32 groupId, u32 cacheIndex, u8 lockFlag);
FsysCacheManage* _fsysGetFreshCacheManage(u32 groupId, u8 preferTail);

/* fsysTOC */
void _fsysHeaderSizeReadDvdCallback(s32 result, GSdvdFile* file);
void _fsysOffTOCFlag(u32 groupID, u32 flags);
void _fsysOnTOCFlag(u32 groupID, u32 flags);
u8 _fsysCheckExpandCompleteAll(FsysFile* file);
u8 _fsysCheckExpandComplete(FsysFile* file, u32 index);
u8 _fsysInitTOC(void);

/* Work-pool and expand-engine routines the TOC catalog callbacks drive. */
FsysGroup* _fsysGetGROUP(u32 groupID);
FsysFile* _fsysInfo2Work(GSdvdFile* file);
void _fsysWorkSetTaskStatus(FsysFile* file, s32 status);
void _fsysResHeaderSetTaskStatus(FsysResHeader* header, s32 status);
s32 _fsysGetActiveTask(s32 fileMode);
FsysFile* _fsysGetNewWork(void);
FsysFile* _fsysGetNewWork_Sub(s32 ignoreDone);
FsysFile* _fsysGetWork(u32 groupID, u8 busyOnly);
u8 _fsysOpenFile(FsysFile* file);
u8 _fsysCloseFile(FsysFile* file, u8 releaseCache);
u8 _fsysGetFilename(FsysFile* file, char* buffer);
void _fsysMemFreeHeader(FsysFile* file);
FsysBGWork* _fsysGetNewBGWork(void);
void _fsysAddBGWork(FsysBGWork* work);
void _fsysDelBGWork(FsysBGWork* work);
void _fsysAddTAG(void* entry, void* listRoot);
void _fsysDelTAG(void* entry, void* listRoot);

u8 _fsysReadDataExec(FsysFile* file, FsysResHeader* header);
u8 _fsysExpandFileExec(FsysFile* file, FsysResHeader* header);

/* Remaining file-system routines, including additional fsysCache functions. */
FsysResHeader* _fsysGetResHeader(FsysFile* file, u32 index);
FsysHeader* _fsysGetHeader(FsysFile* file);
u32 _fsysID2Index(FsysFile* file, u32 id);
void _fsysMemSet(void* buffer, u8 value, u32 size);
FsysCacheManage* _fsysGetCacheManage(u32 groupId, u32 cacheIndex);
FsysArq* _fsysGetNewArq(void);
FsysArq* _fsysArq2ArqTrans(ARQRequest* request);
void* _fsysDoMalloc(u32 size);
void _fsysMemFreeCache(u32 cacheAddress);
void _fsysDelARQTrans(FsysArq* trans);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

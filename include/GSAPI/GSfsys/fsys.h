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
    void* dvdEntry; /* disc entry the length is read from */
    FsysHeader* header; /* resource table */
    u32 selectedIndex; /* table index the current request was filed with */
    u32 expandIndex; /* table index the expander has reached */
    u8 loadStep; /* load phase the read path advances */
    u8 keepCache; /* request to retain the cache entry */
    u8 present; /* nonzero once the header callback has run */
    u8 pad2F;
    FsysStreamCallback doneCallback; /* completion handler run at close */
    void* doneContext;
    long doneArgument;
    u32 cacheUniqueGroupID; /* unique group ID stamped on new work; seeds cache priority */
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
    u32 taskStatus;          /* task the file system is carrying out for the resource */
    u32 requestedTaskStatus; /* task a caller has asked for; 0xFFFF while none is pending */
    u32 readRemain;
    u32 blockByteOffset; /* byte offset advanced by each cache block */
    u32 readBlockFlags;
    u32 loopStart;
    u32 loopEnd;
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
 * read through cached data. */
#define FSYS_FILE_TYPE_NORMAL 1
#define FSYS_FILE_TYPE_STREAM 4

/*! Set when data is read out of the cache into main memory. */
#define FSYS_RES_STATUS_CACHED 0x8000000

/*! Set in a resource header when the chunk was read to its very end. */
#define FSYS_RES_STATUS_END 0x800000

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

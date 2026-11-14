#include <GSscratch.h>
#include <dolphin/os/OSCache.h>

struct ScratchBlock {
    u8 start;
    u8 count;
    GSscratchNotifyCallback notify;
};

enum {
    SCRATCH_BLOCK_COUNT = 32,
    SCRATCH_BLOCK_SIZE = 512,
    SCRATCH_BLOCK_UNUSED = 255,
    SCRATCH_QUEUE_LIMIT = 15,
    SCRATCH_CACHE_LINE_SIZE = 32
};

static const u32 SCRATCH_FIRST_BLOCK_BIT = 0x80000000;
static const u32 SCRATCH_ADDRESS_MASK = 0xF0000000;

static ScratchBlock _blockTable[SCRATCH_BLOCK_COUNT];
static u32 _stackSize;
static u32 _entriesQueued;
static u8* _scratchPadBase;
static u32 _scratchInitLR;
static u8 _scratchInvalid;
static u32 _blockMap;

static inline u32 blockMask(u8 start)
{
    u32 mask = SCRATCH_FIRST_BLOCK_BIT;
    while (start--) {
        mask >>= 1;
    }
    return mask;
}

static inline u32 getBlockMap(u8 start, u8 count)
{
    u32 bits = 0;
    u32 mask = SCRATCH_FIRST_BLOCK_BIT;
    u32 map = _blockMap;
    while (start--) {
        mask >>= 1;
    }
    while (count--) {
        bits <<= 1;
        if (mask) {
            if (map & mask) bits |= 1;
            mask >>= 1;
        } else bits |= 1;
    }
    return bits;
}

static inline void setBlockMap(u8 start, u8 count)
{
    u32 mask = SCRATCH_FIRST_BLOCK_BIT;
    while (start--) {
        mask >>= 1;
    }
    u32 map = _blockMap;
    while (count--) {
        map |= mask;
        mask >>= 1;
    }
    _blockMap = map;
}

static inline ScratchBlock* findBlock(u8 start)
{
    for (int i = 0; i < SCRATCH_BLOCK_COUNT; i++) {
        ScratchBlock* block = &_blockTable[i];
        if (block->start == start) {
            return block;
        }
    }
    return NULL;
}

extern "C" void GSscratchSetValid(void)
{
    if (_scratchInvalid != 0) {
        int i;
        ScratchBlock* block = _blockTable;
        for (i = SCRATCH_BLOCK_COUNT; i--; block++) {
            if (block->start != SCRATCH_BLOCK_UNUSED && block->notify != NULL) {
                block->notify(GS_SCRATCH_VALID,
                              _scratchPadBase + block->start * SCRATCH_BLOCK_SIZE,
                              block->count);
            }
        }
        _scratchInvalid = 0;
    }
}

extern "C" void GSscratchSetInvalid(void)
{
    if (_scratchInvalid != 1) {
        int i;
        ScratchBlock* block = _blockTable;
        for (i = SCRATCH_BLOCK_COUNT; i--; block++) {
            if (block->start != SCRATCH_BLOCK_UNUSED && block->notify != NULL) {
                block->notify(GS_SCRATCH_INVALID,
                              _scratchPadBase + block->start * SCRATCH_BLOCK_SIZE,
                              block->count);
            }
        }
        _scratchInvalid = 1;
    }
}

extern "C" u8 GSscratchIsPtr(void* ptr)
{
    return ((u32)ptr & SCRATCH_ADDRESS_MASK) == ((u32)_scratchPadBase & SCRATCH_ADDRESS_MASK);
}

extern "C" void GSscratchWaitForCompletion(void)
{
    LCQueueWait(_entriesQueued);
    _entriesQueued = 0;
}

extern "C" GSscratchStoreResult GSscratchStore(void* dest, void* src, u32 size)
{
    if (LCQueueLength() >= SCRATCH_QUEUE_LIMIT) {
        return GS_SCRATCH_STORE_QUEUE_FULL;
    }
    if (((u32)src & (SCRATCH_CACHE_LINE_SIZE - 1)) || ((u32)dest & (SCRATCH_CACHE_LINE_SIZE - 1))) {
        return GS_SCRATCH_STORE_UNALIGNED;
    }
    if (size & (SCRATCH_CACHE_LINE_SIZE - 1)) {
        return GS_SCRATCH_STORE_UNALIGNED;
    }
    _entriesQueued += LCStoreData(dest, src, size);
    return GS_SCRATCH_STORE_OK;
}

extern "C" void GSscratchFree(void* ptr)
{
    u8 start = ((u32)ptr - (u32)_scratchPadBase) / SCRATCH_BLOCK_SIZE;
    ScratchBlock* block = findBlock(start);
    if (block != NULL) {
        u8 count = block->count;
        u32 mask = blockMask(block->start);
        u32 map = _blockMap;
        while (count--) {
            map &= ~mask;
            mask >>= 1;
        }
        _blockMap = map;
        block->start = SCRATCH_BLOCK_UNUSED;
    }
}

extern "C" void* GSscratchAlloc(u8 blockCount, GSscratchNotifyCallback notify)
{
    if (_scratchInvalid == 1) {
        return NULL;
    }
    if (blockCount == 0 || blockCount > SCRATCH_BLOCK_COUNT) {
        return NULL;
    }
    for (u8 start = 0; start < SCRATCH_BLOCK_COUNT; start++) {
        u32 bits = getBlockMap(start, blockCount);
        if (bits == 0) {
            ScratchBlock* block = findBlock(SCRATCH_BLOCK_UNUSED);
            if (block == NULL) {
                return NULL;
            }
            block->start = start;
            block->count = blockCount;
            block->notify = notify;
            setBlockMap(start, blockCount);
            return _scratchPadBase + block->start * SCRATCH_BLOCK_SIZE;
        }
    }
    return NULL;
}

extern "C" u32 GSscratchInit(u8 stackBlocks)
{
    asm {
        mflr r3
        stw r3, _scratchInitLR
    }
    _blockMap = 0;
    for (int i = 0; i < SCRATCH_BLOCK_COUNT; i++) {
        _blockTable[i].start = SCRATCH_BLOCK_UNUSED;
    }
    LCEnable();
    _scratchPadBase = (u8*)LCGetBase();
    _stackSize = stackBlocks * SCRATCH_BLOCK_SIZE;
    if (stackBlocks) {
        u8 count = stackBlocks;
        u32 mask = blockMask(0);
        while (count--) {
            _blockMap |= mask;
            mask >>= 1;
        }
        _blockTable[0].start = 0;
        _blockTable[0].count = stackBlocks;
    }
    _scratchInvalid = 0;
    if (stackBlocks == 0) {
        return 1;
    }
    // The scratch stack terminates its backchain with an invalid address.
    asm {
        warn_def_reserved_reg off
        lwz r3, _scratchPadBase
        lwz r5, _stackSize
        add r3, r3, r5
        subi r1, r3, 8
        li r3, -1
        stw r3, 0(r1)
        lwz r3, _scratchInitLR
        mtlr r3
        blr
    }
    return 1;
}

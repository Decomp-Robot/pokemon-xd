/*
 * fsysBlock.c
 *
 * The ARAM staging blocks of the file system: the block pool _fsysInitBlock
 * cuts out of main memory and the lists the readers keep their blocks in.
 */

#include <GSAPI/GSfsys/fsys.h>

/* Staging pool _fsysInitBlock cuts from main memory, with the descriptor
 * table and the sizes and cursor describing it. */
u32 _fsysBlockByteSize;
u32 _fsysBlockBufferSize;
u32 _fsysBlockSize;
u32 _fsysBlockIndex;
FsysBlock* _fsysBlockManage;
u8* _fsysBlock;

/*! Move the last block of one list onto the end of another. */
void _fsysMoveBlock(FsysBlock** from, FsysBlock** to)
{
    s32 intr = OSDisableInterrupts();
    FsysBlock* block = *from;
    FsysBlock* last;

    if (block != 0) {
        while (block->next != 0) {
            block = block->next;
        }
        if (block->prev != 0) {
            block->prev->next = 0;
        }
        if (*from == block) {
            *from = 0;
        }
        block->prev = 0;
        block->next = 0;

        last = *to;
        if (last != 0) {
            while (last->next != 0) {
                last = last->next;
            }
            last->next = block;
            block->prev = last;
        } else {
            *to = block;
        }
    }
    OSRestoreInterrupts(intr);
}

/*! Put a block, and anything still chained behind it, at the head of a list. */
void _fsysAddBlock(FsysBlock** list, FsysBlock* block)
{
    s32 intr = OSDisableInterrupts();
    FsysBlock* head = *list;

    if (head != 0) {
        if (block->next != 0) {
            FsysBlock* last = block->next;

            while (last->next != 0) {
                last = last->next;
            }
            last->next = head;
            head->prev = last;
        } else {
            block->next = head;
            head->prev = block;
        }
    }
    *list = block;
    OSRestoreInterrupts(intr);
}

/*! Free every block of a list. */
void _fsysFreeBlockAll(FsysBlock** list)
{
    while (1) {
        if (*list == 0) {
            break;
        }
        _fsysFreeBlock(list, 1);
    }
}

/*! Free one block.  `freeHead` picks the end of the list it is taken from:
 *  the head when it is set, the tail when it is clear. */
u8 _fsysFreeBlock(FsysBlock** list, u8 freeHead)
{
    s32 intr = OSDisableInterrupts();
    FsysBlock* block = *list;

    if (block != 0) {
        if (freeHead == 0) {
            while (block->next != 0) {
                block = block->next;
            }
            if (block->prev != 0) {
                block->prev->next = 0;
            }
            if (*list == block) {
                *list = 0;
            }
            block->data = 0;
        } else {
            if (block->prev != 0) {
                block->prev->next = block->next;
            }
            if (block->next != 0) {
                block->next->prev = block->prev;
            }
            block->data = 0;
            *list = block->next;
        }
    }
    OSRestoreInterrupts(intr);
    return 1;
}

/*! Hand out the next free staging block, going round the pool once. */
FsysBlock* _fsysAllocBlock(void)
{
    FsysBlock* candidate;
    FsysBlock* block = 0;
    s32 intr = OSDisableInterrupts();
    u32 i = _fsysBlockIndex;

    while (block == 0) {
        candidate = &_fsysBlockManage[i];

        if (candidate->data == 0) {
            candidate->data = _fsysBlock + i * _fsysBlockByteSize;
            candidate->prev = 0;
            candidate->next = 0;
            candidate->size = 0;
            block = candidate;
        }
        i++;
        if (i >= _fsysBlockSize) {
            i = 0;
        }
        if (_fsysBlockIndex == i) {
            break;
        }
    }
    _fsysBlockIndex = i;
    OSRestoreInterrupts(intr);
    return block;
}

/*! Cut the staging memory into blocks.  `size` is how much main memory the
 *  staging pool may spend; zero asks for the default. */
u8 _fsysInitBlock(u32 size)
{
    u32 i;
    _fsysBlockIndex = 0;
    _fsysBlockByteSize = ARQGetChunkSize();
    _fsysBlockBufferSize = size;
    if (size == 0) {
        _fsysBlockBufferSize = FSYS_DEFAULT_BLOCK_POOL_SIZE;
    }

    _fsysBlockSize = _fsysBlockBufferSize / _fsysBlockByteSize;
    if (_fsysBlockSize == 0) {
        return 0;
    }

    _fsysBlock = (u8*)_fsysDoMalloc(_fsysBlockByteSize * _fsysBlockSize);
    if (_fsysBlock == 0) {
        return 0;
    }
    _fsysBlockManage = (FsysBlock*)_fsysDoMalloc(_fsysBlockSize * sizeof(FsysBlock));
    if (_fsysBlockManage == 0) {
        return 0;
    }

    for (i = 0; i < _fsysBlockSize; i++) {
        _fsysBlockManage[i].prev = 0;
        _fsysBlockManage[i].next = 0;
        _fsysBlockManage[i].data = 0;
        _fsysBlockManage[i].size = 0;
    }
    return 1;
}

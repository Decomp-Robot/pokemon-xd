#include "alloc.h"
#include "../Runtime.PPCEABI.H/GCN_Mem_Alloc.h"
#include "critical_regions.h"

/* ------------------------------------------------------------------ */
/* Pooled allocator                                                    */
/* ------------------------------------------------------------------ */

/*
 * Every block of allocator memory is preceded by two words.  The first is
 * its tag, holding the block's span; the span is rounded up to the eight
 * byte granularity the tag arithmetic relies on, so the rest of the low
 * bits are free for flags.  The second word is the block's owner: a block
 * cut out of a variable-size pool points at that pool with bit 0 set, a
 * block cut out of a fixed-size pool points at its PoolBlock.  A fixed
 * block's size cannot be recovered from its span, because the cells are
 * rounded up, so the descriptor remembers the size it was asked for.
 */
#define BLOCK_VAR_POOL 0x1
#define BLOCK_SPAN_MASK 0xFFFFFFF8
#define BLOCK_FLAGS 0x7
#define BLOCK_USED 0x2
#define BLOCK_PREV_USED 0x4

/* Sizes served from the fixed-size pools, smallest first. */
extern const int fix_pool_sizes[];

/* Largest request still satisfied out of a fixed-size pool. */
#define FIXED_POOL_LIMIT 68
#define MAX_FIXED_POOLS 6

/*
 * A pool block hands out cells of one size, carved out of one run of
 * blocks that goes back to the variable-size pools as soon as the last of
 * its cells is free.  'used' counts how many of the run's cells are handed
 * out.  Free cells are threaded through the caller's memory: 'free_cells'
 * is the word in front of the next free cell, so that word still names the
 * cell's own descriptor and the word after it, the first of the cell's
 * user area, holds the link to the cell after that.
 */
typedef struct PoolBlock {
    struct PoolBlock *next;   /* +0x00 */
    struct PoolBlock *prev;   /* +0x04 */
    unsigned int size;        /* +0x08 size of the cells it hands out */
    void *free_cells;         /* +0x0c head of the stack of free cells */
    int used;                 /* +0x10 cells of the run handed out */
} PoolBlock;

/*
 * The two ends of one size's list of pool blocks.  The list is circular
 * and 'last' is the block in front of which one whose cells were all
 * handed out is relinked once a cell comes back to it.
 */
typedef struct Pool {
    PoolBlock *head;   /* +0x00 */
    PoolBlock *last;   /* +0x04 */
} Pool;

/*
 * A variable-size pool owns one region of memory: this descriptor, the
 * chunks carved out of the rest of the region, and one chunk header at the
 * very end whose tag terminates the region and whose owner word is unused,
 * so it holds the head of the free list the chunks are chained on.  The
 * region's first chunk starts right behind the descriptor, so the pool can
 * tell that it is empty -- a single free chunk covering all of the region
 * -- without walking the list.  'high' is the largest span that has been
 * given back to the pool, which bounds what the pool can still serve.
 */
typedef struct VarPool {
    struct VarPool *prev;    /* +0x00 */
    struct VarPool *next;    /* +0x04 */
    unsigned int high;       /* +0x08 largest span given back so far */
    unsigned int span;       /* +0x0c bytes this pool owns */
} VarPool;

/*
 * A chunk of a variable-size pool.  The word below the tag of the chunk
 * that follows holds this chunk's span, so a chunk can find the one in
 * front of it; while the chunk is free its first two payload words carry
 * its links in the pool's free list.
 */
typedef struct PoolChunk {
    unsigned int tag;            /* +0x00 span and flags */
    unsigned int owner;          /* +0x04 PoolBlock, or VarPool with bit 0 */
    struct PoolChunk *next;      /* +0x08 while free */
    struct PoolChunk *prev;      /* +0x0c while free */
} PoolChunk;

/*
 * Bytes of a pool's region that are not chunk space: the descriptor, and
 * the tag and owner word of the chunk header the region ends with.
 */
#define VAR_POOL_OVERHEAD (sizeof(VarPool) + 2 * sizeof(unsigned int))

/* The head of a pool's free list, kept in the last word of its region. */
#define POOL_FREE_LIST(pool, span) \
    (*(PoolChunk **)((char *)(pool) + (span) - sizeof(unsigned int)))

/*
 * Allocator state, owned by the allocator's own translation unit.  The
 * variable-size pools form a plain list; the fixed-size pools follow it as
 * six entries, because the deallocation routines address them as an offset
 * from the state.
 */
typedef struct AllocState {
    VarPool *var_pools;              /* +0x00 */
    Pool fixed[MAX_FIXED_POOLS];     /* +0x04 */
} AllocState;                        /* 0x34 bytes */

extern AllocState defaultAllocatorState;

/* Set once the state block has been zeroed, so free() stays cheap. */
extern unsigned char allocatorInitialized;

extern void *memset(void *dst, int c, unsigned long n);

extern void __pool_free(AllocState *state, void *ptr);
extern void deallocate_from_fixed_pools(AllocState *state, void *ptr, unsigned int size);
extern void deallocate_from_var_pools(AllocState *state, void *ptr);

void free(void *ptr) {
    __begin_critical_region(malloc_pool_access);
    if (allocatorInitialized == 0) {
        memset(&defaultAllocatorState, 0, sizeof(defaultAllocatorState));
        allocatorInitialized = 1;
    }
    __pool_free(&defaultAllocatorState, ptr);
    __end_critical_region(malloc_pool_access);
}

void __pool_free(AllocState *state, void *ptr) {
    unsigned int owner;
    unsigned int size;

    if (ptr == 0) {
        return;
    }
    owner = *(unsigned int *)((char *)ptr - 4);
    if ((owner & BLOCK_VAR_POOL) == 0) {
        size = *(unsigned int *)(owner + 8);
    } else {
        size = (*(unsigned int *)((char *)ptr - 8) & BLOCK_SPAN_MASK) - 8;
    }
    if (size <= FIXED_POOL_LIMIT) {
        deallocate_from_fixed_pools(state, ptr, size);
    } else {
        deallocate_from_var_pools(state, ptr);
    }
}

void deallocate_from_fixed_pools(AllocState *state, void *ptr, unsigned int size) {
    unsigned int i;
    Pool *pool;
    PoolBlock *block;
    PoolBlock *adjacent;
    unsigned int *cell;

    for (i = 0; size > (unsigned int)fix_pool_sizes[i]; i++) {
        ;
    }
    cell = (unsigned int *)ptr - 1;
    block = *(PoolBlock **)cell;
    pool = &state->fixed[i];

    if (block->free_cells == 0) {
        /* All of the block's cells were handed out, so the block left the
         * pool's list; give the cell back at the end of that list. */
        adjacent = pool->last;
        if (adjacent != block) {
            if (pool->head == block) {
                /* It is the head, so the list only has to be rotated. */
                pool->last = adjacent->next;
                pool->head = pool->head->next;
            } else {
                block->next->prev = block->prev;
                block->prev->next = block->next;
                block->prev = pool->last;
                block->next = block->prev->next;
                block->next->prev = block;
                block->prev->next = block;
                pool->last = block;
            }
        }
    }
    cell[1] = (unsigned int)block->free_cells;
    block->free_cells = cell;
    if (--block->used == 0) {
        /* The run is idle again: unlink the block and hand its cells back
         * to the variable-size pool they were carved out of. */
        if (pool->last == block) {
            pool->last = block->prev;
        }
        if (pool->head == block) {
            pool->head = block->next;
        }
        block->next->prev = block->prev;
        block->prev->next = block->next;
        if (pool->last == block) {
            pool->last = (PoolBlock *)0;
        }
        if (pool->head == block) {
            pool->head = (PoolBlock *)0;
        }
        deallocate_from_var_pools(state, block);
    }
}

void deallocate_from_var_pools(AllocState *state, void *ptr)
{
  PoolChunk *base;
  unsigned int prevspan;
  PoolChunk *node;
  int release;
  PoolChunk *merged;
  VarPool *pool_next;
  PoolChunk *first;
  PoolChunk *head;
  unsigned int node_tag;
  PoolChunk **head_slot;
  VarPool *pool;
  int span;
  unsigned int tag;
  unsigned int pool_span;
  PoolChunk *chunk;
  int raw_span;
  PoolChunk *next;
  unsigned int total;
  char *header;
  header = ((char *) ptr) - 8;
  chunk = (PoolChunk *) header;
  tag = *((unsigned int *) header);
  raw_span = tag & BLOCK_SPAN_MASK;
  span = raw_span;
  next = (PoolChunk *) (((char *) chunk) + span);
  pool_span = *((unsigned int *) (((char *) chunk) + 4));
  pool = (VarPool *) (pool_span & (~BLOCK_VAR_POOL));
  chunk->tag = tag & (~BLOCK_USED);
  next->tag &= ~BLOCK_PREV_USED;
  *((unsigned int *) (((char *) next) - 4)) = span;
  pool_span = pool->span & BLOCK_SPAN_MASK;
  head = POOL_FREE_LIST(pool, pool_span);
  if (head != 0)
  {
    chunk->next = head->next;
    chunk->next->prev = chunk;
    chunk->prev = POOL_FREE_LIST(pool, pool_span);
    (POOL_FREE_LIST(pool, pool_span))->next = chunk;
    POOL_FREE_LIST(pool, pool_span) = chunk;
    head_slot = &POOL_FREE_LIST(pool, pool_span);
    node = *head_slot;
    if ((node->tag & BLOCK_PREV_USED) == 0)
    {
      prevspan = *((unsigned int *) (((char *) node) - 4));
      if (prevspan & BLOCK_USED)
      {
        merged = node;
      }
      else
      {
        base = (PoolChunk *) (((char *) node) - prevspan);
        base->tag = base->tag & BLOCK_FLAGS;
        base->tag = base->tag | ((prevspan + (node->tag & BLOCK_SPAN_MASK)) & BLOCK_SPAN_MASK);
        if ((base->tag & BLOCK_USED) == 0)
        {
          *((unsigned int *) ((((char *) base) + (prevspan + (node->tag & BLOCK_SPAN_MASK))) - 4)) = prevspan + (node->tag & BLOCK_SPAN_MASK);
        }
        if ((*head_slot) == node)
        {
          POOL_FREE_LIST(pool, pool_span) = (POOL_FREE_LIST(pool, pool_span))->prev;
        }
        node->prev->next = node->next;
        node->prev->next->prev = node->prev;
        merged = base;
      }
    }
    else
    {
      merged = node;
    }
    POOL_FREE_LIST(pool, pool_span) = merged;
    node = POOL_FREE_LIST(pool, pool_span);
    node_tag = node->tag;
    span = node->tag & BLOCK_SPAN_MASK;
    next = (PoolChunk *) (((char *) node) + span);
    if ((next->tag & BLOCK_USED) == 0)
    {
      total = span + (next->tag & BLOCK_SPAN_MASK);
      node->tag = node_tag & BLOCK_FLAGS;
      node->tag = node->tag | (total & BLOCK_SPAN_MASK);
      if (!(node->tag & BLOCK_USED))
      {
        *((unsigned int *) ((((char *) node) + total) - 4)) = total;
      }
      if ((node->tag & BLOCK_USED) == 0)
      {
        *((unsigned int *) (((char *) node) + total)) &= ~BLOCK_PREV_USED;
      }
      else
      {
        *((unsigned int *) (((char *) node) + total)) |= BLOCK_PREV_USED;
      }
      if ((POOL_FREE_LIST(pool, pool_span)) == next)
      {
        POOL_FREE_LIST(pool, pool_span) = (POOL_FREE_LIST(pool, pool_span))->prev;
      }
      if ((POOL_FREE_LIST(pool, pool_span)) == next)
      {
        POOL_FREE_LIST(pool, pool_span) = 0;
      }
      next->prev->next = next->next;
      next->next->prev = next->prev;
    }
  }
  else
  {
    POOL_FREE_LIST(pool, pool_span) = chunk;
    chunk->next = chunk;
    chunk->prev = chunk;
  }
  head = POOL_FREE_LIST(pool, pool_span);
  span = head->tag & BLOCK_SPAN_MASK;
  if (pool->high < span)
  {
    pool->high = span;
  }
  first = (PoolChunk *) (((char *) pool) + (sizeof(VarPool)));
  release = 0;
  if (((first->tag & BLOCK_USED) == 0) && ((first->tag & BLOCK_SPAN_MASK) == ((pool->span & BLOCK_SPAN_MASK) - VAR_POOL_OVERHEAD)))
  {
    release = 1;
  }
  if (release)
  {
    pool_next = pool->next;
    if (pool_next == pool)
    {
      pool_next = 0;
    }
    if (state->var_pools == pool)
    {
      state->var_pools = pool_next;
    }
    if (pool_next != 0)
    {
      pool_next->prev = pool->prev;
      pool_next->prev->next = pool_next;
    }
    pool->next = 0;
    pool->prev = 0;
    __sys_free(pool);
  }
}

